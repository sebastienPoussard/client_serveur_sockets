#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


// cette fonction créer et renvoie une socket DGRAM en écoute.
// la socket sera ouverte sur le port spécifié en parametre.
int socDgramEcoute(char port[5]) {

    // ==================================== VARIABLES ====================================================
    int soc;		        	        // descripteur de socket
    int status;			                // permet de verifier s'il n'y à pas d'erreur
    struct addrinfo hints;		        // structure qui contient les informations (ip, port ...) à renseigner
    struct addrinfo *servinfo;          // retour de getaddrinfo (structure completée)
    struct addrinfo *p;		            // curseur

    // ============================= CONSTRUCTION DE LA SOCKET DGRAM =====================================
    // s'assurer que la structure addrinfo à passer est bien vide
    memset(&hints, 0, sizeof hints); // initialiser la structure avec des 0
    // remplir avec nos données
    hints.ai_family = AF_UNSPEC;     // IPv4 ou IPv6
    hints.ai_socktype = SOCK_DGRAM;  // mode DATAGRAM
    hints.ai_flags = AI_PASSIVE;     // écoute sur toute les interfaces, remplis automatiquement par mes IP
  
    printf("Création d'une socket DGRAM d'écoute...\n");
    // passer les parametres à getaddrinfo pour autocomplétion du reste
    if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }	
    // chercher dans la liste chainée de résultats retournés par getaddrinfo une structure addrinfo correcte
    // et l'associer à une nouvelle socket "soc"
    for(p = servinfo; p != NULL; p = p->ai_next) {
        // verifier que la commande de création de socket n'échoue pas
        if ((soc = socket(p->ai_family, p->ai_socktype,	p->ai_protocol)) == -1) {
            perror("Erreur de socket() à la création de la socket.");
            continue;
        }
        // modifier l'option de la socket SO_REUSEADDR pour pouvoir re utiliser la socket rapidement 
        // évite les probleme de socket non liberée 
        int yes=1;
        if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("Echec à la modification des options de la socket");
            exit(1);
        }
        // associer la socket au port (stocké dans la structure renvoyé par getaddrinfo)
        if (bind(soc, p->ai_addr, p->ai_addrlen) == -1) {
            close(soc);
            perror("Erreur de bind() à l'association du port à la socket.");
            continue;
        }
        break;
    }
    // libérer la liste d'addrinfo
    freeaddrinfo(servinfo); // liberer la liste (plus utilisé)
    // verifier que on à réussis à trouver une structure addrinfo correcte dans la liste retourné par getaddrinfo
    if (p == NULL) {
        fprintf(stderr, "getaddrinfo n'as pas réussis à renvoyer une structure addrinfo valable pour l'association de la socket\n");
        exit(1);
    }
    return soc;
}

// cette fonction créer une socket DATAGRAM et envoie un message
// les parametres adresse et port permettent de spécifier où envoyer le message
// le parametre message contient le message à envoyer limité à 1024 caractères
// la fonction renvoie 0 en cas de succès
int socDgramEnvoie(char adresse[50], char port[5], char message[1024]) {

    // ==================================== VARIABLES ====================================================
    int soc;		        	        // descripteur de socket
    int status;			                // permet de verifier s'il n'y à pas d'erreur
    struct addrinfo hints;		        // structure qui contient les informations (ip, port ...) à renseigner
    struct addrinfo *servinfo;          // retour de getaddrinfo (structure completée)
    struct addrinfo *p;		            // curseur
    int numbytes;                       // compteur de bits envoyés

    // ============================= CONSTRUCTION DE LA SOCKET DGRAM =====================================
    // s'assurer que la structure addrinfo à passer est bien vide
    memset(&hints, 0, sizeof hints); // initialiser la structure avec des 0
    // remplir avec nos données
    hints.ai_family = AF_UNSPEC;     // IPv4 ou IPv6
    hints.ai_socktype = SOCK_DGRAM;  // mode DATAGRAM
    hints.ai_flags = AI_PASSIVE;     // écoute sur toute les interfaces, remplis automatiquement par mes IP
 
    printf("Création d'une socket d'envoie...\n");
        // passer les parametres à getaddrinfo pour autocomplétion du reste
        if ((status = getaddrinfo(adresse, port, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            exit(1);
        }	
        // chercher dans la liste chainée de résultats retournés par getaddrinfo une structure addrinfo correcte
        // et l'associer à une nouvelle socket "soc"
        for(p = servinfo; p != NULL; p = p->ai_next) {
            // verifier que la commande de création de socket n'échoue pas
            if ((soc = socket(p->ai_family, p->ai_socktype,	p->ai_protocol)) == -1) {
                perror("Erreur de socket() à la création de la socket.");
                continue;
            }
            // modifier l'option de la socket SO_REUSEADDR pour pouvoir re utiliser la socket rapidement 
            // évite les probleme de socket non liberée 
            int yes=1;
            if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
                perror("Echec à la modification des options de la socket");
                exit(1);
            }
            break;
        }
        // libérer la liste d'addrinfo
        freeaddrinfo(servinfo); // liberer la liste (plus utilisé)
        // verifier que on à réussis à trouver une structure addrinfo correcte dans la liste retourné par getaddrinfo
        if (p == NULL) {
            fprintf(stderr, "getaddrinfo n'as pas réussis à renvoyer une structure addrinfo valable pour l'association de la socket\n");
            exit(1);
        }
        // envoie du message 
        if ((numbytes = sendto(soc, message, strlen(message), 0, p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        } 
    return 0;
}

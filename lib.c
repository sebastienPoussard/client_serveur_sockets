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

//Variable Globale
struct addrinfo *p;   // curseur

// cette fonction créer et renvoie une socket DGRAM en écoute.
// la socket sera ouverte sur le port spécifié en parametre.
int socDgramEcoute(char port[5]) {

    // ==================================== VARIABLES ====================================================
    int soc;		        	        // descripteur de socket
    int status;			                // permet de verifier s'il n'y à pas d'erreur
    struct addrinfo hints;		        // structure qui contient les informations (ip, port ...) à renseigner
    struct addrinfo *servinfo;          // retour de getaddrinfo (structure completée)

    // ============================= CONSTRUCTION DE LA SOCKET DGRAM =====================================
    // s'assurer que la structure addrinfo à passer est bien vide
    memset(&hints, 0, sizeof hints); // initialiser la structure avec des 0
    // remplir avec nos données
    hints.ai_family = AF_UNSPEC;     // IPv4 ou IPv6
    hints.ai_socktype = SOCK_DGRAM;  // mode DATAGRAM
    hints.ai_flags = AI_PASSIVE;     // écoute sur toute les interfaces, remplis automatiquement par mes IP
  
    printf("DEBUG : Construction socket DGRAM ecoute\n");
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
int socDgramEnvoie(char adresse[50], char port[5]) {

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
 
    printf("DEBUG : construction socket DGRAM envoie\n");
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
     
    return soc;
}

//Envoie un msg au serveur en mode DGRAM
int envoieMsgDgram(int soc, char message[1024]){
	int numbytes;	// compteur de bits envoyés

        if ((numbytes = sendto(soc, message, strlen(message), 0, p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
	close(soc);
	return 1;
}

//Reception d'un msg en mode DGRAM
int recepMsgDgram(int socEcoute){
	int tailleDonnees;        		// taille de données reçues
	int bufferMAX = 500;                	// taille max du buffer
	char buffer[bufferMAX];             	// buffer permettant de stocker les données
	int tcli;                           	// taille de la structure du client qui envoie des données
	struct sockaddr_storage addrClient; 	// structure qui contient les informations du client

	printf("En attente de reception d'un message ...\n");
	if ((tailleDonnees = recvfrom(socEcoute, buffer, bufferMAX-1 , 0,(struct sockaddr *)&addrClient, &tcli)) == -1) {
		perror("recvfrom");
		exit(1);
	}
 	printf("Message reçue :");
	printf("%s\n",buffer);
	close(socEcoute);
	return 1;
}

// cette fonction crée une socket STREAM (TCP)
// prend en parametre le port à ouvrir 
// renvoie le pointeur vers la socket
int socStreamRdv(char port[5]) {
    
    // ==================================== VARIABLES ====================================================
    int soc;		        	        // descripteur de socket
    int status;			                // permet de verifier s'il n'y à pas d'erreur
    struct addrinfo hints;		        // structure qui contient les informations (ip, port ...) à renseigner
    struct addrinfo *servinfo;          // retour de getaddrinfo (structure completée)
    int numbytes;                       // compteur de bits envoyés


    // ============================= CONSTRUCTION DE LA SOCKET STREAM ====================================

    // s'assurer que la structure addrinfo à passer est bien vide
    memset(&hints, 0, sizeof hints); // initialiser la structure avec des 0
    // remplir avec nos données
    hints.ai_family = AF_UNSPEC;        // IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM;    // mode STREAM
    hints.ai_flags = AI_PASSIVE;        // écoute sur toute les interfaces, remplis automatiquement par mes IP
 
    printf("DEBUG : Construction socket TCP RDV sur %s\n",port);
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
    freeaddrinfo(servinfo); 
    // verifier que on à réussis à trouver une structure addrinfo correcte dans la liste retourné par getaddrinfo
    if (p == NULL) {
        fprintf(stderr, "getaddrinfo n'as pas réussis à renvoyer une structure addrinfo valable pour l'association de la socket\n");
        exit(1);
    }
    return soc;
}

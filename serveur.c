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


int main() {

    // ==================================== VARIABLES ====================================================
    char port[5];     			        // port définis par l'utilisateur
    int soc;		        	        // descripteur de socket
    int status;			                // permet de verifier s'il n'y à pas d'erreur
    int tailleDonnees;                  // taille de données reçues
    int tcli;                           // taille de la structure du client qui envoie des données
    struct addrinfo hints;		        // structure qui contient les informations (ip, port ...) à renseigner
    struct addrinfo *servinfo;          // retour de getaddrinfo (structure completée)
    struct addrinfo *p;		            // curseur
    struct sockaddr_storage addrClient; // structure qui contient les informations du client
    int bufferMAX = 500;                // taille max du buffer
    char buffer[bufferMAX];             // buffer permettant de stocker les données
    
    
    // afficher message de bienvenue et demander le port sur lequele lancer l'écoute
    printf("lancement du serveur\n\n");
    printf("Sur quel port faut il ouvrir le service ?\n\n");
    
    scanf("%s", port);
    printf("Initialisation du service sur le port %s\n",port);
    
    // ============================= CONSTRUCTION DE LA SOCKET  ===========================================
    // s'assurer que la structure addrinfo à passer est bien vide
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    // remplir avec nos données
    hints.ai_family = AF_UNSPEC;     // IPv4 ou IPv6
    hints.ai_socktype = SOCK_DGRAM;  // mode DATAGRAM
    hints.ai_flags = AI_PASSIVE;     // écoute sur toute les interfaces, remplis automatiquement par mes IP
    
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
    freeaddrinfo(servinfo); // free the linked-list
    // verifier que on à réussis à trouver une structure addrinfo correcte dans la liste retourné par getaddrinfo
    if (p == NULL) {
        fprintf(stderr, "getaddrinfo n'as pas réussis à renvoyer une structure addrinfo valable pour l'association de la socket\n");
        exit(1);
    }

    // ================================== TRAITEMENT DES DONNEES RECUES  ======================================= 
     
    tcli = sizeof addrClient;
    while(1) {
        if ((tailleDonnees = recvfrom(soc, buffer, bufferMAX-1 , 0, (struct sockaddr *)&addrClient, &tcli)) == -1) {
            perror("recvfrom");
            exit(1);
        }    
    }
    printf("%s\n",buffer); 
    
}

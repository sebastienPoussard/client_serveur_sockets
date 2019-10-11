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
#include "lib.h"

// couleurs pour le terminal
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

int main() {

    // ==================================== VARIABLES ====================================================
    char port[5];     			        // port définis par l'utilisateur
    int socEcoute;	        	        // descripteur de socket en écoute de requêtes
    int socRep;                         // descripteur de socket en réponse aux requêtes
    char ipClient[1024];                // adresse IP du client
    char service[20];
    int tailleDonnees;                  // taille de données reçues
    int tcli;                           // taille de la structure du client qui envoie des données
    struct sockaddr_storage addrClient; // structure qui contient les informations du client
    int bufferMAX = 500;                // taille max du buffer
    char buffer[bufferMAX];             // buffer permettant de stocker les données
    
    
    // afficher message de bienvenue et demander le port sur lequele lancer l'écoute
    printf("lancement du serveur\n\n");
    printf("Sur quel port faut il ouvrir le service ?\n\n");
    
    scanf("%s", port);
    printf("Initialisation du service sur le port %s\n",port);
   

    // ================================== TRAITEMENT DES DONNEES RECUES  ======================================= 
    // création de le socket d'écoute
    socEcoute = creationSocketDgram(NULL,port);
    tcli = sizeof addrClient;
    // nettoyer le buffer
    memset(buffer, 0, sizeof buffer);
    while(1) {
        printf("En attente de reception d'un message ...\n");
        if ((tailleDonnees = recvfrom(socEcoute, buffer, bufferMAX-1 , 0, (struct sockaddr *)&addrClient, &tcli)) == -1) {
            perror("recvfrom");
            exit(1);
        }    
        // récuperer l'ip du client et le port du client
        getnameinfo(&addrClient, sizeof addrClient, ipClient, sizeof ipClient, service, sizeof service, 0);
        printf("Message reçue de ");
        printf(BLU "%s:%s",ipClient,service);
        printf(RESET " --> ");
        printf(MAG "%s\n" RESET,buffer);
        // création d'une socket dgram pour répondre au client
        socRep = creationSocketDgram(ipClient,service);
        // envoi de données au client
        int numbytes;
        if ((numbytes = sendto(socRep, buffer, strlen(buffer), 0, ipClient, service)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        // nettoyer le buffer pour la prochaine reception
        memset(buffer, 0, sizeof buffer);
    }
    
}

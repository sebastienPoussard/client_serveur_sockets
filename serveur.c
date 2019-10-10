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

int main() {

    // ==================================== VARIABLES ====================================================
    char port[5];     			        // port définis par l'utilisateur
    int soc;		        	        // descripteur de socket
    char adresseCli[50];                // adresse IP du client
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
    soc = creationSocketDgram(NULL,port);
    tcli = sizeof addrClient;
    // nettoyer le buffer
    memset(buffer, 0, sizeof buffer);
    while(1) {
        printf("En attente de reception d'un message ...\n");
        if ((tailleDonnees = recvfrom(soc, buffer, bufferMAX-1 , 0, (struct sockaddr *)&addrClient, &tcli)) == -1) {
            perror("recvfrom");
            exit(1);
        }    
        printf("J'ai reçue : %s \n",buffer); 
        // création d'une socket dgram pour envoie de données

        // nettoyer le buffer pour la prochaine reception
        memset(buffer, 0, sizeof buffer);
    }
    
}

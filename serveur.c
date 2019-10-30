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
    int socEcoute;	        	        // descripteur de socket en écoute de requêtes UDP 
    int socRep;                         // descripteur de socket en réponse aux requêtes UDP
    int socRdv;                         // descripteur de socket de rendez vous TCP
    int socCom;                         // descripteur de socjet de communication TCP
    char ipClient[1024];                // adresse IP du client
    char service[20];                   // service correspond au port utilisé (peut être nommé ex: HTTPS pour 443)
    int tailleDonnees;                  // taille de données reçues
    int tcli;                           // taille de la structure du client qui envoie des données
    struct sockaddr_storage addrClient; // structure qui contient les informations du client
    int bufferMAX = 1024;               // taille max du buffer
    char buffer[bufferMAX];             // buffer permettant de stocker les données
    int choix;                          // choix si le serveur est ouvert en TCP ou UDP
    int backlog = 50;                   // nombre de connexions qui peuvent être dans la queue en TCP
    
    
    // afficher message de bienvenue et demander si l'utilisateur souhaite ouvrir un port TCP ou UDP 
    printf("lancement du serveur...\n");
    choix = -1;
    while (choix != 1 && choix !=2) {
        printf("Quel type de port souhaitez vous ouvrir ?\n1 : TCP\n2 : UDP\n");
        memset(buffer, 0, sizeof buffer);
        fgets(buffer, sizeof buffer, stdin);
        buffer[strlen(buffer) - 1] = 0;
        choix = strtol(buffer, NULL, 10);
    }
    // demander le numero du port TCP ou UDP
    printf("Sur quel port faut il ouvrir le service ?\n\n");
    scanf("%s", port);
   

    // si l'utilisateur ouvre en UDP 
    if (choix == 2) {

        printf("Ouverture du port %s UDP en écoute... \n",port);
        // ================================== TRAITEMENT DES DONNEES RECUES  ======================================= 
        // création de le socket d'écoute
        socEcoute = socDgramEcoute(port);
        tcli = sizeof addrClient;
        // nettoyer le buffer
        memset(buffer, 0, sizeof buffer);
        while(1) {
            printf("En attente de reception d'un message ...\n");
            if ((tailleDonnees = recvfrom(socEcoute, buffer, bufferMAX-1 , 0,(struct sockaddr *)&addrClient, &tcli)) == -1) {
                perror("recvfrom");
                exit(1);
            }    
            // récuperer l'ip du client et le port du client
            getnameinfo((struct sockaddr *)&addrClient, sizeof addrClient, ipClient, sizeof ipClient, service, sizeof service, NI_NUMERICHOST);
            printf("Message reçue de ");
            printf(BLU "%s:%s",ipClient,service);
            printf(RESET " --> ");
            printf(MAG "%s\n" RESET,buffer);
            // envoyer la reponse au client
            socRep = socDgramEnvoie(ipClient, service);
            envoieMsgDgram(socRep, buffer);
            // nettoyer le buffer pour la prochaine reception
            memset(buffer, 0, sizeof buffer);
        }
    } else {
        // l'utilisateur ouvre un port en TCP
        printf("Ouverture du port %s TCP en écoute... \n",port);
        // ouvrir la socket TCP de rendez vous
        socRdv = socStreamRdv(port); 

        // la socket de rendez vous écoute les nouvelles connexions
        if (listen(socRdv, backlog) == -1) {
            perror("erreur de la socket de rendez vous à listen");
            exit(1);
        }
        
        printf("En attente d'une connexion...\n");
        // attendre les connexions
        while(1) {
            // accpeter une nouvelle connexion
            tcli = sizeof addrClient;
            socCom = accept(socRdv, (struct sockaddr *)&addrClient, &tcli);
            if (socCom == -1) {
                perror("Erreur à l'acceptation d'une nouvelle connexion TCP");
                continue;
            }
            // récuperer l'ip du client et le port du client
            getnameinfo((struct sockaddr *)&addrClient, sizeof addrClient, ipClient, sizeof ipClient, service, sizeof service, NI_NUMERICHOST);
            printf(YEL "Nouvelle connexion TCP de ");
            printf(BLU "%s:%s",ipClient,service);
            printf(MAG "\n" RESET,buffer);

            // faire un fork pour traiter la connexion du client
            if (!fork()) { 
                // traitement de la connexion, échange de données
                // le fils n'as pas besoin de la socket de rdv
                close(socRdv);
                // boucle de reception de messages
                while(1) {
                    // nettoyer le buffer
                    memset(buffer, 0, sizeof buffer);
                    // reception du message
                    if ((tailleDonnees = recv(socCom, buffer, bufferMAX-1, 0)) == -1) {
                        perror("erreur à la recpetion du message");
                        exit(1);
                    }              
                    // envoie d'une réponse au client
                    if (send(socCom, buffer, bufferMAX-1, 0) == -1) {
                        perror("erreur à l'envoie de la reponse au client");
                    }
                }
                // fermeture de la socket de communication
                close(socRdv);
                exit(0);
            }
            // processus parent ferme la socket d'échange de données
            close(socCom);
        }
    }   
}


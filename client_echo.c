#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "lib.h"

//fonction qui vide le buffer
void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}


int main (){
	char adresse[50];			//adresse du serveur
	char msg[bufferMAX];			//message a envoye au serveur
	char port[5];				//port sur lequel envoyer et ecouter
	int choix;				//choix de communication
	int socket; 				//socket de communication 

	//on recupere l'adresse du serveur
	printf("Adresse du serveur :");
	scanf("%s", adresse);
	viderBuffer();

	//on recupere le port du serveur
	printf("Port du serveur :");
	scanf("%s", port);
	viderBuffer();

	//on selectionne le protocole de transport
	printf("Quel type de communication souhaitez-vous utiliser ?\n");
	printf("1 : TCP\n2 : UDP\n");
	scanf("%i", &choix);
    viderBuffer();

	//on selectionne le type de communication
	switch (choix) {
		case 1:
            // creation de la socket STREAM (TCP)
			socket = socStream(adresse, port);
            // boucle d'interaction avec le serveur
            while(1) {
                // nettoyer le buffer msg
                memset(msg, 0, sizeof msg);

                //on recupere le msg a envoyer
                printf("message a envoyer au serveur :");
                scanf("%[^\n]", msg);
                viderBuffer();

                // envoie du message
                envoieMsgStr(socket, msg);
                // reception de la reponse du serveur
                recepMsgStr(socket);
            }
            // fermeture de la socket
			close(socket);
			break;
		case 2:
			//on cree une socket d'envoie DGRAM (UDP)
			socket = socDgram(adresse, port);
            // boucle d'interaction avec le serveur
            while (1) {
                // nettoyer le buffer msg
                memset(msg, 0, sizeof msg);

                //on recupere le msg a envoyer
                printf("message a envoyer au serveur :");
                scanf("%[^\n]", msg);
                viderBuffer();

                // envoie du message au serveur
                envoieMsgDgram(socket, msg);
                // reception de la reponse du serveur
                recepMsgDgram(socket);
            }
            // fermeture de la socket
			close(socket);
			break;
	}
	return 0;
}
	

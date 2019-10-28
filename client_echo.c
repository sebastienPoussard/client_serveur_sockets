#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include "lib.h"


int main (){
	int bufferMAX = 500;
	char adresse[50];
	char msg[bufferMAX];
	char port[5];
	int socket_envoie;
	int socket_ecoute;
	int tailleDonnees; 
	int tserveur;                           
	struct sockaddr_storage addrServeur; 


	printf("Adresse du serveur :");
	scanf("%s", adresse);

	printf("Port du serveur :");
	scanf("%s", port);

	printf("message a envoyer au serveur :");
	scanf("%s", msg);

	socket_envoie = creationSocketDgram(adresse, port);

	// envoi de données au serveur
	int numbytes;
	if ((numbytes = sendto(socket_envoie, msg, strlen(msg), 0, adresse, port)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	socket_ecoute = creationSocketDgram(NULL, port);
	// nettoyer le buffer
	memset(msg, 0, sizeof msg);
	while(1) {
		printf("En attente de reception d'un message ...\n");
		if ((tailleDonnees = recvfrom(socket_ecoute, msg, bufferMAX-1 , 0, (struct sockaddr *)&addrServeur, &tserveur)) == -1) {
			perror("recvfrom");
			exit(1);
		}

	}
	printf("Message reçue : %s", msg);
}

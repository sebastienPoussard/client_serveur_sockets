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

void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}


int main (){
	int bufferMAX = 500;
	char adresse[50];
	char msg[bufferMAX];
	char port[5];
	int socket_envoie, socket_ecoute;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage addrServeur;
	int rv;
	int numbytes;

	printf("Adresse du serveur :");
	scanf("%s", adresse);
	viderBuffer();

	printf("Port du serveur :");
	scanf("%s", port);
	viderBuffer();

	printf("message a envoyer au serveur :");
	scanf("%[^\n]", msg);
	viderBuffer();

	socket_envoie = socDgramEnvoie(adresse, port);
	envoieMsgDgram(socket_envoie, msg);

	//reception
	socket_ecoute = socDgramEcoute(port);
	recepMsgDgram(socket_ecoute);
	return 0;
}
	

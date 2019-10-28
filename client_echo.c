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

	printf("Port du serveur :");
	scanf("%s", port);

	printf("message a envoyer au serveur :");
	scanf("%s", msg);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(adresse, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_envoie = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

	if ((numbytes = sendto(socket_envoie, msg, strlen(msg), 0,
					p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	freeaddrinfo(servinfo);

	close(socket_envoie);

	//reception
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(adresse, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_envoie = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

	getnameinfo(&addrServeur, sizeof addrServeur, adresse, sizeof adresse, port, sizeof port, 0);

	if ((numbytes = recvfrom(socket_ecoute, msg, bufferMAX-1 , 0,
					(struct sockaddr *)&addrServeur, sizeof(addrServeur))) == -1) {
		perror("recvfrom");
		exit(1);
	}	

	freeaddrinfo(servinfo);

	close(socket_envoie);


	return 0;
}

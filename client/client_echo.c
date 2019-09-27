#include <stdio.h>
#include <string.h>
#include <sys/types.h>        
#include <sys/socket.h>


int main (){
	char adresse[50];
	int port;
	struct addrinfo adresse_serveur, *res, *p;
	struct sockaddr_storage serveur;


	printf("Adresse du serveur :");
	scanf("%s", adresse);

	printf("Port du serveur :");
	scanf("%d", &port);

	memset(&adresse_serveur, 0, sizeof(struct addrinfo));
	adresse_serveur.ai_family=AF_UNSPEC;
	adresse_serveur.ai_socktype=SOCK_DGRAM;

	if((getaddrinfo(adresse, port, &adresse_serveur, &res))!=0){
		printf("Erreur getaddrinfo");
		return 0;
	}



		

}

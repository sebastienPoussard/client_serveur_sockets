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
#define bufferMAX 1024	// taille max du buffer

// couleurs pour le terminal
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"




//Variable Globale
struct addrinfo *p;	// curseur

// cette fonction créer une socket DATAGRAM et envoie un message
// les parametres adresse et port permettent de spécifier où envoyer le message
// le parametre message contient le message à envoyer limité à 1024 caractères
// la fonction renvoie 0 en cas de succès
// Si l'adresse passée en parametre est NULL, alors le socket ecoute
int socDgram(char adresse[50], char port[5]) {

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
		if(adresse == NULL){
			// associer la socket au port (stocké dans la structure renvoyé par getaddrinfo)
			if (bind(soc, p->ai_addr, p->ai_addrlen) == -1) {
				close(soc);
				perror("Erreur de bind() à l'association du port à la socket.");
				continue;
			}
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
int envoieMsgDgram(int soc, char message[bufferMAX]){
	int numbytes;	// compteur de bits envoyés

    // renvoie du message
	if ((numbytes = sendto(soc, message, strlen(message), 0, p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	return 1;
}

//Reception d'un msg en mode DGRAM
int recepMsgDgram(int socEcoute){
	int tailleDonnees;        		// taille de données reçues
	char buffer[bufferMAX];             	// buffer permettant de stocker les données
	int tcli;                           	// taille de la structure du client qui envoie des données
	struct sockaddr_storage addrClient; 	// structure qui contient les informations du client

    // nettoyer le buffer
    memset(buffer, 0, sizeof buffer);
    // reception du message
	printf("En attente de reception d'un message ...\n");
	if ((tailleDonnees = recvfrom(socEcoute, buffer, bufferMAX-1 , 0,(struct sockaddr *)&addrClient,(socklen_t *)&tcli)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	printf("Message reçue :");
	printf(MAG "%s\n" RESET,buffer);
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

	// ============================= CONSTRUCTION DE LA SOCKET STREAM ====================================

	// s'assurer que la structure addrinfo à passer est bien vide
	memset(&hints, 0, sizeof hints); // initialiser la structure avec des 0
	// remplir avec nos données
	hints.ai_family = AF_UNSPEC;        // IPv4 ou IPv6
	hints.ai_socktype = SOCK_STREAM;    // mode STREAM
	hints.ai_flags = AI_PASSIVE;        // écoute sur toute les interfaces, remplis automatiquement par mes IP

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


//creation socket stream
int socStream(char adresse[50], char port[5]){
	int soc; 				//socket Stream utilise 
	struct addrinfo hints, *servinfo;	//structures qui contiennent des infos ip, af etc.
	int ecode;				//erreur code

	//on rentre les premieres informations
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; //IPv4 ou IPv6
	hints.ai_socktype = SOCK_STREAM; //socket stream = tcp

	//on recupere les infos de la machine a contacter
	if ((ecode = getaddrinfo(adresse, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", strerror(ecode));
		return 1;
	}

	// chercher dans la liste chainée de résultats retournés par getaddrinfo une structure addrinfo correcte
	// et l'associer à une nouvelle socket "soc"
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((soc = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(soc, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(soc);
			continue;
		}

		break;
	}

	// verifier que on à réussis à trouver une structure addrinfo correcte dans la liste retourné par getaddrinfo
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	//libere la structure
	freeaddrinfo(servinfo); 
	return soc;
}

//recoit un msg depuis le serveur en STREAM
int recepMsgStr(int soc){
	int numbytes;		//compteur de bits envoyés
	char buffer[bufferMAX];	//msg recu

	//reception du msg stream
	if ((numbytes = recv(soc, buffer, bufferMAX-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	//affichage du msg
	printf("Le serveur répond :");
    printf(MAG " '%s'\n" RESET,buffer);

	return 0;
}

//envoie un msg au serveur en STREAM
int envoieMsgStr(int soc, char msg[bufferMAX]){
	int numbytes;   	// compteur de bits envoyés

	//envoie msg stream
	if ((numbytes = send(soc, msg, strlen(msg), 0)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	return 0;
}

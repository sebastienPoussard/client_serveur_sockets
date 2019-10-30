// cette fonction créer et renvoie une socket DGRAM en écoute.
// la socket sera ouverte sur le port spécifié en parametre.
int socDgramEcoute(char port[5]) ;

// cette fonction créer une socket DATAGRAM et envoie un message
// les parametres adresse et port permettent de spécifier où envoyer le message
// le parametre message contient le message à envoyer limité à 1024 caractères
// la fonction renvoie 0 en cas de succès
int socDgramEnvoie(char adresse[50], char port[5]); 

// cette fonction crée une socket STREAM (TCP)
// prend en parametre le port à ouvrir 
// renvoie le pointeur vers la socket
int socStreamRdv(char port[5]); 

//Envoie un msg au serveur en mode DGRAM
int envoieMsgDgram(int soc, char message[1024]);

//Reception d'un msg en mode DGRAM
int recepMsgDgram(int socEcoute);

// cette fonction crée une socket STREAM (TCP)
// prend en parametre le port à ouvrir 
// renvoie le pointeur vers la socket
int socStreamRdv(char port[5]) ;



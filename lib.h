// cette fonction crée et renvoeie une socket
// si le paramétre "adresse" est laissé à "NULL", la méthode renvoie une socket qui écoute sur le port donné
// si le paramétre "port" est laissé à "NULL", la méthode renvoie une socket qui va communiquer vers un serveur
// sur un port donné
int creationSocketDgram(char adresse[50], char port[5]);


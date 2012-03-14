#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


#include "defobj.h"


#include <sys/socket.h>
#include "iniobj.h"


#include <stdio.h>


#include <netdb.h> //pour gethostbyname






int main(int argc, char* argv[]){

	//Verification de la syntaxe d'appel
	if(argc < 3)
	{
		fprintf(stderr,"Usage : %s <host name> <port number>\n",argv[0]);
		exit(-1);
	}

	obj * tableObj = initTable();

	if (!tableObj){

		fprintf(stderr,"Client - Erreur dans l'initialisation de la table d'objet.\n");
		exit(-1);



	}


	int ClntSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	int ServSocket;
	struct sockaddr_in echoServAddr; /*Adresse locale*/ //NB in = internet pas in/out
	struct sockaddr_in echoClntAddr; /*Adresse client*/
	struct hostent * hid;

	//PF_INET = domaine de la socket = internet
	//SOCK_STREAM = Mode connecté = TCP
	

	//Traitement des erreur
	if (ClntSocket < 0){

		perror("Client : Erreur pendant la creation de la Socket");
		exit(-1);
	}


	//SOCKET DU CLIENT

	//function bzero, plus rapide ?
	memset(&echoClntAddr,0,sizeof(echoClntAddr)); /*mise à zero de la structure*/

	echoClntAddr.sin_family = AF_INET; /*internet address family*/ //= domaine d'écoute
	echoClntAddr.sin_addr.s_addr = htonl(INADDR_ANY); /*n'importe quelle interface entrante*/ // = Sur quelle carte réseau j'écoute ? //hton1 = conversion little endian big endian h(host) to nl ( network long) 
	echoClntAddr.sin_port = 0; /*Port local, 0 laisse le choix à l'OS*/ 

//LIASON DE LA SOCKET CLIENT

	if(bind(ClntSocket,(struct sockaddr *) &echoClntAddr,sizeof(echoClntAddr)) < 0)  // ou alors juste faire un scocket() et un connect(ClntSocket,&echoClntAddr,0,sizeof(echoClntAddr)) qui va initialiser la structure adresse echoClntAddr tout seul 
	{

		perror("Client : Erreur lors de la liason de la socket");
		exit(-1);
	}

//SOCKET DU SERVEUR
	memset(&echoServAddr,0,sizeof(echoServAddr)); /*mise à zero de la structure*/

	echoServAddr.sin_family = AF_INET; /*internet address family*/ 
//= domaine d'écoute carte réseau j'écoute ? //hton1 = conversion little endian big endian h(host) to nl ( network long) 
	echoServAddr.sin_port = htons(atoi(argv[2])); /*Port local*/ 
	 //htons = conversion little endian big endian h(host) to ns ( network short)
	hid = gethostbyname(argv[1]);

	

	bcopy(hid->h_addr, (char *)&echoServAddr.sin_addr,hid->h_length);



//CONNECTION A LA SOCKET SERVEUR
if (connect(ClntSocket,(struct sockaddr *) &echoServAddr,sizeof(echoServAddr)) < 0){

		perror("Client : Erreur lors de la connexion de la socket au serveur");
		exit(-1);

}

printf("Client - Socket connectee au serveur avec succes.\n");

int i=0;
for(i;i<tablen;i++){

	if(send(ClntSocket,&tableObj[0],sizeof(obj),0) != sizeof(obj)){

		printf("Client : Erreur lors de l'envoi de l'objet %d",i);
		perror("");
		exit(-1);

	} else {

		printf("Client - Objet %d envoye avec succes.\n",i);
	}


}




close(ClntSocket);

printf("Client - Fermeture.\n\n");
exit(0);

/*
initialise le socket TCP
fait une demande de connexion au serveur
boucle :
envoi de "n" objets "obj" (obtenus de iniobj.h)
le dernier objet envoyé contient un marqueur de fin
obj.iqt = -1
fermer la connexion
fin
*/


// ./clio localhost numero de port
//argv 2 - Port du serveur




return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>


#include "defobj.h"


#include <sys/socket.h>

const int NB_MAX_CLIENT_SIMULTANES = 5;



int main(void){

	int ServSocket;
	struct sockaddr_in echoServAddr; /*Adresse locale*/
	struct sockaddr_in echoClntAddr; /*Adresse client*/
	int clntLen; /*Taille de la strucutre de donnees adresse client*/


	int ServSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	
	
	
	if (ServSocket < 0){
	
		perror("Serveur : Erreur pendant la creation de la Socket");
		exit(-1);
	}
	
	
	/*CONFIGURATION DE L'ADRESSE LOCALE*/
	
	memset(&echoServAddr,0,sizeof(echoServAddr)); /*mise à zero de la structure*/
	
	echoServAddr.sin_family = AF_INET; /*internet address family*/
	echoServAddr.sin_addr.s_addr = hton1(INADDR_ANY); /*n'importe quelle interface entrante*/
	echoServAddr.sin_port = htons(echoServAddr); /*Port local*/
	
	/*Bind a l'adresse locale*/
	
	if(bind(servSocket,(struct sockaddr_in *) &echoServAddr,sizeof(echoServAddr)) < 0)
	{
		perror("Serveur : Erreur pendant le reliage de la Socket");
		exit(-1);
	}
	
	/*Mise en ecoute*/
	
	if(listen(servSock,NB_MAX_CLIENT_SIMULTANES) < 0){
	
		perror("Serveur : Erreur lors de la mise en ecoute");
		exit(-1);
	}

	while(1){ //Boucle infinie
	
		/*Definir la taille du parametre d'entree-sortie*/
		clntLen = sizeof(echoClntAddr);
		
		if(clntSock=accept(servSock,(struct sockaddr_in *) &echoClntAddr,&clntLen) < 0){
		
			perror("Serveur : Erreur pendant l'acceptation d'un client")
			exit(-1);
			
		}
		
		
		//ICI : TRAITEMENT CLIENT	
	}


return 0;

}
/*

      initialise le socket TCP
      boucle :
		  attente des connexions clients
		  quand connexion client: fork()
			  fils : traiterclient()
				  boucle : lire data client sur socket
						   jusqu'à objet contenant "fin"
			  père : waitpid() attends fin du fils
				  si statut fin fils = arrêt : fin serveur


*/



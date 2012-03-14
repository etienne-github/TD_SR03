#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


#include "defobj.h"


#include <sys/socket.h>



void traiterClient(int ClntSocket){


	int iqtFound=0;
	obj objBuffer;
	int receivedDataSize;
	if(receivedDataSize=recv(ClntSocket,objBuffer,sizeof(obj),0) < 0)
	{
		perror("Server - Erreur reception de message");
		exit(-1);
	} else 
	{
		while(receivedDataSize > 0){

			//Afficher message		
			//Regarder si -1;
			//refaire un received comme dans le poly ?
			
		}
	}


	if()
}
/*boucle : lire data client sur socket
		jusqu'à objet contenant "fin"*/

}



int main(int argc, char* argv[]){

	pid_t pid;
	int ServSocket;
	int ClntSocket;
	struct sockaddr_in echoServAddr; /*Adresse locale*/ //NB in = internet pas in/out
	struct sockaddr_in echoClntAddr; /*Adresse client*/
	int clntLen; /*Taille de la strucutre de donnees adresse client*/
	int portNumber;


	//Verification de la syntaxe d'appel
	if(argc < 2)
	{
		fprintf(stderr,"Usage : %s <port number>\n",argv[0]);
		exit(-1);
	}

	
	//Récuperation du numero de port
	portNumber=atoi(argv[1]);

	//creation de la socket
	ServSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	//PF_INET = domaine de la socket = internet
	//SOCK_STREAM = Mode connecté = TCP
	

	//Traitement des erreur
	if (ServSocket < 0){

		perror("Serveur : Erreur pendant la creation de la Socket");
		exit(-1);
	}


	/*Configuration de l'adresse locale*/
	//utiliser bzero
	memset(&echoServAddr,0,sizeof(echoServAddr)); /*mise à zero de la structure*/

	echoServAddr.sin_family = AF_INET; /*internet address family*/ //= domaine d'écoute
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /*n'importe quelle interface entrante*/ // = Sur quelle carte réseau j'écoute ? //hton1 = conversion little endian big endian h(host) to nl ( network long) 
	echoServAddr.sin_port = htons(portNumber); /*Port local*/ 
	 //htons = conversion little endian big endian h(host) to ns ( network short)

	//LE RESEAU EST BIG ENDIAN
	//INTEL EST LITTLE ENDIAN 

	//ARM = Advanced RISC Machine -> Reduced Instruction-Set Computer > Eficient d'un pt de vue energétique

	/*Bind a l'adresse locale*/

	if(bind(ServSocket,(struct sockaddr *) &echoServAddr,sizeof(echoServAddr)) < 0)
	{
		perror("Serveur : Erreur pendant le reliage de la Socket");
		exit(-1);
	}

	/*Mise en ecoute*/

	if(listen(ServSocket,SOMAXCONN) < 0){ //SOMAXCONN = Maximum des capacités de la socket

		perror("Serveur : Erreur lors de la mise en ecoute");
		exit(-1);
	}

	while(1){ //Boucle infinie

	/*Definir la taille du parametre d'entree-sortie*/
		clntLen = sizeof(echoClntAddr);

		if(ClntSocket=accept(ServSocket,(struct sockaddr *) &echoClntAddr,&clntLen) < 0){ //accept est bloquant

		perror("Serveur : Erreur pendant l'acceptation d'un client");
		exit(-1);

		}


		//Traitement du client
		pid=fork();

		if(pid < 0) /*erreur*/
		{
			perror("Serveur - Erreur creation du processus fils");
			exit(-1);

		}else if(pid == 0) /*fils*/
		{
		
		traiterClient();
 		/*
		traiterclient()
		

		}else /*pere*/
		{
		/*
		waitpid() attends fin du fils
		si statut fin fils = arrêt : fin serveur au bout de trois clients*/
		}
	}	

return 0;

}
/*

initialise le socket TCP
boucle :
attente des connexions clients


*/



// boucler sur send tant que pas tt envoyer
// boucler sur receive tant que pas tt envoyer
// si erreur, le signifier et sortir
//Pour eviter la confusion des serveurs avec les autres groupes de TD, on choisira un numéro de port 10 000 + n° compte SR03 + n° debug
//quand debug plante, il faut fermer le serveur sinon erreur addresse déja utilisée, ajouter 100 au premier débug, 200 au deuxiem etc.. puis apres un certain temps, retourner a 0
//le numéro de port sera passé en parametre
// cat /etc/services <- liste des ports conventionée

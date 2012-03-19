#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h> //pour waitpid



#include "defobj.h"


#include <sys/socket.h>


void handlerTerminaison(int sig);


const int MAX_CLIENTS_A_TRAITER=3;
int nbClientsTraites = 0;
int nbClientsConnectes = 0;




struct sigaction act = {handlerTerminaison,0,0};


void handlerTerminaison(int sig){


	int status;
			
			waitpid(-1,&status,WNOHANG); 
			 if (WIFEXITED(status))
                {
            
                    fprintf(stderr,"Serveur - Un traitement vient de se terminer avec le statut %d.\n", WEXITSTATUS(status));
                    nbClientsTraites++;
                    nbClientsConnectes--; 
                    fflush(stderr);
            
                    //Sinon indiqué qu'il ne s'est pas terminé correctement.    
                } else {
            
                    fprintf(stderr,"Serveur - Un traitement vint de se terminer de facon inatendue avec le status.\n", WEXITSTATUS(status));
                    fflush(stderr);
            
                }


}








void traiterClient(int ClntSocket){


	int iqtFound=0;
	obj objBuffer;
	int receivedDataSize;
	printf("Serveur - En attente de reception de donnees...\n");
	sleep(2);
	if((receivedDataSize=recv(ClntSocket,&objBuffer,sizeof(obj),0)) < 0)
	{
		perror("Serveur : Erreur pendant la reception des donnees");
		exit(1);
	} else 
	{
		while(receivedDataSize > 0){

			printf("Objet recu :\n");
			printf("	%s\n",objBuffer.section1);
			printf("	%s\n",objBuffer.section2);
			printf("	%d\n",objBuffer.ii);
			printf("	%d\n",objBuffer.jj);
			printf("	%f\n\n",objBuffer.dd);
			
			receivedDataSize=recv(ClntSocket,&objBuffer,sizeof(obj),0);
			
		}
	}


	
}
/*boucle : lire data client sur socket
		jusqu'à objet contenant "fin"*/





int main(int argc, char* argv[]){

	pid_t pid;
	int ServSocket;
	int ClntSocket;

	struct sockaddr_in echoServAddr; /*Adresse locale*/ //NB in = internet pas in/out
	struct sockaddr_in echoClntAddr; /*Adresse client*/
	int clntLen; /*Taille de la strucutre de donnees adresse client*/
	int portNumber;

	sigset_t set, oset;

	sigemptyset(&set);


    



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

	while(nbClientsTraites < MAX_CLIENTS_A_TRAITER){ //Boucle infinie

	/*Definir la taille du parametre d'entree-sortie*/
		clntLen = sizeof(echoClntAddr);

		printf("Serveur - En attente de connection client...\n");
		
		//bloquer SIGCHILD
		sigaddset(&set, SIGCHLD);
    	sigprocmask(SIG_BLOCK, &set, &oset);
		
		if((ClntSocket=accept(ServSocket,(struct sockaddr *) &echoClntAddr,&clntLen)) < 0){ //accept est bloquant

		perror("Serveur : Erreur pendant l'acceptation d'un client");
		exit(-1);

		}
		
		//debloquer SIGCHLD
		sigdelset(&oset, SIGCHLD);
    	sigprocmask(SIG_SETMASK, &oset, NULL);
		
		
		printf("Serveur - Client connecte : %d.\n",ClntSocket);
		nbClientsConnectes++;

		//Traitement du client
		pid=fork();

		if(pid < 0) /*erreur*/
		{
			perror("Serveur : Erreur creation du processus fils");
			exit(-1);

		}else if(pid == 0) /*fils*/
		{
			printf("Serveur - Traitement (%d) de la requete en cours..\n",pid);
			traiterClient(ClntSocket);
 			exit(0);
		

		}else /*pere*/
		{
			
			sigaction(SIGCHLD,&act,0);
			
			
		}
	}	
printf("Serveur - %d clients traites, fermeture.\n\n",MAX_CLIENTS_A_TRAITER);
close(ClntSocket);
close(ServSocket);
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

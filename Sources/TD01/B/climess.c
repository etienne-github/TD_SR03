#include "MsgDef.h"
#include "signal.h"


/*****************************************************

		PROTOTYPES

******************************************************/


//Routine d'interruption
void handlerInteruption();

//Attendre entrée pour continuer
void PressEnterToContinue(void);

//Connexion
int login(int* adresse, int* id_file, int* id_client);

//Deconnexion
void logout();


/*****************************************************

		DECLARATION DE VARIABLES

******************************************************/



/*----------------STRUCTURE SIGACTION----------------------*/


struct sigaction act = {handlerInteruption,0,0,0};


/*----------------IDENTIFIANT CLIENT----------------------*/

int id=0;

/*----------------FILE DE MESSAGES----------------------*/
int id_file=-1;


/*****************************************************

		DECLARATION DE FONCTIONS

******************************************************/



/*----------------ROUTINE D'INTERRUPTION----------------------*/

void handlerInteruption()
{
	msgbuf msgBufferSnd;

	//Deconnexion
	//Création d'un message pour demander la deconnexion
	msgBufferSnd.mtype = 1; //ADRESSE SERVEUR
	msgBufferSnd.qType = typ_dem_deco;
	msgBufferSnd.id = id;

	//Envoi du message demandant la deconnexion
	msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);

	fprintf(stderr,"@%d - Signal d'interruption reçu ",id);
	exit(-1);

}

/*----------------ATTENDRE AVANT DE CONTINUER----------------------*/

void PressEnterToContinue(void)
{ 
	char* C=(char*)malloc(100*sizeof(char));
    if(C!=NULL)
    {
        printf("Entrez n'importe quel caractère pour continuer....\n");
        scanf("%s",C);
    }
}

/*----------------CONNEXION----------------------*/


int login(int* adresse, int* id_file, int* id_client)
{
	//Création d'un message pour une demande d'identification
	msgbuf loginMsg;
	msgbuf msgBufferRcv;
	loginMsg.mtype = 1; //ADRESSE SERVEUR
	loginMsg.qType = typ_dem_num;
	loginMsg.id=-1;

	//Envoi du message demandant l'identification
	msgsnd(*id_file,&loginMsg,sizeof(msgbuf)-sizeof(long),0); 
	printf("NouveauClient - Envoi d'une requete [Connexion] à @1(Serveur) %ld.\n",(long)loginMsg.mtype);

	//Attente de réception d'un id
	msgrcv(*id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),ADR_COMMUNE,0);

	//Définition de l'id envoyé par le serveur
	*adresse = msgBufferRcv.id;
	
	if(*adresse <= 0){
		return -1;
	}
	else{

	return 0;
	}	
	
}

/*----------------DECONNEXION----------------------*/

void logout(){


}


/*****************************************************

		DEBUT DU PROGRAMME

******************************************************/




int main(){
	int id=ADR_COMMUNE; //Initialiser l'adresse du client à l'adresse de recetin commune
	int i;
	int choix_menu;
	msgbuf msgBufferRcv;
	msgbuf msgBufferSnd;

	//Création d'une clé
	key_t cle;
	cle= ftok("./mon_login_sr03",0);
	if (cle == -1)
	{
		perror("NouveauClient - Erreur création clé");
		return -1;
	}
	
	//Récupération de la file de messages
	id_file= msgget(cle,0); //Flag = 0 -> Si pas de file, notifier et quitter
	if (id_file == -1)
	{
		perror("NouveauClient - Erreur création de file");
		return -1;
	}
	if(login(&id,&id_file,&id)!=0){
		printf("NouveauClient - Connexion refusée.\n");
		return 0;
		}
	else
	{

		printf("NouveauClient - Identification réussie, (ID : %d).\n\n",id);

		//Traitement des signaux d'interruption
		sigaction(SIGINT,&act,0);
		sigaction(SIGQUIT,&act,0);
		sigaction(SIGKILL,&act,0);
	}
	while (1)
	{
		printf("****************************************************************\n");
		printf("		            	MENU PRINCIPAL                          \n");
		printf("****************************************************************\n");
		printf("1 - Afficher la liste des objets disponibles\n");
		printf("2 - Afficher le stock d'un type d'objet\n");
		printf("3 - Afficher le prix d'un type d'objet\n");
		printf("4 - Se déconnecter\n");
		printf("****************************************************************\n");
		scanf("%d",&choix_menu);
		system("clear");
		switch (choix_menu){
	
		case 1 :
			//Création d'un message pour une demande de la liste d'objets'
			msgBufferSnd.mtype = 1; //ADRESSE SERVEUR
			msgBufferSnd.qType = typ_dem_list;
			msgBufferSnd.id= id;

			//Envoi du message demandant la liste
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
			
			//Attente d'une réponse
			printf("@%d - Attente d'une réponse. \n\n",id);
			msgrcv(id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),id,0);
			printf("***********************Liste des objets ************************\n");	
			for (i=0; i < NB_MAX_TYP_OBJ; i++)
			{
				printf ("%d - %s \n",i+1,msgBufferRcv.nom[i]);
			}		
			printf("****************************************************************\n");
			PressEnterToContinue();
			system("clear");
			break;
			
		case 2 :
			printf("****************************************************************\n");
			printf("Entrez le numéro de l'objet dont vous souhaitez consulter le stock:\n");
			printf("****************************************************************\n");
			scanf("%d",&choix_menu);
			system("clear");
			//Création d'un message pour demander le stock d'un type d'objet
			msgBufferSnd.mtype = 1; //ADRESSE SERVEUR
			msgBufferSnd.qType = typ_dem_obj_stock;
			msgBufferSnd.id = id;
			msgBufferSnd.stock = choix_menu - 1;  //numéro de l'objet dont on souhaite consulter le stock 

			//Envoi du message demandant le stock d'un objet
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
			
			//Attente d'une réponse
			printf("@%d - Attente d'une réponse..\n\n",id);
			msgrcv(id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),id,0);
			printf("***********************Stock de l'objet %d***********************\n",choix_menu);	
			printf ("%d\n",msgBufferRcv.stock);		
			printf("****************************************************************\n");
			PressEnterToContinue();
			system("clear");
			break;
		case 3 :
			printf("****************************************************************\n");
			printf("Entrez le numéro de l'objet dont vous souhaitez consulter le prix:\n");
			printf("****************************************************************\n");
			scanf("%d",&choix_menu);
			system("clear");
			//Création d'un message pour demander le prix d'un type d'objet
			msgBufferSnd.mtype = 1; //ADRESSE SERVEUR
			msgBufferSnd.qType = typ_dem_obj_prix;
			msgBufferSnd.id = id;
			msgBufferSnd.stock = choix_menu - 1;  //numéro de l'objet dont on souhaite consulter le prix 

			//Envoi du message demandant le prix d'un objet
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
			
			//Attente d'une réponse
			printf("@%d - Attente d'une réponse.. \n\n",id);
			msgrcv(id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),id,0);
			printf("***********************Prix de l'objet %d************************\n",choix_menu);	
			printf ("%.2f euros.\n",msgBufferRcv.prix);		
			printf("****************************************************************\n");
			PressEnterToContinue();
			system("clear");
			break;
		case 4 :
			//Création d'un message pour demander la deconnexion
			msgBufferSnd.mtype = 1; //ADRESSE SERVEUR
			msgBufferSnd.qType = typ_dem_deco;
			msgBufferSnd.id = id;

			//Envoi du message demandant la deconnexion
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
			
			//Attente d'une réponse
			printf("@%d - Attente d'une réponse.. \n",id);
			msgrcv(id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),id,0);
			if (msgBufferRcv.qType == typ_info_deco)
			{
			printf("***********************Déconnexion************************\n");
			return 0;					
			}
			else
			{
			printf("***********************Echec déconnexion************************\n");
			}
	
		};	
	}
	return 0;

}


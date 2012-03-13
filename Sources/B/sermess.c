#include "MsgDef.h"
#include "iniobj.h"
#include "signal.h"



/*****************************************************

		PROTOTYPES

******************************************************/

//Routine d'interruption
void handlerInteruption();



//Traitement des requêtes
int traitementDemandeConnexion(int* nbClientServis,int* nbClientActuels, msgbuf* msgBufferSnd);

int traitementAffichageListeObjet(msgbuf msgBufferRcv,msgbuf* msgBufferSnd,fruit *table);

int traitementDemandeStock(msgbuf msgBufferRcv,msgbuf* msgBufferSnd,fruit *table);

int traitementDemandePrix(msgbuf msgBufferRcv,msgbuf* msgBufferSnd,fruit *table);

int traitementDemandeDeconnexion(int* nbClientActuels,msgbuf msgBufferRcv, msgbuf* msgBufferSnd);



/*****************************************************

		DECLARATION DE VARIABLES

******************************************************/



/*----------------STRUCTURE SIGACTION----------------------*/


struct sigaction act = {handlerInteruption,0,0,0};


/*----------------FILE DE MESSAGES----------------------*/
int id_file=-1;



/*****************************************************

		DECLARATION DE FONCTIONS

******************************************************/



/*----------------ROUTINE D'INTERRUPTION----------------------*/

void handlerInteruption()
{
	//fermeture de la file des messages
	while(!msgctl ( id_file, IPC_RMID,0)){}
	perror("@1 (Serveur) - Signal d'interruption reçu ");
	exit(-1);

}



/*----------------TRAITEMENT DE REQUETES----------------------*/


int traitementDemandeConnexion(int* nbClientServis,int* nbClientActuels, msgbuf* msgBufferSnd){

			if ((*nbClientActuels < NB_MAX_CLT_SIM)&&(*nbClientServis<NB_MAX_CLT))
			{
				(*nbClientServis)++;
				(*nbClientActuels)++;

				msgBufferSnd->mtype=ADR_COMMUNE; //adresse de la boite de réception commune
				msgBufferSnd->id=1+*nbClientServis;
				msgBufferSnd->qType=typ_new_num;
	
				return 0;
			}
			else
			{
				//Le client est rejeté	
				msgBufferSnd->mtype=ADR_COMMUNE; //adresse d'un client non-identifié
				msgBufferSnd->id=-2;
				msgBufferSnd->qType=typ_new_num;

				return -1;
			}
}

int traitementAffichageListeObjet(msgbuf msgBufferRcv,msgbuf* msgBufferSnd,fruit *table)
{
	int i;
	//création du message
	msgBufferSnd->mtype=msgBufferRcv.id; //adresse du client demandeur
	msgBufferSnd->qType=typ_list_obj; //message de type list_obj
		
	//Generation du message
	for (i=0; i < NB_MAX_TYP_OBJ; i++)
	{
		strcpy(msgBufferSnd->nom[i], table[i].nom);
	}
	return 0;
}


int traitementDemandeStock(msgbuf msgBufferRcv,msgbuf* msgBufferSnd,fruit *table)
{
		msgBufferSnd->mtype=msgBufferRcv.id; //adresse du client demandeur
		msgBufferSnd->qType=typ_etat_obj_stock;
		msgBufferSnd->stock=table[msgBufferRcv.stock].stock;
		return 0;
}


int traitementDemandePrix(msgbuf msgBufferRcv,msgbuf* msgBufferSnd,fruit *table)
{
		msgBufferSnd->mtype=msgBufferRcv.id; //adresse du client demandeur
		msgBufferSnd->qType=typ_etat_obj_prix;
		msgBufferSnd->prix=table[msgBufferRcv.stock].prix;

		return 0;
}


int traitementDemandeDeconnexion(int* nbClientActuels,msgbuf msgBufferRcv, msgbuf* msgBufferSnd)
{
		msgBufferSnd->mtype=msgBufferRcv.id; //adresse du client demandeur
		msgBufferSnd->qType=typ_info_deco;
		
		
		(*nbClientActuels)--;

		return 0;
}




/*****************************************************

		DEBUT DU PROGRAMME

******************************************************/



/*----------------STRUCTURE SIGACTION----------------------*/


int main(){
	
	
	
	//Déclaration des variables locales
	int nbClientServis=0;
	int nbClientActuels=0;

	msgbuf msgBufferRcv;
	msgbuf msgBufferSnd;
	fruit *table = init_obj();//initialisation de la table

	if(!table)
	{
		printf("@1 (Serveur) - Erreur initialisation table d'objets.");
	}

	//Début de la fonction 


	//Création d'une clé
	key_t cle;
	cle= ftok("./mon_login_sr03",0);
	if (cle == -1)
	{
		perror("@1 (Serveur) - Erreur création clé");
		return -1;
	}

	//Création de la file de messages
	id_file= msgget(cle,IPC_CREAT|IPC_EXCL|0666);
	if (id_file == -1)
	{
		perror("@1 (Serveur) - Erreur creation de file");
		return -1;
	}

	//Traitement des signaux d'interruption
	sigaction(SIGINT,&act,0);
	sigaction(SIGQUIT,&act,0);
	sigaction(SIGKILL,&act,0);
	
	



	//Tant que l'on a pas traité le nombre de client à traiter ou qu'il reste des clients connectés
	while((nbClientServis<NB_MAX_CLT) || (nbClientActuels > 0)){


	//Reception des requetes
	printf("@%d (Serveur) - Attente d'une requete..\n",1);	
	msgrcv(id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),1,0); 
	

	//Traitement des requetes
	switch(msgBufferRcv.qType)
	{
		//traitement d'une demande de connexion
		case typ_dem_num :

			printf("@1 (Serveur) - Requête [Demande de Connexion] reçue.\n");
			//il est possible de traiter la demande
			if (traitementDemandeConnexion(&nbClientServis,&nbClientActuels, &msgBufferSnd)==0) 	
			{		
				msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
				printf("@1 (Serveur) - Envoi d'un nouvel ID (%d) à l'adresse @6(Commun)\n",msgBufferSnd.id);
			} else {
				msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
				printf("@1 (Serveur) - Envoi une notification de refus de connexion à l'adresse @6(Commun)\n",msgBufferSnd.id);
			
			}
			break;

		//demande d'affichage de la liste d'objets
		case typ_dem_list :
			printf("@1 (Serveur) - Requête [Liste d'Objet] reçue de @%d.\n",msgBufferSnd.id);
			traitementAffichageListeObjet(msgBufferRcv,&msgBufferSnd,table);
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
			printf("@1 (Serveur) - Envoi de la liste des objets à l'adresse @%d \n",msgBufferSnd.id);
		
			break;

		//demande d'affichage du stock d'un objet
		case typ_dem_obj_stock :
			printf("@1 (Serveur) - Requête [Stock d'un Objet] reçue de @%d.\n",msgBufferSnd.id);
			traitementDemandeStock(msgBufferRcv,&msgBufferSnd,table);
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
			printf("@1 (Serveur) - Envoi du stock à l'adresse @%d \n",msgBufferSnd.id);

			break;

		//demande d'affichage du prix d'un objet
		case typ_dem_obj_prix :

			printf("@1 (Serveur) - Requête [Prix d'un Objet] reçue de @%d.\n",msgBufferSnd.id);
			traitementDemandePrix(msgBufferRcv,&msgBufferSnd,table);
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
			printf("@1 (Serveur) - Envoi du prix à l'adresse @%d \n",msgBufferSnd.id);	

			break;

		//traitement d'une demande de déconnexion
		case typ_dem_deco :
			printf("@1 (Serveur) - Requête [Demande de Déconnexion] reçue de @%d.\n",msgBufferSnd.id);
			traitementDemandeDeconnexion(&nbClientActuels,msgBufferRcv,&msgBufferSnd);
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
			printf("@1 (Serveur) - Acceptation de la déconnexion pour le client @%d \n",msgBufferRcv.id);
			break;

		
		default:
			printf("@1 (Serveur) - Requête reçue inconnue.\n");
			break;

		}
	}
	
	
	
	//Fermeture du serveur
	printf("@1 (Serveur) - Les %d clients ont été traités, fermeture.\n",NB_MAX_CLT);

	//Fermeture de la liste de message
	while(!msgctl ( id_file, IPC_RMID,0)){}
	
	return 0;


}


#include "MsgDef.h"



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
	printf("NouveauClient - Envoi d'une requete typ_dem_num à @1(Serveur) %ld\n",(long)loginMsg.mtype);

	//Attente de réception d'un id
	msgrcv(*id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),*id_client,0);

	//Définition de l'id envoyé par le serveur
	*adresse = msgBufferRcv.id;
	
	if(*adresse <= 0){
		return -1;
	}
	else{

	return 0;
	}	
	
}

void logout(){


}

int main(){
	int id=ADR_COMMUNE; //Initialiser l'adresse du client à l'adresse de recetin commune
	int i;
	int choix_menu;
	msgbuf msgBufferRcv;
	msgbuf msgBufferSnd;

	//Création d'une clé
	key_t cle;
	cle= ftok("./SR03P0xxx",0);
	if (cle == -1)
	{
		perror("Erreur création clé");
		return -1;
	}
	
	//Récupération de la file de messages
	int id_file= msgget(cle,0); //Flag = 0 -> Si pas de file, notifier et quitter
	if (id_file == -1)
	{
		perror("NouveauClient - Erreur creation de file\n");
		return -1;
	}
	if(login(&id,&id_file,&id)!=0){
		printf("Connexion refusée\n");
		return 0;
		}
	else
	{
		printf("@%d (ex NouveauClient) - Identification réussie\n",id);
	}
	while (1)
	{
		printf("****************************************************************\n");
		printf("1 - Afficher la liste des objets disponibles\n");
		printf("2 - Afficher le stock d'un type d'objet\n");
		printf("3 - Afficher le prix d'un type d'objet\n");
		printf("4 - Se déconnecter\n");
		printf("****************************************************************\n");
		scanf("%d",&choix_menu);
	
		switch (choix_menu){
	
		case 1 :
			//Création d'un message pour une demande de la liste d'objets'
			msgBufferSnd.mtype = 1; //ADRESSE SERVEUR
			msgBufferSnd.qType = typ_dem_list;
			msgBufferSnd.id= id;

			//Envoi du message demandant la liste
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
			
			//Attente d'une réponse
			printf("Attente d'une réponse, mon id est %d \n",id);
			msgrcv(id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),id,0);
			printf("***********************Liste des objets ************************\n");	
			for (i=0; i < NB_MAX_TYP_OBJ; i++)
			{
				printf ("%d - %s \n",i+1,msgBufferRcv.nom[i]);
			}		
			printf("****************************************************************\n");
			break;
			
		case 2 :
			printf("****************************************************************\n");
			printf("Entrez le numéro de l'objet dont vous souhaitez consulter le stock\n");
			printf("****************************************************************\n");
			scanf("%d",&choix_menu);
			//Création d'un message pour demander le stock d'un type d'objet
			msgBufferSnd.mtype = 1; //ADRESSE SERVEUR
			msgBufferSnd.qType = typ_dem_obj_stock;
			msgBufferSnd.id = id;
			msgBufferSnd.stock = choix_menu - 1;  //numéro de l'objet dont on souhaite consulter le stock 

			//Envoi du message demandant le stock d'un objet
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
			
			//Attente d'une réponse
			printf("Attente d'une réponse, mon id est %d \n",id);
			msgrcv(id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),id,0);
			printf("***********************Stock de l'objet %d************************\n",choix_menu);	
			printf ("%d\n",msgBufferRcv.stock);		
			printf("****************************************************************\n");
			break;
		case 3 :
			printf("****************************************************************\n");
			printf("Entrez le numéro de l'objet dont vous souhaitez consulter le prix\n");
			printf("****************************************************************\n");
			scanf("%d",&choix_menu);
			//Création d'un message pour demander le prix d'un type d'objet
			msgBufferSnd.mtype = 1; //ADRESSE SERVEUR
			msgBufferSnd.qType = typ_dem_obj_prix;
			msgBufferSnd.id = id;
			msgBufferSnd.stock = choix_menu - 1;  //numéro de l'objet dont on souhaite consulter le prix 

			//Envoi du message demandant le prix d'un objet
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
			
			//Attente d'une réponse
			printf("Attente d'une réponse, mon id est %d \n",id);
			msgrcv(id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),id,0);
			printf("***********************Prix de l'objet %d************************\n",choix_menu);	
			printf ("%f euros\n",msgBufferRcv.prix);		
			printf("****************************************************************\n");
			break;
		case 4 :
			//Création d'un message pour demander la deconnexion
			msgBufferSnd.mtype = 1; //ADRESSE SERVEUR
			msgBufferSnd.qType = typ_dem_deco;
			msgBufferSnd.id = id;

			//Envoi du message demandant la deconnexion
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
			
			//Attente d'une réponse
			printf("Attente d'une réponse, mon id est %d \n",id);
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

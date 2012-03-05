#include "MsgDef.h"
#include "iniobj.h"



//Struct Fruit {

//Nom
//Stock
//Prix
//}





//Dans client faire un menu pour faire les requetes sur les objets
//ipcsrm -q id (-q pour queue)

// 1 - Nouveau Client
// 2 - Requete
// 3 - Deconnexion 


int main(){
	//initialisation de la table
	fruit *table = init_obj();
	
	int i;
	int nbClientServis=0;
	int nbClientActuels=0;
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

	//Création de la file de messages
	int id_file= msgget(cle,IPC_CREAT|IPC_EXCL|0666);
	if (id_file == -1)
	{
		perror("Erreur creation de file\n");
		return -1;
	}

	while(nbClientServis<NB_MAX_CLT || nbClientActuels > 0){
	
	printf("@%d - Attente d'une requete\n",1);	
	msgrcv(id_file,&msgBufferRcv,sizeof(msgbuf)-sizeof(long),1,0); 
	printf("@%d - Requête reçue %ld\n",1,(long)msgBufferRcv.mtype);
	
	//traitement d'une demande de connexion
	if (msgBufferRcv.qType == typ_dem_num)
	{
		//il est possible de traiter la demande
		if (nbClientActuels < NB_MAX_CLT_SIM)
		{
			nbClientServis++;
			nbClientActuels++;
		
			msgBufferSnd.mtype=NB_MAX_CLT + 2; //adresse d'un client non-identifié
			msgBufferSnd.id=1+nbClientServis;
			msgBufferSnd.qType=typ_new_num;
	
	
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
			printf("@1 - Envoi de l'id %d à l'adresse @6(Commun)\n",msgBufferSnd.id);
		}
		else
		{
			//Le client est rejeté	
			msgBufferSnd.mtype=NB_MAX_CLT + 2; //adresse d'un client non-identifié
			msgBufferSnd.id=-2;
			msgBufferSnd.qType=typ_new_num;
			msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
			printf("@1 - Envoi de l'id %d à l'adresse @6(Refus)\n",msgBufferSnd.id);
		}
	}
	//demande d'affichage de la liste d'objets
	if (msgBufferRcv.qType == typ_dem_list)
	{
		//création du message
		msgBufferSnd.mtype=msgBufferRcv.id; //adresse du client demandeur
		msgBufferSnd.qType=typ_list_obj;
		for (i=0; i < NB_MAX_TYP_OBJ; i++)
		{
			strcpy(msgBufferSnd.nom[i], table[i].nom);
			printf("%s \n",msgBufferSnd.nom[i]);
		}
		
		//envoi du message
		msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
		printf("@1 - Envoi de la liste des objets à l'adresse @%d \n",msgBufferSnd.id);
		
		
		
	}
	
	if (msgBufferRcv.qType == typ_dem_obj_stock)
	{
		//création du message
		msgBufferSnd.mtype=msgBufferRcv.id; //adresse du client demandeur
		msgBufferSnd.qType=typ_etat_obj_stock;
		msgBufferSnd.stock=table[msgBufferRcv.stock].stock;
		
		//envoi du message
		msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
		printf("@1 - Envoi du stock à l'adresse @%d \n",msgBufferSnd.id);
		
	}
	
	if (msgBufferRcv.qType == typ_dem_obj_prix)
	{
		//création du message
		msgBufferSnd.mtype=msgBufferRcv.id; //adresse du client demandeur
		msgBufferSnd.qType=typ_etat_obj_prix;
		msgBufferSnd.prix=table[msgBufferRcv.stock].prix;
		
		//envoi du message
		msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0); 
		printf("@1 - Envoi du prix à l'adresse @%d \n",msgBufferSnd.id);		
	}	
	
	if (msgBufferRcv.qType == typ_dem_deco)
	{
		//création du message
		msgBufferSnd.mtype=msgBufferRcv.id; //adresse du client demandeur
		msgBufferSnd.qType=typ_info_deco;
		
		//envoi du message
		msgsnd(id_file,&msgBufferSnd,sizeof(msgbuf)-sizeof(long),0);
		nbClientActuels--;
		printf("@1 - Acceptation de la déconnexion pour le client @%d \n",msgBufferRcv.id);		
	}			
	
	}
	
	printf("Les %d clients ont été traités, fermeture.\n",NB_MAX_CLT);
	return 0;


}

#include <stdio.h>
#include <sys/types.h> //pour pid_t 
#include <stdlib.h> //pour exit()
#include <errno.h> //pour afficher les messages d'erreur
#include <string.h> //pour strerror
#include <sys/wait.h> //pour waitpid
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define NB_MAX_TYP_OBJ 3 //Nombre max de type d'objet
#define LONGUEUR_TEXTE 25 //Longueur du texte dans un message
const int NB_MAX_CLT_SIM = 2; //Nombre maximum de client simultanés
const int NB_MAX_CLT = 4; //Nombre max de client à traiter avant fermeture
const int ADR_COMMUNE = NB_MAX_CLT + 2; //Adresse de reception commune pour les nouveaux clients


typedef enum qType_t {
	typ_dem_num, //Demande d'identification
	typ_new_num, //Envoie d'un identifiant
	typ_dem_list, //Demande de la liste des objets
	typ_list_obj, //Envoi de la liste des objets
	typ_dem_obj_stock, //Demande le stock d'un objet
	typ_etat_obj_stock, //Envoie le stock d'un objet
	typ_dem_obj_prix, //Demande le prix d'un objet
	typ_etat_obj_prix, //Envoi du prix d'un objet
	typ_dem_deco, //Demande de déconnexion
	typ_info_deco //Envoi du resultat de la deconnexion

} qType_t;


typedef struct msgbuf {
   	  long mtype;	
	  int id;
	  qType_t qType;
	  char nom[NB_MAX_TYP_OBJ][LONGUEUR_TEXTE];
	  int stock;
	  float prix;
} msgbuf;



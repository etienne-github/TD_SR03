#include <stdio.h>
#include <sys/types.h> //pour pid_t 
#include <stdlib.h> //pour exit()
#include <errno.h> //pour afficher les messages d'erreur
#include <string.h> //pour strerror
#include <sys/wait.h> //pour waitpid
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define NB_MAX_TYP_OBJ 3
#define LONGUEUR_TEXTE 25
const int NB_MAX_CLT_SIM = 2;
const int NB_MAX_CLT = 4;


typedef enum qType_t {
	typ_dem_num,
	typ_new_num,
	typ_dem_list,
	typ_list_obj,
	typ_dem_obj_stock,
	typ_etat_obj_stock,
	typ_dem_obj_prix,
	typ_etat_obj_prix,
	typ_dem_deco,
	typ_info_deco

} qType_t;


typedef struct msgbuf {
   	  long mtype;	
	  int id;
	  qType_t qType;
	  char nom[NB_MAX_TYP_OBJ][LONGUEUR_TEXTE];
	  int stock;
	  float prix;
} msgbuf;



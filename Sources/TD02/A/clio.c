#include "defobj.h"
#include "iniobj.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>







int main(void){

	obj * tableObj = initTable();
	
	if (!tableObj){
		
		fprintf(stderr,"Client - Erreur dans l'initialisation de la table d'objet.\n");
		exit(-1);
	
	}
	
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
	
	


return 0;
}
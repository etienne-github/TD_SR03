#include <stdio.h>
#include <stdlib.h>

#include "defobj.h"


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



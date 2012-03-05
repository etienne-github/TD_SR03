

typedef struct fruit{
	char* nom;
	int stock;
	float prix;
	}fruit;
	
fruit* init_obj()
{
	//création de la table interne d'objets
	fruit *tab=(fruit *)malloc(NB_MAX_TYP_OBJ*sizeof(fruit));
	if (tab == NULL )
	{
		printf ("Erreur d'allocation mémoire");
		return -1;
	}
	
	//Remplissage de la table
	tab[0].nom = "Pomme";
	tab[0].stock = 10;
	tab[0].prix = 0.95;
	
	tab[1].nom = "Poire";
	tab[1].stock = 50;
	tab[1].prix = 1.25;
	
	tab[2].nom = "Abricot";
	tab[2].stock = 25;
	tab[2].prix = 2.0;	
	
	return tab;
}

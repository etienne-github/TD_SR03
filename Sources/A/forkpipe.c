/*

 Ecrire un programme qui cr�e DEUX sous-process communiquant par un pipe :
        +------+
        | p�re |
        +------+
        /    \
 +-----+      +-----+
 |fils1|======|fils2|
 +-----+ pipe +-----+
 
 Le premier sous-process (fils1) �crit une suite de cha�nes de caract�res dans le pipe. Il trouve cette suite de caract�res dans un fichier input.txt.
 
 Celles-ci sont lues par l'autre sous-process (fils2) et imprim�es sur stdout.
 
 Nota: le buffer de lecture du sous-process fils2 sera de longueur 20.
 
 "fils1" va envoyer sur le pipe des "messages" format�s ainsi :
 
 [009][input.txt]
 [nnn][ligne de nnn caract�res]
 ...
 "fils2" va lire le pipe, reconstituer les "messages" et reproduire sur stdout la suite des messages encadr�s d'un marqueur :
 
 re�u>>>input.txt<<<
 re�u>>>[Fichier d'entr�e du programme TD0 forkpipe.c].<<<
 re�u>>><<<
 ...
 Ainsi, le premier fils envoie PLUSIEURS messages, tous de longueur INCONNUE du process lecteur, et le lecteur doit les reconstituer (s�parer le premier message du deuxi�me, etc.).
 
 Inventez et implantez une solution permettant cette reconstitution des messages.
 
 G�rez les codes retour de lecture sur le pipe pour programmer correctement la terminaison du deuxi�me process.
 
 Discussion sur le probl�me des messages dans un flux
*/

#include <stdio.h>
#include <sys/types.h> //pour pid_t 
#include <stdlib.h> //pour exit()
#include <errno.h> //pour afficher les messages d'erreur
#include <string.h> //pour strerror
#include <sys/wait.h> //pour waitpid



//A definir si l'on souhaite afficher stdout et stderr sur deux terminaux diff�rents (Terminaux � ouvrir manuellement)
//define SEPARATE_STDERR


/*-------------------------------------------------------
                    DECLARATIONS DE TYPES
 -------------------------------------------------------*/

typedef enum boolean{ //Sert dans la fonction reader(), pour la decapsulation; 
    FALSE = 0U,
    TRUE = 1U
        
} boolean;


/*-------------------------------------------------------
                DECLARATIONS DE VARIABLES
 -------------------------------------------------------*/

//Descripteur pour le pipe
int fd[2];


/*-------------------------------------------------------
                PROTOTYPES DE FONCTIONS                                     <--- A BOUGER DANS UN HEADER
 -------------------------------------------------------*/

char* Encapsulation(char* String,int nbDeChar,char* dest);
int writer(),reader();




/*-------------------------------------------------------
                DEFINITION DE FONCTIONS
 -------------------------------------------------------*/


/***************************
Fils 1 : Ecrire dans le pipe
****************************/

int writer(){ //Encapsuler et envoyer des lignes de fichier txt � travers le pipe
    
    //Ouvrir le fichier 
    FILE* inputFile;
    inputFile = fopen("./input.txt","r");
    if(inputFile==NULL){
        fprintf(stderr,"FILS1 >> Erreur ouverture fichier.\n");
        return -1;
    }
    
    //Variables utiles � la lecture du fichier
    char *buffer=(char *)malloc(100*sizeof(char));
    if(buffer==NULL){
        fprintf(stderr,"FILS1 >> Erreur Allocation m�moire.\n");
        return -1;
    }
    char tempChar;
    int length;
    
    
    //Initialisation de la lecture
    length=0;
    tempChar='0';
    
    //Lire caract�re par caract�re jusqu'� la fin du fichier
    while(fscanf(inputFile,"%c",&tempChar)!=EOF)
    {
        //Si le carac�te n'est pas un saut de ligne, le concatener au buffer
        if(tempChar!='\n'){
            buffer[length]=tempChar;
            length++;
            
        //Sinon encapsuler le buffer et l'envoyer dans le pipe
        } else {
            
            //Encapsulation du buffer
                //variable temporaire
            char * encMsg=(char*)malloc(107*sizeof(char));
            if(encMsg==NULL){
                fprintf(stderr,"FILS1 >> Erreur Allocation memoire.\n");
                return -1;
            }
                //Encapsulation
            encMsg = Encapsulation((char*)buffer,length,(char*)encMsg);
            
                //Si l'encapsulation � r�ussie, l'envoyer � travers le pipe
            if (encMsg!=NULL){
               // fprintf(stderr,"envoie >> %s <<\n",encMsg);
                write(fd[1],encMsg,107);
            } else {
                //Sinon le signifier
                fprintf(stderr,"FILS1 >> Erreur dans l'encapsulation.\n");
                fflush(stderr);
            }
            
            //Preparation de la lecture de la ligne suivante
            buffer[length]='\0';
            length=0;
            
        }

    }
    
    //Fermeture du fichier
    int cl = fclose(inputFile);
    
    if(cl==EOF){
        fprintf(stderr,"FILS1 >> Erreur fermeture fichier.\n");
        return -1;
    }
    
    return 0;
}


/**************
 Encapsulation
***************/

char* Encapsulation(char* String,int nbDeChar,char* dest){  //Message_de_longueur_XXX -> [XXX][Message_de_longueur_XXX]
    
    //Variables temporaires
    char tempString[nbDeChar+7];
    
    //Ouvrir la partie longueur de la trame avec [
    tempString[0]='[';
    
    
    //Si la taille de la chaine � encapsuler n'est pas valide
    if((nbDeChar < 0)||(nbDeChar > 999)){
        
        return NULL; //retourne un ptd null.
    
    } else {
        
        //Sinon convertir son nombre de caract�res en chaine
        char tempNb[5];
        

        int sp = sprintf(tempNb,"%d",nbDeChar);
        if(sp<0){

            return NULL; //retourne un ptd null.
            
        }

        

        //Ajouter ce nombre de caract�re � la suite de la trame suivant le format 000
        if (nbDeChar < 10) {
            tempString[1]='0';
            tempString[2]='0';
            tempString[3]=tempNb[0];
        } else  if (nbDeChar < 100){
            tempString[1]='0';
            tempString[2]=tempNb[0];
            tempString[3]=tempNb[1];

        } else {
            tempString[1]=tempNb[0];
            tempString[2]=tempNb[1];
            tempString[3]=tempNb[2];
        }
        
        //Fermer la partir longueur avec ]
        tempString[4]=']';
        
        //Ouvrir la partie donn�es avec [
        tempString[5]='[';
        int i;
        
        //Recopier le message � encapsuler
        for (i=0; i < nbDeChar; i++)
        {
            tempString[6+i]=String[i];
        }
        
        //Fermer la partie longueur de la trame avec ]
        tempString[6+nbDeChar]=']';
        
        //finir la trame avec le caract�re de fin de chaine \0
        tempString[6+nbDeChar+1]='\0';
        
        //Renvoyer le resultat
        strcpy(dest,tempString);

        return dest;

    }
    
}




/**************************
Fils 2 : Lire dans le pipe
***************************/

int reader(){ //On lit dans le pipe � la recherche de trames [XXX][Message_de_longueur_XXX] et on affiche "Message_de_longueur_XXX".
    
    //Variables locales
    
    char buffer[20]; //Lecture du pipe
   
    char nbDeCharBuff[3]; //Determination du nombre de caracteres de la trame
    int lengthNbDeCharBuff=0; //Determination du nombre de caracteres de la trame
    
    int NbDeChar; //Nombre de caract�re de la trame
    
    
    
    boolean beginingTrameFound = FALSE; //Recherche de d�but de trame
    boolean endTrameFound = FALSE; //Recherche de fin de trame
   
    
    //Initialisation
    NbDeChar = 0;
    int nbRead=0;
    

    
    while(read(fd[0],buffer,sizeof(buffer))) //Tant qu'il y a des choses dans le pipe
    {
                
        
        //Variable de parsage
        int index=0;

        
        while((index < 20)&&(!beginingTrameFound)){ //Parser le buffer � la recherche de d�but de trame
                
                //Si trouv�
                if(buffer[index]=='['){
                    
                    //reinitialis� les varaible de determination de longueur de trame
                    nbDeCharBuff[0]='\0';
                    lengthNbDeCharBuff=0;
                    NbDeChar=0;
                    
                    //Mettre flag � trouv�
                    beginingTrameFound=TRUE;
                    
                    //sauter le ] de fin de  section longueur
                    index++; 

                } else { //Sinon continuer � parser
                    index++;
                }
                    
        }

        
        

        //Quand un d�but de tram est trouv�, chercher la fin de la section longueur de trame dans un espace de 3 caract�res
        while ((index < 20)&&(!endTrameFound)&&(beginingTrameFound)){

                //Si pas fin de section trouv�e
                if(buffer[index]!=']'){
                    //et pas 3 caract�re atteints -> concatener
                    if(lengthNbDeCharBuff<3){
                        nbDeCharBuff[lengthNbDeCharBuff]=buffer[index];
                        lengthNbDeCharBuff++;
                        index++; //et continuer � parser
                        
                    //Si 3 caract�re atteint -> recommencer recherche de d�but de trame    
                    } else {
                        beginingTrameFound=FALSE; //remise du flag � non trouv�
                    }
                    
                 //Si fin de section trouv�e  
                } else { 
                    //Mettre le flag � trouv�
                    endTrameFound=TRUE;
  
                    //Convertir le nombre de char en int
                    NbDeChar=atoi(nbDeCharBuff);
                    
                    //Initialiser l'afichage du message
                    printf("FILS2 : recu >>>");
 
                    index++;//sauter le ] de fin de section longueur de trame.
                    index++;//sauter le [ de d�but de  section donn�e
                }
                
        }
        

        //Quand d�but et fin de section longueur de tram trouv�s, afficher le message
        while ((index < 20)&&(endTrameFound)&&(beginingTrameFound)){
            
            //Si il reste encore des char � afficher, les afficher
            if(NbDeChar > 0){
               
                printf("%c",buffer[index]);
                index++;
                NbDeChar--;
                
            } else { //Sinon reinitialiser la recherche de d�but de trame
                
                beginingTrameFound=FALSE;
                endTrameFound=FALSE;
                printf("<<<\n");
                nbDeCharBuff[0]='\0';
                NbDeChar=0;
                lengthNbDeCharBuff=0;
                
            }
            
            
            
        }

    }
    return 0;
}







/*-------------------------------------------------------
                    DEBUT DU PROGRAMME
 -------------------------------------------------------*/




int main(int argc, char* argv[])
{

//Si On ouvre plusieurs terminaux on peut rediriger stderr sur le second terminal pour isoler les messages de "debuggage" de l'affichage des trames

#ifdef SEPARATE_STDERR  
    close(2);
    fopen("/dev/pts/1","w"); //ATTENTION lancer la commande "w" dans le terminal pour connaitre le nom du terminal dans lequel rediriger stderr
#endif

    pid_t pid; 
    if(pipe(fd) != 0)
    {
	perror("Echec creation pipe");
	return 0;
    }
    pid=fork();
    
    if (pid < 0) //Erreur de fork
    {
        fprintf(stderr,"PERE >> Erreur de fork n�1 : %s.\n",strerror(errno));
        fflush(stderr);
        
    } else if (pid == 0) { // Dans le fils 1
        
        //R�cupere son pid et le pid de son pere
        pid_t ppid = getppid();
        pid=getpid();
        
        //Affichage des informations sur les pid
        fprintf(stderr,"FILS1 >> J'ai le pid %d et mon pere est %d.\n",pid,ppid);
        fflush(stderr);
        
        
        //Ferme le pipe en lecture (non utilis�)
        close(fd[0]);
        
        //Ferme le descripteur "out"
        close(1);
        
        //Ouvre le pipe en descripteur "out"
        dup(fd[1]);
        
        //Ecrire les donn�es
        int ret = writer();
        
        //Rebasculer sur stdout
        close(1);
        close(fd[1]);
        fopen("/dev/tty","w");
        
        //Check fin d'�criture des donnees
        if (ret==0){
          fprintf(stderr,"FILS1 >> Fin de l'ecriture des donnees dans le pipe avec succes.\n");
          fprintf(stderr,"FILS1 >> Fin du processus.\n");
          fflush(stderr);
          exit(0);
        } else {
          fprintf(stderr,"FILS1 >> L'ecriture des donnees dans le pipe s'est terminee avec une erreur.\n");
          fprintf(stderr,"FILS1 >> Fin du processus.\n");
          fflush(stderr);
          exit(1);
        }
        
    } else { //Dans le pere
        
         //R�cup�re son pid et le pid de son fils
         pid_t pidFils = pid;
         pid = getpid();
    
        pid_t pid2;
        pid2 = fork();
        
        if(pid2 < 0) //Erreur de fork
        {
            fprintf(stderr,"PERE >> Erreur de fork n�2 : %s.",strerror(errno));
            fflush(stderr);
            
        } else if (pid2 ==0) { //Dans le fils 2
            //R�cup�re son pid et le pid de son pere
            pid_t ppid = getppid();
            pid = getpid();
            
            //Affichage des informations
            fprintf(stderr,"FILS2 >> J'ai le pid %d et mon pere est %d.\n",pid,ppid);
            fflush(stderr);
            
            //Ferme le pipe en �criture (non utilis�)
            close(fd[1]);
            
            //ferme le descripteur "in"
            close(0);
            
            //Ouvre le pipe en descipteur "in"
            dup(fd[0]);
            
            int ret=reader();
            
            //Rebasculer sur le clavier
            close(0);
            close(fd[0]);
            fopen("/dev/stdin","r");
            
            
            //Check fin de la lecture des donn�es
            if (ret==0){
                fprintf(stderr,"FILS2 >> Fin de la lecture des donnees depuis le pipe avec succes.\n");
                fprintf(stderr,"FILS2 >> Fin du processus.\n");
                fflush(stderr);
                exit(0);
            } else {
                fprintf(stderr,"FILS2 >> la lecture des donnees depuis le pipe s'est terminee avec une erreur.\n");
                fprintf(stderr,"FILS2 >> Fin du processus.\n");
                fflush(stderr);
                exit(1);
            }

            
            
            
        } else { //Dans le pere
        
            close(fd[0]);
            close(fd[1]);
            
            //R�cup�re son pid et le pid de son fils
            pid_t pidFils2 = pid2;
            pid = getpid();
            int status=0; //pour r�cuperer le statut de terminaison des fils
            pid_t pid_ended=0; //pour r�cuperer le pid du processus termin�
            char nomFilsEnded[6]; //pour determiner le nom du fils termin�
            
            //Affichage des informations
            fprintf(stderr,"PERE >> Mon pid est %d.\n",pid);
            fprintf(stderr,"PERE >> Mon Fils 1 a le pid %d.\n",pidFils);
            fprintf(stderr,"PERE >> Mon Fils 2 a le pid %d.\n",pidFils2);
            fflush(stderr);

            
            
            //Attendre la fin des fils
            int filsTermine=0;
            while(filsTermine < 2)
            {
                //Attendre n'importe quel fils
                pid_ended = waitpid(-1,&status,WUNTRACED);
        
                
                //Determiner le nom du fils termin�
                if (pid_ended==pidFils){
                    strcpy(nomFilsEnded,"FILS1");
                } else if (pid_ended==pidFils2){
                    strcpy(nomFilsEnded,"FILS2");
                } else {
                    strcpy(nomFilsEnded,"?????");
                }

                
                
                //Si le fils se termine correctement, indiqu� son code de terminaison.
                if (WIFEXITED(status))
                {
            
                    fprintf(stderr,"PERE >> %s (%d) termine avec le statut %d.\n",nomFilsEnded,pid_ended, WEXITSTATUS(status));
                    fflush(stderr);
            
                    //Sinon indiqu� qu'il ne s'est pas termin� correctement.    
                } else {
            
                    fprintf(stderr,"PERE >> %s (%d) ne s'est pas termine correctement.\n",nomFilsEnded,pid_ended);
                    fflush(stderr);
            
                }
                filsTermine++;
            }
            fprintf(stderr,"PERE >> Fin du processus.\n");
            fflush(stderr);
        }

    }
    
    return 0;
}


/*-------------------------------------------------------
                    FIN DU PROGRAMME
 -------------------------------------------------------*/





















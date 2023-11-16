#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#ifdef NCURSES
#include <ncurses.h>
#endif
#include "listes.h"


/*
 *  Auteur(s) :
 *  Date :
 *  Suivi des Modifications :
 *
 */

bool silent_mode = true;
bool silent_avant_apres = true; //sert a montrer au debut et a la fin de l'execution


cellule_t* nouvelleCellule (void)
{
    cellule_t* nouvelle_cellule = (cellule_t*)(malloc (sizeof(cellule_t)));
    if (!nouvelle_cellule){
        fprintf(stderr, "Errueur : probleme allocation cellule dnas nouvelleCellule.\n");
        exit(EXIT_FAILURE);
    }
    nouvelle_cellule->type = 0;
    nouvelle_cellule->buffer.command = 'L'; //valeur impossible dans le jeu
    nouvelle_cellule->suivant = NULL;
    return nouvelle_cellule;
}

bool verif_type(int type, cellule_t* cel){
    return cel->type == type;
}

//detruit une cellule est renvoie NULL
cellule_t* detruireCellule (cellule_t* cel)
{
    free(cel);
    return NULL;
}


//fonction recursive qui detruit aussi les groupes de commandes
cellule_t* detruireSeq(cellule_t* cel){
    if (!cel)
        return NULL;
    
    if (cel->suivant)
        cel->suivant = detruireSeq(cel->suivant);
    
    detruireCellule(cel);
    
    return NULL;
        
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/


//converti un texte en sequence (liste chaine)
void conversion (char *texte, sequence_t *seq){

    //creation de la tete de la sequence
    seq->tete = NULL;
    
    int len = strlen(texte); 
    cellule_t* queue_seq;

    for (int i = 0 ; i < len ; i++){

        if (texte[i]!= '\n' && texte[i] != '\0'){ //on ne fais rien si texte[i] == \n ou \0
            cellule_t* nouv_cel = nouvelleCellule();
        
            if (!seq->tete){ //si seq->tete == NULL (sert que la premiere itération)
                seq->tete = nouv_cel;
                queue_seq = nouv_cel; //queue de la sequence sert a rajouter les prochaine cellule
            }
            else{ //pour toutes les itération sauf la premiere ( seq->tete != NULL)
                queue_seq->suivant = nouv_cel;
                queue_seq = nouv_cel;
            }

            if ( '0' <= texte[i] && texte[i] <= '9') {//Alors on change le type de la cellule en 1 (chiffre et on met le chiffre dans la cellule)
                nouv_cel->type = 1;
                nouv_cel->buffer.chiffre = texte[i] - '0';
            }

            else if (texte[i] == '{' || texte[i] == '}'){ //si on est dans un groupe de commande ---> Alors on change le type en 2 et on met la commande dans la cellule
                nouv_cel->type = 2;
                nouv_cel->buffer.command = texte[i];
            }

            else { //si c'est une commande executable
                
                nouv_cel->buffer.command = texte[i];
                
            }
        
        }
    }
}


void afficher (sequence_t* seq)
{
    assert (seq); /* Le pointeur doit être valide */
    cellule_t* cel = seq->tete;
    while(cel){
        if (verif_type(1, cel)){
            printf("%d", cel->buffer.chiffre);
        }
        else{
            printf("%c", cel->buffer.command);
        }
        cel = cel->suivant;
    }
}


#ifndef LISTES_H
#define LISTES_H

#include <stdbool.h>

/*
 * Pour réaliser des tests de performance, désactiver tous les 
 * affichages.
 * Pour cela, le plus simple est de compiler avec la macro 'SILENT' définie, en 
 * faisant 'make silent'
 */

#ifdef SILENT

/* Desactive tous les affichages */
#define printf(fmt, ...) (0)
#define eprintf(fmt, ...) (0)
#define putchar(c) (0)
/* Desactive les 'Appuyer sur entrée pour continuer' */
#define getchar() ('\n')

#else

#define eprintf(...) fprintf (stderr, __VA_ARGS__)

#endif

extern bool silent_mode;




struct cellule {
    int type;   //pour savoir de quel type est dans le union par defaut 0 (0 pour commande, 1 pour chiffre et 2 pour groupe commande)

    union {
        char command;   //utiliser pour les commandes executables
        int chiffre;    //utilier uniquement pour les chiffre (pour les calcul et les commandes qui se servent de chiffre)
    } buffer;
    
    struct cellule *suivant;
};
typedef struct cellule cellule_t;

struct sequence {
    cellule_t *tete;
};
typedef struct sequence sequence_t;

/*---------------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* nouvelleCellule (void); //cree une nouvelle cellule de type 0

bool verif_type(int type, cellule_t* cel); //verifie le type de la cellule renvoie true si c'est le bon type false sinon

cellule_t* detruireCellule (cellule_t* cel); 

cellule_t* detruireSeq(cellule_t* cel); //detruit une sequence meme si il y un groupe de commande

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void conversion (char *texte, sequence_t *seq);

void afficher (sequence_t* seq);


#endif

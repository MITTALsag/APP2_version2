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
extern bool silent_avant_apres;


struct cellule {
    int type;   //pour savoir de quel type est dans le union par defaut 0 (0 pour commande, 1 pour chiffre et 2 pour groupe commande debut et 3 pour fin de groupe commande)

    union {
        char command;   //utiliser pour les commandes executables
        int chiffre;    //utilier uniquement pour les chiffre (pour les calcul et les commandes qui se servent de chiffre)
        struct cellule* fin_groupe; //pointeur qui sert pour le type 2 pour pointer vers la fi du groupe de commande
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

void conversion_mise_en_place(char *texte, sequence_t *seq); //init la seq sans les pointeur de fin de groupe de commande

cellule_t* recherche_fin_groupe(cellule_t* deb_grp); //recherche et renvoie un pointeur de la fin du groupe de commande passe en argument

void mettre_fin_groupe(sequence_t* seq);//met dans chaque debut de groupe le pointeur correspondant a la fin de son groupe

void conversion(char *texte, sequence_t *seq);

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void afficher (sequence_t* seq);

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void supprime_tete(sequence_t* seq); //supprime la tete de seq (meme si il y a un groupe)

void ajouter_tete(sequence_t* seq, cellule_t* cel); //ajoute cel au debut de seq

void ajout_pile(sequence_t* pile,sequence_t* seq);//ajoute l'element du debut de seq au debut de la pile (evite de detruire puis recree)

void ajout_queue(sequence_t* pile, sequence_t* seq); //ajoute le dernier element de seq a la tete de la pile

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void exec_groupe(sequence_t* pile, sequence_t* seq); //prend le groupe en tete de la pile et le met sous forme "executable" au debut de la sequence 

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void echanger_place(sequence_t* seq); //echange les deux premiere cellule d'une sequence de place

/*---------------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* clone_chiffre(cellule_t* cel); //revoie l'adresse d'une nouvelle cellule qui est du type 1 est qui contien le meme chiffre que cel (sert dans clone_cellule)

cellule_t* clone_command(cellule_t* cel); //revoie l'adresse d'une nouvelle cellule qui est du type 0 est qui contien la meme command que cel (sert dans clone_cellule)

sequence_t clone_groupe(cellule_t* groupe); //revoie l'adresse d'une nouvelle cellule qui est du type 2 est qui contien le meme groupe que groupe (sert dans clone_cellule)

cellule_t* clone_cellule(cellule_t* cel); //revoie l'adresse d'une nouvelle cellule qui est du type 0, 1, 2 est qui contien la meme valeur que cel (se sert des 3 fonction precedente)

/*---------------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* cell_pos_i(sequence_t* seq, int pos); //renvoie l'adresse de la cellule a la position pos (la premiere cellule est a la position 0)

cellule_t* trouver_queue_seq(sequence_t* seq); //renvoie l'adresse de la queue d'une sequance et NULL si la sequence est vide

cellule_t* inversee_seq(sequence_t* seq); //inverse la sequence est revoie la cellule au debut de la nouvelle sequence

#endif

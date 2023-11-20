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

int len;


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
void conversion(char *texte, sequence_t *seq){

    //creation de la tete de la sequence
    seq->tete = NULL;
    
    len = strlen(texte); 
    cellule_t* queue_seq;

    cellule_t** tab_cel = (cellule_t**) malloc(len*sizeof(cellule_t*));
    int tab_cel_len = 0;


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

            else if (texte[i] == '{'){ //si on est dans un groupe de commande ---> Alors on change le type en 2 et on met la commande dans la cellule
                nouv_cel->type = 2;
                nouv_cel->buffer.command = texte[i];

                
                tab_cel[tab_cel_len] = nouv_cel;
                tab_cel_len++;
            
            }
            else if (texte[i] == '}'){ //si on est a la fin d'un groupe de commande ---> Alors on change le type en 3 et on met la commande dans la cellule
                nouv_cel->type = 3;
                nouv_cel->buffer.command = texte[i];
                
                
                cellule_t* deb_grp = tab_cel[tab_cel_len-1];
                deb_grp->buffer.fin_groupe = nouv_cel;
                tab_cel_len--;
                
            }
            else { //si c'est une commande executable
                
                nouv_cel->buffer.command = texte[i];
                
            }
        
        }
    }
    free(tab_cel);
}


cellule_t* recherche_fin_groupe(cellule_t* deb_grp){
    if (!verif_type(2, deb_grp)){
        printf("LA CELLULE ENVOYÉ DANS LA FONCTION recherche_fin_groupe N'EST PAS UN GROUPE\n");
        exit(EXIT_FAILURE);
    }

    int compteur = 1; //sert a compter si il y a des sous groupe
    deb_grp = deb_grp->suivant;
    while (deb_grp){

        if (verif_type(2, deb_grp)){ //Il y a donc un sous groupe et il faut donc croiser un } de plus avant la fin du groupe
            compteur++;
        }
        if (verif_type(3, deb_grp)){ 
            if (compteur == 1){//si on a atteint la fin du groupe
                return deb_grp;
            }
            compteur--; //fin d'un des sous groupe
        }

        deb_grp = deb_grp->suivant;
    }

    printf("LA SEQUENCE N'A PAS DE FIN DE GROUPE\n");
    exit(EXIT_FAILURE);
}

void mettre_fin_groupe(sequence_t* seq){
    cellule_t* cel = seq->tete;

    while (cel){
        if (verif_type(2, cel)){
            cel->buffer.fin_groupe = recherche_fin_groupe(cel);
        }
        cel = cel->suivant;
    }
}

void conversion_mise_en_place(char *texte, sequence_t *seq){
    //conversion_mise_en_place(texte, seq);
    mettre_fin_groupe(seq);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void afficher (sequence_t* seq)
{
    assert (seq); /* Le pointeur doit être valide */
    cellule_t* cel = seq->tete;
    while(cel){
        if (verif_type(1, cel)){
            printf("%d", cel->buffer.chiffre);
        }
        else if (verif_type(0, cel) || verif_type(3, cel)){
            printf("%c", cel->buffer.command);
        }
        else{
            printf("{");
        }
        cel = cel->suivant;
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

//supprime la tete de la sequence, la nouvelle tete est le deuxieme element de la sequance avant la fonction 
void supprime_tete(sequence_t* seq){

    if (verif_type(2, seq->tete)){ 
        //on detruit tout jusqu'a la fin du groupe de commande
        cellule_t* fin = seq->tete->buffer.fin_groupe->suivant; //le suivant est pour dettruire aussi la derniere cellule
        cellule_t* cel = seq->tete;
        cellule_t* det_cel = cel;

        while (cel != fin){
            cel = cel->suivant;
            detruireCellule(det_cel);
            det_cel = cel;
        }

        seq->tete = cel;
    }
    else {
        cellule_t* tmp = seq->tete;
        seq->tete = seq->tete->suivant;
        detruireCellule(tmp);
    }
}


void ajouter_tete(sequence_t* seq, cellule_t* cel){

    if (verif_type(2, cel)){
        cel->buffer.fin_groupe->suivant = seq->tete;
    }
    else{
        cel->suivant = seq->tete;
    }
    seq->tete = cel;

}

void ajout_pile(sequence_t* pile,sequence_t* seq){
       
    cellule_t* cel = seq->tete;
    cellule_t* fin;
    int type = cel->type;
    switch(type){
        case 0:
        case 1:
            seq->tete = cel->suivant;
            cel->suivant = pile->tete;
            pile->tete = cel;
            break;
        case 2:
            fin = cel->buffer.fin_groupe;
            seq->tete = fin->suivant;
            fin->suivant = pile->tete;
            pile->tete = cel;
            break;
    }       


}

void ajout_queue(sequence_t* pile, sequence_t* seq){
    
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void exec_groupe(sequence_t* pile, sequence_t* seq){

    cellule_t* cel = pile->tete;
    if (!verif_type(2, cel)){
        printf("Dans la fonction exec_groupe, la tete de la pile n'est pas un groupe\n");
        exit(EXIT_FAILURE);
    }
    cellule_t* fin = cel->buffer.fin_groupe;
    pile->tete = fin->suivant;
    fin->suivant = seq->tete;
    seq->tete = cel;
    cel->type = 0;
    cel->buffer.command = '{';
    fin->type = 0;
    fin->buffer.command ='}';

}

/*---------------------------------------------------------------------------------------------------------------------------------------*/


void echanger_place(sequence_t* seq){
    cellule_t* premiere = seq->tete;
    cellule_t* deuxieme;
    if (verif_type(2, premiere)){
        deuxieme = premiere->buffer.fin_groupe->suivant;
    }
    else {
        deuxieme = premiere->suivant;
    }

    if (verif_type(2, deuxieme) && verif_type(2, premiere)){ //les deux sont des groupes
        seq->tete = deuxieme;
        premiere->buffer.fin_groupe->suivant = deuxieme->buffer.fin_groupe->suivant;
        deuxieme->buffer.fin_groupe->suivant = premiere;
    }
    else if (verif_type(2, deuxieme) && !verif_type(2, premiere)){ //si uniquement la deuxieme est un groupede commande
        seq->tete = deuxieme;
        premiere->suivant = deuxieme->buffer.fin_groupe->suivant;
        deuxieme->buffer.fin_groupe->suivant = premiere;
    }
    else if (!verif_type(2, deuxieme) && verif_type(2, premiere)){ //si uniquement la premiere est un groupede commande
        seq->tete = deuxieme;
        premiere->buffer.fin_groupe->suivant = deuxieme->suivant;
        deuxieme->suivant = premiere;
    }
    else { //aucune des deux n'est un grouped e commande 
        seq->tete = deuxieme;
        premiere->suivant = deuxieme->suivant;
        deuxieme->suivant = premiere;
    }
}


/*-----------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* clone_chiffre(cellule_t* cel){

        if (!verif_type(1, cel)){
        printf("Erreur : fonction clone_chiffre ----> Le type de la cellule passé en argument est %d alors qu'il doit etre du type 1\n", cel->type);
        exit(EXIT_FAILURE);  
    }
    cellule_t* clone = nouvelleCellule();
    clone->type = 1;
    clone->buffer.chiffre = cel->buffer.chiffre;
    return clone;
}

cellule_t* clone_command(cellule_t* cel){

    if (!verif_type(0, cel)){
        printf("Erreur : fonction clone_command ----> Le type de la cellule passé en argument est %d alors qu'il doit etre du type 0\n", cel->type);
        exit(EXIT_FAILURE);  
    }
    cellule_t* clone = nouvelleCellule();
    clone->buffer.command = cel->buffer.command;
    return clone;
}



sequence_t clone_groupe(cellule_t* groupe){ //dans cette fonction on se preocupe pas de la fin du groupe et des sous groupe

    if (!verif_type(2, groupe)){ //si pas de groupe donc erreur
        printf("Erreur : fonction clone_groupe ----> Le type de la cellule passé en argument est %d alors qu'il doit etre du type 2\n", groupe->type);
        exit(EXIT_FAILURE);
    }

    sequence_t seq;
    cellule_t* cel = nouvelleCellule();
    cel->type = 2;
    seq.tete = cel;

    cellule_t* queue_cel = cel; 

    cellule_t* fin = groupe->buffer.fin_groupe->suivant; //le suivant est pour aller jusqu'au bout du groupe (meme le })
    groupe = groupe->suivant;

    while (groupe != fin){

        cellule_t* cel = nouvelleCellule();
        queue_cel->suivant = cel;
        queue_cel = cel;
        cel->type = groupe->type;

        if (verif_type(1, groupe)){
            cel->buffer.chiffre = groupe->buffer.chiffre;
        }
        else if (verif_type(0, groupe)){
            cel->buffer.command = groupe->buffer.command;

        }
        else if (verif_type(3, groupe)){
            cel->buffer.command = '}';
        }

        groupe = groupe->suivant;

    }
    return seq;
}


cellule_t* clone_cellule(cellule_t* cel){
    int type = cel->type;

    sequence_t seq_tmp;
    
    switch(type){
        case 0 :
            return clone_command(cel);
            break;
        case 1 : 
            return clone_chiffre(cel);
            break;
        
        case 2 : 
            seq_tmp = clone_groupe(cel);
            mettre_fin_groupe(&seq_tmp); //pour remettre les fin_groupe
            return seq_tmp.tete;
            break;
        
        default :
            printf("Type de cellule inconnu : %d\n", type);
            exit(EXIT_FAILURE);
            break;
    }

    return NULL; //impossible normalement d'arriver ici

}

/*-----------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* cell_pos_i(sequence_t* seq, int pos){
    cellule_t* cel = seq->tete;
    
    for(int i = 0 ; i < pos ; i++){

        if(verif_type(2, cel)){
            cel = cel->buffer.fin_groupe;
        }

        cel = cel->suivant;

        if (!cel){
            printf("Erreur (fonction cell_pos_i): La sequence est trop petite pour acceder au %d eme elemnet.\nElle a une taille de %d", pos, i+1),
            exit(EXIT_FAILURE);
        }
    }
    return cel;
}



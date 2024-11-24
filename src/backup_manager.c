#include "backup_manager.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct ch
{
    char ID[32]; // Signature unique du chunk (ex : SHA-256)
    char chunk[32];
    int size;
    int position;
    struct ch *suivant;
} Chunk;

typedef struct
{
    char filename[256];
    int n_chunks;
    Chunk *chunks;
} FileBackup;

int sizestr(char *str, int compteur)
{ // Renvoi la taille d'un string
    while (*str != '\0' && *str != '\n')
    {
        ++str;
        ++compteur;
    }
    return compteur;
}

int Nlinefile(FILE *file)
{
    int ligne_actuelle = 0;
    char *stringfile;
    while (fgets(stringfile[ligne_actuelle], Max_len_ligne, file))
    { // Récupere les lignes du fichier csv
        stringfile[ligne_actuelle][sizestr(stringfile[ligne_actuelle], 0)] = '\0';
        ++ligne_actuelle;
    }
}

void create_backup(const char *source, const char *destination)
{ // Implémenter la logique de création d'une sauvegarde

    FILE *file = NULL;
    int Max_len_ligne = (Maxlenmot + 1) * Maxmotcorrection;
    char lignecorrection_Recup[nb_joueur_euse][Max_len_ligne];

    // Ouvre le fichier de la player en fonction du nombre de joueur et de qui joue
    file = fopen(source, "r");

    // Vérifie que la variable a bien récupéré un fichier
    if (Verif_file(file) != 0)
    {
        return;
    }
}

void restore_backup(const char *backup_id, const char *destination)
{
    // Implémenter la logique de restauration d'une sauvegarde
}

void list_backup(const char *directory)
{
    // Implémenter la logique de liste des sauvegardes
}

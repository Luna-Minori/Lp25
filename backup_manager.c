#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <libgen.h>
#include "backup_manager.h"
#include "file_modifier.h"
#include "file_handler.h"

#define PRINT_TYPE(var) _Generic((var), \
    int: "int",                         \
    float: "float",                     \
    double: "double",                   \
    char: "char",                       \
    char *: "char*",                    \
    default: "unknown")

void copier_fichier(FILE *source, FILE *dest, int ligne_debut)
{ // copie un fichier dans un autre fichier

    rewind(source);
    // rewind(dest);

    for (int i = 0; i < ligne_debut; i++)
    {
        char ligne[4096];
        fgets(ligne, sizeof(ligne), source);
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0)
    {
        fwrite(buffer, 1, bytes, dest);
    }
}

int existe_deja_index(int index, FILE *file)
{
    char ligne[4096];
    int index_ligne = -1;
    rewind(file); // Rewind pour lire depuis le début du fichier
    fgets(ligne, sizeof(ligne), file);
    while (fgets(ligne, sizeof(ligne), file))
    {
        if (ligne[0] != '\n' || strlen(ligne) > 33)
        {
            if (ligne[32] == ';')
            {
                char temp_index[16] = {};
                int i = 33;
                while (ligne[i] != ';')
                { // cherche l'index sur la ligne
                    char temp[2] = {ligne[i]};
                    strncat(temp_index, temp, 1);
                    i++;
                }
                index_ligne = atoi(temp_index);
            }
            if (index == index_ligne)
            {
                return 1; // Trouvé
            }
        }
    }
    return 0; // Pas trouvé
}

int existe_deja_version(int version, int index, FILE *file)
{
    char ligne[4096];
    int version_ligne = -1;
    int index_ligne = -1;
    rewind(file); // Rewind pour lire depuis le début du fichier
    fgets(ligne, sizeof(ligne), file);
    while (fgets(ligne, sizeof(ligne), file))
    {
        if (ligne[0] != '\n' || strlen(ligne) > 33)
        {

            if (ligne[32] == ';')
            {
                char temp_version[16] = {};
                char temp_index[16] = {};
                int i = 33;
                while (ligne[i] != ';')
                { // cherche l'index sur la ligne
                    char temp[2] = {ligne[i]};
                    strncat(temp_index, temp, 1);
                    i++;
                }
                index_ligne = atoi(temp_index);

                if (index_ligne == index)
                {

                    i++;
                    while (ligne[i] != '\0')
                    { // cherche la versions sur la ligne
                        char temp[2] = {ligne[i]};
                        strncat(temp_version, temp, 1);
                        i++;
                    }
                    version_ligne = atoi(temp_version);
                }
            }
            if (version == version_ligne)
            {
                return 1; // Trouvé
            }
        }
    }
    return 0; // Pas trouvé
}

int existe_deja_md5(char *md5, int version, int index, FILE *file)
{
    char ligne[4096];
    int version_ligne = -1;
    int index_ligne = -1;
    rewind(file); // Rewind pour lire depuis le début du fichier
    fgets(ligne, sizeof(ligne), file);
    while (fgets(ligne, sizeof(ligne), file))
    {
        if (ligne[0] != '\n' || strlen(ligne) > 33)
        {

            if (ligne[32] == ';')
            {
                char temp_version[16] = {};
                char temp_index[16] = {};
                int i = 33;
                while (ligne[i] != ';')
                { // cherche l'index sur la ligne
                    char temp[2] = {ligne[i]};
                    strncat(temp_index, temp, 1);
                    i++;
                }
                index_ligne = atoi(temp_index);

                if (index_ligne == index)
                {

                    i++;
                    while (ligne[i] != '\0')
                    { // cherche la versions sur la ligne
                        char temp[2] = {ligne[i]};
                        strncat(temp_version, temp, 1);
                        i++;
                    }
                    version_ligne = atoi(temp_version);
                    if (version == version_ligne)
                    {
                        if (strncmp(ligne, md5, 32) == 0)
                        {
                            return 1; // Trouvé
                        }
                    }
                }
            }
        }
    }
    return 0; // Pas trouvé
}

void sauvegarder(Chunk *chunks, int nombre_de_chunks, char *nom_fichier, char *path)
{
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
    compute_md5_file(path, md5, &md5_len);

    char md5_fichier[33];
    for (int i = 0; i < 16; ++i)
    {
        snprintf(&md5_fichier[i * 2], 3, "%02x", md5[i]);
    }

    char nom_fichier_sauvegarde[50];
    snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "%s_sauvegarde.txt", md5_fichier);

    if (access(nom_fichier_sauvegarde, F_OK) == -1)
    {
        FILE *file = fopen(nom_fichier_sauvegarde, "w");

        if (file == NULL)
        {
            perror("Erreur de création du fichier");
            return;
        }

        fprintf(file, "%d\n", 0);
        fclose(file);
        // return;
    }

    FILE *file = fopen(nom_fichier_sauvegarde, "a+"); // Ouvrir le fichier en mode lecture-écriture

    if (file == NULL)
    {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    // crée un fichier temporaire
    char nom_fichier_temp[60];
    snprintf(nom_fichier_temp, sizeof(nom_fichier_temp), "%s_sauvegarde_tmp.txt", md5_fichier);

    FILE *temp_file = fopen(nom_fichier_temp, "w");
    if (temp_file == NULL)
    {
        perror("Erreur de création du fichier temporaire");
        fclose(file);
        return;
    }

    char version_temp[16] = {};
    char ligne_version[16] = {};
    fgets(ligne_version, sizeof(ligne_version), file);
    int k = 0;
    while (ligne_version[k] != '\0')
    {
        char tempV[2] = {ligne_version[k]};
        strncat(version_temp, tempV, 1);
        k++;
    }

    fprintf(temp_file, "%d\n", atoi(version_temp) + 1); // incremente la version de sauvegarde du fichier 1
    copier_fichier(file, temp_file, 1);

    for (int i = 0; i < nombre_de_chunks; i++)
    {
        if (!existe_deja_index(chunks[i].index, file)) // Si le chunk n'existe pas, on l'ajoute
        {
            fprintf(temp_file, "%s;%d;%d\n%s\n", chunks[i].MD5, chunks[i].index, chunks[i].version, chunks[i].data);
        }
        else
        {
            printf("versiiion : %d",chunks[i].version);
            if (!existe_deja_version(chunks[i].version, chunks[i].index, file)) // Si la version n'existe pas, on l'ajoute
            {
                fprintf(temp_file, "%s;%d;%d\n%s\n", chunks[i].MD5, chunks[i].index, chunks[i].version, chunks[i].data);
            }
            else
            {
                if (existe_deja_md5(chunks[i].MD5, chunks[i].version, chunks[i].index, file) == 0)
                {
                    fprintf(temp_file, "%s;%d;%d\n%s\n", chunks[i].MD5, chunks[i].index, chunks[i].version + 1, chunks[i].data);
                }
            }
        }
    }

    fclose(file);
    fclose(temp_file);

    // Remplacer l'ancien fichier par le fichier temporaire
    if (remove(nom_fichier_sauvegarde) != 0)
    {
        perror("Erreur de suppression de l'ancien fichier");
        return;
    }

    if (rename(nom_fichier_temp, nom_fichier_sauvegarde) != 0)
    {
        perror("Erreur de renommage du fichier temporaire");
        return;
    }
}

#include <stdio.h>
#include <string.h>

typedef struct Chunk
{
    char *data;
    char MD5[33];
} Chunk;

int existe_deja(const char *md5, FILE *file)
{
    char ligne[1024];
    rewind(file); // Rewind pour lire depuis le début du fichier
    while (fgets(ligne, sizeof(ligne), file))
    {
        // Si une ligne commence par le MD5 du chunk, il existe déjà
        if (strncmp(ligne, md5, 32) == 0)
        {
            return 1; // Trouvé, il existe déjà
        }
    }
    return 0; // Pas trouvé
}

void sauvegarder(Chunk *chunks, int nombre_de_chunks, char *nom_fichier)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_sauvegarde.txt", nom_fichier);
    FILE *file = fopen(filename, "r+"); // Ouvrir le fichier en mode lecture-écriture

    if (file == NULL)
    {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    char ligne[1024];
    long pos;  // Position de la ligne à remplacer

    for (int i = 0; i < nombre_de_chunks; i++)
    {
        if (!existe_deja(chunks[i].MD5, file)) // Si le MD5 n'existe pas déjà
        {
            fprintf(file, "%s;%d\n%s\n", chunks[i].MD5, i, chunks[i].data);
        }
        else
        {
            rewind(file); // Rewind pour lire depuis le début du fichier
            while (fgets(ligne, sizeof(ligne), file))
            {
                pos = ftell(file); // Enregistrer la position actuelle

                // Si on trouve le MD5 du chunk, on remplace la ligne
                if (strncmp(ligne, chunks[i].MD5, 32) == 0)
                {
                	if (fgets(ligne, sizeof(ligne), file)==chunks[i].data){
		    		

                    		fseek(file, pos - strlen(ligne), SEEK_SET);
                    		fprintf(file, "%s;%d\n%s\n", chunks[i].MD5, i, chunks[i].data);
                    		break; // Sortir de la boucle une fois la ligne remplacée
                }
            }
        }
    }

    fclose(file);
}

int main()
{
    Chunk chunks[2] = {
        {"data10", "d41d8cd98f00b204e9800998ecf8427e"},
        {"data2", "d41d8cd98f00b204e9800998ecf8428e"}};

    // Exemple de nom de fichier de base
    char *nom_fichier = "fichier_test";

    sauvegarder(chunks, 2, nom_fichier);
    return 0;
}


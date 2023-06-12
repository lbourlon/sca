#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include "pearson.h"
#include "matrice.h"
#include "ingest.h"
#include "attacks.h"
#include "others.h"



void print_usage_exit(){
    printf("usage : ./prog <traces_file> <textes_file> <keys_file> <normal/online/test> <start_trace> <end_traces> <nb_echant>\n");
    printf("example: ./prog traces.raw textes.raw keys.raw online 32 279 870\n");
    exit(1);
}

int main(int argc, char *argv[]){
    int attack = 0;
    int start_sp = 0; int end_sp = 870; // marche pas ne pas changer
    int start_tr = 0; int end_tr = 370; //marche bien
    char *traces_raw = NULL; 
    char *textes_raw = NULL; 
    char *keys_raw   = NULL;

    // Gestion de l'entrée
    if(argc != 8 ) print_usage_exit();
    else {
        traces_raw = argv[1];
        textes_raw = argv[2];
        keys_raw   = argv[3];

        if     (strcmp(argv[4], "normal") == 0)  attack = 1; 
        else if(strcmp(argv[4], "online") == 0)  attack = 0;
        else if(strcmp(argv[4], "test")   == 0)  attack = 2;
        else print_usage_exit();

        start_tr = atoi(argv[5]);        
        end_tr   = atoi(argv[6]);        
        end_sp   = atoi(argv[7]);      
    }


    int nb_traces = end_tr - start_tr;
    int nb_echant = end_sp - start_sp;

    // Construire le HW lookup table.
    u_char HW[256];
    for(int i = 0; i < 256; ++i)HW[i] = HammingWeight(i); 

    // Allocation de matrice de taille nb_echant x nb_traces
    double** traces_mat = NULL; // (double**) malloc_mat(nb_echant, nb_traces, sizeof(double*));
    u_char** textes_mat = NULL; // (u_char**) malloc_mat(16,        nb_traces, sizeof(u_char*));

    // lecture des fichiers raw, ce qui nous donne listes avec tout les données
    // raw_ingest(traces_raw, textes_raw, &traces_mat, &textes_mat, &nb_traces, &nb_echant);
    // raw_ingest_mem(traces_raw, textes_raw, &traces_mat, &textes_mat, nb_traces, nb_echant);
    raw_ingest_borne(traces_raw, textes_raw, &traces_mat, &textes_mat, start_sp, end_sp, start_tr, end_tr, 1);
    
    u_char* key = ingest_key(keys_raw);

    if (attack == 0 ){
        // CPA (Avec Pearson Online (qui est aussi un peu plus rapide))
        printf("Starting by CPA Online Attack\n");
        for (int byte = 0; byte < 16; ++byte) {
            online_cpa(traces_mat, textes_mat,  nb_traces, nb_echant, HW, byte);
            printf(" / %3d  |\n",key[byte]);
        }
    } else if (attack == 1){
        // CPA (Avec Pearson normal)
        printf("Starting CPA Attack\n");
        for (int byte = 0; byte < 16; ++byte) {
            normal_cpa(traces_mat, textes_mat, nb_traces, nb_echant, HW, byte);
            printf(" / %3d  |\n",key[byte]);
        }
    } else if (attack == 2){
        // DPA NE MARCHE PAS BIEN
        printf("Starting DPA attack (doesn't work)!\n");
        int poi[16] = {78, 271, 500, 708, 90, 310, 539, 170, 380, 590, 775, 630, 110, 425, 395};
        for (int byte = 0; byte < 16; ++byte){
            dpa(traces_mat, textes_mat, nb_traces, nb_echant, HW, byte, poi[byte]);
            printf(" / %3d\n",key[byte]);
        }
    } else if (attack == 3){
        // CPA (Avec Batch Pearson) // Ne marche pas bien
        printf("Starting by Batch attack (doesn't work)!\n");
        for (int byte = 0; byte < 16; ++byte) {
            mem_cpa(traces_raw, textes_raw, nb_traces, nb_echant, HW, byte);
            printf(" / %3d  |\n",key[byte]);
        }

    }

    //Libère la mémoire
    free_mat((void**) traces_mat, nb_echant);
    free_mat((void**) textes_mat, 16);
    free(key);
    return 0;
}


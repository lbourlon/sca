#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "matrice.h"

u_char* ingest_key(char* keys_raw){
    // lis en Mémoire les 16 premiers u_char des fichiers keys.
    u_char* key_list = calloc(16, sizeof(u_char));
    int fd = open(keys_raw, O_RDONLY);
    if (fd == -1)  perror("open");

    int out_k = pread(fd, key_list, 16, 0);
    if (out_k == -1) perror("read");

    return key_list;
}

void raw_ingest(char* traces_file, char* textes_file, double*** traces_mat, u_char*** textes_mat, int* nb_traces, int* nb_echant){
    // Ouvertures de fichier en lecture uniquement
    int fd_tr = open(traces_file, O_RDONLY); int fd_tx = open(textes_file, O_RDONLY);
    if (fd_tr == -1 || fd_tx == -1 )  perror("open");
    
    // Récupérer la taille des fichiers en bytes
    struct stat st;
    fstat(fd_tr, &st); int size_tr = st.st_size;
    fstat(fd_tx, &st); int size_tx = st.st_size;

    // Calcul du nombre de traces et nombre d'échantillons par trace
    int nb_tra = size_tx / 16;
    int nb_ech = size_tr / (nb_tra * 8);

    // Listes temporaires    
    double* tra_concat = calloc(nb_tra * nb_ech, sizeof(double*));
    u_char* txt_concat = calloc(nb_tra * 16, sizeof(u_char*));

    // Lecture des Fichiers trace et textes
    // Ceci peut diminuer le nombre de traces en mémoire mais pas d'échantillons
    int offset = 0;
    int out_tr = pread(fd_tr, tra_concat, nb_tra * nb_ech * 8,  offset);
    int out_tx = pread(fd_tx, txt_concat, nb_tra * 16, offset);

    // Vérifie que les lectures se sont bien effectuées
    if (out_tr == -1 || out_tx == -1) perror("read");

    // Allocation des matrices finales
    double** tra_mat = (double**) malloc_mat(nb_ech, nb_tra, sizeof(double*));
    u_char** txt_mat = (u_char**) malloc_mat(16,     nb_tra, sizeof(u_char*));

    //Inverser i et j ici est dans l'execution de cpa divise le temps d'execution par ~6
    for(int i = 0; i < nb_tra; ++i) for (int j = 0; j < nb_ech; ++j) tra_mat[j][i] = tra_concat[i*nb_ech + j];
    for(int i = 0; i < nb_tra; ++i) for (int j = 0; j < 16;     ++j) txt_mat[j][i] = txt_concat[i*16 + j];

    printf("bytes read traces : %d, textes :  %d\n", out_tr, out_tx);
    printf("Nb of samples : %d | traces : %d\n", nb_ech, nb_tra);

    // Pour récupérer nos données
    *textes_mat = txt_mat;  *traces_mat = tra_mat;
    *nb_traces = nb_tra;    *nb_echant = nb_ech;

    free(tra_concat);
    free(txt_concat);
}

void raw_ingest_mem(char* traces_file, char* textes_file, double*** traces_mat, u_char*** textes_mat, int in_nb_traces, int in_nb_echant){
    /* Pemrmet de lire les fichiers en mémoire en faisant la transposition des matrices.
    Mais utiliser moins de mémoire que ingest normal. */

    // ouvertures de fichier en lecture uniquement
    int fd_tr = open(traces_file, O_RDONLY); int fd_tx = open(textes_file, O_RDONLY);
    if (fd_tr == -1 || fd_tx == -1 )  perror("open");
    
    // récupérer la taille des fichiers en bytes
    struct stat st;
    fstat(fd_tr, &st); int size_tr = st.st_size;
    fstat(fd_tx, &st); int size_tx = st.st_size;
    
    // calcul du nombre de traces et nombre d'échantillons par trace
    int nb_traces = size_tx / 16;
    int nb_echant = size_tr / (nb_traces * 8);


    if(in_nb_traces > nb_traces || in_nb_echant > nb_echant){
        printf("subset of traces biger than input files !\n"); 
        printf("actual file : traces = %5d, samples = %4d\n", nb_traces, nb_echant);
        printf("your input  : traces = %5d, samples = %4d\n", in_nb_traces, in_nb_echant);
        exit(1);
    }

    double** out_tra_mat = (double**) malloc_mat(in_nb_echant, in_nb_traces, sizeof(double*));
    u_char** out_txt_mat = (u_char**) malloc_mat(          16, in_nb_traces, sizeof(u_char*));

    int taille_trace_fichier  = nb_echant * sizeof(double); 
    int taille_trace_demandee = in_nb_echant * sizeof(double); 
    int taille_texte  = 16 * sizeof(u_char) ;
    // lecture des fichiers trace et textes
    int read_tr = 0; int read_tx= 0;
    for (int t = 0; t < in_nb_traces; ++t) {
        double* buf_tr = calloc(in_nb_echant, sizeof(double));
        u_char* buf_tx = calloc(16, sizeof(u_char));

        int out_tr = pread(fd_tr, buf_tr, taille_trace_demandee, t * taille_trace_fichier);
        int out_tx = pread(fd_tx, buf_tx, taille_texte, t * taille_texte );
        if (out_tr == -1 || out_tx == -1) perror("read"); // gestion d'erreurs

        for (int s = 0; s < in_nb_echant; ++s) out_tra_mat[s][t] = buf_tr[s];
        for (int b = 0; b < 16      ;     ++b) out_txt_mat[b][t] = buf_tx[b];

        read_tr += out_tr; read_tx += out_tx; // pour confirmer les bytes lis
        free(buf_tr); free(buf_tx);
    }

    printf("bytes read traces out of : %d / %d, textes :  %d / %d\n", read_tr, size_tr, read_tx, size_tx);
    printf("nb of samples out of  : %d / %d | traces : %d / %d\n\n", in_nb_echant, nb_echant, in_nb_traces, nb_traces);

    *textes_mat = out_txt_mat; *traces_mat = out_tra_mat;
}


void raw_ingest_borne(char* traces_file, char* textes_file, double*** traces_mat, u_char*** textes_mat, int start_sp, int end_sp, int start_tr, int end_tr, int print_bool){
    /* Work in progress
       Permet de charger des échantillons et traces arbitraires.
       Le choix des traces semble marcher, mais pas les échantillons.
    */
    // Ouvertures de fichier en lecture uniquement
    int fd_tr = open(traces_file, O_RDONLY); int fd_tx = open(textes_file, O_RDONLY);
    if (fd_tr == -1 || fd_tx == -1 )  perror("open");
    
    // Récupérer la taille des fichiers en bytes
    struct stat st;
    fstat(fd_tr, &st); int size_tr = st.st_size;
    fstat(fd_tx, &st); int size_tx = st.st_size;

    // Calcul du nombre de traces et nombre d'échantillons par trace dans le fichier
    int nb_traces = size_tx / 16;
    int nb_echant = size_tr / (nb_traces * 8);

    if(start_sp >= end_sp) {perror("La Borne A doit être strictement inférieure à la Borne B");}
    int in_nb_echant = end_sp - start_sp;
    int in_nb_traces = end_tr - start_tr;

    int padding = nb_echant - end_sp;

    if(in_nb_traces > nb_traces || in_nb_echant > nb_echant){
        printf("Subset of traces biger than input files !\n"); 
        printf("actual file : traces = %5d, samples = %4d\n", nb_traces, nb_echant);
        printf("your input  : traces = %5d, samples = %4d\n", in_nb_traces, in_nb_echant);
        printf("your input  : A = %4d, B = %4d\n, C = %d, D = %d\n", start_sp, end_sp, start_tr, end_sp);
        exit(1);
    }

    double** out_tra_mat = (double**) malloc_mat(in_nb_echant, in_nb_traces, sizeof(double*));
    u_char** out_txt_mat = (u_char**) malloc_mat(          16, in_nb_traces, sizeof(u_char*));

    int taille_trace_fichier  = nb_echant * sizeof(double); 
    int taille_trace_demandee = in_nb_echant * sizeof(double); 
    int taille_texte  = 16 * sizeof(u_char);

    // Lecture des Traces
    int read_tr = 0; int read_tx= 0;

    for (int t = start_tr; t < end_tr; ++t) {
        double* buf_tr = calloc(in_nb_echant, sizeof(double));
        u_char* buf_tx = calloc(16, sizeof(u_char));

        int out_tr = pread(fd_tr, buf_tr, taille_trace_demandee, t * taille_trace_fichier);
        int out_tx = pread(fd_tx, buf_tx, taille_texte, t * taille_texte);
        if (out_tr == -1 || out_tx == -1) perror("read"); // gestion d'erreurs
        read_tr += out_tr; read_tx += out_tx;// Pour confirmer les octets lis

        for (int s = 0; s < in_nb_echant; ++s) out_tra_mat[s][t-start_tr] = buf_tr[s];
        for (int b = 0; b < 16 ; ++b) out_txt_mat[b][t-start_tr] = buf_tx[b];
        free(buf_tr); free(buf_tx);
    }

    if (print_bool == 1){
        printf("Bytes read traces : %d / %d    | textes : %d / %d\n", read_tr, size_tr, read_tx, size_tx);
        printf("Number of Traces  : %d / %d    | Traces : %d / %d\n",  in_nb_traces, nb_traces, in_nb_echant, nb_echant);
        printf("RANGES    Traces  : [%d : %d]   | Samples : [%d : %d]\n\n", start_tr, end_tr, start_sp, end_sp);
    }

    *textes_mat = out_txt_mat; *traces_mat = out_tra_mat;

}


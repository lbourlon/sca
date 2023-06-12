#include <sys/types.h>
void** malloc_mat(int I, int J, long unsigned int sizeof_element);
void free_mat(void** mat, int I);
void afficher_trace(double* x, int n);
void affichier_matrice(double** matrice, int I, int J);
void afficher_texte(u_char* x, int n);
void remplir_matrice_de_trucs(double** mat, int I, int J);
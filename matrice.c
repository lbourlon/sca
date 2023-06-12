#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void** malloc_mat(int I, int J, long unsigned int sizeof_element){
  // Allocation de matrice de taille I x J 
  void** mat = calloc(I, sizeof_element);
  for(int i = 0; i < I; ++i) mat[i] = calloc(J, sizeof_element);
  
  return mat;}

void free_mat(void** mat, int I){
  // Désalouer la mémoire de la matrice
  for(int i = 0; i < I; i++) free(mat[i]);
  free(mat);}

void afficher_trace(double* x, int n){
    for (int i = 0; i < n; ++i) printf("%f\n", x[i]);}

void afficher_texte(u_char* x, int n){
    for (int i = 0; i < n; ++i) printf("%u\n", x[i]);}

void affichier_matrice(double** matrice, int I, int J){
    for (int i = 0; i <= I; ++i) {printf("----");} printf("\n");

    for (int i = 0; i < I; ++i) {
        for (int j = 0; j < J; ++j) {
            printf("| %.0f ", matrice[i][j]);
        }
        printf("|\n");
    }
    for (int i = 0; i <= I; ++i) {printf("----");} printf("\n");
}

void remplir_matrice_de_trucs(double** mat, int I, int J){
    for (int i = 0; i < I; ++i) {
        for (int j = 0; j < J; ++j) {
            mat[i][j] = j;
        }
    }
}
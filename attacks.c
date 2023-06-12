
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "others.h"
#include "pearson.h"
#include "matrice.h"
#include "ingest.h"

u_char normal_cpa(double** traces_mat, u_char** textes_mat, int nb_traces, int nb_echant, u_char* HW, u_char bnum){
    /*  Implémentation de Correlation Power Analysis en utilisant
        le coefficient de pearson.                                  */
    double max_corr   = 0;
    u_char best_guess = 0;
    int xmax = 0;

    for (int kg = 0; kg < 255; ++kg){
        double temp_max_corr = 0;
        double max_corr_all_x = 0;
        xmax = 0;

        double* hw_of_traces = malloc(nb_traces * sizeof(double));
        // Loop sur toutes les traces pour contstuire hw_of_traces
        for(int t = 0; t < nb_traces; ++t){
            hw_of_traces[t] = HW[intermediate(textes_mat[bnum][t], kg)];
        }
        // loop sur tout les x en calculants les corr entre 
        // hw[nb_traces] et power_of_given_x[nb_traces]
        for(int x = 0; x < nb_echant; ++x){
            temp_max_corr = full_pearson(hw_of_traces, traces_mat[x], nb_traces);
            if(temp_max_corr > max_corr_all_x){
                max_corr_all_x= temp_max_corr;
                xmax = x;
            }
        }

        if(max_corr_all_x > max_corr ){
            max_corr = max_corr_all_x;
            best_guess = kg;
        }
        free(hw_of_traces);
    }
    printf("Byte : %2d | corr : %f | x = %4d | kg/key = %3hhu",bnum, max_corr, xmax, best_guess);
    return best_guess;
}

u_char online_cpa(double** traces_mat, u_char** textes_mat, int nb_traces, int nb_echant, u_char* HW, u_char bnum){
    /*  Implémentation de Correlation Power Analysis en utilisant
        la méthode online du coefficient de pearson.             */
    double max_corr   = 0;
    u_char best_guess = 0;
    int xmax = 0;

    for (int kg = 0; kg < 255; ++kg){
        double temp_max_corr = 0;
        double max_corr_all_x = 0;
        xmax = 0;

        double* hw_of_traces = malloc(nb_traces * sizeof(double));
        // Loop sur toutes les traces pour contstuire hw_of_traces
        for(int t = 0; t < nb_traces; ++t){
            hw_of_traces[t] = HW[intermediate(textes_mat[bnum][t], kg)];
        }
        // loop sur tout les x en calculants les corr entre 
        // hw[nb_traces] et power_of_given_x[nb_traces]
        for(int x = 0; x < nb_echant; ++x){
            temp_max_corr = online_pearson(hw_of_traces, traces_mat[x], nb_traces);
            if(temp_max_corr > max_corr_all_x){
                max_corr_all_x= temp_max_corr;
                xmax = x;
            }
        }

        if(max_corr_all_x > max_corr ){
            max_corr = max_corr_all_x;
            best_guess = kg;
        }
        free(hw_of_traces);
    }
    printf("Byte : %2d | corr : %f | x = %4d | kg/key = %3hhu",bnum, max_corr, xmax, best_guess);
    return best_guess;
}

u_char mem_cpa(char* traces_raw, char* textes_raw, int nb_traces, int nb_echant, u_char* HW, u_char bnum){
    /*  Implémentation de Correlation Power Analysis en utilisant
        la méthode online du coefficient de pearson.             */
    double max_corr   = 0;
    u_char best_guess = 0;
    int xmax = 0;
    
    int batch_size = 50;
    int nb_of_batches = nb_traces / batch_size;
    
    for (int kg = 0; kg < 255; ++kg){
        double temp_max_corr = 0;
        double max_corr_all_x = 0;
        xmax = 0;
        double sum_a = 0;
        double sum_b = 0;
        double sum_ab   = 0;
        double sum_asq  = 0;
        double sum_bsq  = 0;
        // Pour chaque échantillon nous gardons un jeux de variables de pearson
        // ordre : sum_a, sum_b, sum_ab, sum_asq, sum_bsq
        // double** lbv = (double**) malloc_mat(nb_echant, 5, sizeof(double*));
        double* lbv = calloc(5, sizeof(double));

        double batch_max = 0;
        // Boucle pour les batchs
        for (int batch = 0; batch < nb_of_batches; ++batch) {
            double** traces_mat  = NULL;
            u_char** textes_mat  = NULL;
            double* hw_of_traces = malloc(batch_size * sizeof(double));
                
            int print_bool = 0;
            // if(batch == 6){
            //     print_bool = 1;
            // } 
            int tot = batch * batch_size;
            raw_ingest_borne(traces_raw, textes_raw, &traces_mat, &textes_mat, 0, nb_echant, tot, tot + batch_size, print_bool);

            // Loop sur toutes batch traces pour contstuire hw_of_traces
            for(int t = 0; t < batch_size; ++t) hw_of_traces[t] = HW[intermediate(textes_mat[bnum][t], kg)];

            batch_max = 0;

            // loop sur tout les x en calculants les correlations 
            int cond = 0;
            for(int x = 0; x < nb_echant; ++x){
                if (x == nb_echant -1) cond =1;
                else cond = 0;
                // printf("sum_ a : %f\n", lbv[0]);

                online_pearson_by_batch(hw_of_traces, traces_mat[x], batch_size, tot, &sum_a, &sum_b, &sum_ab, &sum_asq, &sum_bsq, cond);

                // temp_max_corr = online_pearson(hw_of_traces, traces_mat[x], tot);
                // online_pearson_by_batch_test(hw_of_traces, traces_mat[x], batch_size, batch + batch_size, lbv, cond);
                // printf("kg = %3d, lbv[%3d][0] =  %0.1f\n",kg,x, lbv[0]);

                double num   = tot * sum_ab - sum_a * sum_b;
                double denum = (sum_a * sum_a - tot * sum_asq) * (sum_b * sum_b - tot * sum_bsq);
                // if (batch < 2) {
                //     printf("num = %0.3f , denum = %0.3f\n", num, denum);
                // }

                // temp_max_corr =  fabs(num / sqrt(denum));

                if(temp_max_corr > max_corr_all_x){
                    max_corr_all_x = temp_max_corr;
                    batch_max = temp_max_corr;
                    xmax = x;
                }
            }

            if(max_corr_all_x > max_corr ){
                max_corr = max_corr_all_x;
                best_guess = kg;
            }

            free_mat((void**) traces_mat, nb_echant);
            free_mat((void**) textes_mat, 16);
            free(hw_of_traces);

            // printf("Batch Max : %f\n", batch_max);
        }
        
        free(lbv);
    }
    printf("Byte : %2d | corr : %f | x = %4d | kg/key = %3hhu",bnum, max_corr, xmax, best_guess);

    return best_guess;
}


void dpa(double** traces_mat, u_char** textes_mat, int nb_traces, int nb_echant, u_char* HW, u_char bnum, int poi){
    /* Pour un kg et bnum donnée trouve le point x de plus grande correlation
        voir pears_on_r_mat */

    double true_diff    = 0;
    u_char best_guess   = 0;
    int x_of_best_guess = 0;

    for (int kg = 0; kg < 255; ++kg){
        double temp_diff  = 0;
        double local_diff = 0;

        double* hw_lst = malloc(nb_traces * sizeof(double));
        // Loop sur toutes les traces pour contstuire hw_of_traces
        for(int s = 0; s < nb_traces; ++s){
            hw_lst[s] = HW[intermediate(textes_mat[bnum][s], kg)];
        }

        // Loop sur tout les échantillons x
        for (int x = poi -20; x < poi + 20; ++x){
            double s0 = 0; double s1 = 0;
            double len_s0 = 0.0; double len_s1 = 0.0;

            // Loop sur toutes les traces et separation en 2 groupes hw
            for (int s = 0; s < nb_traces ; ++s) {
                if   (hw_lst[s] <  5) { s0 += traces_mat[x][s]; len_s0 += 1 ; }
                else                  { s1 += traces_mat[x][s]; len_s1 += 1 ; }
                // else{printf("AAAAAAAAAAAAAAA\n\n");}
            }

            s0 /= len_s0 ;  s1 /= len_s1 ;
            
            temp_diff = fabsl(s0 - s1);
            if (temp_diff > local_diff) {
                local_diff = temp_diff;
                x_of_best_guess = x;
            }
        }

        free(hw_lst);

        if(temp_diff > true_diff){
            true_diff = temp_diff;
            best_guess = kg;
        }
    }

  printf("Byte : %2d | diff : %f | x = %4d | kg/key = %3hhu",bnum, true_diff, x_of_best_guess, best_guess);
}

void bad_cpa(double** traces_mat, u_char** textes_mat, int nb_traces, int nb_echant, u_char* HW, u_char bnum){
     /*  Première Implémentation incorrecte de Correlation Power Analysis
        la méthode online du coefficient de pearson.             */

    double true_pears_max = 0;
    u_char kmax = 0;

    double temp_max = 0;
    double pears_xmax = 0;
    int xmax = 0;

    for (int kg = 0; kg < 255; ++kg){
        temp_max = 0;
        pears_xmax = 0;
        xmax = 0;

        // Loop sur tout les échantillons x
        for (int x = 0; x < nb_echant; ++x){
            double hw_list[9] = {0};
            int hw_count[9] = {0}; //Pour faire la moyenne

            u_char temp_txt_byte;
            u_char temp_hw;

            // Loop sur un même x pour trouver la plus grande correlation r
            for(int s = 0; s < nb_traces; ++s){
                temp_txt_byte = textes_mat[bnum][s];
                temp_hw = HW[intermediate(temp_txt_byte, kg)];

                // somme des valeurs x pour le hw donné
                hw_list[temp_hw] += traces_mat[x][s]; 
                hw_count[temp_hw] += 1;
            }

            // Division par le nombre de traces dans chaque hw
            // Pour avoir la moyenne des x pour les hw donnés.
            for (int h = 0; h < 9; ++h) hw_list[h] /=  hw_count[h];
            
            // temp_max = fabs(hw_pearsonr2(hw_list));
            temp_max = fabs(hw_pearson(hw_list));

            if(temp_max > pears_xmax){
                pears_xmax = temp_max;
                xmax = x;
            }
        }

        if(pears_xmax > true_pears_max){
            true_pears_max = pears_xmax;
            kmax = kg;
        }
    }

  printf("Byte : %2d | corr : %f | x = %4d | kg = %hhu\n",bnum, true_pears_max, xmax, kmax);
}


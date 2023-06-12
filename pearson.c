#include <math.h>
#include <stdio.h>

// Fonctions Maths
double moy(double* x, int n){
    // Calcule la moyenne des élements d'une liste x
    double sum = 0;
    for(int i = 0; i < n ; i++) sum += x[i];
    return sum / n;
}



double hw_pearson(double* hw_list){
  // première implémentation de pearson
    double mx = moy(hw_list, 9);
    int my = 4; // 4 est la moyenne de range(0,9)
    double a = 0;
    double b = 0;
    double asq = 0;
    double bsq = 0;
    double ab = 0;
    
    for(int i = 0; i < 9 ; i++){
        a   = hw_list[i] - mx;        
        b   = i - my;     
        ab  += a * b;
        asq += a * a;
        bsq += b * b;
    }

    return (ab) / (sqrt(asq) * sqrt(bsq));
}


double full_pearson(double* hw, double* Pw, int S){
    /* Implémentation de Pearson r */

    // Calcul des moyennes de hw et Pw
    double moy_hw = 0; double moy_Pw = 0;
    for(int s = 0; s < S ; s++){ moy_hw += hw[s]; moy_Pw += Pw[s];}
    moy_hw /= S; moy_Pw /= S;

    // Initialisation des variables
    double a   = 0; double b   = 0;
    double asq = 0; double bsq = 0;
    double ab = 0;

    for(int s = 0; s < S ; ++s){
        // Déreferrence des fonctions en [s] dans a et b
        a   = hw[s] - moy_hw;        
        b   = Pw[s] - moy_Pw;     

        // Incrémentation des sommes
        ab  += a * b;
        asq += a * a;
        bsq += b * b;
    }

    // Retour en valeur absolue
    return fabs((ab) / (sqrt(asq * bsq)));
}


double online_pearson(double* hw, double* Pw, int S){
    // Implémentation pour des tailles arbitraires de n
    // Où est est le nombre de traces
    double a = 0; double b = 0;
    double sum_a = 0; double sum_b = 0;
    double sum_asq = 0; double sum_bsq = 0;
    double sum_ab = 0;
    double num = 0; double denum = 0;

    for(int s = 0; s < S ; ++s){
        a = hw[s];
        b = Pw[s];
        
        sum_a += a;
        sum_b += b;
        sum_ab  += a * b;
        sum_asq += a * a;
        sum_bsq += b * b;
    }

    num = S * sum_ab - sum_a * sum_b;
    denum =  (sum_a * sum_a - S * sum_asq) *  (sum_b * sum_b - S * sum_bsq);
    return fabs(num / sqrt(denum));
}



void online_pearson_by_batch(double* hw, double* Pw, int S, int tot, double* sum_a_pt, double* sum_b_pt,
                             double* sum_ab_pt, double* sum_asq_pt, double* sum_bsq_pt, int save_cond){
    /* Implémentation un peu plus complexe que les précedentes
       Nous travaillons ici par batch, pour garder la continuité des variables
       nous faisons l'itération sur x ici.*/
    double a = 0; double b = 0;
    double num = 0; double denum = 0;

    // Pour ne pas avoir à passer par références pendant la somme
    double sum_a   = *sum_a_pt;    double sum_b   = *sum_b_pt;
    double sum_asq = *sum_asq_pt;  double sum_bsq = *sum_bsq_pt;
    double sum_ab  = *sum_ab_pt;

    // printf("sum a %f\n",*sum_a_pt);

    for(int s = 0; s < S ; ++s){
        a = hw[s]; b = Pw[s];
        sum_a   += a     ;  sum_b   += b     ;
        sum_asq += a * a ;  sum_bsq += b * b ;
        sum_ab  += a * b ;
    }

    // // Enfin réecrire les valeurs sur les pointeurs
    if  (save_cond == 1) {
        *sum_a_pt   = sum_a   ; *sum_b_pt   = sum_b   ;
        *sum_asq_pt = sum_asq ; *sum_bsq_pt = sum_bsq ;
        *sum_ab_pt  = sum_ab  ;
    }

}

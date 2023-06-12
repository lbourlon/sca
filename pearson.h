double moy(double* x, int n);
double hw_pearson(double* hw_list);
double full_pearson(double* x, double* y, int N);
double online_pearson(double* x, double* y, int N);
void online_pearson_by_batch( double* hw, double* Pw, int S, int tot, double* sum_a_pt, double* sum_b_pt,
                            double* sum_ab_pt, double* sum_asq_pt, double* sum_bsq_pt, int save_cond);

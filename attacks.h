
#include <sys/types.h>
u_char normal_cpa(double** traces_mat, u_char** textes_mat, int nb_traces, int nb_echant, u_char* HW, u_char bnum);
u_char online_cpa(double** traces_mat, u_char** textes_mat, int nb_traces, int nb_echant, u_char* HW, u_char bnum);
u_char mem_cpa(char* traces_raw, char* textes_raw, int nb_traces, int nb_echant, u_char* HW, u_char bnum);
void dpa(double** traces_mat, u_char** textes_mat, int nb_traces, int nb_echant, u_char* HW, u_char bnum, int poi);
void bad_cpa(double** traces_mat, u_char** textes_mat, int nb_traces, int nb_echant, u_char* HW, u_char bnum);
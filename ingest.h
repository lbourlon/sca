
#include <sys/types.h>
u_char* ingest_key(char* keys_raw);
void raw_ingest(char* traces_file, char* textes_file, double*** traces_mat, u_char*** textes_mat, int* nb_traces, int* nb_echant);
void raw_ingest_mem(char* traces_file, char* textes_file, double*** traces_mat, u_char*** textes_mat, int in_nb_traces, int in_nb_echant);
void raw_ingest_borne(char* traces_file, char* textes_file, double*** traces_mat, u_char*** textes_mat, int start_sp, int end_sp, int start_tr, int end_tr, int print_bool);
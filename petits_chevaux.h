#ifndef PROJET_H_INCLUDED
#define PROJET_H_INCLUDED

typedef struct position{
	int position_x;
	int position_y;
}position;

int height = 15;
int width = 15;
bool gagnant = false;
int nombre_joueur = 4;
int **pipes;
int premier_joueur;
int prochain_joueur;
int precedent_joueur;
int res_du_tirage_the;
position* position_joueur;
bool* sortie_pions;

int joueur_gagnant;
time_t t;

int pipe4_wr;
int pipe5_wr;
int pipe6_wr;

char** init_terrain();
void  print_terrain(char** terrain);
void init_joueur();
void init_pipes();
void init_pions();
int tirage_the();
void premier_joue();
int num_prochain_joueur();
void pions_sortie();
int joue_une_fois();
void change_position();
void change_terrain(char** terrain);



#endif

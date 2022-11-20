#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#ifdef PROJET_H_INCLUDED
#include "projet.h"
#endif

//Initialisation les variables globales

//Definition
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




char** init_terrain(){
	char** terrain;
	terrain = (char**)malloc(height*sizeof(char*));
    for(int i = 0; i < (height+1); i++){
        terrain[i] = (char*)malloc(width*sizeof(char));
    }
	
    	for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
            	if(((i==6)||(i==8))&&(j!=7)){
            	    terrain[i][j] = '*';
            	}
            	else if(((j==6)||(j==8))&&(i!=7)){
            	    terrain[i][j] = '*';
            	}
            	else if(i==0||i==14||j==0||j==14){
            	    terrain[i][j] = '*';
            	}
            	//zone joueur zero
            	else if((i==2&&j==3)||(i==2&&j==2)||(i==3&&j==3)||(i==3&&j==2)){
                    terrain[i][j] = 'Z';
            	}
            	//zone joueur un
            	else if((i==11&&j==3)||(i==11&&j==2)||(i==12&&j==3)||(i==12&&j==2)){
                    terrain[i][j] = 'U';
            	}
            	//zone joueur deux
            	else if((i==3&&j==12)||(i==3&&j==11)||(i==2&&j==12)||(i==2&&j==11)){
                    terrain[i][j] = 'D';
            	}
            	//zone joueur trois
            	else if((i==12&&j==12)||(i==12&&j==11)||(i==11&&j==12)||(i==11&&j==11)){
                    terrain[i][j] = 'T';
            	}
            	
            	
            	else{
            	    terrain[i][j] = ' ';
            	}
            	
            	
            }
    }
    return terrain;
}

void  print_terrain(char** terrain){
	for(int i = 0;i < height; i++){
	    for(int j = 0; j < width; j++){
	    	printf("%c",terrain[i][j]);
	    }
	    printf("\n");
	}
	sleep(1);
}

void init_joueur(){
    
    printf("Vous êtes quatre joueurs!");
    position_joueur = (position*)malloc(nombre_joueur * sizeof(position));
    position_joueur[0].position_x=0;
    position_joueur[0].position_y=6;
    position_joueur[1].position_x=8;
    position_joueur[1].position_y=0;
    position_joueur[2].position_x=6;
    position_joueur[2].position_y=14;
    position_joueur[3].position_x=14;
    position_joueur[3].position_y=8;
    
    printf("Les joueur est bien initialise...\n");
}

/*
exemple de 4 joueurs:
pour initialiser les pipes :
    pipe0: pere → Joueur_0
    pipe1: pere → Joueur_1
    pipe2: pere → Joueur_2
    pipe3: pere → Joueur_3


                             pipe4
                             			→→→
                Joueur_0 ================ Joueur_1
                ||                          ||
                ||                          ||
                ||                          ||
         pipe7  ||          Pere            ||  pipe5
           ↑           ||                          ||    ↓
           ↑    		||                          ||    ↓
           ↑    		||                          ||    ↓
                ||                          ||
                Joueur_3 ================== Joueur_2
                           			 ←←←
                            pipe6


*/

void init_pipes(){
    pipes = (int**)malloc((nombre_joueur*2)*sizeof(int*));
    for(int i = 0; i < (nombre_joueur*2); i++){
        pipes[i] = (int*)malloc(2*sizeof(int));
        pipe(pipes[i]);
    }
    printf("Pipes est bien initialise...\n");

}

void init_pions(){
	sortie_pions = (bool*)malloc((nombre_joueur)*sizeof(bool));
	 for(int i = 0; i < nombre_joueur; i++){
        //sortie_pions[i] = false;
        sortie_pions[i] = true;
    }
}

int tirage_the(){
    return (rand()%5+1);
}

//saisir le numero du premier joueur par clavier
//le numero est valide quand il est entre[1,nombre_joueur]
void premier_joue(){
    printf("Le numero du premier joueur?\n");
    scanf("%d",&premier_joueur);
    while ((premier_joueur > 3)||(premier_joueur < 0)){
        printf("Le numero n'a pas valide, saisir un nouvel nombre.\n");
        scanf("%d",&premier_joueur);
    }
    prochain_joueur = premier_joueur;
    printf("le joueur prochain est bien joueur %d\n", prochain_joueur);
}

//si la resultat est 6, on peux rejouer
int num_prochain_joueur(){
	int tmp;
	if(res_du_tirage_the == 6){
		tmp = prochain_joueur;
	}
	else{
		tmp = ((1 + prochain_joueur) % nombre_joueur);
	}
    return tmp;
}


void pions_sortie(){
	if(res_du_tirage_the == 6){
		sortie_pions[precedent_joueur] = true;
		printf("Joueur %d est sorti son pion!",precedent_joueur);
	}


}


int joue_une_fois(){
    printf("c'est le joueur %d qui va jouer!\n",prochain_joueur);
    res_du_tirage_the = tirage_the();
    precedent_joueur = prochain_joueur;
    pions_sortie();
    printf("Le joueur %d a bien joue, est obtenu %d\n",precedent_joueur,res_du_tirage_the);
    prochain_joueur = num_prochain_joueur();
    //fils 1
    pid_t pid1;
    pid1 = fork();
    if(pid1 == -1){
    	printf("probleme de creation");
    	exit(-1);
    }
    else if(pid1 == 0){
    	close(pipes[0][0]);
    	write(pipes[0][1],&prochain_joueur,sizeof(int));
    
    
    	close(pipes[0][1]);
    	read(pipes[0][0],&prochain_joueur,sizeof(int));
    	
    	printf("Je suis joueur 1,je suis d'accord le joueur prochain est %d\n",prochain_joueur);
    	
    	close(pipes[0+nombre_joueur][0]);
    	write(pipes[0+nombre_joueur][1],&precedent_joueur,sizeof(int));
    	
    	printf("Je suis joueur 1,je suis d'accord le joueur precedent est %d\n",precedent_joueur);
    	
    	printf("1:ppid=%d\n",getppid());
    	exit(0);
    }

    
    //fils 2
    pid_t pid2;
    pid2 = fork();
    if(pid2 == -1){
    	printf("probleme de creation");
    	exit(-1);
    }
    else if(pid2 == 0){
    
    	close(pipes[1][0]);
    	write(pipes[1][1],&prochain_joueur,sizeof(int));
    
    	close(pipes[1][1]);
    	read(pipes[1][0],&prochain_joueur,sizeof(int));
    	
    	printf("Je suis joueur 2,je suis d'accord le joueur prochain est %d\n",prochain_joueur);
    	
    	close(pipes[0+nombre_joueur][1]);
    	read(pipes[0+nombre_joueur][0],&pipe4_wr,sizeof(int));
    	
    	close(pipes[1+nombre_joueur][0]);
    	write(pipes[1+nombre_joueur][1],&pipe4_wr,sizeof(int));
    	
    	printf("Je suis joueur 2,je suis d'accord le joueur precedent est %d\n",precedent_joueur);
    	
    	printf("2:pid=%d\n",getppid());
    	
    	exit(0);
    }

    //fils 3
    pid_t pid3;
    pid3 = fork();
    if(pid3 == -1){
    	printf("probleme de creation");
    	exit(-1);
    }
    else if(pid3 == 0){
    	close(pipes[2][0]);
    	write(pipes[2][1],&prochain_joueur,sizeof(int));
    	
    	close(pipes[2][1]);
    	read(pipes[2][0],&prochain_joueur,sizeof(int));
    	
    	printf("Je suis joueur 3,je suis d'accord le joueur prochain est %d\n",prochain_joueur);
    	
    	close(pipes[1+nombre_joueur][1]);
    	read(pipes[1+nombre_joueur][0],&pipe5_wr,sizeof(int));
    	
    	close(pipes[2+nombre_joueur][0]);
    	write(pipes[2+nombre_joueur][1],&pipe5_wr,sizeof(int));
    	
    	printf("Je suis joueur 3,je suis d'accord le joueur precedent est %d\n",precedent_joueur);
    	
    	printf("3:pid=%d\n",getppid());
    	
    	exit(0);
    }
    //fils 4
    pid_t pid4;
    pid4 = fork();
    if(pid4 == -1){
    	printf("probleme de creation");
    	exit(-1);
    }
    else if(pid4 == 0){
    
    	close(pipes[3][0]);
    	write(pipes[3][1],&prochain_joueur,sizeof(int));
    	
    	close(pipes[3][1]);
    	read(pipes[3][0],&prochain_joueur,sizeof(int));
    	
    	printf("Je suis joueur 4,je suis d'accord le joueur prochain est %d\n",prochain_joueur);
    	
    	close(pipes[2+nombre_joueur][1]);
    	read(pipes[2+nombre_joueur][0],&pipe6_wr,sizeof(int));
    	
    	printf("Je suis joueur 4,je suis d'accord le joueur precedent est %d\n",precedent_joueur);
    	
    	printf("4:pid=%d\n",getppid());
    	
    	exit(0);
    }

    
	while(wait(NULL) != -1);
	return 0;

}




void change_position(){
		if(precedent_joueur == 0){ 
			if(sortie_pions[0] == true){
				for(int k  = 0;k < res_du_tirage_the; k++){
					if((position_joueur[0].position_y == 6)&&((position_joueur[0].position_x >=0)&&(position_joueur[0].position_x <=5))){
						position_joueur[0].position_x +=1;
					}
					else if((position_joueur[0].position_x == 6)&&((position_joueur[0].position_y >= 1)&&(position_joueur[0].position_y <=6))){
						position_joueur[0].position_y -= 1;
					}
					else if((position_joueur[0].position_x == 6)&&(position_joueur[0].position_y == 0)){
						position_joueur[0].position_x +=1;
					}
					else if((position_joueur[0].position_x == 7)&&((position_joueur[0].position_y >= 0)&&(position_joueur[0].position_y <=6))){
						position_joueur[0].position_y += 1;
						if((position_joueur[0].position_x == 7)&&(position_joueur[0].position_y == 7)){
							gagnant = true;
							joueur_gagnant = 0;
						}
					}
					
				}
				
				printf("Je deplace\n");
			}
			else{
				printf("Joueur 0 n'est pas encore sotir son pion!");
			}
		}	
		else if(precedent_joueur == 1){
			if(sortie_pions[1] == true){
				for(int k  = 0;k < res_du_tirage_the; k++){
					if((position_joueur[1].position_x == 8)&&((position_joueur[1].position_y >= 0)&&(position_joueur[1].position_y <=5))){
						position_joueur[1].position_y +=1;
					}
					else if((position_joueur[1].position_y == 6)&&((position_joueur[1].position_x >=8)&&(position_joueur[1].position_x <=13))){
						position_joueur[1].position_x +=1;

					}
					else if((position_joueur[1].position_x == 14)&&(position_joueur[1].position_y == 6)){
						position_joueur[1].position_y +=1;
					}
					else if((position_joueur[1].position_y == 7)&&((position_joueur[1].position_x >=8)&&(position_joueur[1].position_x <=14))){
						position_joueur[1].position_x -=1;
						if((position_joueur[1].position_x == 7)&&(position_joueur[1].position_y == 7)){
								gagnant = true;
								joueur_gagnant = 1;
						}
					}
				}
				printf("Je deplace\n");
			}
			else{
				printf("Joueur 1 n'est pas encore sotir son pion!");
			}
		}
		else if(precedent_joueur == 2){
			if(sortie_pions[2] == true){
				for(int k  = 0;k < res_du_tirage_the; k++){
					if((position_joueur[2].position_x == 6)&&((position_joueur[2].position_y >=9)&&(position_joueur[2].position_x <=14))){
						position_joueur[2].position_y -=1;
					}
					else if((position_joueur[2].position_y == 8)&&((position_joueur[2].position_x >=1)&&(position_joueur[2].position_x <=6))){
						position_joueur[2].position_x -=1;
					}
					else if((position_joueur[2].position_x==0)&&(position_joueur[2].position_y==8)){
						position_joueur[2].position_y -=1;
					}
					else if((position_joueur[2].position_y == 7)&&((position_joueur[2].position_x >= 0)&&(position_joueur[2].position_x <=6))){

						position_joueur[2].position_x +=1;
						if((position_joueur[2].position_x == 7)&&(position_joueur[2].position_y == 7)){
								gagnant = true;
								joueur_gagnant = 2;
							}
					}
				}
				printf("Je deplace\n");
			}
			else{
				printf("Joueur 2 n'est pas encore sotir son pion!");
			}
		}
		else if(precedent_joueur == 3){
			if(sortie_pions[2] == true){
				for(int k  = 0;k < res_du_tirage_the; k++){
					if((position_joueur[3].position_y == 8)&&((position_joueur[3].position_x >=9)&&(position_joueur[3].position_x <=14))){
						position_joueur[3].position_x -=1;
					}
					else if((position_joueur[3].position_x == 8)&&((position_joueur[3].position_y >=8)&&(position_joueur[3].position_y <=13))){
						position_joueur[3].position_y +=1;
					}
					else if((position_joueur[3].position_x == 8)&&(position_joueur[3].position_y == 14)){
						position_joueur[3].position_x -= 1;
					}
					else if((position_joueur[3].position_x == 7)&&((position_joueur[3].position_y >= 8)&&(position_joueur[3].position_y <= 14))){
						position_joueur[3].position_y -= 1;
						if((position_joueur[3].position_x == 7)&&(position_joueur[3].position_y == 7)){
								gagnant = true;
								joueur_gagnant = 3;
							}
					}

				}
				printf("Je deplace\n");
			}
			else{
				printf("Joueur 3 n'est pas encore sotir son pion!");
			}
		}

}




void change_terrain(char** terrain){
		terrain[position_joueur[0].position_x][position_joueur[0].position_y] = 'Z';
		terrain[position_joueur[1].position_x][position_joueur[1].position_y] = 'U';
		terrain[position_joueur[2].position_x][position_joueur[2].position_y] = 'D';
		terrain[position_joueur[3].position_x][position_joueur[3].position_y] = 'T';
}

void main(){
	
	srand((unsigned) time(&t));
	
    char **terrain;
    terrain = init_terrain();
    init_joueur();
    init_pipes();
    init_pions();
    premier_joue();
    print_terrain(terrain);
   

    while(gagnant == false){
    	joue_une_fois();
    	
    	change_position();
    	terrain = init_terrain();
    	change_terrain(terrain);
    	
    	print_terrain(terrain);
    	
    	
    }
    printf("Joueur %d est gagne",joueur_gagnant);
    
    
 
	
}





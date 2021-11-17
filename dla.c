/*
andre.dc@ua.pt 
@16/11/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//https://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
#define BOLD   "\x1B[1m"		
#define BLINK  "\x1B[5m"		
#define BLK   "\x1B[30m"		
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define BBLK   "\x1B[40m"		
#define BRED   "\x1B[41m"
#define BGRN   "\x1B[42m"
#define BYEL   "\x1B[43m"
#define BBLU   "\x1B[44m"
#define BMAG   "\x1B[45m"
#define BCYN   "\x1B[46m"
#define BWHT   "\x1B[47m"


#if 0

struct Particle{
	int number;
	int neighbours[10];
	double position[3];
	struct Particle *next;
}
#endif

void screenPrint(int *, int, int, int, int);
void randomWalk(int, int *, int, int);
int radDistance(float, int *, int);

int main(){
	int j, i = 1, nParticles = 500, stepSize = 1, nSteps = 1000, skipFrames = 1;
	int xLim = 80, yLim = 40;
	int nPositions[nParticles][2];
	nPositions[0][0] = xLim / 2;
	nPositions[0][1] = yLim / 2;
	int (*pnPositions)[2]= nPositions; 
	srand(time(NULL));

	while (i < nParticles) {
		// criar posições random para as "i" particulas
		*(*(pnPositions+i)+0) = rand() % xLim;
		*(*(pnPositions+i)+1) = rand() % yLim;
		for (j = 0; j <= nSteps; j++){
			if (!radDistance(1, *pnPositions, i)){ 
				randomWalk(stepSize, *(pnPositions+i), xLim, yLim);
				if (j == nSteps){
					// Descartar particulas que não tenham agregado em nSteps
					*(*(pnPositions+i)+0) = -9; 
					*(*(pnPositions+i)+1) = -9;	
				}
				if (j % skipFrames == 0){ // adicionar como opção a argv
					screenPrint(*pnPositions, xLim, yLim, j, i);
					// usleep(50000);
				}
			}
			else break;
		}
		i++;		
	}
	return 0;
}


int radDistance(float radius, int *particlesPosition,int particleNum){
	int i, neighbours = 0;
	for (i = 0; i < particleNum; i++){
		float distance;
		distance = sqrt( ((*(particlesPosition+(2*i))) - (*(particlesPosition+(2*particleNum)))) * ((*(particlesPosition+(2*i))) - (*(particlesPosition+(2*particleNum)))) + ((*(particlesPosition+(2*i+1))) - (*(particlesPosition+(2*particleNum+1)))) *	((*(particlesPosition+(2*i+1))) - (*(particlesPosition+(2*particleNum+1)))) );
		(distance <= radius) ?  neighbours++ : 0;
	}
	return neighbours; // True ou False chegava mas queria ter uma maneira de contabilizar evolução das vizinhanças no futuro
}

void randomWalk(int stepSize, int *position, int xLim, int yLim){
	int move = rand() % 8;
	switch (move) {
		case 0: position[0] -= stepSize; break;
		case 1: position[0] += stepSize; break;
		case 2: position[1] -= stepSize; break;
		case 3: position[1] += stepSize; break;
		case 4: position[0] += stepSize; position[1] += stepSize; break;
		case 5: position[0] += stepSize; position[1] -= stepSize; break;
		case 6: position[0] -= stepSize; position[1] += stepSize; break;
		case 7: position[0] -= stepSize; position[1] -= stepSize; break;	
		default: exit(1);
	}
	position[0]=abs(position[0] % xLim); //não está a funcionar como queria... adicionei abs() para não deixar sair da caixa para z < zLim 
	position[1]=abs(position[1] % yLim);
}

void screenPrint(int *position, int xLim, int yLim, int step, int particle){
		int i, x, y;
		system("clear");
		for (y = yLim; y >= -1; y--){
			for (x = -1; x <= xLim+1; x++){
				for (i = 1; i < particle; i++){
					if (*(position+(2*i)) == x && *(position+(2*i+1)) == y){ //isto está a dar barraca com adição de espaços onde não deviam... ainda não sei como dar a volta ao problema.
						printf(BOLD BLINK BYEL GRN "%c" RESET, '#');}
				}
				if (*(position+0) == x && *(position+1) == y){
						printf(BOLD BLINK BCYN BLK "%c" RESET, 'X');}
				else if (*(position+(2*particle)) == x && *(position+(2*particle+1)) == y){
					printf(BOLD BLINK BRED BLU "%c" RESET, '0');}
				else if (x == -1 || x == xLim){
					printf("|");}	
				else if (x == xLim+1){
					printf("\n");}
				else if (y == -1 || y == yLim){
					printf("-");}
				else{ printf(" ");}	
			}
		}
		printf("\nposition: (%d, %d), step: %d, particle: %d\n", *(position+(2*particle+0)), *(position+(2*particle+1)), step, particle);
}
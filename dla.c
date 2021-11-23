/*
andre.dc@ua.pt 
@16/11/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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
#define BBLK   "\x1B[48;5;232m"	// "\x1B[40m"
#define BRED   "\x1B[41m"
#define BGRN   "\x1B[42m"
#define BYEL   "\x1B[43m"
#define BBLU   "\x1B[44m"
#define BMAG   "\x1B[45m"
#define BCYN   "\x1B[46m"
#define BWHT   "\x1B[47m"

#if 0
Adicionar argv para selecionar:
- ver corrida
- ver agregação final
- gravar pdb
#endif 

void screenPrint(int *, int, int, int, int);
void randomWalk(int, int *, int, int);
int radDistance(float, int *, int);

int main(){
	int j, kia = 0, i = 1, nParticles = 2000, stepSize = 1, nSteps = 1500, skipFrames = 1, xLim = 100, yLim = 55 ;
	int nPositions[nParticles][2];
	nPositions[0][0] = xLim / 2;
	nPositions[0][1] = yLim / 2;
	int (*pnPositions)[2]= nPositions;
	double x, y, r, theta;
	int seed = time(NULL);
	srand(seed);

	while (i < nParticles) {
		// criar posições random para as particulas a um determinado raio do centro
		// https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
		r = sqrt(((rand() % 10001) / 10000.0) * (((xLim+yLim)/3)*((xLim+yLim)/3) - ((xLim+yLim)/6)*((xLim+yLim)/6)) + ((xLim+yLim)/6)*(((xLim+yLim)/6)));
  		theta = ((rand() % 10001) / 10000.0) * 2 * 3.1415;
  		x = xLim/2 + r * cos(theta);
  		y = yLim/2 + r * sin(theta);
		*(*(pnPositions+i)+0) = (int) x;//rand() % xLim;
		*(*(pnPositions+i)+1) = (int) y;//rand() % yLim;
		for (j = 0; j <= nSteps; j++){
			if (!radDistance(1.42, *pnPositions, i)){ 
				randomWalk(stepSize, *(pnPositions+i), xLim, yLim);
				if (j == nSteps){
					// Descartar particulas que não tenham agregado em nSteps
					*(*(pnPositions+i)+0) = -xLim; 
					*(*(pnPositions+i)+1) = -yLim;	
					kia += 1;
				}
				// if (j % skipFrames == 0){ // adicionar como opção a argv
				// 	screenPrint(*pnPositions, xLim, yLim, j, i);
				// 	// usleep(50000);
				// }
			}
			else break;
		}
		i++;		
	}
	screenPrint(*pnPositions, xLim, yLim, nSteps, nParticles);
	printf(RESET"\n______________________________________________________________________\n");
	printf("Box: [%d, %d],  N. particles: %d,  Particles destroyed: %d\nRandom seed: %d,  Stepsize: %d,  N. steps: %d\n", xLim, yLim, nParticles, kia, seed, stepSize, nSteps);
	return 0;
}


int radDistance(float radius, int *particlesPosition,int particleNum){
	int i, neighbours = 0;
	for (i = 0; i < particleNum; i++){
		float distance;
		distance = sqrt( ((*(particlesPosition+(2*i))) - (*(particlesPosition+(2*particleNum)))) * ((*(particlesPosition+(2*i))) - (*(particlesPosition+(2*particleNum)))) + ((*(particlesPosition+(2*i+1))) - (*(particlesPosition+(2*particleNum+1)))) * ((*(particlesPosition+(2*i+1))) - (*(particlesPosition+(2*particleNum+1)))) );
		(distance <= radius) ?  neighbours++ : 0;
	}
	return neighbours; 
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
	// position[0]=(position[0] % xLim); //não está a funcionar como queria... 
	// position[1]=(position[1] % yLim);
}

void screenPrint(int *position, int xLim, int yLim, int step, int particle){
		int i, x, y;
		system("clear");
		for (y = yLim; y >= -1; y--){
			for (x = -1; x <= xLim+1; x++){
				for (i = 1; i < particle; i++){
					if (*(position+(2*i)) == x && *(position+(2*i+1)) == y){ 
						printf(BOLD BLINK BGRN YEL "%c" BBLK, '#'); goto skipSpace;}
				}
				if (*(position+0) == x && *(position+1) == y){
						printf(BOLD BLINK BYEL BLK "%c" BBLK, 'X');}
				else if (*(position+(2*particle)) == x && *(position+(2*particle+1)) == y){
					printf(BOLD BLINK BRED BLU "%c" BBLK, '0');}
				// else if (x == -1 || x == xLim){
				// 	printf(RESET BBLK YEL "|");}	
				else if (x == xLim+1){
					printf("\n");}
				// else if (y == -1 || y == yLim){
				// 	printf(RESET BBLK YEL "-");}
				else{ printf(RESET BBLK YEL" ");}	
				skipSpace:
			}
		}
		// printf(RESET"\nposition: (%d, %d), step: %d, particle: %d\n", *(position+(2*particle+0)), *(position+(2*particle+1)), step, particle);
}

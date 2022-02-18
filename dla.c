/*
Trabalho extra para AED 2021/2022

Simulador da agregação limitada por difusão 
v1.0

andre.dc@ua.pt 
@16/02/2022
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Symbol modifiers
#define BOLD   "\x1B[1m"		
#define BLINK  "\x1B[5m"	
//Symbol colors
#define BLK   "\x1B[30m"		
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
//Background colors
#define BBLK   "\x1B[48;5;232m"	// "\x1B[40m"
#define BRED   "\x1B[41m"
#define BGRN   "\x1B[42m"
#define BYEL   "\x1B[43m"
#define BBLU   "\x1B[44m"
#define BMAG   "\x1B[45m"
#define BCYN   "\x1B[46m"
#define BWHT   "\x1B[47m"
//Reset color and modifiers
#define RESET "\x1B[0m"

#define XLIM 79
#define YLIM 34
#define NSTEPS 1500
#define STEPSIZE 1
#define NPARTICLES 500
#define SKIPFRAMES 1
#define REALTIMERUN 0
#define PBC 0

#if 0
Por fazer:
- opção para escolher entre corrida 2D ou 3D
- opção para gravar pdb da trajétoria
- opção para gravar ficheiro para impressão 3D
#endif 

void screenPrint(int *position, int xLim, int yLim, int step, int particle, char * filename);
void screen2file(int *position, int xLim, int yLim, int step, int particle, char * filename);
void randomWalk(int stepSize, int *position, int xLim, int yLim, int pbc);
int radDistance(float radius, int *particlesPosition, int particleNum);
double cpu_time(void);

int main(int argc, char ** argv){
    int seed = time(NULL);
    int j, kia = 0, i = 1, nParticles = NPARTICLES, stepSize = STEPSIZE, nSteps = NSTEPS, skipFrames = SKIPFRAMES, xLim = XLIM, yLim = YLIM, watch = REALTIMERUN, pbc = PBC, outOfbox = 1;

    for(int i = 1;i < argc;i++){
        if (i % 2 != 0){
            if(strncmp(argv[i],"-help", 2) == 0){
                printf(BOLD "\n\t\t|*** Difussion limited agreggation simulation ***|\n");
                printf("\t\t|v: 1.0\t\t16/2/2022\t   andre.dc@ua.pt|\n" RESET);
                printf("\nThis is an implementation of the Diffusion-limited aggregation (DLA) algorithm.\nDLA is the process whereby particles undergoing a random walk due to Brownian\nmotion cluster together to form fractal aggregates.\n");
                printf(BOLD "\nSYNOPSIS:" RESET "\ndla [-help] [-watch <int>] [-skipframes <int>] [-nparticles <int>] [-stepsize <int>] [-nsteps <int>]  [-xlim <int>] [-ylim <int>] [-seed <int>]\n");
                printf(BOLD "\nOPTIONS:" RESET"\n\n");
                printf(BOLD "-watch 0 = False (default = 0)" RESET "\nWatch aggregation live in CLI." RED BOLD " WARNING: PHOTOSENSITIVITY/EPILEPSY SEIZURES\n" RESET);
                printf("There are two visualization options:\n\t< 1 > shows the structure growth.\n \tAny other positive integer shows the full trajectory of particles. (very slow)\n\n");
                printf(BOLD "-skipframes <int> (default = 1)"RESET"\nSelects the number of frames to skip when the 'watch' is enabled.\n\n");
                printf(BOLD "-nparticles <int> (default = 500)"RESET"\nSelects the total number of particles to deploy.\n\n");
                printf(BOLD "-stepsize <int> (default = 1)"RESET"\nSelects the stepsize of which random walk.\n\n");
                printf(BOLD "-nsteps <int> (default = 1500)"RESET"\nDefine the total number of random walk steps before the particle is discarded.\n\n");
                printf(BOLD "-xlim <int> (default = 79)"RESET"\nLength of the box in 'x'.\n\n");
                printf(BOLD "-ylim <int> (default = 34)"RESET"\nLength of the box in 'y'.\n\n");
                printf(BOLD "-pbc 0 = False (default = 0)"RESET"\nWrap particule position around the box.\n\n");
                printf(BOLD "-out_of_box 0 = False (default = 1)"RESET"\nIf False, discard the particle when it gets out of the box.\n\n");
                printf(BOLD "-help <int> (default = 1)"RESET"\nPrint help and quit\n\n");
                exit(1);}
            else if(strncmp(argv[i],"-nparticles", 3) == 0) {nParticles = atoi(argv[i+1]);}
            else if(strncmp(argv[i],"-stepsize", 3) == 0) {stepSize = atoi(argv[i+1]);}
            else if(strncmp(argv[i],"-nsteps", 3) == 0) {nSteps = atoi(argv[i+1]);}
            else if(strncmp(argv[i],"-skipframes", 3) == 0) {skipFrames = atoi(argv[i+1]);}
            else if(strncmp(argv[i],"-xlim", 3) == 0) {xLim = atoi(argv[i+1]);}
            else if(strncmp(argv[i],"-ylim", 3) == 0) {yLim = atoi(argv[i+1]);}
            else if(strncmp(argv[i],"-seed", 3) == 0) {seed = atoi(argv[i+1]);}
            else if(strncmp(argv[i],"-watch", 2) == 0) {watch = atoi(argv[i+1]);}
            else if(strncmp(argv[i],"-pbc", 2) == 0) {pbc = atoi(argv[i+1]);}
            else if(strncmp(argv[i],"-out_of_box", 2) == 0) {outOfbox = atoi(argv[i+1]);}
            else {printf("Error! %s flag not supported\n", argv[i]); exit(1);}
        }
    }
    
    int count = nParticles;
    int nPositions[nParticles][2];
    nPositions[0][0] = xLim / 2;
    nPositions[0][1] = yLim / 2;
    int (*pnPositions)[2]= nPositions;
    double x, y, r, theta;
    srand(seed);

      char filename[50];
      sprintf(filename, "results_%d_%d_%d_%d_%d_%d", xLim, yLim, nParticles, stepSize, nSteps, seed);
      FILE *fout = fopen(filename, "a");
      if (fout == NULL)
      {
          printf("Error opening file!\n");
          exit(1);
      }

    while (count > 0) {
        // criar posições random para as particulas a um determinado raio do centro
        // https://stackoverflow.com/questions/13064912/generate-a-uniformly-random-point-within-an-annulus-ring
        r = sqrt(((rand() % 10001) / 10000.0) * (((xLim+yLim)/7)*((xLim+yLim)/7) - ((xLim+yLim)/4)*((xLim+yLim)/4)) + ((xLim+yLim)/4)*(((xLim+yLim)/4)));
          theta = ((rand() % 10001) / 10000.0) * 2 * 3.1415;
          x = xLim/2 + r * cos(theta);
          y = yLim/2 + r * sin(theta);
        *(*(pnPositions+i)+0) = (int) x;
        *(*(pnPositions+i)+1) = (int) y;
        int neighbours;
        for (j = 0; j <= nSteps; j++){
            neighbours = radDistance(1.42, *pnPositions, i);
            float distance2seed = sqrt( ((*(pnPositions+(2*i))) - (*(pnPositions+(2*0)))) * ((*(pnPositions+(2*i))) - (*(pnPositions+(2*0)))) + ((*(pnPositions+(2*i+1))) - (*(pnPositions+(2*0+1)))) * ((*(pnPositions+(2*i+1))) - (*(pnPositions+(2*0+1)))) );
            if (!neighbours){ // Enquanto não tem vizinhos move
                randomWalk(stepSize, *(pnPositions+i), xLim, yLim, pbc);
                if (!outOfbox){
                    if ( distance2seed > sqrt(((xLim/2) * (xLim/2)) + ((yLim/2) * (yLim/2))) ){  // Critério de paragem de acordo com uma distância à semente
                        kia += 1;
                        i--;
                        break;
                    }
                }
                if (j == nSteps){
                    // Descartar particulas que não tenham agregado em nSteps
                    // *(*(pnPositions+i)+0) = -xLim; // inicialmente não tinha o count
                    // *(*(pnPositions+i)+1) = -yLim; // as particulas descartadas eram movidas para fora da caixa mas ficavam no array de posições
                    kia += 1; 
                    i--; // evita ter que iterar sobre todo o array no screenprint, apenas as agregadas fazem parte do array de posições
                    break;
                }
                if (watch > 1){ // Ver trajetória em tempo real, não indicado a epiléticos
                    if (j % skipFrames == 0){ 
                         screenPrint(*pnPositions, xLim, yLim, j, i, filename);
                        usleep(50000);
                    }
                }
            }
            else break;
        }
        if (watch == 1){// Ver crescimento em tempo real.
            screenPrint(*pnPositions, xLim, yLim, j, i, filename);
            usleep(50000);
        }
        i++; 
        count--;	
    }
    screen2file(*pnPositions, xLim, yLim, nSteps, i, filename);
    // screenPrint(*pnPositions, xLim, yLim, nSteps, i, filename);
    // printf(RESET"\n______________________________________________________________________\n");
    // printf(RESET"Box: [%d, %d],  N. particles: %d,  Particles destroyed: %d\nRandom seed: %d,  Stepsize: %d,  N. steps: %d\n", xLim, yLim, nParticles, kia, seed, stepSize, nSteps);
    fprintf(fout, "Box (x, y), Total N. particles, Particles destroyed, N. Particles in Aggregate, Random seed,  Step size,  N. steps, simulation time (s)\n");
    fprintf(fout, "[%d,%d] %d %d %d %d %d %d %f", xLim, yLim, nParticles, kia, i, seed, stepSize, nSteps, dt);
    fclose(fout);
    return 0;
}

int radDistance(float radius, int *particlesPosition, int particleNum){
    int i, neighbours = 0;
    for (i = 0; i < particleNum; i++){
        float distance;
        distance = sqrt( ((*(particlesPosition+(2*i))) - (*(particlesPosition+(2*particleNum)))) * ((*(particlesPosition+(2*i))) - (*(particlesPosition+(2*particleNum)))) + ((*(particlesPosition+(2*i+1))) - (*(particlesPosition+(2*particleNum+1)))) * ((*(particlesPosition+(2*i+1))) - (*(particlesPosition+(2*particleNum+1)))) );
        (distance <= radius) ?  neighbours++ : 0;
    }
    return neighbours; 
}

void randomWalk(int stepSize, int *position, int xLim, int yLim, int pbc){
    int move = rand() % 8;
    switch (move) {
        case 0: position[0] -= stepSize; break;
        case 1: position[0] += stepSize; break;
        case 2: position[1] -= stepSize; break;
        case 3: position[1] += stepSize; break;
        case 4: position[0] += stepSize; position[1] += stepSize; break; // movimentos extra
        case 5: position[0] += stepSize; position[1] -= stepSize; break;
        case 6: position[0] -= stepSize; position[1] += stepSize; break;
        case 7: position[0] -= stepSize; position[1] -= stepSize; break;
        default: exit(1);
    }
    if (pbc){// condições periodicas de fronteira (screen wrap)
        position[0] %= xLim; 
        position[1] %= yLim;
        if (position[0] < 0) position[0]+=xLim;
        if (position[1] < 0) position[1]+=yLim;
    }
}

void screenPrint(int *position, int xLim, int yLim, int step, int particle, char * filename){
    int i, x, y;
    system("clear");
    for (y = yLim; y >= -1; y--){
        for (x = -1; x <= xLim+1; x++){
            for (i = 1; i < particle; i++){
                if (*(position+(2*i)) == x && *(position+(2*i+1)) == y){ 
                    printf(BOLD BLINK BGRN YEL "%c" BBLK, '#', RESET);
                    goto skipSpace;
                }
            }
            if (*(position+0) == x && *(position+1) == y){
                    printf(BOLD BLINK BYEL BLK "%c" BBLK, 'X', RESET);
            }
            else if (*(position+(2*particle)) == x && *(position+(2*particle+1)) == y){
                printf(BOLD BLINK BRED BLU "%c" BBLK, '0', RESET);
            }
            else if (x == -1 || x == xLim){
                printf(RESET BBLK YEL "|" RESET);
            }	
            else if (x == xLim+1){
                printf(RESET"\n" RESET);
            }
            else if (y == -1 || y == yLim){
                printf(RESET BBLK YEL "-" RESET);
            }
            else { 
                printf(RESET BBLK " " RESET);
            }	
            skipSpace:
        }
    }
    printf(RESET"\nposition: (%d, %d), step: %d, particle: %d \n", *(position+(2*particle+0)), *(position+(2*particle+1)), step, particle);
}

void screen2file(int *position, int xLim, int yLim, int step, int particle, char * filename){
    strcat(filename, "_graph");
    printf("%s\n",filename);
      FILE *foutGraph = fopen(filename, "a");
      if (foutGraph == NULL)
      {
          printf("Error opening file!\n");
          exit(1);
      }
    int i, x, y;
    system("clear");
    for (y = yLim; y >= -1; y--){
        for (x = -1; x <= xLim+1; x++){
            for (i = 1; i < particle; i++){
                if (*(position+(2*i)) == x && *(position+(2*i+1)) == y){ 
                    fprintf(foutGraph, BOLD BLINK BGRN YEL "%c" BBLK, '#', RESET);
                    goto skipSpace;
                }
            }
            if (*(position+0) == x && *(position+1) == y){
                    fprintf(foutGraph, BOLD BLINK BYEL BLK "%c" BBLK, 'X', RESET);
            }
            else if (*(position+(2*particle)) == x && *(position+(2*particle+1)) == y){
                fprintf(foutGraph, BOLD BLINK BRED BLU "%c" BBLK, '0', RESET);
            }
            else if (x == -1 || x == xLim){
                fprintf(foutGraph, RESET BBLK YEL "|" RESET);
            }
            else if (x == xLim+1){
                fprintf(foutGraph, "\n" RESET);
            }				
            else if (y == -1 || y == yLim){
                fprintf(foutGraph, RESET BBLK YEL "-" RESET);
            }
            else { 
                fprintf(foutGraph, RESET BBLK " " RESET);
            }	
            skipSpace:
        }
    }
    fclose(foutGraph);
}

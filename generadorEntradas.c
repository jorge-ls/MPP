#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char *argv[]) {

	int numTests = 0;
	int numTest = 1;
	int numFichero = 1;
	int numFicheros = 0;
	int numLineas = 0;
	int numEntradas = 0;
	int numLinea = 0;
	double tiempoEjec = 5.0;
	FILE * fp;
	fp = fopen("example-3.cov","w");
	printf("Introduce el numero de test a ejecutar:\n");
	scanf("%d",&numTests);
	printf("Introduce el numero de ficheros:\n");
	scanf("%d",&numFicheros);
	printf("Entradas de cobertura:\n");
	scanf("%d",&numEntradas);
	srand(time(NULL));
	int fileLineas[numFicheros];
	fprintf(fp,"%d\n",numTests);
	for (int i=0;i<numTests;i++){
		
		fprintf(fp,"%d %lf Test%d\n",numTest,tiempoEjec,numTest);
		numTest++;
	}
	fprintf(fp,"%d\n",numFicheros);
	for (int i=0;i<numFicheros;i++){
		numLineas = rand() % 21 + 30;
		fileLineas[i] = numLineas;
		fprintf(fp,"%d %d Fichero%d\n",numFichero,numLineas,numFichero);
		numFichero++;
	}
	fprintf(fp,"%d\n",numEntradas);
	for (int i=0;i<numEntradas;i++){
		numTest = rand() % numTests + 1;
		numFichero = rand() % numFicheros + 1;
		numLinea = rand() % fileLineas[numFichero-1] + 1;
		fprintf(fp,"%d %d %d\n",numTest,numFichero,numLinea);
	}
	

}

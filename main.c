#include <stdlib.h>
#include <stdio.h>
#include "reader.h"
#include "backtracking.h"

int main(int argc, char **argv)
{
  if (argc != 2) {
    printf("./main inputfile\n");
    return -1;
  }

  char *input = argv[1];
  
  FILE *f = fopen(input, "r");
  data *d = read(f);
  print_data(d);
  int nivel = 0; //nivel en el arbol
  int numTest = 0; //Numero de test ejecutados
  int minNumTest = 0; //Minimo numero de test ejecutados en la mejor solucion encontrada
  int maxCobertura = 0; //Maxima cobertura en la mejor solucion encontrada
  int numCoberturaTotal = 0; //Variable para llevar el conteo de las lineas cubiertas; 
  int numPosMaxCobertura = 0; //Indica la posicion en el array solucion en el que se ha llegado a la maxima cobertura
  int * solucion = (int *) malloc(sizeof(int) * d->num_cases); //Array donde se almacena la solucion actual
  int * mejorSolucion = (int *) malloc(sizeof(int) * d->num_cases); //Array donde se almacena la mejor solucion encontrada hasta el momento
  int * coberturas = (int *) malloc(sizeof(int) * d->num_cases); //Array donde se almacena el total de lineas cubiertas por cada test
  //Array donde se almacenan las lineas cubiertas
  line_coverage ** lineasCubiertas = (line_coverage **) malloc(sizeof(line_coverage *) * d->num_coverage);
  backtracking(d,solucion,mejorSolucion,d->num_cases,nivel,lineasCubiertas,numCoberturaTotal,numTest,minNumTest,maxCobertura
	,numPosMaxCobertura,coberturas);
  printf("Ordenacion óptima de los casos de prueba: \n");
  for (int i=0;i<d->num_cases;i++){
	printf("%d ",mejorSolucion[i]);
  }
  printf("\n");
  return 0;
}

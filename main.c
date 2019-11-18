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
  int numCoberturaTotal = 0; //Variable para llevar el conteo de las lineas cubiertas; 
  int * solucion = (int *) malloc(sizeof(int) * d->num_cases); //Array donde se almacena la solucion
  //Array donde se almacenan las lineas cubiertas
  line_coverage ** lineasCubiertas = (line_coverage **) malloc(sizeof(line_coverage *) * d->num_coverage);
  backtracking(d,solucion,d->num_cases,nivel,lineasCubiertas,numCoberturaTotal);
  return 0;
}

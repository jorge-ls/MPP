#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include <string.h>
#include <omp.h>
#include "vector.h"

#define NUM_THREADS 12


typedef struct {
  int nivel; //Nivel actual 
  int coberturaTotal; //Cobertura total de las lineas de codigo de la solucion encontrada
  int * solucion; //Array donde se va guardando la solución encontrada
  int * coberturas; //Array en el cual se indican las coberturas asociadas a la solución
  int * isCubierta; //Array que indica si una entrada de cobertura esta cubierta por un test
} tarea;
 

//Función auxiliar para comprobar si un caso de prueba es válido
int valido(int * solucion,int n,int i){
	for (int j=0;j<n;j++){
		if (solucion[j] == i) //Si el caso de prueba ya esta en el conjunto solución, no es válido
			return 0;
	}
	return 1;
}

//Funcion para escribir la mejor solucion encontrada hasta el momento
void escribirMejorSolucion(int * solucion,int n,int * mejorSolucion){
	for (int i=0;i<n;i++){
		mejorSolucion[i] = solucion[i];
	}
}

//Funcion para guardar las coberturas de la mejor solucion encontrada hasta el momento
void escribirMejoresCoberturas(int * coberturas,int n,int * mejoresCoberturas){
	for (int i=0;i<n;i++){
		mejoresCoberturas[i] = coberturas[i];
	}
}

//Funcion para obtener la cobertura de lineas de un test
int getCoberturaTest(tarea * r,data * d,int idTest){
	int cubierta;
	int numCobertura = 0;
	for (int i=0;i<d->num_coverage;i++){
		if (d->coverage[i]->id_test == idTest){
			cubierta = 0;
			for (int j=0;!cubierta && j<d->num_coverage;j++){
				if ((d->coverage[j]->id_test != idTest) && (d->coverage[j]->id_file == d->coverage[i]->id_file)
					&& (d->coverage[j]->line == d->coverage[i]->line) && r->isCubierta[j]){
					cubierta = 1;
				}		
			}
			if (!cubierta){//Si no se ha encontrado ningun otro test que ya haya cubierto la misma linea se cubre por idTest
				r->isCubierta[i] = 1;
				numCobertura++;			
			}
		}
	}
	return numCobertura;
		
}

//Se crea una nueva tarea
tarea * tarea_new(int nivel,int num_cases,int num_coverage) {
  tarea * t = (tarea *) malloc(sizeof(tarea));
  t->nivel = nivel;
  t->coberturaTotal = 0;
  t->solucion = (int *) malloc(sizeof(int) * num_cases);
  t->coberturas = (int *) malloc(sizeof(int) * num_cases);
  t->isCubierta = (int *) malloc(sizeof(int) * num_coverage);
  return t;
}

//Se añade un nuevo test a la tarea actual
tarea * addTest(data * d,int idTest,int nivel,tarea * actual)
{
  tarea *t = tarea_new(nivel,d->num_cases,d->num_coverage);
  memcpy(t->solucion, actual->solucion, sizeof(int) * d->num_cases);
  memcpy(t->coberturas, actual->coberturas, sizeof(int) * d->num_cases);
  memcpy(t->isCubierta,actual->isCubierta, sizeof(int) * d->num_coverage);
  /*for (int i = 0;i < d->num_cases;i++){
	t->solucion[i] = actual->solucion[i];
	t->coberturas[i] = actual->coberturas[i];
  }
  for (int i=0; i< d->num_coverage;i++){
	t->isCubierta[i] = actual->isCubierta[i];
  }*/
  t->solucion[nivel] = idTest;
  t->coberturaTotal = actual->coberturaTotal;
  int numCobertura = getCoberturaTest(t,d,idTest);
  t->coberturaTotal += numCobertura;
  t->coberturas[nivel] = t->coberturaTotal;
  return t;
}

//Función para comprobar si una solucion encontrada es mejor que la mejor solucion que habia hasta el momento, para ello se comparan las
//coberturas de ambas soluciones
int isMejorSolucion(int * coberturas,int * mejoresCoberturas,int n){
	int mejorSolucion = 0;
	for (int i=0;i<n;i++){
		if (coberturas[i] > mejoresCoberturas[i]){
			mejorSolucion = 1;
			return mejorSolucion;
		}
	}
	return mejorSolucion;
}

//Algoritmo de backtracking
void backtracking(data * d,tarea * t,int n,int nivel,int * mejorSolucion,int * mejoresCoberturas){

	if (nivel == n-1){ //Caso en el que llegamos al ultimo nivel
		for (int i=0;i<n;i++){
			if (valido(t->solucion,n,i+1)){
				tarea * r = addTest(d,i+1,nivel,t);
				#pragma omp critical (solucion)
				{
				if (isMejorSolucion(r->coberturas,mejoresCoberturas,n)){
					escribirMejorSolucion(r->solucion,n,mejorSolucion);
					escribirMejoresCoberturas(r->coberturas,n,mejoresCoberturas);
				}
				}
				free(r->solucion);
				free(r->coberturas);
				free(r->isCubierta);
				free(r);
			}	
		}
	}
	else {
		for (int i=0;i<n;i++){
			if (valido(t->solucion,n,i+1)){
				tarea * r = addTest(d,i+1,nivel,t); //Añade un nuevo test y se procesa en el arbol de busqueda
				backtracking(d,r,n,nivel+1,mejorSolucion,mejoresCoberturas);
				free(r->solucion);
				free(r->coberturas);
				free(r->isCubierta);
				free(r);		
			}
		}
	}
}

void printTarea(tarea * t,int tamSolucion,int tamCoverage){
	printf("Nivel: %d\n",t->nivel);
	printf("Cobertura total: %d\n",t->coberturaTotal);
	printf("Solucion:\n");
	for (int i=0;i<tamSolucion;i++){
		printf("%d ",t->solucion[i]);
	}
	printf("\n");
	printf("Coberturas:\n");
	for (int j=0;j<tamSolucion;j++){
		printf("%d ",t->coberturas[j]);
	}
	printf("\n");
	printf("Lineas cubiertas:\n");
	for (int k=0;k<tamCoverage;k++){
		printf("%d ",t->isCubierta[k]);
	}
	printf("\n");
}

void generarTareas(Vector * bolsa,data * d,tarea * t,int maxNivel,int nivelActual,int n){
	if (nivelActual == maxNivel - 1){
		for (int i=0;i<n;i++){
			if (valido(t->solucion,n,i+1)){
				tarea * r = addTest(d,i+1,nivelActual,t);
				r->nivel++;
				append(bolsa, r);
			}
		}
	}
	else{
		for (int i=0;i<n;i++){
			if (valido(t->solucion,n,i+1)){
				tarea * r = addTest(d,i+1,nivelActual,t);
				//#pragma omp task firstprivate(r)
				//{
					generarTareas(bolsa,d,r,maxNivel,nivelActual+1,n);
					free(r->solucion);
					free(r->coberturas);
					free(r->isCubierta);
					free(r);
				//}
			}
		}
	}
	
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    printf("./main inputfile\n");
    return -1;
  }
  
  char *input = argv[1];
  FILE *f = fopen(input, "r");
  data *d = practica_reader(f);
  double inicio,fin=0;
  //print_data(d);
  tarea * t = NULL;
  tarea * tareas;
  int nivel = 0; //nivel en el arbol
  int maxTareas = d->num_cases;
  int * mejorSolucion = (int *) malloc(sizeof(int) * d->num_cases);
  int * mejoresCoberturas = (int *) malloc(sizeof(int) * d->num_cases); 
  int maxNivel = 1;
  omp_set_num_threads(NUM_THREADS);
  if (d->num_cases < 4){//Si el problema es muy pequeño se hace secuencial
	printf("Se hace problema en secuencial\n");
	t = tarea_new(nivel,d->num_cases,d->num_coverage);
	backtracking(d,t,d->num_cases,nivel,mejorSolucion,mejoresCoberturas);
  }
  else{
  //Se generan las tareas a repartir entre los hilos
  
  while (maxTareas < NUM_THREADS){
	maxTareas = maxTareas * (d->num_cases - maxNivel);
	maxNivel++;
  }
  Vector *bolsa = vector_new_with_capacity(maxTareas);
  tarea * tareas = (tarea *) malloc(sizeof(tarea) * maxTareas);
  printf("El numero maximo de tareas generadas es %d\n",maxTareas);
  inicio = omp_get_wtime();
  t = tarea_new(nivel,d->num_cases,d->num_coverage);
  generarTareas(bolsa,d,t,maxNivel,nivel,d->num_cases);

  printf("El numero de elementos de la bolsa es: %d\n",size(bolsa));
  for (int i=0;i<maxTareas;i++){
	tarea * actual = (tarea *) pop(bolsa);
	tareas[i] = *actual;
  }
  
  #pragma omp parallel shared(tareas,mejorSolucion,mejoresCoberturas)
  {
	#pragma omp for 
	for (int i=0;i<maxTareas;i++){
		backtracking(d,&tareas[i],d->num_cases,tareas[i].nivel,mejorSolucion,mejoresCoberturas);
	}
			
		
  }
  fin = omp_get_wtime();
  printf("Tiempo de ejecucion: %2.4f segundos\n",fin-inicio);
  free(tareas);
  }
  printf("Ordenacion óptima de los casos de prueba: \n");
  for (int i=0;i<d->num_cases;i++){
	printf("%d ",mejorSolucion[i]);
  }
  printf("\n");
  printf("Coberturas: \n");
  for (int i=0;i<d->num_cases;i++){
	printf("%d ",mejoresCoberturas[i]);
  }
  printf("\n");

  free(mejorSolucion);
  free(mejoresCoberturas);
  free(t->solucion);
  free(t->coberturas);
  free(t->isCubierta);
  free(t);
  return 0;
  
}

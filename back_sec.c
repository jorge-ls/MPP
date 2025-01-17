#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include <string.h>


typedef struct {
  int nivel;
  int * solucion; 
  int * coberturas;
  int coberturaTotal;
  line_coverage * lineasCubiertas;
} tarea;
 

//Función auxiliar para comprobar si un caso de prueba es válido
int valido(int * solucion,int n,int i){
	for (int j=0;j<n;j++){
		if (solucion[j] == i) //Si el caso de prueba ya esta en el conjunto solución, no es válido
			return 0;
	}
	return 1;
}

//Comprueba si una linea de un fichero ya ha sido cubierta por un caso de prueba
int isCubierta(line_coverage linea,line_coverage * lineasCubiertas,int numLineas){
	for (int i=0;i<numLineas;i++){
		if ((lineasCubiertas[i].id_file == linea.id_file) && (lineasCubiertas[i].line == linea.line)){
			return 1;
		}
	}
	return 0;
}
//Función para obtener el número actual de entradas de cobertura procesadas 
int getNumCoverage(line_coverage * lineasCubiertas){
	int i = 0;
	while (lineasCubiertas[i].id_test != 0){
		i++;
	}
	return i;
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


//Obtiene el número de lineas cubiertas por un caso de prueba
int getCoberturaTestCase(line_coverage * lineasCubiertas,int numLineas,int id){
	int numTest = 0;
	for (int i=0;i<numLineas;i++){
		if (lineasCubiertas[i].id_test == id){
			numTest++;
		}
	}
	return numTest;
}


// Añade las lineas de ficheros cubiertas por el caso de prueba "id" a la lista de lineas cubiertas y devuelve el numero de lineas cubiertas
// por ese caso de prueba
int addLineasCubiertas(data * d,line_coverage * lineasCubiertas,int id){
	int numLineasTest = 0;
	int numCobertura = 0;
	int j = 0;
	int numLineas = getNumCoverage(lineasCubiertas);
	line_coverage * lineasTest = get_coverage_testCase(d,id,&numLineasTest);
	for (int i=0;i<numLineasTest;i++){
		if (numLineas > 0){
			if (!isCubierta(lineasTest[i],lineasCubiertas,numLineas)){
				lineasCubiertas[numLineas+j] = lineasTest[i];
				numCobertura++;
				j++;	
			}	
		}
		else{
			lineasCubiertas[i] = lineasTest[i];
			numCobertura++;
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
  t->lineasCubiertas = (line_coverage *) malloc(sizeof(line_coverage) * num_coverage);
  return t;
}

//Se añade un nuevo test a la tarea actual
tarea * addTest(data * d,int idTest,int nivel,tarea * actual)
{
  tarea *t = tarea_new(nivel,d->num_cases,d->num_coverage);
  memcpy(t->solucion, actual->solucion, sizeof(int) * d->num_cases);
  memcpy(t->coberturas, actual->coberturas, sizeof(int) * d->num_cases);
  memcpy(t->lineasCubiertas,actual->lineasCubiertas, sizeof(line_coverage) * d->num_coverage);
  /*for (int i = 0;i < d->num_cases;i++){
	t->solucion[i] = actual->solucion[i];
	t->coberturas[i] = actual->coberturas[i];
  }
  for (int i=0; i< d->num_coverage;i++){
	t->lineasCubiertas[i] = actual->lineasCubiertas[i];
  }*/
  t->solucion[nivel] = idTest;
  t->coberturaTotal = actual->coberturaTotal;
  int numCobertura = addLineasCubiertas(d,t->lineasCubiertas,idTest);
  t->coberturaTotal += numCobertura;
  t->coberturas[nivel] = t->coberturaTotal;
  return t;
}

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
	for (int k=0;t->lineasCubiertas[k].id_test!=0;k++){
		//printf("coverage %d\n",k);
		printf("Line:%d ",t->lineasCubiertas[k].line);
		printf("Id_test:%d ",t->lineasCubiertas[k].id_test);
		printf("Id_file:%d\n",t->lineasCubiertas[k].id_file);
	}
	printf("\n");
	
}

void printSolucion(tarea * t,int tamSolucion){
	printf("Solucion:\n");
	for (int i=0;i<tamSolucion;i++){
		printf("%d ",t->solucion[i]);
	}
	printf("\n");
}

void backtracking(data * d,tarea * t,int n,int nivel,int * mejorSolucion,int * mejoresCoberturas){

	if (nivel == n-1){ //Caso en el que llegamos al ultimo nivel
		for (int i=0;i<n;i++){
			if (valido(t->solucion,n,i+1)){
				tarea * r = addTest(d,i+1,nivel,t);
				if (isMejorSolucion(r->coberturas,mejoresCoberturas,n)){
					escribirMejorSolucion(r->solucion,n,mejorSolucion);
					escribirMejoresCoberturas(r->coberturas,n,mejoresCoberturas);
				}
				free(r->solucion);
				free(r->coberturas);
				free(r->lineasCubiertas);
				free(r);
			}	
		}
	}
	else {
		for (int i=0;i<n;i++){
			if (valido(t->solucion,n,i+1)){
				tarea * r = addTest(d,i+1,nivel,t); //Añade un nuevo test y se procesa en el arbol de busqueda
				//if (r->coberturas[nivel] >= mejoresCoberturas[nivel]){ //Si la solucion actual no mejora a la mejor solucion encontrada hasta el momento se aplica poda
					backtracking(d,r,n,nivel+1,mejorSolucion,mejoresCoberturas);
					free(r->solucion);
					free(r->coberturas);
					free(r->lineasCubiertas);
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
  //print_data(d);
  tarea * t = NULL;
  int nivel = 0; //nivel en el arbol
  int * mejorSolucion = (int *) malloc(sizeof(int) * d->num_cases);
  int * mejoresCoberturas = (int *) malloc(sizeof(int) * d->num_cases); 
  
  
  t = tarea_new(nivel,d->num_cases,d->num_coverage);
  backtracking(d,t,d->num_cases,nivel,mejorSolucion,mejoresCoberturas);
  printf("Ordenacion óptima de los casos de prueba: \n");
  for (int i=0;i<d->num_cases;i++){
	printf("%d ",mejorSolucion[i]);
  }
  printf("\n");
  free(mejorSolucion);
  free(mejoresCoberturas);
  free(t->solucion);
  free(t->coberturas);
  free(t->lineasCubiertas);
  free(t);
  return 0;
  
}

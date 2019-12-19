#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "reader.h"

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

//Funcion para borrar la solucion encontrada
void cleanSolucion(int * solucion,int n){
	for (int i=0;i<n;i++){
		solucion[i] = 0;
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

//Elimina las lineas de ficheros cubiertas por el caso de prueba "id"
void deleteLineasCubiertas(data * d,line_coverage * lineasCubiertas,int id){
	int numLineas = getNumCoverage(lineasCubiertas);
	int numLineasTest = getCoberturaTestCase(lineasCubiertas,numLineas,id);
	int posInicial = (numLineas-numLineasTest); 
	while (posInicial < numLineas){
		lineasCubiertas[posInicial].id_test = 0;
		lineasCubiertas[posInicial].id_file = 0;
		lineasCubiertas[posInicial].line = 0;
		//free(lineasCubiertas[posInicial]);
		posInicial++;
	}
	
}
void backtracking(data * d,int solucion[],int * mejorSolucion,int n,int nivel,line_coverage * lineasCubiertas,int numCoberturaTotal,
		int * coberturas,int * mejoresCoberturas){

	if (nivel == n-1){ //Caso en el que llegamos al ultimo nivel
		for (int i=0;i<n;i++){
			if (valido(solucion,n,i+1)){
				solucion[nivel] = i+1;
				int numCobertura = addLineasCubiertas(d,lineasCubiertas,i+1);
				numCoberturaTotal += numCobertura;
				coberturas[nivel] = numCoberturaTotal;
				escribirMejorSolucion(solucion,n,mejorSolucion);
				escribirMejoresCoberturas(coberturas,n,mejoresCoberturas);
				deleteLineasCubiertas(d,lineasCubiertas,i+1);
				solucion[nivel] = 0;
				coberturas[nivel] = 0;
				//cleanSolucion(solucion,n); 
			}	
		}
	}
	else {
		for (int i=0;i<n;i++){
			if (valido(solucion,n,i+1)){
				solucion[nivel] = i+1;
				int numCobertura = addLineasCubiertas(d,lineasCubiertas,i+1);
				numCoberturaTotal += numCobertura;
				coberturas[nivel] = numCoberturaTotal;
				if (coberturas[nivel] >= mejoresCoberturas[nivel]){ //Si la solucion actual no mejora a la mejor solucion encontrada hasta el momento se aplica poda
					if (nivel < 10){ //Se crean nuevas tareas
		#pragma omp task firstprivate(lineasCubiertas,numCoberturaTotal) shared(solucion,mejorSolucion,coberturas,mejoresCoberturas) 
					backtracking(d,solucion,mejorSolucion,n,nivel+1,lineasCubiertas,numCoberturaTotal,coberturas,
						mejoresCoberturas);
					#pragma omp taskwait
					}else{ //Se hace en secuencial
						backtracking(d,solucion,mejorSolucion,n,nivel+1,lineasCubiertas,numCoberturaTotal,coberturas,
							mejoresCoberturas);
					}
				}
				solucion[nivel] = 0;
				coberturas[nivel] = 0;
				deleteLineasCubiertas(d,lineasCubiertas,i+1);
				numCoberturaTotal -= numCobertura;		
			}
		}
	}
}

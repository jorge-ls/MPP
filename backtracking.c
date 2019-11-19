#include <stdio.h>
#include <stdlib.h>
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
int isCubierta(line_coverage * linea,line_coverage ** lineasCubiertas,int numLineas){
	for (int i=0;i<numLineas;i++){
		if ((lineasCubiertas[i]->id_file == linea->id_file) && (lineasCubiertas[i]->line == linea->line)){
			return 1;
		}
	}
	return 0;
}
//Función para obtener el número actual de entradas de cobertura procesadas 
int getNumCoverage(line_coverage ** lineasCubiertas){
	int i = 0;
	while (lineasCubiertas[i] != NULL){
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

//Funcion para borrar la solucion encontrada
void cleanSolucion(int * solucion,int n){
	for (int i=0;i<n;i++){
		solucion[i] = 0;
	}
}

// Añade las lineas de ficheros cubiertas por el caso de prueba "id" a la lista de lineas cubiertas y devuelve el numero de lineas cubiertas
// por ese caso de prueba
int addLineasCubiertas(data * d,line_coverage ** lineasCubiertas,int id){
	int numLineasTest = 0;
	int numCobertura = 0;
	int j = 0;
	int numLineas = getNumCoverage(lineasCubiertas);
	line_coverage ** lineasTest = get_coverage_testCase(d,id,&numLineasTest);
	for (int i=0;i<numLineasTest;i++){
		if (numLineas > 0){
			if (!isCubierta(lineasTest[i],lineasCubiertas,numLineas)){
				lineasCubiertas[numLineas+j] = lineasTest[i];
				numCobertura++;
				j++;	
			}	
		}
		else{
			lineasCubiertas[numLineas+i] = lineasTest[i];
			numCobertura++;
		}
	}
	/*for(int i = 0; i < numLineasTest; i++) {
    		free(lineasTest[i]);
  	}*/
	return numCobertura;
	
}

void backtracking(data * d,int * solucion,int * mejorSolucion,int n,int nivel,line_coverage ** lineasCubiertas,int numCoberturaTotal,
		int numTest,int minNumTest,int maxCobertura,int numPosMaxCobertura,int * coberturas){

	if (nivel == n-1){ //Caso en el que llegamos al ultimo nivel
		for (int i=0;i<n;i++){
			if (valido(solucion,n,i+1)){
				solucion[nivel] = i+1;
				numTest++;
				int numCobertura = addLineasCubiertas(d,lineasCubiertas,i+1);
				coberturas[nivel] = numCobertura;
				//numCoberturaTotal += numCobertura;
				if (minNumTest == 0)
					minNumTest = numTest;
				maxCobertura = numCoberturaTotal;
				if (minNumTest < numPosMaxCobertura){
					minNumTest = numPosMaxCobertura;
					numPosMaxCobertura = 0;
					escribirMejorSolucion(solucion,n,mejorSolucion);
				}
				//cleanSolucion(solucion,n); 
			}	
		}
	}
	else {
		for (int i=0;i<n;i++){
			if (valido(solucion,n,i+1)){
				solucion[nivel] = i+1;
				int numCobertura = addLineasCubiertas(d,lineasCubiertas,i+1);
				numTest++;
				numCoberturaTotal += numCobertura;
				coberturas[nivel] = numCoberturaTotal;
				if (numCoberturaTotal == maxCobertura && numPosMaxCobertura == 0){ //Caso en el que se llega a la maxima cobertura
					numPosMaxCobertura = numTest; 
				}
				backtracking(d,solucion,mejorSolucion,n,nivel+1,lineasCubiertas,numCoberturaTotal,numTest,
					minNumTest,maxCobertura,numPosMaxCobertura,coberturas);		
			}
		}
	}
}

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

// Añade las lineas de ficheros cubiertas por el caso de prueba "id" a la lista de lineas cubiertas y devuelve el numero de lineas cubiertas
// por ese caso de prueba
int addLineasCubiertas(data * d,line_coverage ** lineasCubiertas,int id,int numCobertura){
	int numLineasTest = 0;
	int numCobertura = 0;
	int numLineas = getNumCoverage(lineasCubiertas);
	line_coverage ** lineasTest = get_coverage_testCase(d,id,&numLineasTest);
	for (int i=0;i<numLineasTest;i++){
		if (numLineas > 0){
			if (!isCubierta(lineasTest[i],lineasCubiertas,numLineas)){
				lineasCubiertas[numLineas+i] = lineasTest[i];
				numCobertura++;	
			}	
		}
		else{
			lineasCubiertas[numLineas+i] = lineasTest[i];
			numCobertura++;
		}
	}
	for(int i = 0; i < numLineasTest; i++) {
    		free(lineasTest[i]);
  	}
	return numCobertura;
	
}

void backtracking(data * d,int * solucion,int n,int nivel,line_coverage ** lineasCubiertas,int numCoberturaTotal){
	
	if (nivel == n-1){ //Caso en el que llegamos al ultimo nivel
		for (int i=0;i<n;i++){
			if (valido(solucion,n,i+1)){
				solucion[nivel] = i+1;
			}	
		}
	}
	else {
		for (int i=0;i<n;i++){
			if (valido(solucion,n,i+1)){
				solucion[nivel] = i+1;
				int numCobertura = addLineasCubiertas(d,lineasCubiertas,i+1);
				numCoberturaTotal += numCobertura;
				backtracking(d,solucion,n,nivel+1,lineasCubiertas,numCoberturaTotal);		
			}
		}
	}
}

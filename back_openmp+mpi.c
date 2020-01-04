#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include <string.h>
#include <mpi.h>
#include <omp.h>
#include "vector.h"


typedef struct {
  int nivel;
  int coberturaTotal;
  int * solucion; 
  int * coberturas;
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


//Se inicializa una nueva tarea
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
  //memcpy(t->solucion, actual->solucion, sizeof(int) * d->num_cases);
  //memcpy(t->coberturas, actual->coberturas, sizeof(int) * d->num_cases);
  //memcpy(t->lineasCubiertas,actual->lineasCubiertas, sizeof(int) * d->num_coverage);
  for (int i = 0;i < d->num_cases;i++){
	t->solucion[i] = actual->solucion[i];
	t->coberturas[i] = actual->coberturas[i];
  }
  for (int i=0; i< d->num_coverage;i++){
	t->lineasCubiertas[i] = actual->lineasCubiertas[i];
  }
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

void backtracking(data * d,tarea * t,int n,int nivel,int * mejorSolucion,int * mejoresCoberturas){

	if (nivel == n-1){ //Caso en el que llegamos al ultimo nivel
		for (int i=0;i<n;i++){
			if (valido(t->solucion,n,i+1)){
				tarea * r = addTest(d,i+1,nivel,t);
				#pragma omp critical
				{
				if (isMejorSolucion(r->coberturas,mejoresCoberturas,n)){
					escribirMejorSolucion(r->solucion,n,mejorSolucion);
					escribirMejoresCoberturas(r->coberturas,n,mejoresCoberturas);
				}
				}
				free(r);
			}	
		}
	}
	else {
		for (int i=0;i<n;i++){
			if (valido(t->solucion,n,i+1)){
				tarea * r = addTest(d,i+1,nivel,t); //Añade un nuevo test y se procesa en el arbol de busqueda
				//if (r->coberturas[nivel] >= mejoresCoberturas[nivel]){ 
					backtracking(d,r,n,nivel+1,mejorSolucion,mejoresCoberturas);
					free(r);
				//}		
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
		printf("coverage %d\n",k);
		printf("Line:%d ",t->lineasCubiertas[k].line);
		printf("Id_test:%d ",t->lineasCubiertas[k].id_test);
		printf("Id_file:%d\n",t->lineasCubiertas[k].id_file);
	}
	printf("\n");
	
}
//Funcion que genera las tareas iniciales a repartir entre los procesos hasta maxNivel
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
				//{
					generarTareas(bolsa,d,r,maxNivel,nivelActual+1,n);
					free(r);
				//}
			}
		}
	}
	
}

int main(int argc, char **argv)
{

  MPI_Init(&argc, &argv);
  int rank;
  int procs;
  MPI_Comm_size(MPI_COMM_WORLD, &procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if (argc != 2) {
    printf("./main inputfile\n");
    return -1;
  }
  
  char *input = argv[1];
  FILE *f = fopen(input, "r");
  data *d = practica_reader(f);

  //Se crea el tipo de datos

  MPI_Datatype lineCoverage_type;
  MPI_Datatype tarea_type;

  MPI_Datatype type[3] = { MPI_INT, MPI_INT, MPI_INT };
  int blocklen[3] = { 1, 1, 1 };
  //MPI_Aint addresses[3];
  //MPI_Aint disp[3];

  MPI_Aint displacements[3] = {0, sizeof(int), sizeof(int) + sizeof(int)};
  MPI_Type_create_struct(3,blocklen, displacements, type, &lineCoverage_type);
  MPI_Type_commit(&lineCoverage_type);

  /*MPI_Datatype types[5] = {MPI_INT,MPI_INT,MPI_INT,MPI_INT,lineCoverage_type};
  int blocklen2[5] = {1,1,d->num_cases,d->num_cases,d->num_coverage};
  //const MPI_Aint disp[5] = {0,sizeof(int),sizeof(int)*2,sizeof(int)*2+(sizeof(int)*d->num_cases),sizeof(int)*2+(sizeof(int)*d->num_cases*2)};
  const MPI_Aint disp[5] = {offsetof(tarea,nivel),offsetof(tarea,coberturaTotal),offsetof(tarea,solucion),offsetof(tarea,coberturas),
				offsetof(tarea,lineasCubiertas)};
  MPI_Type_create_struct(5,blocklen2, disp, types, &tarea_type);
  MPI_Type_commit(&tarea_type);*/
  

  //print_data(d);
  tarea * t = NULL;
  //int maxTareas,auxNivel = d->num_cases;
  int fin = 0;
  int position =0;
  int NUM_THREADS = 4;
  omp_set_num_threads(NUM_THREADS);
  double start,end = 0;
  int nivel = 0; //nivel en el arbol
  int * mejorSolucion = (int *) malloc(sizeof(int) * d->num_cases);
  int * mejoresCoberturas = (int *) malloc(sizeof(int) * d->num_cases); 
  int sizeBuffer = sizeof(int)*2+(sizeof(int)*d->num_cases)*2+sizeof(line_coverage)*d->num_coverage;
  int sizeSolucion = (sizeof(int)*d->num_cases)*2;
  char * buffer; //Buffer para guardar los datos de una tarea
  char * bufferSolucion; //Buffer para guardar los datos de una solucion
  int maxTareas = d->num_cases;
  //int maxNivel = d->num_cases/4;
  if (d->num_cases < 5){ //Si el problema es muy pequeño se hace en secuencial
	t = tarea_new(nivel,d->num_cases,d->num_coverage);
  	backtracking(d,t,d->num_cases,nivel,mejorSolucion,mejoresCoberturas);	
  }
  else{
  if (rank == 0){  //Proceso maestro
	int * solucionActual = (int *) malloc(sizeof(int) * d->num_cases);
	int * coberturasActual = (int *) malloc(sizeof(int) * d->num_cases);  
	//Se generan las tareas
	
	/*for (int j=1;j<maxNivel;j++){ //Se calcula el numero de tareas inicial
		maxTareas = maxTareas * (d->num_cases - j);
		//printf("Max tareas %d\n",maxTareas);
  	}*/
	int maxNivel=1;
	while (maxTareas < procs){//Si el número de tareas es menor que el numero de procesos se incrementan las tareas  
		maxTareas = maxTareas * (d->num_cases - maxNivel);
		maxNivel++;
	}
	Vector *bolsa = vector_new_with_capacity(maxTareas);
	printf("El numero maximo de tareas generadas por el proceso maestro es %d\n",maxTareas);
	start = MPI_Wtime();
	t = tarea_new(nivel,d->num_cases,d->num_coverage);
  	generarTareas(bolsa,d,t,maxNivel,nivel,d->num_cases);
	printf("El numero de elementos de la bolsa es: %d\n",size(bolsa));
	int ocupados = 0;
	
	while (!fin){
		// Enviar tareas a cada proceso esclavo
      		for(int i = 1; i < procs; i++) {
			if (!is_empty(bolsa)) {
				//printf("Before enviar\n");
				position = 0;
	  			tarea * actual = (tarea *) pop(bolsa);
				buffer = (char *) malloc(sizeof(char) * sizeBuffer);	
				MPI_Pack(&actual->nivel,1,MPI_INT,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
				MPI_Pack(&actual->coberturaTotal,1,MPI_INT,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
				MPI_Pack(actual->solucion,d->num_cases,MPI_INT,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
				MPI_Pack(actual->coberturas,d->num_cases,MPI_INT,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
				MPI_Pack(actual->lineasCubiertas,d->num_coverage,lineCoverage_type,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
				MPI_Send(buffer, sizeBuffer,MPI_PACKED, i, 0, MPI_COMM_WORLD);
				free(buffer);
	  			//MPI_Send(actual, 1, tarea_type, i, 0, MPI_COMM_WORLD);
				//MPI_Send(actual,sizeBuffer,MPI_BYTE,i,0,MPI_COMM_WORLD);
				//MPI_Send(solucion,sizeSolucion,MPI_BYTE,i,0,MPI_COMM_WORLD);
				//printf("Maestro envia tarea al proceso esclavo %d\n",i);
	  			ocupados++;
 				free(actual);
			}
      		}
		while (ocupados > 0){
			//MPI_Recv(solucionActual,d->num_cases,MPI_INT, MPI_ANY_SOURCE, 20, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			//MPI_Recv(coberturasActual,d->num_cases,MPI_INT, MPI_ANY_SOURCE, 10, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			bufferSolucion = (char *) malloc(sizeof(char) * sizeSolucion);
			position = 0;
			MPI_Recv(bufferSolucion,sizeSolucion,MPI_PACKED, MPI_ANY_SOURCE, 10, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Unpack(bufferSolucion,sizeSolucion,&position,solucionActual,d->num_cases,MPI_INT,MPI_COMM_WORLD);
			MPI_Unpack(bufferSolucion,sizeSolucion,&position,coberturasActual,d->num_cases,MPI_INT,MPI_COMM_WORLD);
			free(bufferSolucion);
			if (isMejorSolucion(coberturasActual,mejoresCoberturas,d->num_cases)){
				escribirMejorSolucion(solucionActual,d->num_cases,mejorSolucion);
				escribirMejoresCoberturas(coberturasActual,d->num_cases,mejoresCoberturas);
			}
			ocupados--; //Si se recibe una solucion es porque el proceso esclavo ya ha terminado
			if (ocupados == 0 && is_empty(bolsa)){
				fin = 1;
			}
			//append(soluciones,r);
			
		}

	}
	end = MPI_Wtime();
	printf("Tiempo de ejecucion: %2.4f segundos\n",end-start);
	//Si se han procesado todas las soluciones se envia una marca de fin a cada esclavo para que terminen su ejecucion
	for(int i = 1; i < procs; i++) {
		tarea * marca_fin = tarea_new(0,d->num_cases,d->num_coverage);
		marca_fin->nivel = -1;
		position = 0;
		//MPI_Send(marca_fin, sizeBuffer,MPI_BYTE, i, 0, MPI_COMM_WORLD);
		buffer = (char *) malloc(sizeof(char) * sizeBuffer);
		MPI_Pack(&marca_fin->nivel,1,MPI_INT,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
		MPI_Pack(&marca_fin->coberturaTotal,1,MPI_INT,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
		MPI_Pack(marca_fin->solucion,d->num_cases,MPI_INT,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
		MPI_Pack(marca_fin->coberturas,d->num_cases,MPI_INT,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
		MPI_Pack(marca_fin->lineasCubiertas,d->num_coverage,lineCoverage_type,buffer,sizeBuffer,&position,MPI_COMM_WORLD);
		MPI_Send(buffer, sizeBuffer,MPI_PACKED, i, 0, MPI_COMM_WORLD);
		free(buffer);
		free(marca_fin);
		//printf("Proceso maestro envia marca de fin al proceso esclavo %d\n",i);
	}
	
	free(solucionActual);
	free(coberturasActual);
	
  }
  else{ //procesos esclavos
	while(true){
		//printf("Proceso %d\n",rank);
		MPI_Request request;
		MPI_Status status;
		int nivelesRestantes = 0;
		int maxNivelLocal = 0;
		int maxTareasLocal = 0;
		Vector * bolsa_local;
      		tarea * actual = tarea_new(0,d->num_cases,d->num_coverage);
		position = 0;
		//printf("Proceso %d entra en el bucle\n",rank);
		//MPI_Recv(actual,1,tarea_type, 0, 0, MPI_COMM_WORLD, &status);
		buffer = (char *) malloc(sizeof(char) * sizeBuffer);
		MPI_Recv(buffer, sizeBuffer,MPI_PACKED, 0, 0, MPI_COMM_WORLD,&status);
		MPI_Unpack(buffer,sizeBuffer,&position,&actual->nivel,1,MPI_INT,MPI_COMM_WORLD);
		MPI_Unpack(buffer,sizeBuffer,&position,&actual->coberturaTotal,1,MPI_INT,MPI_COMM_WORLD);
		MPI_Unpack(buffer,sizeBuffer,&position,actual->solucion,d->num_cases,MPI_INT,MPI_COMM_WORLD);
		MPI_Unpack(buffer,sizeBuffer,&position,actual->coberturas,d->num_cases,MPI_INT,MPI_COMM_WORLD);
		MPI_Unpack(buffer,sizeBuffer,&position,actual->lineasCubiertas,d->num_coverage,lineCoverage_type,MPI_COMM_WORLD);
		free(buffer);
		//printTarea(actual,d->num_cases,d->num_coverage);
		if (actual->nivel == -1){
			//printf("El proceso %d termina su ejecucion\n",rank);
			break;
		}
		////Parte openMP+MPI////
		nivelesRestantes = d->num_cases - (actual->nivel);
		if (nivelesRestantes < 5){ //Si quedan pocos niveles por explorar el proceso lo hace en secuencial
			backtracking(d,actual,d->num_cases,actual->nivel,mejorSolucion,mejoresCoberturas);	
		}
		else{ //En caso contrario, el proceso crea varios hilos para terminar de explorar los niveles restantes
			maxNivelLocal = actual->nivel;
			maxTareasLocal = d->num_cases - (actual->nivel);
			maxNivelLocal++;
			/*for (int i=actual->nivel+1;i<maxNivelLocal;i++){
				maxTareasLocal = maxTareasLocal * (d->num_cases - i);
			}*/
			int numThreads = omp_get_num_threads();
			while (maxTareasLocal < numThreads){
				maxTareasLocal = maxTareasLocal * (d->num_cases - maxNivelLocal);
				maxNivelLocal++;
			}
			//printf("El nivel actual es %d\n",actual->nivel);
			//printf("El nivel hasta el que se generan tareas es %d\n",maxNivelLocal);
			//printf("El numero de niveles restantes es %d\n",nivelesRestantes);
			printf("El numero de tareas generadas por el proceso %d es %d\n",rank,maxTareasLocal);
			bolsa_local = vector_new_with_capacity(maxTareasLocal);
			tarea * tareas_local = (tarea *) malloc(sizeof(tarea) * maxTareasLocal);
			generarTareas(bolsa_local,d,actual,maxNivelLocal,actual->nivel,d->num_cases);
			//printf("Proceso %d:El numero de elementos de la bolsa es: %d\n",rank,size(bolsa_local));
  			for (int i=0;i<maxTareasLocal;i++){
				tarea * actual_local = (tarea *) pop(bolsa_local);
				tareas_local[i] = *actual_local;
  			}
	
			#pragma omp parallel shared(tareas_local,mejorSolucion,mejoresCoberturas)
  			{
				#pragma omp for 
				for (int i=0;i<maxTareasLocal;i++){
					//printTarea(tareas[i],d->num_cases,d->num_coverage);
					backtracking(d,&tareas_local[i],d->num_cases,tareas_local[i].nivel,mejorSolucion,mejoresCoberturas);
				}
					
  			}
			//backtracking(d,actual,d->num_cases,actual->nivel,mejorSolucion,mejoresCoberturas);
		}
		//Los procesos esclavos envian la mejor solucion encontrada al maestro
		position = 0; 
		//MPI_Send(mejorSolucion,d->num_cases,MPI_INT,0,20,MPI_COMM_WORLD);
		//MPI_Send(mejoresCoberturas,d->num_cases,MPI_INT,0,10,MPI_COMM_WORLD);
		bufferSolucion = (char *) malloc(sizeof(char) * sizeSolucion);
		MPI_Pack(mejorSolucion,d->num_cases,MPI_INT,bufferSolucion,sizeSolucion,&position,MPI_COMM_WORLD);
		MPI_Pack(mejoresCoberturas,d->num_cases,MPI_INT,bufferSolucion,sizeSolucion,&position,MPI_COMM_WORLD);
		MPI_Send(bufferSolucion,sizeSolucion,MPI_PACKED, 0, 10, MPI_COMM_WORLD);
		free(bufferSolucion);
		free(actual);
		
	} 
  }
  }
  if (rank == 0){ //Se imprime la mejor solucion obtenida
	printf("Ordenacion óptima de los casos de prueba: \n");
  	for (int i=0;i<d->num_cases;i++){
		printf("%d ",mejorSolucion[i]);
  	}
  	printf("\n");
  }
  free(mejorSolucion);
  free(mejoresCoberturas);
  

  MPI_Finalize();
  return 0;
  
}
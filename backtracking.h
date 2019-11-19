int valido(int * solucion,int n,int i);
int isCubierta(line_coverage * linea,line_coverage ** lineasCubiertas,int numLineas);
int getNumCoverage(line_coverage ** lineasCubiertas);
int addLineasCubiertas(data * d,line_coverage ** lineasCubiertas,int id);
void escribirMejorSolucion(int * solucion,int n,int * mejorSolucion);
void backtracking(data * d,int * solucion,int * mejorSolucion,int n,int nivel,line_coverage ** lineasCubiertas,int numCoberturaTotal,
		int numTest,int minNumTest,int maxCobertura,int numTestMaxCobertura,coberturas);

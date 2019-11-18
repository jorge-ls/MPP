int valido(int * solucion,int n,int i);
int isCubierta(line_coverage * linea,line_coverage ** lineasCubiertas,int numLineas);
int getNumCoverage(line_coverage ** lineasCubiertas);
int addLineasCubiertas(data * d,line_coverage ** lineasCubiertas,int id);
void backtracking(data * d,int * solucion,int n,int nivel,line_coverage ** lineasCubiertas,int numCoberturaTotal);

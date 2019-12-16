#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reader.h"

#define STRING_MAX 256

void print_data(data *d)
{
  for(int i = 0; i < d->num_cases; i++) {
    printf("%s\n", d->cases[i]->name);
  }

  for(int i = 0; i < d->num_files; i++) {
    printf("%s\n", d->files[i]->name);
  }

  for(int i = 0; i < d->num_coverage; i++) {
    line_coverage *cov = d->coverage[i];
    test_case *tcase  = find_test_case(d, cov->id_test);
    source_file *file = find_source_file(d, cov->id_file);

    printf("%s %s %d\n", tcase->name, file->name, cov->line);
  }
}

test_case * find_test_case(data *d, int id) 
{
  for(int i = 0; i < d->num_cases; i++) {
    if (d->cases[i]->id == id)
      return d->cases[i];
  }
  return NULL;
}

source_file * find_source_file(data *d, int id) 
{
  for(int i = 0; i < d->num_files; i++) {
    if (d->files[i]->id == id)
      return d->files[i];
  }
  return NULL;
}

void free_data(data *d)
{
  for(int i = 0; i < d->num_cases; i++) {
    free(d->cases[i]->name);
    free(d->cases[i]);
  }

  for(int i = 0; i < d->num_files; i++) {
    free(d->files[i]->name);
    free(d->files[i]);
  }

  for(int i = 0; i < d->num_coverage; i++) {
    free(d->coverage[i]);
  }
}

data * read(FILE *f)
{
  int num_test_cases;
  int num_files;
  int coverage_elements;
  char buffer[STRING_MAX];

  data *d = (data *) malloc(sizeof(data));
  
  // Read the test cases
  fscanf(f, "%d", &num_test_cases);
  d->num_cases = num_test_cases;
  d->cases = (test_case **) malloc(sizeof(test_case *) * num_test_cases);
  for(int i = 0; i < num_test_cases; i++) {
    int id;
    double time;
    test_case *test = (test_case *) malloc(sizeof(test_case));
    
    fscanf(f, "%d %lf %s", &id, &time, buffer);
    test->id = id;
    test->time = time;
    test->name = (char *) malloc(sizeof(char) * (strlen(buffer)));
    strcpy(test->name, buffer);

    d->cases[i] = test;
  }

  // Read the classes
  fscanf(f, "%d", &num_files);
  d->num_files = num_files;
  d->files = (source_file **) malloc(sizeof(source_file *) * num_files);
  for(int i = 0; i < num_files; i++) {
    int id;
    int loc;
    source_file *file = (source_file *) malloc(sizeof(file));
    
    fscanf(f, "%d %d %s", &id, &loc, buffer);
    file->id = id;
    file->lines_of_code = loc;
    file->name = (char *) malloc(sizeof(char) * (strlen(buffer)));
    strcpy(file->name, buffer);

    d->files[i] = file;
  }

  // Coverage
  fscanf(f, "%d", &coverage_elements);
  d->num_coverage = coverage_elements;
  d->coverage = (line_coverage **) malloc(sizeof(line_coverage *) * coverage_elements);
  for(int i = 0; i < coverage_elements; i++) {
    int id_test;
    int id_file;
    int line;

    line_coverage *cov = (line_coverage *) malloc(sizeof(line_coverage));
    
    fscanf(f, "%d %d %d", &id_test, &id_file, &line);

    cov->id_test = id_test;
    cov->id_file = id_file;
    cov->line = line;
    
    d->coverage[i] = cov;
  }

  return d;
}

//Obtiene el numero de entradas de cobertura de un caso de prueba
int getNumCoverageTestCase(data * d,int id){
	int numLineas = 0;
	for (int i=0;i < d->num_coverage;i++){
		if (d->coverage[i]->id_test == id){
			numLineas++;
		}		
	}
	return numLineas;
}

//Obtiene las entradas de cobertura de un caso de prueba
line_coverage * get_coverage_testCase(data * d ,int id, int * numLineas){
	int j=0;
	*numLineas = getNumCoverageTestCase(d,id);
	line_coverage * lineas = (line_coverage *) malloc(sizeof(line_coverage) * (*numLineas));  
	for (int i=0;i < d->num_coverage;i++){
		if (d->coverage[i]->id_test == id){
			lineas[j] = *(d->coverage[i]);
			j++;
		}		
	}
	return lineas;	
}


int get_num_cases(data *d)
{
  return d->num_cases;
}

int get_num_files(data *d)
{
  return d->num_files;
}

int get_num_coverage_elements(data *d)
{
  return d->num_coverage;
}


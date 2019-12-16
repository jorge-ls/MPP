
typedef struct {
  int id;
  double time;
  char *name;
} test_case;

typedef struct {
  int id;
  int lines_of_code;
  char *name;
} source_file;

typedef struct {
  int id_test;
  int id_file;
  int line;
} line_coverage;

typedef struct {
  int num_cases;
  int num_files;
  int num_coverage;
  
  test_case **cases;
  source_file **files;
  line_coverage **coverage;
} data;


/* Query functions */
test_case * find_test_case(data *d, int id);

source_file * find_source_file(data *d, int id);

line_coverage * get_coverage_testCase(data * d ,int id,int * numLineas);

int getNumCoverageTestCase(data * d,int id);


/* Accessor functions */
int get_num_cases(data *d);

int get_num_files(data *d);

int get_num_coverage_elements(data *d);


/* Read / write */
data * read(FILE *f);

void print_data(data *d);


void free_data(data *d);

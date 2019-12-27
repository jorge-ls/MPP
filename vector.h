#include <stdbool.h>
#define VECTOR_INITIAL_CAPACITY 16

typedef struct {
    int size;
    int capacity;
    void **data;
} Vector;

Vector * vector_new();
Vector * vector_new_with_capacity(int capacity);

void vector_free(Vector *v);

void init(Vector *vector, int capacity);
void append(Vector *vector, void * value);
void prepend(Vector *vector, void * value);
void delete(Vector *vector, int index);
void delete_value(Vector *vector, void * value);
void set(Vector *vector, int index, void * value);
void resize(Vector *vector);
void free_memory(Vector *vector);

void * get(Vector *vector, int value);
void * pop(Vector *vector);
int find_value(Vector *vector, void * value);
int size(Vector *vector);
int capacity(Vector *vector);

bool is_empty(Vector *vector);

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

Vector * vector_new() {
  Vector * v = (Vector *) malloc(sizeof(Vector));
  init(v, VECTOR_INITIAL_CAPACITY);
  return v;
}

Vector * vector_new_with_capacity(int capacity) {
  Vector * v = (Vector *) malloc(sizeof(Vector));
  init(v, capacity);
  return v;
}

void vector_free(Vector *v) {
  free_memory(v);
  free(v);
}

void init(Vector *vector, int capacity){
    vector->size = 0;
    vector->capacity = capacity;
    vector->data = malloc(sizeof(void *) * vector->capacity);
}

void * get(Vector *vector, int index){
    if(index > vector->capacity || index < 0){
        printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
        exit(1);
    }

    return vector->data[index];
}

void set(Vector *vector, int index, void * value){
    while(index >= vector->size){
        append(vector, 0);
    }

    vector->data[index] = value;
}

void append(Vector *vector, void * value){
    resize(vector);

    vector->data[vector->size++] = value;
}

void prepend(Vector *vector, void * value){
    set(vector, 0, value);
    vector->size++;
}

void * pop(Vector *vector){
    void * data = vector->data[vector->size - 1];
    set(vector, vector->size - 1, 0);
    vector->size = vector->size - 1;
    return data;
}

void delete(Vector *vector, int index){
    for(int i = 0; i < index; i++){
        vector->data[index + i] = vector->data[index + i + 1];
    }
    vector->size = vector->size - 1;
}

void delete_value(Vector *vector, void * value){
    for(int i = 0; i < vector->size; i++){
        if(vector->data[i] == value){
            delete(vector, i);
        }
    }
}

int find_value(Vector *vector, void * value){
    for(int i = 0; i < vector->size; i++){
        if(vector->data[i] == value){
            return i;
        }
    }

    return -1;
}

void resize(Vector *vector){
    if(vector->size >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(void *) * vector->capacity);
    }
}

int size(Vector *vector){
    return vector->size;
}

int capacity(Vector *vector){
    return vector->capacity;
}

bool is_empty(Vector *vector){
    return vector->size == 0;
}

void free_memory(Vector *vector){
    free(vector->data);
}

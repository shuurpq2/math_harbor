#ifndef MHARRAY_H
#define MHARRAY_H

typedef struct {
    double* data;
    int _ndim;
    int* _strides;
} MHArray;

#endif
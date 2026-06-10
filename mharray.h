#ifndef MHARRAY_H
#define MHARRAY_H

typedef struct {
    double* data;
    int size;
    int ndim;
    int* strides;
} MHArray;

void free_mharray(MHArray mharray);

void copy_mharray(MHArray *dest, const MHArray src);

MHArray mharray_add_num(const MHArray mharray, const double num);

MHArray mharray_substract_num(const MHArray mharray, const double num);

MHArray mharray_mult_by_num(const MHArray mharray, const double num);

MHArray mharrays_div_by_num(const MHArray mharray, const double num);

#endif
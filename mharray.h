#ifndef MHARRAY_H
#define MHARRAY_H

typedef struct MHArray
{
    double *data;
    int size;
    int ndim;
    int *shape;
    int *strides;
} MHArray;

MHArray *mharray_create(double *data, int ndim, int *shape);

void mharray_free(MHArray *mharray);

MHArray *mharray_copy(const MHArray *src);
// Returns MHArray of the given shape and ndim
MHArray *mharray_reshaped(const MHArray *mharray, const int *new_shape, int new_ndim);
// MHArray + N
MHArray *mharray_add_num(const MHArray *mharray, const double num);
// MHArray - N
MHArray *mharray_substract_num(const MHArray *mharray, const double num);
// MHArray * N
MHArray *mharray_mult_by_num(const MHArray *mharray, const double num);
// MHArray / N
MHArray *mharray_div_by_num(const MHArray *mharray, const double num);
// MHArray * MHArray. Matrix multiplication
MHArray *mharray_mult_by_mharray(const MHArray *mharray1, const MHArray *mharray2);

#endif
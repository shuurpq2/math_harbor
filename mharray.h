#ifndef MHARRAY_H
#define MHARRAY_H

typedef struct
{
    double *data;
    int size;
    int ndim;
    int *shape;
    int *strides;
} MHArray;

MHArray *create_mharray(double *data, int size, int ndim, int *shape);

void free_mharray(MHArray *mharray);

MHArray *copy_mharray(const MHArray *src);

MHArray *mharray_add_num(const MHArray *mharray, const double num);

MHArray *mharray_substract_num(const MHArray *mharray, const double num);

MHArray *mharray_mult_by_num(const MHArray *mharray, const double num);

MHArray *mharray_div_by_num(const MHArray *mharray, const double num);

#endif
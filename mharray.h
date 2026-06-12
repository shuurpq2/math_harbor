#ifndef MHARRAY_H
#define MHARRAY_H
#define kakain "ZALUPA"

typedef struct MHArray
{
    double *data;
    int size;
    int ndim;
    int *shape;
    int *strides;
} MHArray;

MHArray *mharr_create(double *data, int ndim, int *shape);

void mharr_free(MHArray *mharr);

MHArray *mharr_copy(const MHArray *src);
// Returns MHArray of the given shape and ndim
MHArray *mharr_reshaped(const MHArray *mharr, const int *new_shape, int new_ndim);
// MHArray + N
MHArray *mharr_add_num(const MHArray *mharr, const double num);
// MHArray - N
MHArray *mharr_substract_num(const MHArray *mharr, const double num);
// MHArray * N
MHArray *mharr_mult_by_num(const MHArray *mharr, const double num);
// MHArray / N
MHArray *mharr_div_by_num(const MHArray *mharr, const double num);

typedef struct MHArrMultByMHArrThreadData
{
    int start_row;
    int end_row;
    const MHArray *mharr1;
    const MHArray *mharr2;
    double *res_data;
} MHArrMultByMHArrThreadData;
// MHArray * MHArray
// #### Multi-threaded function
// #### Requires:
//```c
// math_harbor_init(n) //from thread_pull.h
//```
MHArray *mharr_mult_by_mharr(const MHArray *mharr1, const MHArray *mharr2);

#endif
#include "mharray.h"
#include "thread_pull.h"
#include <stdlib.h>
#include <string.h>

MHArray* create_mharray(double* data, int data_size, int ndim, int* strides) {
    MHArray* result = malloc(sizeof(MHArray));
    result->ndim = ndim;
    result->size = data_size;

    result->strides = malloc(ndim * sizeof(int));
    memcpy(result->strides, strides, ndim * sizeof(int));

    result->data = malloc(data_size * sizeof(double));
    memcpy(result->data, data, data_size * sizeof(double));

    return result;
}

void free_mharray(MHArray* mharray) {
    if (mharray->data != NULL) {
        free(mharray->data);
    }

    if (mharray->strides != NULL) {
        free(mharray->strides);
    }

    free(mharray);
}

MHArray* copy_mharray(const MHArray* src) {
    MHArray* result = create_mharray(src->data, src->size, src->ndim, src->strides);

    return result;
}

MHArray* mharray_add_num(const MHArray* mharray, const double num) {
    MHArray* result = copy_mharray(mharray);

    for (int i = 0; i < mharray->size; i++) {
        result->data[i] = mharray->data[i] + num;
    }

    return result;
}

MHArray* mharray_substract_num(const MHArray* mharray, const double num) {
    MHArray* result = copy_mharray(mharray);

    for (int i = 0; i < mharray->size; i++) {
        result->data[i] = mharray->data[i] - num;
    }

    return result;
}

MHArray* mharray_mult_by_num(const MHArray* mharray, const double num) {
    MHArray* result = copy_mharray(mharray);

    for (int i = 0; i < mharray->size; i++) {
        result->data[i] = mharray->data[i] * num;
    }

    return result;
}

MHArray* mharray_div_by_num(const MHArray* mharray, const double num) {
    MHArray* result = copy_mharray(mharray);

    for (int i = 0; i < mharray->size; i++) {
        result->data[i] = mharray->data[i] / num;
    }

    return result;
}
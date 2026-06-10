#include "mharray.h"
#include "thread_pull.h"
#include <stdlib.h>
#include <string.h>

void free_mharray(MHArray mharray) {
    if (mharray.data != NULL) {
        free(mharray.data);
    }

    if (mharray.strides != NULL) {
        free(mharray.strides);
    }
}

void copy_mharray(MHArray *dest, const MHArray src) {
    dest->data = malloc(src.size * sizeof(double));
    for (int i = 0; i < src.size; i++) {
        dest->data[i] = src.data[i];
    }

    dest->size = src.size;
    dest->ndim = src.ndim;
    
    size_t strides_size = src.ndim * sizeof(int);
    dest->strides = malloc(strides_size);
    for (int i = 0; i < src.ndim; i++) {
        dest->strides[i] = src.strides[i];
    }
}

MHArray mharray_add_num(const MHArray mharray, const double num) {
    MHArray result;
    copy_mharray(&result, mharray);

    for (int i = 0; i < mharray.size; i++) {
        result.data[i] = mharray.data[i] + num;
    }

    return result;
}

MHArray mharray_substract_num(const MHArray mharray, const double num) {
    MHArray result;
    copy_mharray(&result, mharray);

    for (int i = 0; i < mharray.size; i++) {
        result.data[i] = mharray.data[i] - num;
    }

    return result;
}

MHArray mharray_mult_by_num(const MHArray mharray, const double num) {
    MHArray result;
    copy_mharray(&result, mharray);

    for (int i = 0; i < mharray.size; i++) {
        result.data[i] = mharray.data[i] * num;
    }

    return result;
}

MHArray mharrays_div_by_num(const MHArray mharray, const double num) {
    MHArray result;
    copy_mharray(&result, mharray);

    for (int i = 0; i < mharray.size; i++) {
        result.data[i] = mharray.data[i] / num;
    }

    return result;
}
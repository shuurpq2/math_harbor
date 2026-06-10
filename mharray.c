#include "mharray.h"
#include "thread_pull.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

MHArray* create_mharray(double* data, int data_size, int ndim, int* shape) {
    MH_DEBUG_PRINT("Function \"create_mharray\" start\n");

    MHArray* result = malloc(sizeof(MHArray));
    result->size = data_size;
    result->ndim = ndim;

    result->data = malloc(data_size * sizeof(double));
    memcpy(result->data, data, data_size * sizeof(double));

    result->shape = malloc(ndim * sizeof(int));
    memcpy(result->shape, shape, ndim * sizeof(int));

    result->strides = malloc(ndim * sizeof(int));
    for (int i = ndim-1; i >= 0; i--) {
        result->strides[i] = 1;
        if (i < ndim-1) {
            result->strides[i] = result->strides[i+1] * shape[i+1];
        }
    }

    MH_DEBUG_PRINT("Function \"create_mharray\" end\n");
    
    return result;
}

void free_mharray(MHArray* mharray) {
    MH_DEBUG_PRINT("Function \"free_mharray\" start\n");
    
    if (mharray->data != NULL) {
        free(mharray->data);
    }

    if (mharray->shape != NULL) {
        free(mharray->shape);
    }

    if (mharray->strides != NULL) {
        free(mharray->strides);
    }

    free(mharray);

    MH_DEBUG_PRINT("Function \"free_mharray\" end\n");
}

MHArray* copy_mharray(const MHArray* src) {
    MH_DEBUG_PRINT("Function \"copy_mharray\" start\n");

    MHArray* result = create_mharray(src->data, src->size, src->ndim, src->shape);

    MH_DEBUG_PRINT("Function \"copy_mharray\" end\n");

    return result;
}

MHArray* mharray_add_num(const MHArray* mharray, const double num) {
    MH_DEBUG_PRINT("Function \"mharray_add_num\" start\n");

    MHArray* result = copy_mharray(mharray);

    for (int i = 0; i < mharray->size; i++) {
        result->data[i] = mharray->data[i] + num;
    }

    MH_DEBUG_PRINT("Function \"mharray_add_num\" end\n");

    return result;
}

MHArray* mharray_substract_num(const MHArray* mharray, const double num) {
    MH_DEBUG_PRINT("Function \"mharray_substract_num\" start\n");

    MHArray* result = copy_mharray(mharray);

    for (int i = 0; i < mharray->size; i++) {
        result->data[i] = mharray->data[i] - num;
    }

    MH_DEBUG_PRINT("Function \"mharray_substract_num\" end\n");

    return result;
}

MHArray* mharray_mult_by_num(const MHArray* mharray, const double num) {
    MH_DEBUG_PRINT("Function \"mharray_mult_by_num\" start\n");

    MHArray* result = copy_mharray(mharray);

    for (int i = 0; i < mharray->size; i++) {
        result->data[i] = mharray->data[i] * num;
    }

    MH_DEBUG_PRINT("Function \"mharray_mult_by_num\" end\n");
    
    return result;
}

MHArray* mharray_div_by_num(const MHArray* mharray, const double num) {
    MH_DEBUG_PRINT("Function \"mharray_div_by_num\" start\n");

    MHArray* result = copy_mharray(mharray);

    for (int i = 0; i < mharray->size; i++) {
        result->data[i] = mharray->data[i] / num;
    }

    MH_DEBUG_PRINT("Function \"mharray_div_by_num\" end\n");

    return result;
}
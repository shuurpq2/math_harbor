#include "mharray.h"
#include "thread_pull.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

MHArray *mharray_create(double *data, int data_size, int ndim, int *shape)
{
    MH_DEBUG_PRINT("Function \"mharray_create\" start\n");

    MHArray *result = malloc(sizeof(MHArray));
    result->size = data_size;
    result->ndim = ndim;

    result->data = malloc(data_size * sizeof(double));
    memcpy(result->data, data, data_size * sizeof(double));

    result->shape = malloc(ndim * sizeof(int));
    memcpy(result->shape, shape, ndim * sizeof(int));

    result->strides = malloc(ndim * sizeof(int));
    for (int i = ndim - 1; i >= 0; i--)
    {
        result->strides[i] = 1;
        if (i < ndim - 1)
        {
            result->strides[i] = result->strides[i + 1] * shape[i + 1];
        }
    }

    MH_DEBUG_PRINT("Function \"mharray_create\" end\n");

    return result;
}

void mharray_free(MHArray *mharray)
{
    MH_DEBUG_PRINT("Function \"mharray_free\" start\n");

    if (mharray->data != NULL)
    {
        free(mharray->data);
    }

    if (mharray->shape != NULL)
    {
        free(mharray->shape);
    }

    if (mharray->strides != NULL)
    {
        free(mharray->strides);
    }

    free(mharray);

    MH_DEBUG_PRINT("Function \"mharray_free\" end\n");
}

MHArray *mharray_reshaped(const MHArray *mharray, const int *new_shape, int new_ndim)
{
    MH_DEBUG_PRINT("Function \"mharray_reshape\" start\n");

    if (new_ndim < 1)
    {
        new_ndim = mharray->ndim;
    }

    MHArray *result = mharray_copy(mharray);
    result->ndim = new_ndim;
    result->shape = realloc(result->shape, new_ndim * sizeof(int));
    result->strides = realloc(result->strides, new_ndim * sizeof(int));

    int new_shape_mult = 1;
    for (int i = new_ndim - 1; i >= 0; i--)
    {
        new_shape_mult *= new_shape[i];

        result->shape[i] = new_shape[i];
        result->strides[i] = 1;
        if (i < new_ndim - 1)
        {
            result->strides[i] = result->strides[i + 1] * new_shape[i + 1];
        }
    }

    if (new_shape_mult != mharray->size)
    {
        fprintf(stderr, "Impossible to reshape\n");
        exit(1);
    }

    MH_DEBUG_PRINT("Function \"mharray_reshape\" end\n");

    return result;
}

MHArray *mharray_copy(const MHArray *src)
{
    MH_DEBUG_PRINT("Function \"mharray_copy\" start\n");

    MHArray *result = mharray_create(src->data, src->size, src->ndim, src->shape);

    MH_DEBUG_PRINT("Function \"mharray_copy\" end\n");

    return result;
}

MHArray *mharray_add_num(const MHArray *mharray, const double num)
{
    MH_DEBUG_PRINT("Function \"mharray_add_num\" start\n");

    MHArray *result = mharray_copy(mharray);

    for (int i = 0; i < mharray->size; i++)
    {
        result->data[i] = mharray->data[i] + num;
    }

    MH_DEBUG_PRINT("Function \"mharray_add_num\" end\n");

    return result;
}

MHArray *mharray_substract_num(const MHArray *mharray, const double num)
{
    MH_DEBUG_PRINT("Function \"mharray_substract_num\" start\n");

    MHArray *result = mharray_copy(mharray);

    for (int i = 0; i < mharray->size; i++)
    {
        result->data[i] = mharray->data[i] - num;
    }

    MH_DEBUG_PRINT("Function \"mharray_substract_num\" end\n");

    return result;
}

MHArray *mharray_mult_by_num(const MHArray *mharray, const double num)
{
    MH_DEBUG_PRINT("Function \"mharray_mult_by_num\" start\n");

    MHArray *result = mharray_copy(mharray);

    for (int i = 0; i < mharray->size; i++)
    {
        result->data[i] = mharray->data[i] * num;
    }

    MH_DEBUG_PRINT("Function \"mharray_mult_by_num\" end\n");

    return result;
}

MHArray *mharray_div_by_num(const MHArray *mharray, const double num)
{
    MH_DEBUG_PRINT("Function \"mharray_div_by_num\" start\n");

    MHArray *result = mharray_copy(mharray);

    for (int i = 0; i < mharray->size; i++)
    {
        result->data[i] = mharray->data[i] / num;
    }

    MH_DEBUG_PRINT("Function \"mharray_div_by_num\" end\n");

    return result;
}
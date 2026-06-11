#include "mharray.h"
#include "thread_pull.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

MHArray *mharray_create(double *data, int ndim, int *shape)
{
    MH_DEBUG_PRINT("Function \"mharray_create\" start\n");

    MHArray *result = malloc(sizeof(MHArray));

    result->size = 1;
    result->ndim = ndim;

    result->shape = malloc(ndim * sizeof(int));
    memcpy(result->shape, shape, ndim * sizeof(int));

    result->strides = malloc(ndim * sizeof(int));

    for (int i = ndim - 1; i >= 0; i--)
    {
        result->size *= result->shape[i];

        result->strides[i] = 1;
        if (i < ndim - 1)
        {
            result->strides[i] = result->strides[i + 1] * result->shape[i + 1];
        }
    }

    result->data = malloc(result->size * sizeof(double));
    memcpy(result->data, data, result->size * sizeof(double));

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

    MHArray *result = mharray_create(src->data, src->ndim, src->shape);

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

MHArray *mharray_mult_by_mharray(const MHArray *mharray1, const MHArray *mharray2)
{
    MH_DEBUG_PRINT("Function \"mharray_mult_by_mharray\" start\n");

    int mharray1_2d_shape[] = {1, mharray1->shape[mharray1->ndim - 1]};
    int mharray2_2d_shape[] = {1, mharray2->shape[mharray2->ndim - 1]};

    for (int i = 0; i < mharray1->ndim - 1; i++)
    {
        mharray1_2d_shape[0] *= mharray1->shape[i];
    }
    for (int i = 0; i < mharray2->ndim - 1; i++)
    {
        mharray2_2d_shape[0] *= mharray2->shape[i];
    }

    if (mharray1_2d_shape[1] != mharray2_2d_shape[0])
    {
        fprintf(stderr, "Impossible to multiply MHArray (%d, %d) and MHArray (%d, %d)\n",
                mharray1_2d_shape[0], mharray1_2d_shape[1], mharray2_2d_shape[0], mharray2_2d_shape[1]);

        exit(1);
    }

    int *result_shape = malloc(mharray1->ndim * sizeof(int));
    memcpy(result_shape, mharray1->shape, (mharray1->ndim - 1) * sizeof(int));
    result_shape[mharray1->ndim - 1] = mharray2->shape[mharray2->ndim - 1];

    int result_2d_shape[] = {mharray1_2d_shape[0], mharray2_2d_shape[1]};
    double *result_data = calloc(result_2d_shape[0] * result_2d_shape[1], sizeof(double));

    for (int i = 0; i < result_2d_shape[0]; i++)
    {
        for (int k = 0; k < mharray1_2d_shape[1]; k++)
        {
            for (int j = 0; j < result_2d_shape[1]; j++)
            {
                result_data[i * result_2d_shape[1] + j] += mharray1->data[i * mharray1_2d_shape[1] + k] * mharray2->data[k * mharray2_2d_shape[1] + j];
            }
        }
    }

    MHArray *result = mharray_create(result_data, mharray1->ndim, result_shape);

    free(result_data);
    free(result_shape);

    return result;

    MH_DEBUG_PRINT("Function \"mharray_mult_by_mharray\" end\n");
}
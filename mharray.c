#include "mharray.h"
#include "thread_pull.h"
#include "default.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

MHArray *mharr_create(double *data, int ndim, int *shape)
{
    MH_DEBUG_PRINT("Function \"mharr_create\" start\n");

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

    MH_DEBUG_PRINT("Function \"mharr_create\" end\n");

    return result;
}

void mharr_free(MHArray *mharr)
{
    MH_DEBUG_PRINT("Function \"mharr_free\" start\n");

    if (mharr->data != NULL)
    {
        free(mharr->data);
    }

    if (mharr->shape != NULL)
    {
        free(mharr->shape);
    }

    if (mharr->strides != NULL)
    {
        free(mharr->strides);
    }

    free(mharr);

    MH_DEBUG_PRINT("Function \"mharr_free\" end\n");
}

MHArray *mharr_reshaped(const MHArray *mharr, const int *new_shape, int new_ndim)
{
    MH_DEBUG_PRINT("Function \"mharr_reshape\" start\n");

    if (new_ndim < 1)
    {
        new_ndim = mharr->ndim;
    }

    MHArray *result = mharr_copy(mharr);
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

    if (new_shape_mult != mharr->size)
    {
        fprintf(stderr, "Impossible to reshape\n");
        exit(1);
    }

    MH_DEBUG_PRINT("Function \"mharr_reshape\" end\n");

    return result;
}

MHArray *mharr_copy(const MHArray *src)
{
    MH_DEBUG_PRINT("Function \"mharr_copy\" start\n");

    MHArray *result = mharr_create(src->data, src->ndim, src->shape);

    MH_DEBUG_PRINT("Function \"mharr_copy\" end\n");

    return result;
}

MHArray *mharr_add_num(const MHArray *mharr, const double num)
{
    MH_DEBUG_PRINT("Function \"mharr_add_num\" start\n");

    MHArray *result = mharr_copy(mharr);

    for (int i = 0; i < mharr->size; i++)
    {
        result->data[i] = mharr->data[i] + num;
    }

    MH_DEBUG_PRINT("Function \"mharr_add_num\" end\n");

    return result;
}

MHArray *mharr_substract_num(const MHArray *mharr, const double num)
{
    MH_DEBUG_PRINT("Function \"mharr_substract_num\" start\n");

    MHArray *result = mharr_copy(mharr);

    for (int i = 0; i < mharr->size; i++)
    {
        result->data[i] = mharr->data[i] - num;
    }

    MH_DEBUG_PRINT("Function \"mharr_substract_num\" end\n");

    return result;
}

MHArray *mharr_mult_by_num(const MHArray *mharr, const double num)
{
    MH_DEBUG_PRINT("Function \"mharr_mult_by_num\" start\n");

    MHArray *result = mharr_copy(mharr);

    for (int i = 0; i < mharr->size; i++)
    {
        result->data[i] = mharr->data[i] * num;
    }

    MH_DEBUG_PRINT("Function \"mharr_mult_by_num\" end\n");

    return result;
}

MHArray *mharr_div_by_num(const MHArray *mharr, const double num)
{
    MH_DEBUG_PRINT("Function \"mharr_div_by_num\" start\n");

    MHArray *result = mharr_copy(mharr);

    for (int i = 0; i < mharr->size; i++)
    {
        result->data[i] = mharr->data[i] / num;
    }

    MH_DEBUG_PRINT("Function \"mharr_div_by_num\" end\n");

    return result;
}

void mharr_mult_by_mharr_action(void *arg)
{
    MHArrMultByMHArrThreadData *data = (MHArrMultByMHArrThreadData *)arg;

    int mharr1_2d_cols = data->mharr1->shape[data->mharr1->ndim - 1];
    int mharr2_2d_cols = data->mharr2->shape[data->mharr2->ndim - 1];

    for (int i = data->start_row; i < data->end_row; i++)
    {
        for (int k = 0; k < mharr1_2d_cols; k++)
        {
            for (int j = 0; j < mharr2_2d_cols; j++)
            {
                data->res_data[i * mharr2_2d_cols + j] += data->mharr1->data[i * mharr1_2d_cols + k] * data->mharr2->data[k * mharr2_2d_cols + j];
            }
        }
    }
}

void mharr_mult_by_mharr_start_threads(const MHArray *mharr1, int mharr1_2d_shape[2], const MHArray *mharr2, const int mharr2_2d_shape[2], double *res_data)
{
    int rows_per_thread = MAX(mharr1_2d_shape[0] / g_num_threads, 1);
    int num_tasks = MIN(mharr1_2d_shape[0], g_num_threads);

    MHArrMultByMHArrThreadData **args = malloc(num_tasks * sizeof(MHArrMultByMHArrThreadData *));

    for (int i = 0; i < num_tasks; i++)
    {
        args[i] = malloc(sizeof(MHArrMultByMHArrThreadData));
        args[i]->mharr1 = mharr1;
        args[i]->mharr2 = mharr2;
        args[i]->res_data = res_data;
        args[i]->start_row = i * rows_per_thread;
        if (i < num_tasks - 1)
        {
            args[i]->end_row = args[i]->start_row + rows_per_thread;
        }
        else
        {
            args[i]->end_row = mharr1_2d_shape[0];
        }

        pool_add_task(&mharr_mult_by_mharr_action, (void *)args[i]);
    }

    pool_wait();

    for (int i = 0; i < num_tasks; i++)
    {
        free(args[i]);
    }
    free(args);
}

MHArray *mharr_mult_by_mharr(const MHArray *mharr1, const MHArray *mharr2)
{
    MH_DEBUG_PRINT("Function \"mharr_mult_by_mharr\" start\n");

    int mharr1_2d_shape[] = {1, mharr1->shape[mharr1->ndim - 1]};
    int mharr2_2d_shape[] = {1, mharr2->shape[mharr2->ndim - 1]};

    for (int i = 0; i < mharr1->ndim - 1; i++)
    {
        mharr1_2d_shape[0] *= mharr1->shape[i];
    }
    for (int i = 0; i < mharr2->ndim - 1; i++)
    {
        mharr2_2d_shape[0] *= mharr2->shape[i];
    }

    if (mharr1_2d_shape[1] != mharr2_2d_shape[0])
    {
        fprintf(stderr, "Impossible to multiply MHArray (%d, %d) and MHArray (%d, %d)\n",
                mharr1_2d_shape[0], mharr1_2d_shape[1], mharr2_2d_shape[0], mharr2_2d_shape[1]);

        exit(1);
    }

    int *result_shape = malloc(mharr1->ndim * sizeof(int));
    memcpy(result_shape, mharr1->shape, (mharr1->ndim - 1) * sizeof(int));
    result_shape[mharr1->ndim - 1] = mharr2->shape[mharr2->ndim - 1];

    double *res_data = calloc(mharr1_2d_shape[0] * mharr2_2d_shape[1], sizeof(double));

    mharr_mult_by_mharr_start_threads(mharr1, mharr1_2d_shape, mharr2, mharr2_2d_shape, res_data);

    MHArray *result = mharr_create(res_data, mharr1->ndim, result_shape);

    free(res_data);
    free(result_shape);

    return result;

    MH_DEBUG_PRINT("Function \"mharr_mult_by_mharr\" end\n");
}
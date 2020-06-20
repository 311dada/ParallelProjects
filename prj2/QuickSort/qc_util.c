#include "qc_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int* allocate_array(int n, int MAX)
{
    int *num;
    num = (int*)malloc(sizeof(int) * n);

    srand(0);

    for (int i = 0; i < n; i++)
        num[i] = rand() % MAX;
    return num;
}


void display(int* arr, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (i != n - 1)
            printf("%d ", arr[i]);
        else
        {
            printf("%d\n", arr[i]);
        } 
    }
}

void merge(int *arr, int n, int cnt)
{
    int block_num;
    block_num = n / cnt;
    int *index;
    index = (int*)malloc(sizeof(int) * block_num);
    memset(index, 0, sizeof(int) * block_num);

    int *new_arr;
    new_arr = (int*)malloc(sizeof(int) * n);

    int cur_global_index = 0;
    int cur_block_minm;
    int cur_block_min_index;
    while (cur_global_index < n)
    {
        cur_block_minm = INT64_MAX;
        for (int j = 0; j < block_num; j++)
        {
            int cur_block_global_index = cnt * (j - 1) + index[j];
            if (index[j] < cnt && arr[cur_block_global_index] < cur_block_minm)
            {
                cur_block_minm = arr[cur_block_global_index];
                cur_block_min_index = j
            }
        }
        index[cur_block_min_index]++;
        new_arr[cur_global_index++] = cur_block_minm;
    }

    memcpy(arr, new_arr, sizeof(int) * n);
    free(index);
    free(new_arr);
}
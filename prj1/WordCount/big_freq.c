#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "hashmap.h"
#define SEP "\n"
#define MAX_FILE 100
#define MAX_WORD 300


int is_white_space(char ch)
{
    if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\f' || ch == '\t' || ch == '\v')
        return 1;
    return 0;
}


int get_file_size(char *filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;

    return size;
}

void adjust_offset(FILE *f, int *offset)
{
    int ch;
    int little_offset = 0;
    while (1)
    {
        ch = fgetc(f);
        
        if (ch == EOF || is_white_space(ch))
            break;
        ++little_offset;
    }
    if (is_white_space(ch))
        ++little_offset;
    *offset = *offset + little_offset;
}


void add_word(map_t map, char* word, int weight)
{
    int *cnt;
    if (hashmap_get(map, word, (any_t*)&cnt) == MAP_MISSING)
    {
        cnt = (int*)malloc(sizeof(int));
        *cnt = weight;
        hashmap_put(map, word, (void*)cnt);
    }
        
    else
    {
        *cnt += weight;
    }
}


int count_words(FILE *f, int start, int offset, map_t map)
{
    fseek(f,(long)start, 0);
    int ch=' ';
    char pre=' ';
    int count=0;
    char *word;
    int index=0;
    int *cnt;

    word = (char*)malloc(MAX_WORD);
    for (int i = 0; i < offset; i++)
    {
        ch = fgetc(f);
        if (!is_white_space(ch))
            word[index++] = ch;
        if (is_white_space(ch) && (!is_white_space(pre)))
        {
            ++count;
            word[index] = '\0';
            index = 0;
            add_word(map, strdup(word), 1);
        }
        pre = ch;
    }

    if (!is_white_space(ch))
    {
        ++count;
        word[index] = '\0';
        add_word(map, strdup(word), 1);
    }

    free(word);
}


int main(int argc, char **argv)
{

    int offset = 0;
    int start = 0;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    map_t global_map;
    map_t local_map;
    int num=0;
    char **words=NULL;
    int **freq=NULL;

    char **global_words=NULL;
    int **global_freq=NULL;
    int global_num=0;
    int word_len;
    char *word;
    int word_freq;

    double begin, end;
    MPI_Barrier(MPI_COMM_WORLD);
    begin = MPI_Wtime();

    if (!world_rank)
    {
        global_map = hashmap_new();
        int size;
        long block;
        int next_start=0;
        int next_offset=0;
        size = get_file_size("./big_file/big_100.txt");

        FILE *in;
        in = fopen("./big_file/big_100.txt", "r");

        block = size / world_size;
        offset = block;
        fseek(in, block, 1);
        adjust_offset(in, &offset);
        size -= offset;
        next_start = offset;

        // Send to slaves
        for (int i = 1; i < world_size; i++)
        {
            if (i == world_size - 1)
                block = size;
            else
                block = size / (world_size - i);
            next_offset = block;
            fseek(in, block, 1);
            adjust_offset(in, &next_offset);
            
            // Send start and offset
            MPI_Send(&next_start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&next_offset, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

            // Update start and size
            next_start += next_offset;
            size -= next_offset;
        }
        
        count_words(in, start, offset, global_map);
        fclose(in);


    }

    else
    {
        local_map = hashmap_new();
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        FILE *in;
        in = fopen("./big_file/big_100.txt", "r");
        count_words(in, start, offset, local_map);
        fclose(in);

        hashmap_items(local_map, &words, (any_t**)(&freq), &num);

        hashmap_free(local_map);
        MPI_Send(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        // Send to master
        char *cur = NULL;
        char *num_ch;
        num_ch = (char*)malloc(30);
        for (int i = 0; i < num; i++)
        {
            if (cur != NULL)
                word_len = strlen(cur) + strlen(words[i]) + 50;
            else
            {
                word_len = strlen(words[i]) + 50;
            }

            cur = realloc(cur, word_len);
            strcat(cur, words[i]);
            strcat(cur, SEP);
            sprintf(num_ch, "%d%s", *(freq[i]), SEP);
            strcat(cur, num_ch);
        }

        word_len = strlen(cur) + 1;
        MPI_Send(&word_len, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        MPI_Send(cur, word_len, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        free(num_ch);
        free(cur);
    }

    // MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (!world_rank)
    {
        for (int i = 1; i < world_size; i++)
        {
                
                MPI_Recv(&word_len, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                word = (char*)malloc(word_len + 5);
                MPI_Recv(word, word_len, MPI_CHAR, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                
                char *tok;
                char *fq;
                tok = strtok(word, SEP);
                while (tok != NULL)
                {
                    fq = strtok(NULL, SEP);
                    // printf("%s\n", fq);
                    add_word(global_map, tok, atoi(fq));
                    tok = strtok(NULL, SEP);
                }
                // free(word);
        }
    }

    free(words);
    free(freq);


    if (!world_rank)
    {
        hashmap_items(global_map, &global_words, (any_t**)&global_freq, &global_num);   
        int sum = 0;
        for (int i = 0; i < global_num; i++)
        {
            printf("%s: %d\n", global_words[i], *(global_freq[i]));
            sum += *(global_freq[i]);
        }

        end = MPI_Wtime();
        printf("==========================================================\nThere are %d words\n", sum);
        free(global_words);
        free(global_freq);


        printf("time: %f seconds\n", end - begin);
    }

    MPI_Finalize();
    
    
    return 0;
}
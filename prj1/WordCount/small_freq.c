#include <mpi.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#define MAX_FILE 100
#define MAX_WORD 300
#define SEP "\t"


char* add_head(char* filename, char* head)
{
    char* file_path;
    file_path = (char*)malloc(MAX_FILE);
    strcpy(file_path, head);
    strcat(file_path, filename);
    return file_path;
}



int is_white_space(char ch)
{
    if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\f' || ch == '\t' || ch == '\v')
        return 1;
    return 0;
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


void count_words(char* filename, map_t map)
{
    FILE *in;
    in = fopen(filename, "r");
    char ch=' ';
    char pre=' ';
    int temp;
    int count=0;
    char *word;
    int index=0;
    int *cnt;

    int *tmp;

    word = (char*)malloc(MAX_WORD);

    while((temp=fgetc(in)) != EOF)
    {
        ch = temp;
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


    fclose(in);
    free(word);
    // return count;
}



int main(int argc, char** argv)
{
    struct dirent *de;

    DIR *dr = opendir("./small_file");

    char **files = NULL;
    int file_num = 0;
    char *filename;
    int file_len;
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int to_proc_num=0;
    int to_send;
    // int global_count=0;
    int num=0;
    char **words=NULL;
    int **freq=NULL;

    char **global_words=NULL;
    int **global_freq=NULL;
    int global_num=0;
    int word_len;
    char *word;
    int word_freq;

    map_t global_map;
    map_t local_map;

    

    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    if (world_rank)
    {
        local_map = hashmap_new();
        filename = (char*)malloc(MAX_FILE);
    }
        
    if (!world_rank)
    {
        global_map = hashmap_new();
        // Distribute tasks
        while ((de = readdir(dr)) != NULL) 
        {
            if (de->d_name[0] == '.')
                continue;
            to_send = file_num % world_size;
            filename = add_head(de->d_name, "small_file/");
            if (to_send)
            {
                file_len = strlen(filename);
                MPI_Send(&file_len, 1, MPI_INT, to_send, 0, MPI_COMM_WORLD);
                MPI_Send(filename, file_len + 1, MPI_CHAR, to_send, 1, MPI_COMM_WORLD);
            }
                
            else
            {
                to_proc_num++;
                files = (char**)realloc(files, to_proc_num * sizeof(char*));
                files[to_proc_num - 1] = strdup(filename);
            }
            ++file_num;
            free(filename);
        }

        // Send terminal signal
        int terminal = -1;
        for (int i = 1; i < world_size; i++)
        {
            MPI_Send(&terminal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 0; i < to_proc_num; i++)
        {
            count_words(files[i], global_map);
        }
    }

  
    // Process tasks
    else
    {
        while (1)
        {
            MPI_Recv(&file_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            if (file_len == -1)
                break;
            MPI_Recv(filename, file_len + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            to_proc_num++;
            files = (char**)realloc(files, to_proc_num * sizeof(char*));
            files[to_proc_num - 1] = strdup(filename);

        }

        for (int i = 0; i < to_proc_num; i++)
        {
            count_words(files[i], local_map);
        }

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

    // Recv
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
                    add_word(global_map, tok, atoi(fq));
                    tok = strtok(NULL, SEP);
                }
                // free(word);
        }
        
    }

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    // for (int i = 0; i < num; i++)
    // {
    //     free(words[i]);
    // }
    free(words);
    free(freq);

    for (int i = 0; i < to_proc_num; i++)
    {
        free(files[i]);
    }
    free(files);

    if (!world_rank)
    {
        hashmap_items(global_map, &global_words, (any_t**)&global_freq, &global_num);   
        int sum = 0;
        for (int i = 0; i < global_num; i++)
        {
            printf("%s: %d\n", global_words[i], *(global_freq[i]));
            sum += *(global_freq[i]);
        }
        printf("==========================================================\nThere are %d words\n", sum);
        free(global_words);
        free(global_freq);

        printf("time: %f seconds\n", end - start);
        closedir(dr);
    }

    else
    {
        free(filename);
    }

    MPI_Finalize();

    return 0;
}
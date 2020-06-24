#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"
#define MAX_WORD 30


int is_white_space(char ch)
{
    if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\f' || ch == '\t' || ch == '\v')
        return 1;
    return 0;
}

int count_words(char* filename)
{
    FILE *in;
    in = fopen(filename, "r");
    char ch=' ';
    char pre=' ';
    char temp;
    int count=0;
    char *word;
    int index=0;
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
            printf("%s\n", word);
        }
        pre = ch;
    }

    if (!is_white_space(ch))
    {
        ++count;
        word[index] = '\0';
            printf("%s\n", word);
    }

    fclose(in);
    return count;
}

int main(int argc, char** argv)
{
    map_t map;
    map = hashmap_new();
    int a = 1;
    hashmap_put(map, "123", &a);

    a = 2;
    hashmap_put(map, "223", &a);

    printf("%d\n", hashmap_length(map));

    char **words;
    int **freq;
    int num=0;
    hashmap_items(map, &words, (any_t**)&freq, &num);

    for (int i = 0; i < num; i++)
    {
        printf("%s: %d\n", words[i], *(freq[i]));
    }

    return 0;
}
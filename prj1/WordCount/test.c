#include <stdio.h>


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

    while((temp=fgetc(in)) != EOF)
    {
        ch = temp;
        if (is_white_space(ch) && (!is_white_space(pre)))
            ++count;
        pre = ch;
    }

    if (!is_white_space(ch))
        ++count;

    fclose(in);
    return count;
}

int main(int argc, char** argv)
{
    printf("%d\n", count_words("small_file/small_191.txt"));
    return 0;
}
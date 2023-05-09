#include <stdio.h>

int main()
{
    printf("main is at %p\n", &main);
    int numb = 13;
    printf("numb is at %p\n", &numb);
    numb = 127;
    printf("wrote to numb, now = %d\n", numb);
}
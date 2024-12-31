#include <stdio.h>

int main(void){
    printf("\e[2J\e[1;1H");
    printf("\e[7m");
    printf("hello world\n");
    // printf("\e[37m\e[47m");
    // printf("hello world\n");
    printf("\e[49m\e[39m");
    return(0);
}


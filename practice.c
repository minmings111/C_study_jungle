#include <stdio.h>
#include <stdlib.h>

int main(void) {

    int a[] = {10, 20, 30};
    int b[] = {40, 50, 60};
    int c[] = {70, 80, 90};

    int *arr[] = {&a, &b, &c};

    printf("%d\n", **(arr+1));

    return 0;

}
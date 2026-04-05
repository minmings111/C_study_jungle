#include <stdio.h>
#include <stdlib.h>

struct Productinfo{
    int infonum;
    char productname[100];
    int cost;
};

void swapProduct(struct Productinfo *first, struct Productinfo *second){
    struct Productinfo temp = *first;
    *first = *second;
    *second = temp;

}

void discountCost(struct Productinfo *prodt, int percent){
    prodt->cost -= (prodt->cost * percent)/100;
}

int main(void) {

    struct Productinfo apple = {10000, "green_apple", 4000};
    struct Productinfo banana = {10001, "dot_banana", 10000};

    discountCost(&banana, 10);
    printf("%d\n", banana.cost);

    swapProduct(&apple, &banana);
    printf("%d\n", banana.infonum);


    return 0;

}
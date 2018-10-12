
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]){

    int size;
    int spacing;
    int starnum;
    
    size = atoi(argv[1]);
    spacing =(size - 1)/2;
    starnum = 1;

    if(argc != 2){                                                              //checking that only one argument exists
        printf("ERROR: Wrong number of arguments. One required. \n");
    }
    else if ( size<= 0 || size%2 == 0){
        printf("ERROR: Bad argument. Height must be positive odd integer \n");
    }
    else{
        for(int count=(size/2); count>0; count--){
            for(int i=0; i<spacing; i++){
                printf(" ");
            }
            spacing = spacing -1;

            for(int e=0; e<starnum; e++){
                printf("*");
            }
            printf("\n");
            starnum = starnum +2;
        }    
        for(int count=0; count<(size/2) +1 ; count++){
            for(int i=0; i<spacing; i++){
                printf(" ");
            }
            spacing = spacing +1;

            for(int e=0; e<starnum; e++){
                printf("*");
            }
            printf("\n");
            starnum = starnum -2;
        }    
    }
    return 0;
}   


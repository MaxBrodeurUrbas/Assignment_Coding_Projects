#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void print_di(char *matrix[], int original_h, int height, int level,  char unit, int layer){
    
    int star_count = 1;
    int topColumn = original_h/2 ;
    int current_column = height/2;
    
    if(level == 1){
        if(layer==0){
            for(int i=0; i<height/2+1; i++){
                for(int a=0; a < star_count; a++){
                        matrix[topColumn + a][i] = unit;
                }
                star_count = star_count + 2;
                topColumn--;
            }

             print_di(matrix, original_h, height, level, '0', layer+1);
        }
        if(layer!=0){ 

            if((height/2+1)*pow(2,layer) >= original_h){
                return;
                }
            for(int i=0; i< (height/2+1) * pow(2, layer -1); i++){
                        for(int a= - (height/2+1) * pow(2, layer-1)+1 ; a < (height/2+1) * pow(2, layer-1) ; a++){
                                matrix[topColumn + (height/2 +1) * (int)pow(2,layer-1) + a][(int)pow(2,layer-1)*(height/2+1) + i] = matrix[topColumn + a][i];
                        }
                    }   
            for(int i=0; i< (height/2+1) * pow(2, layer-1); i++){
                        for(int a= - (height/2+1) * pow(2, layer-1)+1  ; a < (height/2+1) * pow(2, layer-1); a++){
                                matrix[topColumn - (height/2 +1) * (int)pow(2,layer-1) + a][(int)pow(2,layer- 1)*(height/2+1) + i] = matrix[topColumn + a][i];
                        }
                    }
                print_di(matrix, original_h, height, level, '*', layer+1);
        }
    }
    
    else{
        print_di(matrix, original_h, height/2, level-1, unit, layer); 
        
    } 

    
}

int main(int argc, char* argv[]){

    int size;
    int height = atoi(argv[1]);
    int level = atoi(argv[2]);    

    if (level != 1 && (ceil(log2(height/2+1)) != floor(log2(height/2+1))|| (height/2+1 < (int)pow(2, level-1)))){
        printf("ERROR: Height does not allow evenly dividing requested number of levels\n");
        return(0);
    }

    char *matrix[height];
    for(int z=0; z<height ; z++)
        matrix[z]=(char *)malloc(sizeof(char)*((height/2)+1) );

    for(int i=0; i<height/2+1; i++){
        for(int a=0; a<height; a++){
             matrix[a][i]= ' ';
        }
    }
    
    if(argc != 3){                                                            
        printf("ERROR: Wrong number of arguments. One required. \n");
    }
    else if ( height<= 0 || height%2 == 0){
        printf("ERROR: Bad argument. Height must be positive odd integer \n");
    }

     print_di(matrix, height, height, level, '*', 0);

    for(int i=0; i<height/2+1; i++){
        for(int a=0; a<height; a++){
            printf("%c", matrix[a][i]);
        }
        printf("\n");
    }
    for(int i=height/2-1; i>=0; i--){
        for(int a=0; a<height; a++){
            printf("%c", matrix[a][i]);
        }
        printf("\n");
    }
    return 0;
    }
    







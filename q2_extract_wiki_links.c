#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){

FILE *fp;

char* FileName = argv[1];

if((fp = fopen(FileName, "r")) == NULL){
    printf("ERROR OPENING FILE");
    exit(1);
    }

char input_char;
size_t sz;
fseek(fp, 0L, SEEK_END);
sz = ftell(fp);
rewind(fp);
char file_data_array[sz+1];
fread(file_data_array, 1, sz+1, fp);

const char open_needle[] = "<a href=\"/wiki/";
const char close_needle[] = "\"";
const char title_needle[]= "title=";
const char line_end_needle[] = "</a>";

char *first_position;
char *second_position;
char *title_position;
char *line_end_position;

first_position = strstr(file_data_array, open_needle);

if(first_position == NULL){
        return 0;
    }
    
first_position = first_position +15; 

second_position = strstr(first_position, close_needle);

if(second_position == NULL){
    printf("fuck");
        return 0;
    }

title_position = strstr(second_position, title_needle);
line_end_position = strstr(second_position, line_end_needle);

int difference = second_position - first_position;

if(line_end_position > title_position){
    for(int i =0; i<difference; i++){
        printf("%c", first_position[i]);
        }
    printf("\n");
    }

while(first_position != NULL){
    first_position = strstr(second_position, open_needle);
    if(first_position == NULL){
        return 0;
    }
    first_position = first_position +15;

    second_position = strstr(first_position, close_needle);
    line_end_position = strstr(second_position, line_end_needle);
    title_position = strstr(second_position, title_needle);

     if(title_position == NULL){
        return 0;
    }

    difference = second_position - first_position;

    if(line_end_position > title_position){
        for(int i =0; i<difference; i++){
            printf("%c", first_position[i]);
        }   
        printf("\n");
         }
    }
fclose(fp);
}
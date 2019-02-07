/* FILE: A3_solutions.c is where you will code your answers for Assignment 3.
 * 
 * Each of the functions below can be considered a start for you. They have 
 * the correct specification and are set up correctly with the header file to
 * be run by the tester programs.  
 *
 * You should leave all of the code as is, especially making sure not to change
 * any return types, function name, or argument lists, as this will break
 * the automated testing. 
 *
 * Your code should only go within the sections surrounded by
 * comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A3_solutions.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "A3_provided_functions.h"

unsigned char*
bmp_open( char* bmp_filename,        unsigned int *width, 
          unsigned int *height,      unsigned int *bits_per_pixel, 
          unsigned int *padding,     unsigned int *data_size, 
          unsigned int *data_offset)
{
  unsigned char *img_data=NULL;
  if(bmp_filename==NULL)
    return NULL;

  FILE *bmpfile = fopen(bmp_filename, "rb");

  if(bmpfile == NULL){
    printf("I was unable to open the file.\n");
    return NULL;
  }

  char header[54];

  fread(header,1,54,bmpfile);

  *data_size = *(int*)&header[2];
  *data_offset = *(int*)&header[10];
  *width = *(int*)&header[18];
  *height = *(int*)&header[22];
  *bits_per_pixel = *(short int*)&header[28];


  int remainder = ((*width)*(*bits_per_pixel))%32;
  if(remainder == 0)
    *padding = 0;
  else *padding = (32 - remainder)/8;

  img_data = (unsigned char*)malloc(*data_size*sizeof(char));
  rewind(bmpfile);
  fread(img_data,1,*data_size,bmpfile);
  return img_data;

}

void 
bmp_close( unsigned char **img_data )
{

  if(img_data == NULL)
    return;
  
  if(*img_data == NULL)
    return;

  free(*img_data);
  *img_data = NULL;
}

unsigned char***  
bmp_scale( unsigned char*** pixel_array, unsigned char* header_data, unsigned int header_size,
           unsigned int* width, unsigned int* height, unsigned int num_colors,
           float scale )
{
  unsigned char*** new_pixel_array = NULL; 

  *height = (int)((*height)*(scale));
  *width = (int)((*width)*(scale));

  new_pixel_array = (unsigned char***)malloc( sizeof(char**) * (*height));
  
  for( int row=0; row<*height; row++ ){
    new_pixel_array[row] = (unsigned char**)malloc( sizeof(char*) * (*width) );
    for( int col=0; col<*width; col++ ){
      new_pixel_array[row][col] = (unsigned char*)malloc( sizeof(char) * (num_colors) );
    }
  }
  for(int row=0; row<*height; row++ ){
    for(int col=0; col<*width; col++ ){
      for(int num=0; num <num_colors; num++)
      new_pixel_array[row][col][num] = pixel_array[(int)(row/scale)][(int)(col/scale)][num];
    }
  }

    int* imageSize = (int*) &header_data[34];
    *(int*) &header_data[18] = *width;
    *(int*) &header_data[22] = *height;
    *imageSize = *width * *height * num_colors;

  return new_pixel_array;
}         

int 
bmp_collage( char* background_image_filename,     char* foreground_image_filename, 
             char* output_collage_image_filename, int row_offset,                  
             int col_offset,                      float scale )
{
  unsigned char* f_header_data;
  unsigned int f_header_size, f_width, f_height, f_num_colors; 

  unsigned char* b_header_data;
  unsigned int b_header_size, b_width, b_height, b_num_colors;  

  unsigned char*** background = bmp_to_3D_array(background_image_filename, &b_header_data, &b_header_size, &b_width, &b_height, &b_num_colors);
  unsigned char*** foreground = bmp_to_3D_array(foreground_image_filename, &f_header_data, &f_header_size, &f_width, &f_height, &f_num_colors);

  if(background == NULL | foreground==NULL){
    printf("Converting one or both BMPs to a 3-d resulted in a NULL pointer");
    return -1;
  }

  foreground = bmp_scale( foreground, f_header_data, f_header_size, &f_width, &f_height, f_num_colors, scale);

  if(foreground == NULL){
    printf("Scaling the foreground resulted in a NULL pointer");
    return -1;
  }

  for(int i=0; i<f_height; i++){
    for(int a=0; a<f_width; a++){
      if(foreground[i][a][0] != 0){
        for(int c=0; c<f_num_colors; c++)
          background[i + row_offset][a +col_offset][c] = foreground[i][a][c];
      }
    }
  }

  int result = bmp_from_3D_array(output_collage_image_filename, b_header_data, b_header_size, background, b_width, b_height, b_num_colors);

  if(result == -1){
    printf("Creating a bmp from the 3D array resutled in an Error");
    return -1;
  }
  return 0;
}              


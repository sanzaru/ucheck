/**
 * Simple CSV reader library
 */
#include "csvlib.h"
 
int csvread(char *filename, char *buffer[]) {
  char c, *tmp[sizeof(buffer)];
  int i=0, val=0, v=0, index=0;
  FILE *fd;  
    
  fd = fopen(filename, "r");
  if( fd == NULL ) {
    fprintf(stderr, "Error: Cannot read csv file %s!\n", filename);
    return 0;
  }
    
  while( c != EOF && val <= sizeof(buffer) ) {     
    while( (c = getc(fd)) != '\n' ) {      
      tmp[v] = (char*)malloc(CSV_BUF_SIZE);
      v=0;
      if( c != ',' && c != ';' && c != '\t' ) {
        tmp[v][i++] = c;
      } else {
        i=0; 
        v++;             
        printf("Tmp: %s\n", tmp[v]);
        buffer[val][index++] = tmp[v];
        memset(tmp, 0, sizeof(tmp));
      }      
    }    
    index = 0;
    val++;    
  }
  
  fclose(fd);
  return 1;
}


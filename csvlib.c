/**
 * Simple CSV reader library
 */
#include "csvlib.h"
 
int csvread(char *filename, char *buffer[]) {
  char c, *tmp[];
  int i=0, val=0, v=0, index=0;
  FILE *fd;  
    
  fd = fopen(filename, "r");
  if( fd == NULL ) {
    fprintf(stderr, "Error: Cannot read csv file %s!\n", filename);
    return 0;
  }
  
  if( sizeof(buffer) <= 0 ) {
    fprintf(stderr, "Error: Size of CSV buffer cannot be zero!\n");
    return 0;
  }
  
  printf("Buffer size: %d\nTmp size: %d\n", sizeof(buffer), sizeof(tmp));
    
  while( c != EOF && val <= sizeof(buffer)/2 ) {     
    while( (c = getc(fd)) != '\n' ) {            
      tmp[v] = (char*)malloc(CSV_BUF_SIZE);      
      if( c != ',' && c != ';' && c != '\t' ) {
        tmp[v][i++] = c;
      } else {        
        i=0;         
      }
      printf("Tmp[%d]: %s\n", v, tmp[v]);
      v++;
      buffer[val][index++] = tmp[v];
      memset(tmp[v], 0, sizeof(tmp[v]));
    }
    v=0;
    index = 0;
    val++;    
  }
  
  fclose(fd);
  return 1;
}


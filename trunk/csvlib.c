/**
 * Simple CSV reader library
 */
#include <stdlib.h>
#include <stdio.h>
 
void csv_read(FILE *fd, char **buffer) {
  char c;
  int i=0, val=0;
  while( c != EOF ) {   
    while( (c = getc(fd)) != '\n' && c != ',' && c != ';' ) {
      buffer[val][i++] = c;
    }
    val++;
  }
}


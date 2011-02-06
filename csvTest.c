#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csvlib.h"

int main(int argc, char **argv) {
  char *buffer[2];  
  int status;
  char *filename = "test.csv";    
  if( csvread(filename, buffer) == 1 ) {
    printf("Status: %d\nBuffer[0][1]: %s\n", status, buffer[0]);
  }
  return 0;
}


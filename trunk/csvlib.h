/**
 * Header file for the simple CSV reader library
 * 
 * @author Martin Albrecht <martin.albrecht@javacoffee.de>
 * @version 0.2
 */
#ifndef __CSVLIB_H__
#define __CSVLIB_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CSV_BUF_SIZE 65536

int csvread(char *filename, char **buffer);

#endif

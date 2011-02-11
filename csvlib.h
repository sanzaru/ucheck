/**
 * Header file for the simple CSV reader library
 */
#ifndef __CSVLIB_H__
#define __CSVLIB_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CSV_BUF_SIZE 65536

int csvread(char *filename, char **buffer);

#endif

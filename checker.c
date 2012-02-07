/**
 * URL check program
 * -------------------
 * This program checks the HTTP status for given URLs. The URLs should be in a 
 * text file (one URL per line) and they MUST be relative.
 * To check the URL the program fetches the HTTP header for the given URL and
 * validates the returned status.
 *
 * Usage example: ./checker -u www.yourdomain.com -f urls_to_check.txt
 * Or simply: ./checker -h
 * to see a help screen with all available options.
 *
 * @author Martin Albrecht <martin.albrecht@javacoffee.de>
 * @version 0.2
 *
 * IMPORTANT:
 * -----------
 * This program runs on unix machines only. There is no win32 support planned
 * (anymore), so if you want the program run on windows machines, port it
 * yourself - it's open source! ;)
 * For now, not all HTTP codes are implemented and for some a UNKNOWN will be
 * returned. Full implementation of HTTP codes will be implemented, soon.
 * 
 *
 * LICENSE:
 * --------
 * Copyright (C) 2010 Martin Albrecht <martin.albrecht@javacoffee.de>
 *
 * This library is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along 
 * with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <regex.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define FALSE 0
#define TRUE 1
#define HTTP_HEADER "HEAD %s HTTP/1.1\r\nHost: %s\r\n\r\n"
#define MAX_BUF_SIZE 256


/* Show help */
void show_help() {
  printf("Help:\n \
    -h            Show this screen\n \
    -u <url|ip>   URL or IP address to check\n \
                  NOTE: URL without http:// ! \
    -s <seconds>  Time to sleep (in seconds)\n \
    -v            Verbose mode\n \
    -l <filename> Log file to log all messages to \
    -f <filename> File with URLs to check (one URL per line)\n\n");      
}

/* Match Pattern */
int match(char *pattern, char *source) {
  regex_t re;
  int status;
  regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB);
  status = regexec(&re, source, (size_t)0, NULL, 0);
  regfree(&re);
  /*printf("Status: %d\n", status);*/
}

/* Log message */
void log_message(char *filename, char *message) {
  FILE *fd;
  char timebuf[60];
  struct tm *time_info;
  time_t rtime;
  
  if( strlen(message) > 0 ) {    
    /* Build the date string */
    time(&rtime);
    time_info = localtime(&rtime);
    strftime(timebuf, 60, "%d/%m/%Y %H:%M:%S", time_info);
    
    /* Log message and date string to file */
    fd = fopen(filename, "a+");
    fprintf(fd, "%s:\t%s\n", timebuf, message);
    fclose(fd);
  }
}


/* Main function */
int main(int argc, char **argv) {
  FILE *fd;
  FILE *out;
  char *buf, c, url[MAX_BUF_SIZE], buffer[65536];
  int i, sock, res, round, valid, invalid, o=-1;  
  struct sockaddr_in addr;
  struct hostent *h = NULL;
  char befehl[386] , message[MAX_BUF_SIZE], 
       bltest[MAX_BUF_SIZE], *logfile=NULL;
  int sleepTime = 0, verbose = FALSE;  
  
  if( argc < 2 ) {
    fprintf(stderr, "Error: Not enough arguments given!\nTry %s -h for help!\n", argv[0]);
    return 1;
  }
  
  while( (o = getopt(argc, argv, "vhu:f:s:l:")) != -1 ) {
    switch(o) {    
      case 'h':
        show_help();
        return 0;
        break;
        
      /* URL to fetch */
      case 'u':
        if( inet_addr(optarg) == INADDR_NONE && (h = gethostbyname(optarg)) == NULL ) {    
          fprintf(stderr, "Error: Invalid address!\n"); 
          return 1;
        }        
        if( h != NULL ) {
          memcpy(&addr.sin_addr, h->h_addr_list[0], h->h_length);
        } else {    
          addr.sin_addr.s_addr = inet_addr(optarg);
        }
        break;
      
      /* File with URLs to check */
      case 'f':
        fd = fopen(optarg, "r");
        if( fd == NULL ) {
          fprintf(stderr, "Error: Cannot open file!\n");
          return 1;
        }
        break;
      
      /* Sleep time */
      case 's':
        sleepTime = atoi(optarg);
        break;
      
      /* Verbose */
      case 'v':
        verbose = TRUE;
        break;
        
      /* Log file */
      case 'l':
        logfile = optarg;
        break;
        
      /* Error check */
      case '?':
        if( optopt == 'u' || optopt == 'f' || optopt == 's' || optopt == 'l' ) {
          fprintf(stderr, "Error: Option -%c requires an argument!\n", optopt);
          return 1;
        } else {
          fprintf(stderr, "Error: Unknown option -%c!\n", optopt);
          return 1;
        }
        break;
        
      /* Default */
      default:
        printf("usage: %s <arguments...>\nTry \"%s -h\" for help!\n", argv[0], argv[0]);
        return 1;
        break;
    }
  }  
          
  addr.sin_family = AF_INET;
  addr.sin_port = htons(80);  
  valid = invalid = 0;
  round  = 1;
  while( c != EOF ) {    
    i = 0;        
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if( sock <= 0 )  {
      fprintf(stderr, "Error: Cannot open socket!\n");
      return 1;
    }
    
    while( (c = getc(fd)) != '\n' && c != EOF ) {
      url[i++] = c;
    }
    if( strlen(url) <= 0 ) {
      continue;
    }
    url[i] = '\0';
    sprintf(befehl, HTTP_HEADER, url, argv[1]);
    
    /* Connect to address and try to fetch HTTP header */
    if( connect(sock, (struct sockaddr*) &addr, sizeof(addr)) != -1 ) {      
      write(sock, befehl, sizeof(befehl));
      res = read(sock, buffer, sizeof(buffer));          
      
      if( verbose == TRUE ) {
        printf("%s", buffer);
      }
      
      sprintf(bltest, "Location: %s", url);
      match(bltest, buffer);
      if( strstr(buffer, "HTTP/1.1 200") == NULL) {        
        if( strstr(buffer, "HTTP/1.1 301") != NULL) {
          sprintf(message, "301: %s\n\0", url);
        } else if( strstr(buffer, "HTTP/1.1 302") != NULL) {
          sprintf(message, "302: %s\n\0", url);
        } else if( strstr(buffer, "HTTP/1.1 404") != NULL) {
          sprintf(message, "404: %s\n\0", url); 
        } else if( strstr(buffer, "HTTP/1.1 400") != NULL) {
          sprintf(message, "400: %s\n\0", url);
        } else {
          sprintf(message, "UNKNOWN: %s\n\nData: %s\n\0", url, buffer);
        }                
      }
      
      if( (round%100) == 0 && verbose == TRUE ) {
        printf("Round %d\n", round);
      }
    } else {
      fprintf(stderr, "Error: No connection to: %s\n", argv[1]);
    }       
    round++;
    close(sock);
    
    if( logfile != NULL ) {
      log_message(logfile, message);
    }
    
    if( sleepTime > 0 ) {
      sleep(sleepTime);
    }
  } 
  fclose(fd);  
  printf("All done! Terminating...\n");
  return 0;
}


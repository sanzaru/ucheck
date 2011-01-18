/**
 * URL check program
 * This program checks the HTTP status for given URLs. The URLs should be in a 
 * text file (one URL per line) and they MUST be relative.
 *
 * Usage example: ./checker -u www.yourdomain.com -f urls_to_check.txt
 * Or simply: ./checker -h
 * to see a help screen with all available options.
 *
 * @author Martin Albrecht <martin.albrecht@javacoffee.com>
 * @version 0.1
 *
 * IMPORTANT:
 * -----------
 * This program runs on unix machines only. There is no win32 support planned
 * (anymore), so if you want the program run on windows machines, port it
 * yourself - it's open source! ;)
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
#define HTTP_HEADER "HEAD %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n"

void show_help() {
  printf("Help:\n \
    -h            Show this screen\n \
    -u <url|ip>   URL or IP address to check\n \
    -f <filename> File with URLs to check\n\n");      
}


int main(int argc, char **argv) {
  FILE *fd;
  FILE *out;
  char *buf, c, url[256], buffer[65536];
  int i, sock, res, round, valid, invalid, o=-1;  
  struct sockaddr_in addr;
  struct hostent *h = NULL;
  char befehl[386] , message[256];
  
  /** 
   * Check command line arguments 
   */  
  while( (o = getopt(argc, argv, "hu:f:")) != -1 ) {
    switch(o) {    
      case 'h':
        show_help();
        return 0;
        break;
        
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
      
      case 'f':
        fd = fopen(optarg, "r");
        if( fd == NULL ) {
          fprintf(stderr, "Error: Cannot open file!\n");
          return 1;
        }
      break;
        
      case '?':
        if( optopt == 'u' || optopt == 'f' ) {
          fprintf(stderr, "Error: Option -%c requires an argument!\n", optopt);
          return 1;
        } else {
          fprintf(stderr, "Error: Unknown option -%c!\n", optopt);
          return 1;
        }
        break;
        
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
    out = fopen("invalid_urls.txt", "a+");    
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
      if( strstr(buffer, "HTTP/1.1 200") == NULL) {        
        if( strstr(buffer, "HTTP/1.1 301") != NULL) {
          sprintf(message, "301: %s\n\0", url);
        } else if( strstr(buffer, "HTTP/1.1 302") != NULL) {
          sprintf(message, "302: %s\n\0", url);
        } else if( strstr(buffer, "HTTP/1.1 404") != NULL) {
          sprintf(message, "404: %s - (%d)\n\0", url, strlen(url));
        } else if( strstr(buffer, "HTTP/1.1 400") != NULL) {
          sprintf(message, "400: %s\n\0", url);
        } else {
          sprintf(message, "UNKNOWN: %s\n\0", url);
        }        
        fwrite(message, sizeof(char), strlen(message), out);
      }
      
      if( (round%100) == 0 ) {
        printf("Round %d\n", round);
      }
    } else {
      fprintf(stderr, "Error: No connection to: %s\n", argv[1]);
    }       
    round++;
    close(sock);
    fclose(out);
  } 
  fclose(fd);  
  printf("All done! Terminating...\n");
  return 0;
}


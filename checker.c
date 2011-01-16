/**
 * URL check program
 * This program checks the HTTP status for given URLs. The URLs should be in a 
 * text file (one URL per line) and they MUST be relative.
 *
 * Usage example: ./checker 192.168.1.1 urls_to_check.txt
 *
 * @author Martin Albrecht <martin.albrecht@javacoffee.com>
 * @version 0.1
 *
 * IMPORTANT:
 * -----------
 * For now, this program only runs on unix systems, but win32 support is
 * planned for future versions.
 * Also, it is only possible to check IP addresses, not URL's. URL support is
 * also planned for the future.
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

#ifndef _WIN32
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <sys/param.h>
  #include <netinet/in.h>
  #include <unistd.h>
  #include <signal.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
#endif

#ifndef _WIN32
int main(int argc, char **argv) {
  FILE *fd;
  FILE *out;
  char *buf, c, url[256], buffer[65536];
  int i, sock, res, round, valid, invalid;
  struct sockaddr_in addr;
  struct hostent *h;
  char befehl[386] , message[256];
  
  /** 
   * Check command line arguments 
   */
  if( argc < 3 ) {
    printf("usage: %s <ip address> <filename>\n", argv[0]);
    return 1;
  }
    
  /** 
   * Load file with URLs 
   */
  fd = fopen(argv[2], "r");
  if( fd == NULL ) {
    fprintf(stderr, "Error: Cannot open file!\n");
    return 1;
  }
  
  /**
   * Check IP address
   * TODO Implement DNS fallback
   */
  if( inet_addr(argv[1]) == INADDR_NONE ) {
    fprintf(stderr, "Error: Invalid IP address!\n");
    return 1;
  }
      
  addr.sin_family = AF_INET;
  addr.sin_port = htons(80);  
  addr.sin_addr.s_addr = inet_addr(argv[1]);
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
    
    /** 
     * Read in one line 
     */
    while( (c = getc(fd)) != '\n' ) {
      url[i++] = c;
    }
    url[i] = '\0';
    sprintf(befehl, "HEAD %s HTTP/1.1\r\nHost: www.macnews.de\r\nConnection: close\r\n\r\n", url);
    
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
          sprintf(message, "404: %s\n\0", url);
        } else if( strstr(buffer, "HTTP/1.1 400") != NULL) {
          sprintf(message, "400: %s\n\0", url);
        } else {
          sprintf(message, "UNKNOWN: %s\n\0", url);
        }
        /*fprintf(stdout, "%s", message);*/
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
  return 0;
}
#else
  int main() {
    fprintf("Windows systems are not supported, yet!\n");
    return 1;
  }
#endif

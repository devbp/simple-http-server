#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include"http_parser.h"
#include <string.h>

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[4000];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;

   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001;

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr =  inet_addr("127.0.0.1");
   serv_addr.sin_port = htons(portno);

   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */

   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   while(1){
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }

   /* If connection is established then start communicating */
   bzero(buffer,4000);
   n = read( newsockfd,buffer,4000 );

   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

  int len=strlen(buffer);
  printf("length of buffer %d",len);

	   printf("\n%s\n",buffer);

	   /*
	    * tokenize the content received from the client
	    *  parse json and perform verification.
	    */
	   char *token = NULL;
	   token = strtok(buffer, "\n");
	       while (token) {
	           printf("Current token: %s.\n", token);
	           token = strtok(NULL, "\n");
	       }
       char response[1000];
       strcpy(response,"HTTP/1.1 200 OK\nContent-length: 47\nContent-Type: text/json\n\n<html><body><H1>Hello Dev</H1></body></html>");
       /*
        * send the response in josn, html or other formats
        */
       write(newsockfd,response,sizeof(response));
       if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
     }

 }
   /* Write a response to the client */
 //  n = write(newsockfd,"I got your message",18);

  // if (n < 0) {
    //  perror("ERROR writing to socket");
     // exit(1);
   //}

   return 0;
}

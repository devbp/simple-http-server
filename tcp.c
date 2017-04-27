#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include"http_parser.h"
#include <string.h>


  SSL_CTX *ctx;
void init_openssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
	perror("Unable to create SSL context");
	ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context()
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }
}

void connection_handler(int client)
{

	SSL *ssl;
	char buffer[4000];
	int n;
	int ssl_status=0;
	/* If connection is established then start communicating */
	bzero(buffer,4000);
	//n = read( client,buffer,4000 );
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, client);

	if (SSL_accept(ssl) <= 0) {
	ERR_print_errors_fp(stderr);
	perror("NO SSL reading from socket");

	n= read(client,buffer,sizeof(buffer));
	}
	else {

	ssl_status=1;
	n=SSL_read(ssl, buffer, sizeof(buffer));

	}

	if (n < 0) {
	perror("ERROR reading from socket");

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
	while (token)
	{
	printf("Current token: %s.\n", token);
	token = strtok(NULL, "\n");
	}

	char response[1000];
	strcpy(response,"HTTP/1.1 200 OK\nContent-length: 47\nContent-Type: text/html\n\n<html><body><H1>Hello Dev</H1></body></html>");


	/*
	* send the response in josn, html or other formats
	*/

	if(ssl_status)
	n=SSL_write(ssl, response, sizeof(response));
	else
	n=write(client,response,sizeof(response));
	SSL_free(ssl);

	if (n < 0)
	{
	perror("ERROR writing to socket");

	}

}


int main( int argc, char *argv[] )
{
   int sockfd, client, portno, clilen;
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   int ssl_status=0;
   pthread_t thread;
   init_openssl();
   ctx = create_context();
   configure_context();


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
   int i=0;
   while(1)
   {
   /* Accept actual connection from the client */

	client = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    if (client < 0)
    {
      perror("ERROR on accept");

    }
    if( pthread_create( &thread , NULL ,  connection_handler , client) < 0)
           {
               perror("could not create thread");
               return 1;
           }


   }
   close(sockfd);
   cleanup_openssl();
   return 0;
}

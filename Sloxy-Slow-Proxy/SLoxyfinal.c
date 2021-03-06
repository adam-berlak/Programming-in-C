/*
 * Proxy.c
 *  This program is a Basic HTTP proxy which receives the HTTP Requests from the web browser
 *  and forward it to the desired web server. After receiving the response from the web server
 *  it forward it to the web browser of the client.
 *  The proxy listens on port 8001
 *  Created on: Jan 23, 2018
 */

/* Standard libraries */
#include <stdio.h>
#include <stdlib.h>

/* Libraries for socket programming */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* Library for parsing strings */
#include <string.h>
#include <strings.h>

/* h_addr?! */
#include <netdb.h>

/* Clean exit! */
#include <signal.h>

int lstn_sock;

/* The function will run after catching Ctrl+c in terminal */
void catcher(int sig) {
	close(lstn_sock);
	printf("catcher with signal  %d\n", sig);
	exit(0);

}

int main() {

	/* For catching Crtl+c in terminal */
	signal(SIGINT, catcher);
	int lstn_port = 8001;

	/* Initializing the Address */
	struct sockaddr_in addr_proxy;
	addr_proxy.sin_family = AF_INET;
	addr_proxy.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_proxy.sin_port = htons(lstn_port);
	printf("Address Initialization: done.\n");

	/* Creating the listening socket for proxy */
	lstn_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (lstn_sock < 0) {
		printf("Error in socket() call.\n");
		exit(-1);
	} else {
		printf("Listening Socket creation: done.\n");
	}

	/* Binding the socket to address and port */
	int bind_status;
	bind_status = bind(lstn_sock, (struct sockaddr *) &addr_proxy,
			sizeof(struct sockaddr_in));
	if (bind_status < 0) {
		printf("Error in bind() call.\n");
		exit(-1);
	} else {
		printf("Binding: done.\n");
	}

	/* Listening on binded port number */
	int lstn_status;
	lstn_status = listen(lstn_sock, 10);
	if (lstn_status < 0) {
		printf("Error in listen() call.\n");
		exit(-1);
	}
	/* Infinite while loop for listening accepting connection requests */
	while (1) {
		/* Accepting connection requests */
		int data_sock;
		data_sock = accept(lstn_sock, NULL, NULL);
		if (data_sock < 0) {
			printf("Error in accept() call");
			exit(-1);
		} else {
			printf("Accepting connection request: done.\n");
		}

		/* Receiving HTTP message from the client */
		char c_message_in[1024];
		char s_message_out[1024];
		int c_recv_status;
		c_recv_status = recv(data_sock, c_message_in, sizeof(c_message_in), 0);
		if (c_recv_status < 0) {
			printf("Error in recv() call for client recv message\n");
			exit(-1);
		} else {
			printf("############################################\n");
			printf("## HTTP message received from the client. ##\n ");
			printf("############################################\n");
		}

		/* Preserving the HTTP request for sending it to the web server later */
		strcpy(s_message_out, c_message_in);

		/* Parsing the HTTP message to extract the HOST name of the desired web server */
		char host[1024];
		char URL[1024];
		char PATH[1024];
		int i;

		// find and parse the GET request, isolating the URL for later use
		char *pathname = strtok(c_message_in, "\r\n");
		printf("Found HTTP request: %s\n", pathname);
		if (sscanf(pathname, "GET http://%s", URL) == 1)
			printf("URL = %s\n", URL);

		// seperate the hostname from the pathname
		for (i = 0; i < strlen(URL); i++) {
			if (URL[i] == '/') {
				strncpy(host, URL, i); //copy out the hostname
				host[i] = '\0';
				break;
			}
		}

		bzero(PATH, 500); //to clear junk at the beginning of this buffer. don't know why its there
		for (; i < strlen(URL); i++) {
			strcat(PATH, &URL[i]); //copy out the path
			break;
		}
		printf("******************************\n");
		printf("First Half(host): %s\n", host); //firstHalf is the hostname
		printf("Second Half(path): %s\n", PATH); //secondHalf is the path
		printf("******************************\n");

		/* Creating the TCP socket for connecting to the desired web server */
		// Address initialization
		struct sockaddr_in addr_server;
		struct hostent *server;

		// Getting web server's Address by its host name
		server = gethostbyname(host);
		if (server == NULL)
			printf("Error in gethostbyname() call.\n");
		else
			printf("Web server = %s\n", server->h_name);

		// Initialize socket structure
		bzero((char *) &addr_server, sizeof(addr_server));
		addr_server.sin_family = AF_INET;
		bcopy((char *) server->h_addr, (char *) &addr_server.sin_addr.s_addr, server->h_length);
		addr_server.sin_port = htons(80);

		// Creating the socket
		int web_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (web_sock < 0) {
			printf(
					"Error in socket() call for creating --proxy-WebServer-- socket.\n");
		} else {
			printf("###############################################\n");
			printf("## --proxy-WebServer-- socket creation: done ##\n");
			printf("###############################################\n");
		}

		// Connecting to the web server's socket
		int connect_status;
		connect_status = connect(web_sock, (struct sockaddr *) &addr_server,
				sizeof(addr_server));
		if (connect_status < 0) {
			printf(
					"Error in connect() call for connecting to the web server's socket.\n");
			exit(-1);
		} else {
			printf("Web server's socket connection establishment: done\n ");
		}

		/* Sending the HTTP request of the client to the web server */
		

		char buffer[1024];
		snprintf(buffer, sizeof(buffer), "HEAD %s HTTP/1.1\r\nHost: %s\r\n\r\n", PATH , host);
		printf("buffer is: %s", buffer);
		printf("get is: %s", s_message_out);

		// web_send_status = send(web_sock, s_message_out, sizeof(c_message_in),
		int web_send_status;
		web_send_status = send(web_sock, buffer, sizeof(buffer),
				0);
		if (web_send_status < 0) {
			printf(
					" Error in send() call for sending HTTP request to the web server.\n ");
			exit(-1);
		} else {
			printf("##############################################\n");
			printf("## Sending HTTP request to the server: done ##\n");
			printf("##############################################\n");
		}

		/* Receiving the HTTP response from the web server */
		char w_message_in[10000];
		int web_recv_status;
		web_recv_status = recv(web_sock, w_message_in, sizeof(w_message_in), 0);
		if (web_recv_status < 0) {
			printf(
					" Error in recv() call for receiving web server's HTTP response.\n ");
			exit(-1);
		} else {
			printf("################################################\n");
			printf("## Receiving web server's HTTP response: done ##\n");
			printf("################################################\n");
		}

		close(web_sock);

		/* Getting range, length and type from the received message */
		
		char ranges[1024];
		char length[1024];
		int intlength;
		char type[1024];
		char backup[1024];
		strcpy(backup, w_message_in);
		char *response = strtok(w_message_in, "\n");
		char output[100000];
		
		
		
		while( response != NULL ) {
			printf("%s", response);
			if (sscanf(response, "Accept-Ranges: %s", ranges) == 1)
				printf("The accepted ranges are = %s\n", ranges);
			if (sscanf(response, "Content-Length: %s", length) == 1)
				printf("The length of the content is = %s\n", length);
			if (sscanf(response, "Content-Type: %s", type) == 1)
				printf("The type of the content is = %s\n", type);
			response = strtok(NULL, "\n");
		}

		intlength = atoi(length);
		printf("intLength is: %d", intlength);
		printf("length is: %s", length);


		
		char scrap[1024];
		char completehtmldata[100000];
		if (strstr(type, "html") != NULL && strstr(ranges, "bytes") != NULL) {
			int request_size = intlength / 10;
			
			int x1 = 0;
			int x2 = request_size;
			char html_big[1024] = "";


			while (x1 <= intlength - 1) {

				// Creating the socket
				int new_sock = socket(AF_INET, SOCK_STREAM, 0);
				if (new_sock < 0) {
					printf(
							"Error in socket() call for creating --proxy-WebServer-- socket.\n");
				} else {
					printf("###############################################\n");
					printf("## --proxy-WebServer-- socket creation: done ##\n");
					printf("###############################################\n");
				}

				// Connecting to the web server's socket
				int connect_status;
				connect_status = connect(new_sock, (struct sockaddr *) &addr_server,
						sizeof(addr_server));
				if (connect_status < 0) {
					printf(
							"Error in connect() call for connecting to the web server's socket.\n");
					exit(-1);
				} else {
					printf("Web server's socket connection establishment: done\n ");
				}	
	
				char send_msg[1024];
				int new_send_status = -1;
				int new_recv_status = -1;

				snprintf(send_msg, sizeof(send_msg), "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-%d\r\n\r\n", PATH, host, x1, x1);
				printf("%s", send_msg);
				

				int web_send_status;
				new_send_status = send(new_sock, send_msg, sizeof(send_msg),0);
				if (web_send_status < 0) {
					printf(" Error in send() call for sending HTTP request to the web server.\n ");
					exit(-1);
				} 
				

				char recv_msg[1024];
				int web_recv_status;
				new_recv_status = recv(new_sock, recv_msg, sizeof(recv_msg), 0);
				if (web_recv_status < 0) {
					printf(" Error in recv() call for receiving web server's HTTP response.\n ");
					exit(-1);
				}
				// strcpy(htmldata, recv_msg[(strlen(recv_msg)-1)]

				close(new_sock);

				char copy[1024];
				
				char htmldata[1024];
				char *miniresponse = strtok(recv_msg, "\r\n\r\n");
				while( miniresponse != NULL ) {
					printf("This section is: %s\n", miniresponse);
					strcpy(copy, miniresponse);
					miniresponse = strtok(NULL, "\r\n\r\n");
					if (miniresponse == NULL) {
						strcpy(htmldata, copy);
						if (strlen(htmldata) > 1) {
							strcpy(htmldata, "\n");
						}
					}
				}


				strcat(completehtmldata, htmldata);

				x1++;
				x2 = x2 + request_size;
			}
			
			printf("client message is:\r\n%s%s", backup, completehtmldata);
			snprintf(output, sizeof(backup) + sizeof(completehtmldata), "%s%s", backup, completehtmldata);
			strcpy(completehtmldata, "");

			

		} else {
			// Creating the socket
			int new_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (new_sock < 0) {
				printf("Error in socket() call for creating --proxy-WebServer-- socket.\n");
			} else {
				printf("###############################################\n");
				printf("## --proxy-WebServer-- socket creation: done ##\n");
				printf("###############################################\n");
			}

			// Connecting to the web server's socket
			int connect_status;
			connect_status = connect(new_sock, (struct sockaddr *) &addr_server,
					sizeof(addr_server));
			if (connect_status < 0) {
				printf("Error in connect() call for connecting to the web server's socket.\n");
				exit(-1);
			} else {
				printf("Web server's socket connection establishment: done\n ");
			}	

			int new_send_status;
			new_send_status = send(new_sock, s_message_out, sizeof(c_message_in),0);
			if (web_send_status < 0) {
				printf(" Error in send() call for sending HTTP request to the web server.\n ");
				exit(-1);
			} 
				

			char recv_msg[100000];
			int new_recv_status;
			new_recv_status = recv(new_sock, recv_msg, sizeof(recv_msg), 0);
			if (web_recv_status < 0) {
				printf(" Error in recv() call for receiving web server's HTTP response.\n ");
				exit(-1);
			}
			// strcpy(htmldata, recv_msg[(strlen(recv_msg)-1)]

			close(new_sock);


			/* Closing the socket connection with the web server */
			close(web_sock);
			/* Sending the HTTP response to the client */

			int c_send_status;
			c_send_status = send(data_sock, recv_msg, sizeof(recv_msg), 0);
			if (c_send_status < 0) {
				printf(
						"Error in send() call for sending HTTP response to the client.\n");
				exit(-1);
			} else {
				printf("###############################################\n");
				printf("## Sending HTTP response to the client: done ##\n");
				printf("###############################################\n");
			}
			/* Closing the socket connection with the client */
			close(data_sock);
			printf("data socket is closed.\n");
			
		}


		/* Closing the socket connection with the web server */
		close(web_sock);
		/* Sending the HTTP response to the client */
		int c_send_status;
		c_send_status = send(data_sock, output, sizeof(output), 0);
		if (c_send_status < 0) {
			printf(
					"Error in send() call for sending HTTP response to the client.\n");
			exit(-1);
		} else {
			printf("###############################################\n");
			printf("## Sending HTTP response to the client: done ##\n");
			printf("###############################################\n");
		}
		/* Closing the socket connection with the client */
		close(data_sock);
		printf("data socket is closed.\n");


	}
	close(lstn_sock);
	printf("lstn_sock is closed.\n");
	return 0;

}

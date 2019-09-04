/*
Fatemeh Hosseini - client.c
This is a simple client program which sends DATA to the server
and get the response from server. For running this example,
first run server.c then client.c
*/
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <math.h>


#define DATA "Hello Server"
int main(int argc, char *argv[])
{
	int sock;
	int rval;
	struct sockaddr_in server;
	struct sockaddr *addr_server = (struct sockaddr *) &server;
	struct hostent *hp;
	char buff[1400];
	sock = socket(AF_INET, SOCK_STREAM, 0);
	char info[1024];
	if( sock <0)
	{
		 perror("socket failed");
		 close(sock);
		 exit(1);
	}
	server.sin_family=AF_INET;

	server.sin_addr.s_addr=htonl(INADDR_ANY);
	server.sin_port=htons(8001);

	if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		 perror("connect failed");
		 close(sock);
		 exit(1);
	}
	memset(buff,0,sizeof(buff));
	if((rval = recv(sock, buff, sizeof(buff), 0)) < 0) {
		perror("reading stream message error");
	}
	else if (rval==0) {
		printf("ending connection\n");
	}
	else {
		printf("Recieved MSG: %s\n",buff);
	}

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	
	while (1) {

		scanf("%s", info);
		printf("user inputed value");

		if(sendto(sock, info, sizeof(info), 0, addr_server, sizeof(server)) < 0){
			perror("send failed");
			close(sock);
			exit(1);
		}
		
		//sleep(10);
		int len = sizeof(server);
		memset(buff,0,sizeof(buff));
		if((rval = recvfrom(sock, buff, sizeof(buff), 0, addr_server, &len)) < 0) {
			perror("reading stream message error");
		}
		else if (rval==0) {
			printf("ending connection\n");
		}
		else {
			printf("The server sent this: \n%s\n",buff);
		}
		int cACK = 0;
		int cseq = 0;
		char sACK[1024];
		char sseq[1024];
		int FIN = 0;
		char header[1024];
		char sfilesize[1024];
		int filesize;
		char document[100][1111];


		if (sscanf(buff, "FILESIZE=%s", sfilesize) == 1) {
			int size = atoi(sfilesize);
			printf("we have been sent the filesize");

			FILE * fPointer;
			fPointer = fopen("downloaded.txt", "w");
			

			int sret;
			int i = 0;

			float fsize = (float) size;

			int total_blocks = ceil(fsize / 8888);
			int last_block_size = size - (total_blocks - 1) * 8888;
			int block_number = 0;
			

			while (!FIN) {
				
				sprintf(header, "ACK=%d seq=%d\n", cACK, cseq);
				printf("sending header info\n");
				if (cseq != 3 || i != 3) {
					sendto(sock, header, sizeof(header), 0, addr_server, sizeof(server));
				}
				printf("sending: %s\n", header);
				
				recvfrom(sock, buff, sizeof(buff), 0, addr_server, &len);
				
				printf("%s\n", buff);

				sprintf(sseq, "seq=%d", cACK);
				// if we received fin = 1 terminate
				if (strstr(buff, "FIN=1") != NULL){
					FIN = 1;
					printf("successfully terminated, enter next request\n");

					char *token;
					char myString[1112];
					char byteammounts[1024];
					int bytes;
					token = strtok(buff, " ");
					token = strtok(NULL, " ");
					token = strtok(NULL, " ");
					token = strtok(NULL, " ");
					token = strtok(NULL, " ");
					if (token != NULL) {
						bytes = atoi(token);
						strncpy(myString, buff, bytes);
						myString[bytes] = 0;
						fprintf(fPointer, "%s", myString);
					}
					
					
				}
				// if we have received proper information from the server, add the data to the file, and increment seq and acks
				else if (strstr(buff, sseq) != NULL) {
					char *token;
					char myString[1112];
					int bytes;
					printf("received proper info from server, incrementing\n");
					printf("\nbuff is %s\n", buff);
					char byteammounts[1024];
					printf("buf rn is %s\n", buff);
					token = strtok(buff, " ");
					token = strtok(NULL, " ");
					token = strtok(NULL, " ");
					token = strtok(NULL, " ");
					token = strtok(NULL, " ");
				
					if (token != NULL) {
						bytes = atoi(token);
						strncpy(myString, buff, bytes);
						myString[bytes] = 0;
						printf("\nthe string we are saving is %s\n and bytes is %s\n", myString, token);
						fprintf(fPointer, "%s", myString);
					}
					
					cACK++;
					cseq++;
					
				}
				i++;
			}
			fclose(fPointer);
		}
	}

	
	close(sock);

	return 0;
}

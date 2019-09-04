#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
 
#include<pthread.h> //for threading , link with lpthread
#include<sys/wait.h>
#include<math.h>
 
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    struct sockaddr *addr_server, *addr_client;
    char *message;
     
    // Creating Socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    // Setting up address information
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8001 );
    addr_server = (struct sockaddr *) &server;
     
    // Binding socket to the address
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");
     
    // Listening for incoming connections
    listen(socket_desc , 3);
     
    // Accepting incoming connections
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
	addr_client = (struct sockaddr *) &client;
        puts("Connection accepted");
         
	    // Getting the socket descriptor
	    int sock = new_socket;
	    int read_size;
	    char *message , client_message[2000];
	     
	    // Send welcome message to client
	    message = "Greetings! I am your connection handler\n";
	    write(sock , message , strlen(message));
	     
	    message = "Available commands are 'get', 'quit', and filenames \n";
	    write(sock , message , strlen(message));

	     
	    // Receive a message from client
	    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
	    {
		char out_message[1024];
		sprintf(out_message, "User sent the message: %s\n", client_message);
		puts(out_message);

		if (strstr(client_message, "get") != NULL) {
			puts("get request detected\n");
			strcpy(client_message, "type the name of the file you would like to get\n");   
			send(sock, client_message, strlen(client_message),0);  
		}
		else if (strstr(client_message, "txt") != NULL) {
			puts("txt request detected\n");

			// opening file
			FILE *fp;
			fp = fopen(client_message, "r");
			if (fp == NULL) {
				perror("Error whle opening the file.\n");
			}
		
			// get the size of the file
			fseek(fp, 0L, SEEK_END);
			int size = ftell(fp);
			rewind(fp);

			// declaring variables
			char document[100][1112];		
			int i;
			int j = 0;
			int lettercount = 0;
			int end_seq = -1;
			char end_seq_string[1024];

			float fsize = (float) size;

			// calculating segment sizes, number of blocks and more
			int total_blocks = ceil(fsize / 8888);
			printf("total blocks is %d", total_blocks);
			int last_block_size = size - (total_blocks - 1) * 8888;
			int block_number = 0;
			int segments;
			int outer_segments = last_block_size / 8;
			int inner_segments = 1111;

			// getting each segment from the file and saving it to the document array
			while (block_number < total_blocks) {
				i = 0;
				end_seq = end_seq + 8;

				// adjusting the segment size so that is corresponds to the correct block
				if (block_number < total_blocks - 1) { segments = inner_segments; }
				else { segments = outer_segments; }


				while (i < 8) {
					char string_portion[segments];
					strcpy(string_portion, "");
	
					// get segment length amount of characters from the file
					char ch;
					int count = 0;
					while (count < segments) {
						if ((ch = fgetc(fp)) == EOF){
							break;
						}
						string_portion[count] = ch;
						count++;
						lettercount++;
					}
				
					// add substring to the array
					string_portion[segments] = '\0';
					strcpy(document[j], string_portion);
					j++;
					i++;
					printf("\nlast block size is %d and letter count is %d\n", last_block_size, lettercount);

					// if we are in the last block, and the char counter is no the same as length of file, create another block
					if (i == 8 && lettercount < last_block_size && block_number == total_blocks - 1) {
						end_seq = end_seq + 8;
						while (lettercount < last_block_size) {
							char small[2];
							strcpy(document[i], "");
							strcpy(small, "");
							ch = fgetc(fp);
							small[0] = ch;
							small[1] = '\0';
							strcpy(document[j], small);
							j++;
							i++;
							lettercount++;
						}
					}			
				}
				block_number++;
			}

			// send filesize to the client
			char header[1400];
			int len = sizeof(server);
			sprintf(header, "FILESIZE=%d", size);
			sendto(sock, header, sizeof(header), 0, addr_client, sizeof(server) );

			// declare ack and seq number variables
			int sACK = 0;
			int sseq = -1;
			char cACK[1024];
			char cseq[1024];
			int FIN = 0;
	
			// respond to client requests
			if (1) {
				int i = 0;
				while (!FIN) {

					recvfrom(sock, client_message, sizeof(client_message), 0, addr_client, &len);
					printf("%s\n", client_message);
					
					sprintf(end_seq_string, "seq=%d", end_seq);
					sprintf(cseq, "seq=%d", sACK);

					// if seq number is the last sequence expected, send fin = 1 to client
					if (strstr(client_message, end_seq_string) != NULL){
						puts("terminating\n");
						FIN = 1;
					}
					// if seq number of client matches out ack we have received proper info, increment
					if (strstr(client_message, cseq) != NULL) {
						puts("incrementing acks and seq");
						printf("incrementing");
						sseq++;
						sACK++;
						
					}
					// creating the header
					printf("the size of server is: %d\n", sizeof(server));
					sprintf(header, "%s ACK=%d seq=%d FIN=%d %d ", document[sseq], sACK, sseq, FIN, strlen(document[sseq]));
	
					sendto(sock, header, sizeof(header), 0, addr_client, sizeof(server));
					printf("sending %s\n", header);	
					i++;
				}
			}
		}
		else if (strstr(client_message, "filenames") != NULL) {
			puts("filenames cmd detected\n");
			strcpy(client_message, "Available files are:\n736.txt\n739.txt\n1KB.txt\n2KB.txt\n4KB.txt\n8KB.txt\n8888.txt\n");	
			send(sock, client_message, strlen(client_message),0);    
		}
		else if (strstr(client_message, "quit") != NULL) {
			puts("quit detected\n");
			strcpy(client_message, "you want to quit\n");     
			send(sock, client_message, strlen(client_message),0);
		}
		else {
			puts("not a good cmd\n");
			strcpy(client_message, "invalid command\n");
			  send(sock, client_message, strlen(client_message),0);
		}
	
	    }
	     
	    if(read_size == 0)
	    {
		puts("Client disconnected");
		fflush(stdout);
	    }
	    else if(read_size == -1)
	    {
		perror("recv failed");
	    }
		 
	    //Free the socket pointer
	     
	    return 0;
		
    }
     
    if (new_socket<0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}


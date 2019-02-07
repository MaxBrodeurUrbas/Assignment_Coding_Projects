#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
	char *data;
	char *address;
	char *portnum;
	char *username;
	char *password;
	char *gamename;
	char *square;
	char *command;
	char output[300];

	printf("%s%c%c\n", "Content-Type:text/html;charset=iso-8859-1",13,10);
	data = getenv("QUERY_STRING");

	if(data ==NULL)
		printf("<P> Error in passing data to script");
	else{

	// printf("<p>%s\n",data);

	address = strtok(data, "&");
	portnum = strtok(NULL, "&");
	username = strtok(NULL, "&");
	password = strtok(NULL, "&");
	gamename = strtok(NULL, "&");
	square = strtok(NULL, "&");
	command = strtok(NULL,"&");

	address = strtok(address, "=");
	address = strtok(NULL, "=");

	portnum = strtok(portnum, "=");
	portnum = strtok(NULL, "=");

	username = strtok(username, "=");
	username = strtok(NULL, "=");

	password = strtok(password, "=");
	password = strtok(NULL, "=");

	gamename = strtok(gamename, "=");
	gamename = strtok(NULL, "=");

	square = strtok(square, "=");
	square = strtok(NULL, "=");

	command = strtok(command, "=");
	command = strtok(NULL, "=");

	printf("\
	<html>\
	<body>\
	<form action=\"./../cgi-bin/ttt.cgi\">\
	<b>Server Address: <input type=\"text\" name=\"address\" size=\"20\" value=\"%s\"><br />\
	<b>Server Port: <input type=\"text\" name=\"port\" size=\"20\" value=\"%s\"><br />\
	<b>Username: <input type=\"text\" name=\"username\" size=\"20\" value=\"%s\"><br />\
	<b>Password: <input type=\"text\" name=\"password\" size=\"20\" value=\"%s\"><br />\
	<b>Gamename: <input type=\"text\" name=\"gamename\" size=\"20\" value=\"%s\"><br />\
	<b>Square: <input type=\"text\" name=\"square\" size=\"20\" value=\"%s\"><br />\
	<input type=\"submit\" value=\"LOGIN\" name=\"LOGIN\">\
	<input type=\"submit\" value=\"CREATE\" name=\"CREATE\">\
	<input type=\"submit\" value=\"JOIN\" name=\"JOIN\">\
	<input type=\"submit\" value=\"MOVE\" name=\"MOVE\">\
	<input type=\"submit\" value=\"LIST\" name=\"LIST\">\
	<input type=\"submit\" value=\"SHOW\" name=\"SHOW\">\
	</form>\
	" ,address, portnum, username, password, gamename, square);
	
	sprintf(output, "%s,%s,%s",command,username,password);
	if(strcmp(command,"LOGIN")!=0 && strcmp(command,"LIST")!=0){
		sprintf(output, "%s,%s", output, gamename);
		if(strcmp(command,"CREATE")!=0 && strcmp(command,"SHOW")!=0){
			sprintf(output, "%s,%s", output,square);
		}
	}

	int sock;
	struct sockaddr_in server;
	char *message = output;
	char *addr;
	unsigned short int port = 8888;

	addr = address;
	port = (unsigned short int)atoi(portnum);
	server.sin_addr.s_addr = inet_addr( address );
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

		//Create socket
		sock = socket(AF_INET , SOCK_STREAM , 0);
		if (sock == -1){
			printf("Could not create socket");
			return -1;
		}
		//Connect to remote server
		if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
			perror("connect failed. Error");
			return 1;
		}
		//Send some data
		if( send(sock , message , 200, 0) < 0){
			puts("Send failed");
			return 1;
		}
		size_t read_size;
		int bytes_read = 0;
		char incoming_msg[2000];
		char temp[2000];
		while( bytes_read < 2000 ){
			read_size = recv(sock , temp, 2000, 0);
			if(read_size <= 0){
			    puts("Server disconnected");
			    fflush(stdout);
			    close(sock);
			    return 0;
			}
			memcpy( incoming_msg+bytes_read, temp, read_size );
			bytes_read += read_size;
		}

		char *temp =incoming_msg;
		int count =0;
		while(*temp){
			if(temp[0] == '\r' || temp[0] == '\n'){	
				count++;
			}
			temp++;
		}
		char *toOutput = (char*) malloc(sizeof(char) * (strlen(incoming_msg)*20));
		temp = incoming_msg;
		count = 0;
		while(*temp){
			if(temp[0] == '\r'){	

				toOutput[count++]= '<';
				toOutput[count++]= 'b';
				toOutput[count++]= 'r';
				toOutput[count++]= '>';
				temp+=2;

			}else if(temp[0] == ' '){

				toOutput[count++]= '&';
				toOutput[count++]='n';
				toOutput[count++]='b';
				toOutput[count++]='s';
				toOutput[count++] = 'p';
				toOutput[count++]=';';
				temp++;

			}else{
				toOutput[count++]=temp[0];
				temp++;
			}
		}
		printf( "Server response: %s\n", toOutput );
		close(sock);
		
	printf("</body>\
        </html>");
	}
	return 0;
	
}





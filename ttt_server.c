/*********************************************************
* FILE: ttt_server.c
* 
* A starter code for your answer to A4 Q1. Most of the
* networking is already done for you, and we even gave
* suggested data types (structs and linked lists of these)
* to represent users and tic-tac-toe games. You must just
* figure out how to fill in the various functions required
* to make the games happen.
*
* Good luck, and rember to ask questions quickly if you get 
* stuck. My Courses Dicussions is the first place to try, 
* then office hours.
*
* AUTHOR: YOU!
* DATE: Before Dec 3rd
***********************************************************/

#include <stdio.h>
#include <string.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <unistd.h>	
#include <stdlib.h>
#include <time.h>

struct USER{
	char username[100];
	char password[100];
	struct USER *next;
};

enum GAME_STATE{ 
	CREATOR_WON=-2,
	IN_PROGRESS_CREATOR_NEXT=-1,
	DRAW=0,
	IN_PROGRESS_CHALLENGER_NEXT=1,
	CHALLENGER_WON=2
};

struct GAME{
	char gamename[100];
	struct USER *creator;
	struct USER *challenger;
	enum GAME_STATE state;
	char ttt[3][3];
	struct GAME *next;
};

struct USER *user_list_head = NULL;
struct GAME *game_list_head = NULL;

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , read_size;
	struct sockaddr_in server , client;
	char client_message[2000];

	unsigned short int port = 8888;

	if( argc > 1 )
		port = (unsigned short int)atoi(argv[1]);
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );
	
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}

	listen(socket_desc , 3);

	printf( "Game server ready on port %d.\n", port );

	while( 1 ){
		c = sizeof(struct sockaddr_in);

		//accept connection from an incoming client
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0)
		{
			perror("accept failed");
			return 1;
		}

		char temp[200];
		memset(client_message, '\0', 200);
		int bytes_read = 0;
		while( bytes_read < 200 ){
			read_size = recv(client_sock , temp , 200, 0);
			if(read_size <= 0){
				puts("Client disconnected");
				fflush(stdout);
				close(client_sock);
				close(socket_desc);
				return 0;
			}
			memcpy( client_message+bytes_read, temp, read_size );
			bytes_read += read_size;
		}

	  	char response[2000];
	  	response[0] = '\0';
	  	char* command = strtok( client_message, "," );
	  	char *username = strtok( NULL, "," );
	  	char *password = strtok( NULL, ",");

		if( command == NULL || username == NULL || password == NULL ){
			sprintf( response, "MUST ENTER A VALID COMMAND WITH ARGUMENTS FROM THE LIST:\n" );
			sprintf( response+strlen(response), "LOGIN,USER,PASS\n" );
			sprintf( response+strlen(response), "CREATE,USER,PASS,GAMENAME\n" );
			sprintf( response+strlen(response), "JOIN,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "MOVE,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "LIST,USER,PASS\n" );
			sprintf( response+strlen(response), "SHOW,USER,PASS,GAMENAME\n" );
			write(client_sock , response , 2000);  
		  	close(client_sock);
			continue;
		}

		if( strcmp( command, "LOGIN" ) == 0 ){
			struct USER *current = user_list_head;
			int flag = 0;
			while(current != NULL){
				if(strcmp(current->username, username) == 0) {
					flag = 1;
					if(strcmp(current->password, password) == 0){
						strcpy(response, "User Logged In\n");
					}
					else
						strcpy(response, "Incorrect Password For Existing User\n");
				}
				current = current->next;
			}
			if(flag == 0){
				struct USER *newUser = (struct USER*)malloc(sizeof(struct USER));
				strcpy(newUser->username, username);
				strcpy(newUser->password, password);
				newUser->next = user_list_head;
				user_list_head = newUser;
				strcpy(response, "NEW USER CREATED OK\n");
			}
	  	}
	  	else if( strcmp( command, "CREATE" ) == 0 ){ 
	  		char *game_name = strtok( NULL, ",");
			
			if( game_name == NULL ){
				sprintf( response, "CREATE COMMAND MUST BE CALLED AS FOLLOWS:\n" );
				sprintf( response+strlen(response), "CREATE,USER,PASS,GAMENAME\n" );
				write(client_sock , response , 2000);  
		  		close(client_sock);
		  		continue;
			}

			struct GAME *game = (struct GAME*)malloc( sizeof(struct GAME) );
			strcpy(game->gamename,game_name);
			game->state = 1;
			for( int row=0; row<3; row++ ){
				for( int col=0; col<3; col++ ){
					game->ttt[row][col] = ' ';
				}
			}
			struct USER *currentUser = user_list_head;
			int flag = 0;
			while(currentUser != NULL){
				if(strcmp(currentUser->username, username) == 0) {
					if(strcmp(currentUser->password, password) == 0){
						flag = 1;
						game->creator = currentUser;
						strcpy(response, "GAME CREATED. WAITING FOR OPPONENT TO JOIN\n");
						break;
					}
				}
				currentUser = currentUser->next;
			}
			if(flag ==0){
				strcpy(response, "There was an issue creating the game, check input");
			}

			else{
				// sprintf( response, "EXAMPLE GAME CREATED. COMPLETE THIS IMPLEMENTATION.\r\n");
			sprintf( response, "%s\r\na  %c | %c | %c \r\n",response,  game->ttt[0][0],  game->ttt[0][1],  game->ttt[0][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sb  %c | %c | %c \r\n", response, game->ttt[1][0],  game->ttt[1][1],  game->ttt[1][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sc  %c | %c | %c \r\n", response, game->ttt[2][0],  game->ttt[2][1],  game->ttt[2][2]);
			sprintf( response, "%s\r\n", response );
			sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );
			}

			game->next = game_list_head;
			game_list_head = game;
	
		}
		else if( strcmp( command, "JOIN" ) == 0 ){ 
			struct USER *currentUser = user_list_head;
			struct GAME *currentGame = game_list_head;
			int flag = 0;
			int flagGame = 0;
			int x;
			int y;
			char *gameName = strtok( NULL, ",");
			char *move = strtok( NULL, ",");

			if( gameName==NULL || move==NULL ){
			sprintf( response, "MUST ENTER A VALID COMMAND WITH ARGUMENTS FROM THE LIST:\n" );
			sprintf( response+strlen(response), "LOGIN,USER,PASS\n" );
			sprintf( response+strlen(response), "CREATE,USER,PASS,GAMENAME\n" );
			sprintf( response+strlen(response), "JOIN,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "MOVE,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "LIST,USER,PASS\n" );
			sprintf( response+strlen(response), "SHOW,USER,PASS,GAMENAME\n" );
			write(client_sock , response , 2000);  
		  	close(client_sock);
			continue;
		}
			while(currentUser != NULL){
				if(strcmp(currentUser->username, username) == 0) {
					if(strcmp(currentUser->password, password) == 0){
						flag = 1;
						break;
					}
					else{
						strcpy(response, "Incorrect Password For Existing User\n");
					}
				}
				currentUser = currentUser->next;
			}

			if(flag==0)
				strcpy(response, "USER NOT FOUND\n");
			
			else{
			while(currentGame != NULL){
				if(strcmp(currentGame->gamename, gameName) == 0) {
					if(currentGame->challenger == NULL){
						currentGame->challenger = currentUser;
						currentGame->state = -1;
						strcpy(response, "GAME SUCCESFULLY JOINED\n");
						flagGame = 1;
						break;
					}
					else
						strcpy(response, "Game is full\n");
				}
				currentGame = currentGame->next;
			}
			if(move != NULL){
				if(*move == 'a')
					y = 0;
				else if(*move == 'b')
					y = 1;
				else if(*move == 'b')
					y = 2;
			}
			if(flag==1 && flagGame ==1){
				game_list_head->ttt[y][(int)(move[1]-'1')] = 'x';
			}

				// sprintf( response, "EXAMPLE GAME CREATED. COMPLETE THIS IMPLEMENTATION.\r\n");
			sprintf( response, "%s\r\na  %c | %c | %c \r\n",response,  currentGame->ttt[0][0],  currentGame->ttt[0][1],  currentGame->ttt[0][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sb  %c | %c | %c \r\n", response, currentGame->ttt[1][0],  currentGame->ttt[1][1],  currentGame->ttt[1][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sc  %c | %c | %c \r\n", response, currentGame->ttt[2][0],  currentGame->ttt[2][1],  currentGame->ttt[2][2]);
			sprintf( response, "%s\r\n", response );
			sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );
			}
		}
		else if( strcmp( command, "MOVE" ) == 0 ){ 
			
			char *gameName = strtok( NULL, ",");
			char *move = strtok( NULL, ",");

			if( gameName==NULL || move==NULL ){
			sprintf( response, "MUST ENTER A VALID COMMAND WITH ARGUMENTS FROM THE LIST:\n" );
			sprintf( response+strlen(response), "LOGIN,USER,PASS\n" );
			sprintf( response+strlen(response), "CREATE,USER,PASS,GAMENAME\n" );
			sprintf( response+strlen(response), "JOIN,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "MOVE,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "LIST,USER,PASS\n" );
			sprintf( response+strlen(response), "SHOW,USER,PASS,GAMENAME\n" );
			write(client_sock , response , 2000);  
		  	close(client_sock);
			continue;
		}
			struct USER *currentUser = user_list_head;
			int flag = 0;
			int flagGame=0;
			struct GAME *currentGame = game_list_head;
			//finding User from existing users and ensuring password is right.
			while(currentUser != NULL){
				if(strcmp(currentUser->username, username) == 0) {
					if(strcmp(currentUser->password, password) == 0){
						flag = 1;
						break;
					}
					else{
						strcpy(response, "Incorrect Password For Existing User\n");
						break;
					}
				}
				currentUser = currentUser->next;
			}
			
			//finding exisiting game and making sure USER is a player in it
			while(currentGame != NULL){
				if(strcmp(currentGame->gamename, gameName) == 0) {
					if(strcmp(currentGame->creator->username, username) == 0 || strcmp(currentGame->challenger->username, username) == 0){
						flagGame = 1;
						break;
					}
					else{
						strcpy(response, "User is not playing in selected game\n");
					}
				}
				currentGame = currentGame->next;
			}
			
			
			//if the user is in the game, their move selction is valid, place it and change state
			if(flag ==1 && flagGame ==1){
				if(currentGame->challenger == NULL){
				strcpy(response, "GAME DOES NOT HAVE ENOUGH PLAYERS\n");
				}
				else if(currentGame->ttt[*move -'a'][(int)(move[1]-'1')] != 'o' && currentGame->ttt[*move -'a'][(int)(move[1]-'1')] != 'x'){
					if(currentGame->state==-1){
					currentGame->ttt[*move -'a'][(int)(move[1]-'1')] = 'o';
					currentGame->state=1;
					strcpy( response, "PLAYER ");
					strcat( response, currentGame->creator->username);
					strcat(response, " HAS MOVED.\n");
					}
					else if(currentGame->state==1){
						currentGame->ttt[*move -'a'][(int)(move[1]-'1')] = 'x';
						currentGame->state=-1;
						strcpy( response, "PLAYER ");
						strcat( response, currentGame->challenger->username);
						strcat(response, " HAS MOVED.\n");
					}
				}
				else{
					strcpy(response, "CELL ALREADY TAKEN, TRY A DIFFERENT MOVE.");
				}
			//checking for winning state by row, column and diagonal
			for(int x=0; x<3; x++){
				int xsum=0;
				int osum=0;
				for(int y=0; y<3; y++){
					if(currentGame->ttt[y][x] == 'x')
						xsum++;
					if(currentGame->ttt[y][x] == 'o')
						osum++;
					if(xsum==3){
						strcpy( response, "PLAYER " );
						strcat( response, currentGame->challenger->username);
						strcat(response, " WON.\n");
						currentGame->state=2;
					}
					if(osum==3){
						strcpy( response, "PLAYER " );
						strcat( response, currentGame->creator->username);
						strcat( response, " WON.\n");
						currentGame->state=-2;
					}
				}
			}
			for(int y=0; y<3; y++){
				int xsum=0;
				int osum=0;
				for(int x=0; x<3; x++){
					if(currentGame->ttt[y][x] == 'x')
						xsum++;
					if(currentGame->ttt[y][x] == 'o')
						osum++;
					if(xsum==3){
						strcpy( response, "PLAYER " );
						strcat( response, currentGame->challenger->username);
						strcat(response, " WON.\n");
						currentGame->state=2;
						}
					if(osum==3){
						strcpy( response, "PLAYER " );
						strcat( response, currentGame->creator->username);
						strcat( response, " WON.\n");
						currentGame->state=-2;
						}
				}
			}
			int xsumL=0;
			int osumL=0;
			int xsumR=0;
			int osumR=0;

			for(int i=0; i<3; i++){
				if(currentGame->ttt[i][i] == 'x')
					xsumL++;
				if(currentGame->ttt[i][i] == 'o')
					osumL++;
				if(currentGame->ttt[2-i][2-i] == 'x')
					xsumR++;
				if(currentGame->ttt[2-i][2-i] == 'o')
					osumR++;

				if(xsumL ==3 || xsumR ==3){
					strcpy( response, "PLAYER " );
					strcat( response, currentGame->challenger->username);
					strcat(response, " WON.\n");
					currentGame->state=2;
				}
					
				if(osumL ==3 || osumR ==3){
					strcpy( response, "PLAYER " );
					strcat( response, currentGame->creator->username);
					strcat( response, " WON.\n");
					currentGame->state=-2;
				}
			}
			//checking for possible draw by seeing if board is full
			int drawCounter=0;
			if(currentGame->state !=-2 && currentGame->state !=2){
				for(int y=0; y<3; y++){
					for(int x=0; x<3; x++){
						if(currentGame->ttt[y][x] == 'x' || currentGame->ttt[y][x] =='o')
							drawCounter++;
					}
				}
				if(drawCounter == 9){
					currentGame->state =0;
					strcpy( response, "GAME IS A DRAW\n" );
					}
				}
			
			sprintf( response, "%s\r\na  %c | %c | %c \r\n",response,  currentGame->ttt[0][0],  currentGame->ttt[0][1],  currentGame->ttt[0][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sb  %c | %c | %c \r\n", response, currentGame->ttt[1][0],  currentGame->ttt[1][1],  currentGame->ttt[1][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sc  %c | %c | %c \r\n", response, currentGame->ttt[2][0],  currentGame->ttt[2][1],  currentGame->ttt[2][2]);
			sprintf( response, "%s\r\n", response );
			sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );
			}
		}
		else if( strcmp( command, "LIST" ) == 0 ){ 

			struct USER* current = user_list_head;
			int flag=0;
			struct USER*  challenger = NULL;
			while(current != NULL){
				if(strcmp(current->username,username) == 0){
					if(strcmp(current->password,password) == 0){
						challenger=current;
						flag =1;
					}else{
						sprintf(response, "WRONG PASSWORD FOR USER %s \r\n",username);
						write(client_sock , response , 2000);
						close(client_sock);
						continue;
					}
					break;
				}
				current = current->next;
			}
			if(flag==0){
				sprintf(response, "USERNAME %s NOT FOUND \r\n", username);
				write(client_sock , response , 2000);  
				close(client_sock);
				continue;
			}


			struct GAME* currentGame = currentGame_list_head;
			sprintf(response,"%s LIST OF GAMES: \r\n",response);

			while(currentGame != NULL){
				char* state[] = {"CREATOR WON","IN_PROGRESS:","DRAW","IN_PROGRESS:","CHALLENGER WON"};
				sprintf(response, "%s GAME %s: CREATED BY %s ",response,currentGame->currentGamename, currentGame->creator->username);

				if(currentGame->challenger == NULL)
					sprintf(response,"%s CHALLENGED BY: (null). IN PROGRESS: (null) TO MOVE NEXT AS x\r\n",response);

				else{
					sprintf(response,"%s CHALLENGED BY: %s. %s", response,currentGame->challenger->username,state[currentGame->state+2]);
					if(currentGame->state == 1 || currentGame->state == -1){
						char toPlay = 'x';
						char *nextToPlay[] = {currentGame->creator->username,currentGame->challenger->username};
						int slot;
						if(currentGame->state==1)
							slot=1;

						else
							slot=0;

						if(currentGame->state ==-1)
							toPlay = 'o';
						
						sprintf(response,"%s %s TO MOVE NEXT AS %c \r\n", response,nextToPlay[slot], toPlay);
						
					}else{

						sprintf(response,"%s \r\n",response);
					}
				}
				currentGame=currentGame->next;
			}
			
		}

		else if( strcmp( command, "SHOW" ) == 0 ){
			char *gameName = strtok(NULL,",");

			if(gameName ==NULL){
				sprintf( response, "MUST ENTER A VALID COMMAND WITH ARGUMENTS FROM THE LIST:\n");
				sprintf( response+strlen(response), "LOGIN,USER,PASS\n");
				sprintf( response+strlen(response), "CREATE,USER,PASS,GAMENAME\n");
				sprintf( response+strlen(response), "JOIN,USER,PASS,GAMENAME,SQUARE\n");
				sprintf( response+strlen(response), "MOVE,USER,PASS,GAMENAME,SQUARE\n");
				sprintf( response+strlen(response), "LIST,USER,PASS\n");
				sprintf( response+strlen(response), "SHOW,USER,PASS,GAMENAME\n");
				write(client_sock , response , 2000);  
				close(client_sock);
				continue;
			}

			struct GAME *currentGame = game_list_head;
			struct USER *currentUser =user_list_head;
			int flag=0;
			while(currentUser != NULL){
				if(strcmp(currentUser->username, username) == 0) {
					if(strcmp(currentUser->password, password) == 0){
						flag = 1;
						break;
					}
					else{
						strcpy(response, "Incorrect Password For Existing User\n");
					}
				}
				currentUser = currentUser->next;
			}

			if(flag==0)
				strcpy(response, "USER NOT FOUND\n");

			while(currentGame != NULL){
				if(strcmp(currentGame->gamename, gameName)==0){
					break;
				}
				currentGame = currentGame->next;
			}

			if(currentGame->state == -2)
				sprintf(response, "%s GAME %s BETWEEN %s AND %s. GAME ENDED: %s WON\n",response, currentGame->gamename, currentGame->creator->username, currentGame->challenger->username, currentGame->creator->username);

			else if(currentGame->state == -1)
				sprintf(response, "%s GAME %s BETWEEN %s AND %s. IN PROGRESS: %s TO MOVE NEXT AS x\n ",response, currentGame->gamename, currentGame->creator->username, currentGame->challenger->username, currentGame->creator->username);

			else if(currentGame->state == 0)
				sprintf(response, "%s GAME %s BETWEEN %s AND %s. GAME ENDED: DRAW\n ",response, currentGame->gamename, currentGame->creator->username, currentGame->challenger->username);

			else if(currentGame->state == 1)
				sprintf(response, "%s GAME %s BETWEEN %s AND %s. IN PROGRESS: %s TO MOVE NEXT AS o\n ",response, currentGame->gamename, currentGame->creator->username, currentGame->challenger->username, currentGame->challenger->username);

			else if(currentGame->state == 2)
				sprintf(response, "%s GAME %s BETWEEN %s AND %s. GAME ENDED: %s WON\n",response, currentGame->gamename, currentGame->creator->username, currentGame->challenger->username, currentGame->challenger->username);



			sprintf( response, "%s\r\na  %c | %c | %c \r\n",response,  currentGame->ttt[0][0],  currentGame->ttt[0][1],  currentGame->ttt[0][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sb  %c | %c | %c \r\n", response, currentGame->ttt[1][0],  currentGame->ttt[1][1],  currentGame->ttt[1][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sc  %c | %c | %c \r\n", response, currentGame->ttt[2][0],  currentGame->ttt[2][1],  currentGame->ttt[2][2]);
			sprintf( response, "%s\r\n", response );
			sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );


		}
		else{
	  		sprintf( response, "COMMAND %s NOT IMPLEMENTED", command );
		}

		write(client_sock , response , 2000);  
		close(client_sock);
	}

	close(socket_desc);	
	
	return 0;
}


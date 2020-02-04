#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "../cJSON.h"
#include <string.h>
#define MAX 80
#define PORT 65467
#define SA struct sockaddr

struct sockaddr_in server;
const char address[] = "127.0.0.1";
char token[100];


int init(){
	//Initializing Winsock library
	WORD wVersionRequested;
	WSADATA wsa;
	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsa) != 0) {
		// Tell the user that we could not find a usable Winsock DLL.
        return WSAGetLastError();
	}

	// Assign IP and port
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(address);
	server.sin_port = htons(PORT);

	return 0;
}

int client(){
	// Create and verify socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		printf("could not connect\n");
	}
	//Connecting to server
	if (connect(client_socket, (SA*)&server, sizeof(server)) != 0)
	{
		printf("Connection error\n");
	}
	return client_socket;
}

void firstpart(struct sockaddr_in servaddr,char *token){
   //for server management
    cJSON *serverans;
    int client_socket;
    printf("Account Menu:\n1: Register\n2: Login\n");
    int n1;
    scanf("%d",&n1);
    if(n1==1){
        char username[50];
        printf("please enter username:");
        char junk[20];
        gets(junk);
        gets(username);
        char password[50];
        printf("please enter password:");
        gets(password);
        //final string
        char buffer1[1000];
        sprintf(buffer1, "register %s, %s\n", username, password);
        //connect to socket
        client_socket=client();
       // printf("socket %d\n",client_socket);
        send(client_socket, buffer1, strlen(buffer1), 0);
        //printf("%s\n", buffer1);
        memset(buffer1, 0, sizeof(buffer1));
        recv(client_socket, buffer1, sizeof(buffer1), 0);

        //recv the ans of server
        serverans=cJSON_Parse(buffer1);
        if(strcmp(cJSON_GetObjectItem(serverans,"type")->valuestring, "Error") == 0){
          printf("Error!");
          return firstpart(server,token);
        }
        else{
            //final string
            sprintf(buffer1, "login %s, %s\n", username, password);
            client_socket=client();
            //connect to socket
            send(client_socket, buffer1, sizeof(buffer1), 0);
            memset(buffer1, 0, sizeof(buffer1));
            recv(client_socket, buffer1, sizeof(buffer1), 0);
            //ans from server
            serverans=cJSON_Parse(buffer1);
            strcpy(token , cJSON_GetObjectItem(serverans,"content")->valuestring);
            if(strcmp(cJSON_GetObjectItem(serverans,"type")->valuestring, "Error") == 0){
              closesocket(client_socket);
              printf("Error!");
              return firstpart(server,token);
            }
            else{
                closesocket(client_socket);
                return secondpart(server,token);
            }
        }
    }
    if(n1==2){
        printf("please enter username:");
        char junk1[20];
        gets(junk1);
        char username[50];
        gets (username);
        printf("please enter password:");
        char password[50];
        gets (password);
        char buffer1[1000];
        sprintf(buffer1, "login %s, %s\n", username, password);
        //connect to socket
        client_socket=client();
        send(client_socket, buffer1, sizeof(buffer1), 0);
        memset(buffer1, 0, sizeof(buffer1));
        recv(client_socket, buffer1, sizeof(buffer1), 0);
        //ans from server
        serverans=cJSON_Parse(buffer1);
        strcpy(token, cJSON_GetObjectItem(serverans,"content")->valuestring);

        if(strcmp(cJSON_GetObjectItem(serverans,"type")->valuestring, "Error") == 0){
          closesocket(client_socket);
          printf("Error!");
          return firstpart(server,token);
        }
        else{
            closesocket(client_socket);
            return secondpart(server,token);
        }
    }


}


void secondpart(struct sockaddr_in servaddr,char token[]){
  //for server management
   cJSON *serverans2=(cJSON *)malloc(1000*sizeof(char));
   char *correctness2=(char *)malloc(1000*sizeof(char));
   int client_socket;

printf("1: Create Channel\n2: Join Channel\n3: Logout\n");
int n2;
scanf("%d",&n2);
if(n2==1){
    printf("please write name of the channel:\n");
    char channelname[1000];
    char junk6[20];
    gets(junk6);
    scanf("%[^\n]",channelname);
   /* char junk6[20];
    gets(junk6);
    gets (channelname2);*/
    //final string
    char buffer2[1000];
    sprintf(buffer2, "create channel %s, %s\n", channelname, token);
    //connect to socket
    client_socket=client();
    send(client_socket, buffer2, sizeof(buffer2), 0);
    memset(buffer2, 0, sizeof(buffer2));
    recv(client_socket, buffer2, sizeof(buffer2), 0);

     serverans2=cJSON_Parse(buffer2);
     if(strcmp(cJSON_GetObjectItem(serverans2,"type")->valuestring, "Error") == 0){
            closesocket(client_socket);
            return secondpart(server,token);
     }
     else{
            closesocket(client_socket);
            return thirdpart(server,token);
     }
}
if(n2==2){
    printf("please write name of the channel:\n");
    char *channelname1;
    char junk6[20];
    gets(junk6);
    gets (channelname1);
    //final string
    char buffer2[1000];
    sprintf(buffer2, "join channel %s, %s\n", channelname1, token);
    //connect to socket
    client_socket=client();
    send(client_socket, buffer2, sizeof(buffer2), 0);
    memset(buffer2, 0, sizeof(buffer2));
    recv(client_socket, buffer2, sizeof(buffer2), 0);

    serverans2=cJSON_Parse(buffer2);
     if(strcmp(cJSON_GetObjectItem(serverans2,"type")->valuestring, "Error") == 0){
            closesocket(client_socket);
            return secondpart(server,token);
     }
     else{
        closesocket(client_socket);
        return thirdpart(server,token);
     }
}
if(n2==3){
    //final string
    char buffer2[1000];
    sprintf(buffer2, "logout %s\n", token);
    //connect to socket
    client_socket=client();
    send(client_socket, buffer2, sizeof(buffer2), 0);
    closesocket(client_socket);
    return firstpart(server,token);
}

}



void thirdpart(struct sockaddr_in servaddr,const char token[]){
//for server management
cJSON *serverans3=(cJSON *)malloc(1000*sizeof(char));
char contentS[10000];
char contentC[10000];
int client_socket;


printf("1: Send Message\n2: Refresh\n3: Channel Members\n4: Leave Channel\n");
int n3;
scanf("%d",&n3);
if(n3==1){
   printf("please write your message:");
   char *message;
   char junk7[20];
    gets(junk7);
   scanf("%[^\n]",message);
   //final string
   char buffer3[1000];
   sprintf(buffer3, "send %s, %s\n", message, token);
   //connect to socket
   client_socket=client();
   send(client_socket, buffer3, sizeof(buffer3), 0);
   closesocket(client_socket);
   return thirdpart(server,token);
}
if(n3==2){
    //final string
    char buffer3[1000];
    sprintf(buffer3, "refresh %s\n",token);
    //connect to server
    client_socket=client();
    send(client_socket, buffer3, sizeof(buffer3), 0);
    memset(buffer3, 0, sizeof(buffer3));
    recv(client_socket, buffer3, sizeof(buffer3), 0);

    serverans3=cJSON_Parse(buffer3);
         //if(!strcmp(cJSON_GetObjectItemCaseSensitive(serverans3, "type"), "List"))
    //{
        cJSON* ms = cJSON_GetObjectItemCaseSensitive(serverans3, "content");
        if(cJSON_IsArray(ms))
        {
            cJSON *m = NULL;
            cJSON_ArrayForEach(m, ms)
            {
              strcpy(contentS, cJSON_GetObjectItem(m,"sender")->valuestring);
              strcpy(contentC, cJSON_GetObjectItem(m,"content")->valuestring);
                  printf("%s:",contentS);
                  printf("%s\n",contentC);
            }
        }
    //}
    closesocket(client_socket);
    return thirdpart(server,token);
}
if(n3==3){
    //final string
    char buffer3[1000];
    sprintf(buffer3, "channel members %s\n",token);
    //connect to socket
    client_socket=client();
    send(client_socket, buffer3, sizeof(buffer3), 0);
    memset(buffer3, 0, sizeof(buffer3));
    recv(client_socket, buffer3, sizeof(buffer3), 0);

    serverans3=cJSON_Parse(buffer3);
  //  if(!strcmp(cJSON_GetObjectItemCaseSensitive(serverans3, "type"), "List"))
   // {
        cJSON* ms = cJSON_GetObjectItemCaseSensitive(serverans3, "content");
        if(cJSON_IsArray(ms))
        {
            cJSON *m = NULL;
            cJSON_ArrayForEach(m, ms)
            {
                  strcpy(contentC, cJSON_GetStringValue(m));
                  printf("%s\n",contentC);
            }
        }

  //  }
    closesocket(client_socket);
    return thirdpart(server,token);

}
if(n3==4){
    //final string
    char buffer3[1000];
    sprintf(buffer3, "leave %s\n",token);
    //connect to socket
    client_socket=client();
    send(client_socket, buffer3, sizeof(buffer3), 0);
    closesocket(client_socket);
    return secondpart(server,token);
}

}


int main(){
    //socket
	init();
      firstpart(server ,token);
    return 0;
}

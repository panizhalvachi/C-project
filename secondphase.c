#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#define MAX 80
#define PORT 65467
#define SA struct sockaddr
#include <stdio.h>
#include <stdlib.h>
#include "../cJSON.h"

   struct sockaddr_in server, client;
   cJSON *ClientRequest;
   int server_socket;
   int client_socket;



struct information{char token[25];
               char  username[50];
               char   channelname[50];
               };
struct information clients[30];
int count=-1;
char token[25]={};
char username[50];

int init(){
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData)!= 0) {
         return WSAGetLastError();
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    return 0;
}
void serverSetting(){
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    else
        printf("Socket successfully created..\n");

/*	if (connect(client_socket, (SA*)&server, sizeof(server)) != 0)
	{
		printf("Connection error\n");
	}*/
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully bound..\n");

    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
}
int clientSetting(){
    int len = sizeof(client);
    int client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server accceptance failed...\n");
        exit(0);
    }
    else
        printf("Server acccepted the client..\n");
    return client_socket;
}




int CompareChannelNames(const char channelname[],FILE *Ptr){
    char exchannelname[10000];
     while (!feof(Ptr)) {
          fgets(exchannelname,10000,Ptr);
          sscanf(exchannelname,"%s",exchannelname);
          if (strcmp(exchannelname , channelname )== 0)
             {
                return 0;
             }
        }
    return 1;
}
void sendmessage(char a[],char b[]){
    char *out;
    cJSON *root;
    int client_socket;
    root  = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "type", cJSON_CreateString(a));
    cJSON_AddItemToObject(root, "content", cJSON_CreateString(b));
    out = cJSON_Print(root);
    client_socket=clientSetting();
    send(client_socket, out, sizeof(out), 0);
    free(out);
    cJSON_Delete(root);
}
int CompareTokens(FILE *Ptr,const char token[]){
    char extoken[24];
     while (!feof(Ptr)) {
           fscanf(Ptr, "%s",extoken);
             if (strcmp(extoken , token )== 0)
             {
                return 0;
             }
        }
    return 1;
}
int CompareUserNames(const char username[],const char password[],FILE *Ptr){
 char name[50];
 char pass[50];
        while (!feof(Ptr)) {
           fscanf(Ptr, "%s : %s",name, pass);
             if (strcmp(name , username )== 0)
             {
                 if (strcmp(pass , password )== 0){
                    return 1;//this userName exist and password is true
                 }
                return 0;//this userName exist and password is not correct
             }
        }
 return -1;//this userName  doesn't exist
}
void register_user(char buffer[]){
    char username[50];
    char password[50];
    char junk[10];
    int x;
    FILE *Ptr;

    sscanf(buffer,"%s %s %s",junk,username,password);

    if ((Ptr=fopen("UserList.txt","a+")) == NULL) {
        puts("File could not be opened");
        return;
    }
    else {
    x=CompareUserNames(username,password,Ptr);
         }
    if(x==0||x==1){
    sendmessage("Error","This UserName already exists!");
    }
    else{
    fprintf(Ptr,"\n%s : %s",username,password);
    sendmessage("successful","");
    }
    fclose(Ptr);
}
void login(char buffer[]){
    FILE *Ptr;
    char password[50];
    char junk[10];
    int x;
    int y;
    sscanf(buffer,"%s %s %s",junk,username,password);
    if ((Ptr=fopen("UserList.txt","a+")) == NULL) {
        puts("File could not be opened");
        return;
    }
    else {
    x=CompareUserNames(username,password,Ptr);
         }
    fclose(Ptr);
    if(x==-1){
    sendmessage("Error","No such UserName exists");
    }
    else if(x==0){
      sendmessage("Error","your password is not correct");
    }
    else{
        FILE *Ptr1;
       if ((Ptr1=fopen("Tokens.txt","a+")) == NULL) {
        puts("File could not be opened");
        return;
       }
      else{
      SetToken();
      y=CompareTokens(Ptr1,token);
      while(y==0){
      SetToken();
      }
      count++;
      fprintf(Ptr1,"\n%s",token);
      sendmessage("AuthToken",token);
      strcpy(clients[count].token,token);
      strcpy(clients[count].username,username);
      }
      fclose(Ptr1);
    }
}
void SetToken(){
       srand(time(NULL));
       for(int i=0;i<12;i++){
        token[i]=48+rand()%10;
       }
       for(int j=12;j<24;j++){
        token[j]=97+rand()%26;
       }
}
void channel_members(char buffer[]){
char tohi[15];
char Ctoken[24];
FILE *Ptr;
sscanf(buffer,"%s %s %s",tohi,tohi,Ctoken);
int z;
    if ((Ptr=fopen("Tokens.txt","r+")) == NULL) {
        puts("File could not be opened");
        return;
    }
    else {
    z=CompareTokens(Ptr,token);
         }
    fclose(Ptr);
    if(z==1){
     sendmessage("Error","your token doesn't found");
    }
    if(z==0){
    int j=0;
    for(int i=0;i<=count;i++){
        if(strcmp(clients[i].token,token)==0){
            j=i;
            break;
        }
    }
    if(strcmp(clients[j].channelname,"")==0){
        sendmessage("Error","you are not in channel!");
    }
    else{
       cJSON *root,*members;
       root = cJSON_CreateObject();
      members = cJSON_CreateArray();
      cJSON_AddItemToObject(root,"type",cJSON_CreateString("List"));
      cJSON_AddItemToObject(root,"content",members);
    for(int i=0;i<=count;i++){
      //  printf("%s\n",clients[i].channelname);
        if(strcmp(clients[i].channelname,clients[j].channelname)==0){
            cJSON_AddItemToArray(members,cJSON_CreateString(clients[i].username));
        }
    }
    char *out=cJSON_Print(root);
    int client_socket;
    client_socket=clientSetting();
    send(client_socket, out, sizeof(out), 0);
        free(out);
   cJSON_Delete(root);
}
}
}
void refresh(char buffer[]){
    FILE *Ptr;
    char junk[20];
    int z=0;
    sscanf(buffer,"%s %s",junk,token);
    if ((Ptr=fopen("Tokens.txt","r+")) == NULL) {
        puts("File could not be opened");
        return;
    }
    else {
    z=CompareTokens(Ptr,token);
         }
    fclose(Ptr);
    if(z==1){
     sendmessage("Error","your token doesn't found");
    }
    if(z==0){
        int j=0;
    for(int i=0;i<=count;i++){
        if(strcmp(clients[i].token,token)==0){
            j=i;
            break;
        }
    }
    if(strcmp(clients[j].channelname,"")==0){
        sendmessage("Error","you are not in channel!");
        return;
    }
    FILE *Ptr1;
    if((Ptr1=fopen("Messages.txt","a+"))==NULL){
        puts("File could not be opened");
        return;
    }
    else{
    cJSON *root,*content,*inf;
    root = cJSON_CreateObject();
    content = cJSON_CreateArray();
    cJSON_AddItemToObject(root,"type",cJSON_CreateString("list"));
    cJSON_AddItemToObject(root, "content",content);

    char username[50];
    char channelname[50];
    char message[1000];

    while(!feof(Ptr1)){
        fscanf(Ptr1,"%s : %s : ",channelname,username);
       // printf("%s\n",channelname);
        fgets(message,1000,Ptr1);
        if(strcmp(channelname,clients[j].channelname)==0){
        cJSON_AddItemToArray(content, inf = cJSON_CreateObject());
        cJSON_AddItemToObject(inf, "sender", cJSON_CreateString(username));
        cJSON_AddItemToObject(inf, "content", cJSON_CreateString(message));
        }
    }
      char *out = cJSON_Print(root);
      int client_socket;
      client_socket=clientSetting();
      send(client_socket, out, sizeof(out), 0);
    free(out);
   cJSON_Delete(root);
    }
    }


 /*  root = cJSON_CreateObject();
   PMs = cJSON_CreateArray();

    cJSON_AddItemToObject(root, "type", cJSON_CreateString("List"));
   cJSON_AddItemToObject(root, "content", PMs);*/

}
void send_from_user(char buffer[]){
//remove_first_part_of_string(buffer,sizeof(buffer));

FILE *Ptr;
int q=0;
if((Ptr=fopen("Tokens.txt","a+"))==NULL){
        puts("File could not be opened");
        return;
}
else{
   q=CompareTokens(Ptr,token);
}
fclose(Ptr);
if(q==1){sendmessage("Error","your token doesn't found");}
else{
    int j=0;
    for(int i=0;i<=count;i++){
        if(strcmp(clients[i].token,token)==0){
            j=i;
            break;
        }
    }
    if(strcmp(clients[j].channelname,"")==0){sendmessage("Error","you are not in a channel");}
    else{
    FILE *Ptr8;
    if((Ptr8=fopen("Messages.txt","a+"))==NULL){
        puts("File could not be opened");
        return;
      }
   else{
    fprintf(Ptr8,"\n%s : %s : %s",clients[j].channelname,clients[j].username,buffer);
    }
    fclose(Ptr8);
    }
}
}
void remove_first_part_of_string(char junk[],int size){
for(int i=0;i<size-5;i++){
    junk[i]=junk[5+i];}
for(int i=size-5;i<size;i++){
    junk[i]='\0';
}
int x=0;
for(int j=0;j<size;j++){
    if(junk[j]==','){x=j+1; break;}
}

for(int i=0;i<24;i++){
     token[i]=junk[x+1+i];
     junk[x-1+i]='\0';
}
}
void join_channel(char buffer[]){
    FILE *Ptr;
    char junk[50];
    char channelname[50];
    sscanf(buffer,"%s %s %s %s",junk,junk,channelname,token);
    int w;
    if ((Ptr=fopen("Tokens.txt","r+")) == NULL) {
        puts("File could not be opened");
    }
    else {
    w=CompareTokens(Ptr,token);
         }
    fclose(Ptr);
    if(w==1){
     sendmessage("Error","your token doesn't found");
    }
    if(w==0){
     FILE *Ptr1;
     int e;
     if ((Ptr1=fopen("ChannelNames.txt","r+")) == NULL) {
        puts("File could not be opened");
    }
    else {
    e=CompareChannelNames(channelname,Ptr1);
         }
     fclose(Ptr1);
    if(e==1){
        sendmessage("Error","A channel with this name doesn't exist");
    }
    else{
    int j=0;
    for(int i=0;i<=count;i++){
        if(strcmp(clients[i].token,token)==0){
            j=i;
            break;
        }
    }
    strcpy(clients[j].channelname,channelname);
    sendmessage("successful","");
    }
}
}
void create_channel(char buffer[]){
    FILE *Ptr;
    int r;
    char junk[50];
    char channelname[50];
    sscanf(buffer,"%s %s %s %s",junk,junk,channelname,token);
 //   printf("%s\n",channelname);
    if ((Ptr=fopen("Tokens.txt","r+")) == NULL) {
        puts("File could not be opened");
    }
    else {
    r=CompareTokens(Ptr,token);
         }
    fclose(Ptr);
    if(r==1){
     sendmessage("Error","your token doesn't found");
    }
    if(r==0){
     FILE *Ptr1;
     int t;
     if ((Ptr1=fopen("ChannelNames.txt","a+")) == NULL) {
        puts("File could not be opened");
    }
    else {
    t=CompareChannelNames(channelname,Ptr1);
         }
    if(t==0){
        sendmessage("Error","A channel with this name already exists");
    }
    else{
       fprintf(Ptr1,"\n%s",channelname);
       sendmessage("successful","");
       fclose(Ptr1);
    }
    }
}
void leave_channel(char buffer[]){
FILE *Ptr;
char junk[30];
sscanf(buffer,"%s %s",junk,token);
int u;
    if ((Ptr=fopen("Tokens.txt","r+")) == NULL) {
        puts("File could not be opened");
    }
    else {
    u=CompareTokens(Ptr,token);
         }
    fclose(Ptr);
    if(u==1){
     sendmessage("Error","your token doesn't found");
    }
    if(u==0){
    int j=0;
    for(int i=0;i<=count;i++){
        if(strcmp(clients[i].token,token)==0){
            j=i;
            break;
        }
    }
     if(strcmp(clients[j].channelname,"")==0){
        sendmessage("Error","you are not in a channel");
     }
     else{
    strcpy(clients[j].channelname,"");
     sendmessage("successful","");
     }
}
}
void logout(char buffer[]){
      char junk[30];
      sscanf(buffer,"%s %s",junk,token);
       int ctr = 0;
        char ch;
        int x=0;
        FILE *fptr1, *fptr2;
        char str[300], temp[] = "temp.txt";
        fptr1 = fopen("Tokens.txt", "r");
        if (!fptr1)
		{
                printf(" File can't be opened");
        }
        else{
        x=CompareTokens(fptr1,token);
        }
        if(x==1){sendmessage("Error","your token doesn't found");}
        else{
        int i=0;
        int j=0;
        char extoken[24];
        while(!feof(fptr1)){
            fscanf(fptr1,"%s",extoken);
            i++;
            if(strcmp(extoken,token)==0){
                j=i;
            }
        }
        rewind(fptr1);
        fptr2 = fopen(temp, "w");
        if (!fptr2)
		{
                printf("file can't be opened");
                fclose(fptr1);
        }
        while (!feof(fptr1))
        {
            strcpy(str, "\0");
            fgets(str, 300, fptr1);
            if (!feof(fptr1))
            {
                ctr++;
                if (ctr != j)
                {
                    fprintf(fptr2, "%s", str);
                }
            }
        }
        fclose(fptr1);
        fclose(fptr2);
        remove("tokens.txt");
        rename(temp, "Tokens.txt");
        sendmessage("successful","");
        }
}


int main(){
     char buffer[50];
     char kind[20];
     init();


   while(1){
    serverSetting();
    client_socket=clientSetting();
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    sscanf(buffer,"%s",kind);
    if(strcmp(kind,"register")==0){register_user(buffer);}
    else if(strcmp(kind,"login")==0){login(buffer);}
    else if(strcmp(kind,"create")==0){create_channel(buffer);}
    else if(strcmp(kind,"join")==0){join_channel(buffer);}
    else if(strcmp(kind,"send")==0){
            remove_first_part_of_string(buffer,sizeof(buffer));
            send_from_user(buffer);
    }
    else if(strcmp(kind,"refresh")==0){refresh(buffer);}
    else if(strcmp(kind,"channel")==0){channel_members(buffer);}
    else if(strcmp(kind,"leave")==0){leave_channel(buffer);}
    else{
            logout(buffer);
    }
   }
   return 0;
}

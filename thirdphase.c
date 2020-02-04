#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define arraytype 0
#define objecttype 1
#define stringtype 2


typedef struct CJSON
{
    struct CJSON *next;
    struct CJSON *prev;
    struct CJSON *child;
    int type;
   // char *valuestring;
    //char *string;
    char valuestring[50];
    char string[50];

} CJSON;




////add sth to sth
void CJSON_AddItemToObject(CJSON *object,const char *string,CJSON *item)
{
 //item->string=malloc(1000*sizeof(char));
 CJSON *child=malloc(sizeof(CJSON));
 strcpy(item->string,string);
 if(object->child==NULL){
        object->child=item;
        }
 else{
 child=object->child;
 while (child->next!=NULL)
        {
            child = child->next;
        }
            child->next = item;
}
}

int CJSON_AddItemToArray(CJSON *array, CJSON *item)
{
    CJSON *child = NULL;

    if ((item == NULL) || (array == NULL))
    {
        return 0;
    }
    child = array->child;
    if (child == NULL)
    {
        array->child = item;
    }
    else
    {
        while (child->next!=NULL)
        {
            child = child->next;
        }
            child->next = item;
            item->prev = child;
    }
}

//delete
void CJSON_Delete(CJSON* item)
{
  CJSON *next = NULL;
    while (item != NULL)
    {
        next = item->next;
        if (item->child != NULL && item->child==arraytype)
        {
            CJSON_Delete(item->child);
        }
        if (item->valuestring != NULL && item->child!=arraytype )
        {
            free(item->valuestring);
        }
        if (item->string != NULL && item->child!=arraytype )
        {
            free(item->string);
        }
        item = next;
    }
}

//create something
CJSON * CJSON_CreateArray()
{
    CJSON *root=malloc(sizeof(CJSON));
    root->child=NULL;
    root->prev=NULL;
    root->next=NULL;
    strcpy(root->string,"");
    strcpy(root->valuestring,"");
    root->type=arraytype;
    return root;
}

CJSON * CJSON_CreateObject()
{
    CJSON *root=malloc(sizeof(CJSON));
    root->child=NULL;
    root->prev=NULL;
    root->next=NULL;
    strcpy(root->string,"");
    strcpy(root->valuestring,"");
    root->type=objecttype;
    return root;
}


CJSON * CJSON_CreateString(const char *mystring)
{
    CJSON *root=malloc(sizeof(CJSON));
    root->child=NULL;
    root->prev=NULL;
    root->next=NULL;
    strcpy(root->string,"");
    strcpy(root->valuestring,mystring);
    root->type=stringtype;
   return root;
}

//get sth

CJSON * CJSON_GetArrayItem(const CJSON *Array,int index)
{
int i=0;
CJSON *child=Array->child;
while (child->next)
{
 child = child->next;
 if(i==index){
    return child;
 }
 i++;
}
}

CJSON *CJSON_GetObjectItem(const CJSON * const object, const char * const string)
{
 CJSON *current_element = NULL;

    if ((object == NULL) || (string == NULL))
    {
        return NULL;
    }
    if(object->child==NULL){return NULL;}
    current_element = object->child;
    if(strcmp(current_element->string,string)==0){
    return current_element;
            }
        while (current_element->next != NULL)
        {
            current_element = current_element->next;
            if(strcmp(current_element->string,string)==0){
               return current_element;
            }
        }

    if (current_element == NULL) {
        return NULL;
    }
}


char *CJSON_GetStringValue(const CJSON * const item)
{
    char *temp=item->valuestring;
    return temp;
}

//parse

CJSON *CJSON_Parse(const char str[])
{
     CJSON *root,*content,*inf;
    char* token;
    token=strtok(str, ",");

    root = CJSON_CreateObject();
    content = CJSON_CreateArray();


    char * ans=malloc(1000*sizeof(char));
    *ans='\0';
        char *firstpart=token;
        firstpart=firstpart+1;
        token = strtok(NULL, ",");
    while (token != NULL) {
        strcat(ans,token);
        strcat(ans," ");
        token = strtok(NULL, ",");
    }

    int i=0;
    int x=0;
    char *name=malloc(12*sizeof(char));
    int j=0;
    while(1){
      if(*(firstpart+i)==':'){break;}
      i++;
    }
    x=i+2;
    while(*(firstpart+x)!='\"'){
        *(name+j)=*(firstpart+x);
        j++;
        x++;
    }
    while(j<12){
       *(name+j)='\0';
       j++;
    }
     CJSON_AddItemToObject(root,"type",CJSON_CreateString(name));
     if(*(ans+10)!='['){

       ans=ans+9;
       int n=0;
       while(*(ans+n)!='"'){n++;}
       n++;
       char *Content=malloc(1000*sizeof(char));
       int m=0;
       while(*(ans+n)!='"'){
         *(Content+m)=*(ans+n);
         n++;
         m++;
       }
       if(m<1000){
        *(Content+m)='\0';
       }
       CJSON_AddItemToObject(root,"content",CJSON_CreateString(Content));
     }

 else{
     CJSON_AddItemToObject(root,"content",content);
    ans=ans+12;
    int z=strlen(ans);
    *(ans+z-2)='\0';
    *(ans+z-3)='\0';
    *(ans+z-4)='\0';
    int a=0;
while(1){
    CJSON_AddItemToArray(content, inf = CJSON_CreateObject());
    while(*(ans+a)!=':'){
            a++;}
    a++;
    a++;
    char *sendeR=malloc(50*sizeof(char));
    int b=0;
    while(*(ans+a)!='"'){
      *(sendeR+b)=*(ans+a);
      b++;
      a++;
    }
    while(b<50){
        *(sendeR+b)='\0';
        b++;
    }
    CJSON_AddItemToObject(inf, "sender", CJSON_CreateString(sendeR));
    //add item to object
    while(*(ans+a)!=':'){a++;}
    a++;
    a++;
    char *contenT=malloc(1000*sizeof(char));
    int c=0;
    while(*(ans+a)!='"'){
      *(contenT+c)=*(ans+a);
      c++;
      a++;
    }
    while(c<50){
        *(contenT+c)='\0';
        c++;
    }
    CJSON_AddItemToObject(inf, "content", CJSON_CreateString(contenT));
    if(*(ans+a+1)=='\0'){break;}
}
 }
 return root;
}


//arrays
int CJSON_IsArray(const CJSON* const item)
{
if(item->type==arraytype){return 1;}
else{return 0;}
}

int CJSON_IsString(const CJSON* const item)
{
if(item->type==stringtype){return 1;}
else{return 0;}
}



char * CJSON_Print(const CJSON* item)
{
char *str=malloc(1000*sizeof(char));
*str='\0';
if(item->type==stringtype){
   strcat(str,"\"");
   strcat(str,item->valuestring);
   strcat(str,"\"");
   return str;
}
if(item->type==objecttype){
strcat(str,"{");
CJSON *child=item->child;
char *part1=malloc(1000*sizeof(char));
*part1='\0';
if(child==NULL){return str;}
if(child->type==stringtype){
    strcat(part1,"\"");
    strcat(part1,child->string);
    strcat(part1,"\"");
    strcat(part1,":");
    strcat(part1,"\"");
    strcat(part1,child->valuestring);
    strcat(part1,"\"");
}
else if(child->type==arraytype){
   strcat(part1,CJSON_Print(child));
}
while(child->next!=NULL){
    strcat(part1,",");
    child=child->next;
    if(child->type==stringtype){
    strcat(part1,"\"");
    strcat(part1,child->string);
    strcat(part1,"\"");
    strcat(part1,":");
    strcat(part1,"\"");
    strcat(part1,child->valuestring);
    strcat(part1,"\"");
}
if(child->type==arraytype){
   strcat(part1,CJSON_Print(child));
}
}
strcat(str,part1);
strcat(str,"}");
return str;
}
if(item->type==arraytype){
  char *junk=malloc(1000*sizeof(char));
   *junk='\0';
   CJSON *child=item->child;
   strcat(str,"\"");
   strcat(str,item->string);
   strcat(str,"\"");
   strcat(str,":");
   strcat(str,"[");
   strcat(junk,CJSON_Print(child));
   while(child->next!=NULL){
       strcat(junk,",");
       child=child->next;
       strcat(junk,CJSON_Print(child));
   }
   strcat(str,junk);
   strcat(str,"]");
   return str;
}
}



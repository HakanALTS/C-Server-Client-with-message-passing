#include<stdio.h>
#include<assert.h>
#include<string.h>//strlength
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>// for inet_addr
#include<unistd.h>// for write
#include<pthread.h>// for threading, link with lpthread
#include<semaphore.h>


sem_t semaforeList;
sem_t semaforeFollow;
sem_t semaforeMessage;

typedef struct list{
	char name[50];//this keeps user name
	struct list *next;
};

typedef struct follow{
	char name[50];//this keeps user name
	char followName[50];//this keeps follower
	struct follow *next;
};

typedef struct message{
	char name[50];//This is username
	char text[160];//This user message
	struct message *next;
};

int messageFlag=0;//this control flag message if any user send message that makes flag=1;

struct list *loginUsers;//This is login Users
struct follow *followList;//This is follower list 
struct message *messageList;//This is user 




int controlLogingName(char followName[])//This function provides valiable login
{
	struct list * temp=(struct list*)malloc(sizeof(struct list));
	
	int flag=0;
 	
	temp = loginUsers;
     	while(temp!=NULL)
     	{
		if(strcmp(temp->name,followName)==0)
		{
			flag=1;
			break;
		}
     		temp = temp->next;
     	}
  		
	return flag;

}






void loginOperation(int sock ,char userName[])//This makes login
{
	sem_wait(&semaforeList);//Critical Section
		
	struct list *new_node=(struct list*)malloc(sizeof(struct list));
	struct list * temp=(struct list*)malloc(sizeof(struct list));
	new_node->next=NULL;
	strcpy(new_node->name,userName);

	if(loginUsers==NULL)
	{
   		loginUsers=new_node;
	}
	else
	{
		temp = loginUsers;
     		while(temp->next!=NULL)
     		{
     			temp = temp->next;
     		}
  		temp->next = new_node;
	}
	
	int send_size=send(sock ,"(You are online )\n" ,2000,0);
	sem_post(&semaforeList);
}



void getList(int sock)// get List function for send the user list as comma separated text
{
    	char sendList[251]="(The user list: ";
    	struct list *temp=(struct list*)malloc(sizeof(struct list));
    	temp=loginUsers;
    	while(temp!=NULL)
    	{
		strcat(sendList,temp->name);
		strcat(sendList,", ");
    		temp=temp->next;
    	}
	strcat(sendList,")");
  	int send_size=send(sock , sendList ,2000,0);
	return;
}



int controlFollowName(char followName[])//This function provides valiable follow
{
	struct list * temp=(struct list*)malloc(sizeof(struct list));
	
	int flag=0;
 	
	temp = loginUsers;
     	while(temp!=NULL)
     	{
		if(strcmp(temp->name,followName)==0)
		{
			flag=1;
			break;
		}
     		temp = temp->next;
     	}
  		
	return flag;

}

void followOperation(int sock,char userName[], char followName[])//This function provides follow operation
{

    	char sendMessage[251]="\n(The follow operation: ";

	int control=controlFollowName(followName);//control true user name
	
	if(control==0)
	{

		strcat(sendMessage,"False Username)\n");
		int send_size = send(sock , sendMessage ,2000,0);
		
	}		
	else
	{
		struct follow *new_node=(struct follow*)malloc(sizeof(struct follow));
		struct follow * temp=(struct follow*)malloc(sizeof(struct follow));
		new_node->next=NULL;
		strcpy(new_node->name,followName);
		strcpy(new_node->followName,userName);
				

		if(followList==NULL)//Add head
		{
   			followList=new_node;
			strcat(sendMessage,userName);
			strcat(sendMessage," follows ");
			strcat(sendMessage,followName);
			strcat(sendMessage,")");
			int send_size = send(sock , sendMessage ,2000,0);
		}
		else//find empty position
		{
			temp = followList;
     			while(temp->next!=NULL)
     			{
     				temp = temp->next;
     			}
  			temp->next = new_node;
			strcat(sendMessage,userName);
			strcat(sendMessage," follows ");
			strcat(sendMessage,followName);
			strcat(sendMessage,")\n");
			int send_size = send(sock , sendMessage ,2000,0);

		}	
							
	}
	
	
	
}

void messageOperation(int sock,char txtMessage[],char name[])// this funtion for message
{

	sem_wait(&semaforeMessage);//Critical Section


	messageFlag=1;// for send message to client
	struct message *new_node=(struct message*)malloc(sizeof(struct message));
	struct message * temp=(struct message*)malloc(sizeof(struct message));
	new_node->next=NULL;
	strcpy(new_node->name,name);
	strcpy(new_node->text,txtMessage);


	if(messageList==NULL)
	{
   		messageList=new_node;
	}
	else
	{
		temp =messageList;
     		while(temp->next!=NULL)
     		{
     			temp = temp->next;
     		}
  		temp->next = new_node;
	}
	
	int send_size = send(sock ,"(Message is sent) \n",2000,0);
	sem_post(&semaforeMessage);
}


void myFollowerOperation(int sock,char name[])//this func. provide to see my follower
{

	char messageText[2000]="(My Followers: ";			
	struct follow * tempFollowList=(struct follow*)malloc(sizeof(struct follow));				
	tempFollowList=followList;
	while(tempFollowList!=NULL)	
	{


		if(strcmp(tempFollowList->name,name)==0)
		{
			strcat(messageText,tempFollowList->followName);	
			strcat(messageText,", ");											
		}
					

		tempFollowList=tempFollowList->next;
	}
	
	strcat(messageText,")");
	int send_size = send(sock ,messageText,2000,0);		
}


void *connection_handler(void *socket_desc)//This for read message socket
{
	//Get the socket descriptor
	int sock = *((int*)socket_desc);
	int read_size;
	char *message , client_message[2000];
	int flagLogin=0;//Control, firstly must make login
	char userName[50];
	char userPassword[50];//This for user password

	//Send some messages to the client
	message = "(Welcome deduKodu)\n";
	write(sock , message , strlen(message));
	

	while(messageFlag==1|(read_size = recv(sock , client_message , sizeof client_message , 0)) > 0)//messageFlag control new twitt
	{


			if(messageFlag==1)//Send Message to Followers
			{
				char messageText[2000]="(Follow List: \n";
				
				struct follow * tempFollowList=(struct follow*)malloc(sizeof(struct follow));				
				struct message * tempMessageList=(struct message*)malloc(sizeof(struct message));
	
			
				tempFollowList=followList;
				while(tempFollowList!=NULL)	
				{


					if(strcmp(tempFollowList->followName,userName)==0)//this username for follow
					{
						tempMessageList=messageList;
						while(tempMessageList!=NULL)
						{
							if(strcmp(tempFollowList->name,tempMessageList->name)==0)//follow text 
							{
								strcat(messageText,tempFollowList->name);
								strcat(messageText," said: ");
								strcat(messageText,tempMessageList->text);
								strcat(messageText,"\n");								
							}
							tempMessageList=tempMessageList->next;
						}


					}

				tempFollowList=tempFollowList->next;
				}
	
				strcat(messageText," )");
				int send_size = send(sock ,messageText,2000,0);			
			}

			client_message[read_size]='\0';		

			char str[2000];
	        	char str2[2000];
			strcpy(str,client_message);
			strcpy(str2,client_message);

			const char s[2] = ",";
  			const char d[2] = ":";
  			char *token;
   
   			int length=lengtOfString(str2,d);//find length of command


  			char *cache[length];//for first split part dynamic
   			char cache2[length*2][50];//for second split part

   			/* get the first token */
			token = strtok(str, s);    

   			/* walk through other tokens */
			int count=0;
			while( token != NULL ) // first spilt with first delimeter
			{
   
				cache[count]=token;
				count++;
				token = strtok(NULL, s);
			}   
			count=0;
			int i;
			for(i=0;i<length;i++)
			{
;
				token = strtok(cache[i], d); // second split with second delimeter  
	 			while( token != NULL ) 
   				{
					int k;
					strcpy(cache2[count],token);//cache2 is dynamic array
      					token = strtok(NULL,d);
					count++;
   				}	  		
 
			}; 


			if(strcmp(cache2[0],"command")==0 && strcmp(cache2[1],"login")==0 && strcmp(cache2[2],"username")==0 && cache2[3]!=NULL)// control correct logging
			{
		   	

				int control =controlLogingName(cache2[3]);//Control conflict of same name

				if(control==0)
				{
					strcpy(userName,cache2[3]);
					loginOperation(sock,userName);//make online User
					if(strcmp(cache2[4],"pwd")==0 )//uncessary pwd
					{
						strcpy(userPassword,cache2[5]);
					}
				
					flagLogin=1;//for accept order operation;
					printf("Login %s \n",userName);	

				}
				else
				{
					send(sock,"(False username that has been used) \n",2000 , 0);
				}

					
						
			}
			else if(strcmp(cache2[0],"command")==0 && strcmp(cache2[1],"getList")==0 && flagLogin==1)
			{
				printf("%s wants getList\n",userName);
				getList(sock);//This is for send user list to client
			}
			else if(strcmp(cache2[0],"command")==0 && strcmp(cache2[1],"follow")==0 && strcmp(cache2[2],"username")==0 && cache2[3]!=NULL && flagLogin==1)
			{
				printf("%s wants follow %s\n",userName,cache2[3]);
				followOperation(sock,userName,cache2[3]);//follow operation

			}
			else if(strcmp(cache2[0],"command")==0 && strcmp(cache2[1],"message")==0 && strcmp(cache2[2],"text")==0 && cache2[3]!=NULL && flagLogin==1)
			{
				printf("%s sends message\n",userName);
				messageOperation(sock,cache2[3],userName);//message operation
			}
			else if(strcmp(cache2[0],"command")==0 && strcmp(cache2[1],"getListMyFollowers")==0 && flagLogin==1)
			{
				printf("%s want to see own follower\n",userName);
				myFollowerOperation(sock,userName);//Show My Follower
			}
			else
			{
				char* errorMessage="(You must be login or Enter corrent comment text )\n";
				send(sock, errorMessage ,2000 , 0);
				errorMessage="";
			}

			char temp[]=" ";
			write(sock , temp , 100);	
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

	free(socket_desc); //Free the socket pointer
	return 0;

}


int lengtOfString(char input[], char s[])//Find valiable array length
{
	char *token;
	token = strtok(input, s);
	int count=-1;   
	while( token != NULL ) 
	{
		count++;
		token = strtok(NULL, s);
 	}
	return count;
   
}


int main(int argc, char *argv[])
{

	int socket_desc, new_socket, c, *new_sock;
	struct sockaddr_in server, client;
	char *message;
	
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		puts("Could not create socket");
		return 1;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);
	

	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("Binding failed");
		return 1;
	}	

	sem_t semaforeFollow;

	sem_init(&semaforeList, 0, 1);
	sem_init(&semaforeFollow, 0, 1);	
	sem_init(&semaforeMessage, 0, 1);

	listen(socket_desc, 5);//maximum client number
	puts("deduKodu Server");
	c = sizeof(struct sockaddr_in);

	while((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)))//infinite loop for socket
	{
		pthread_t sniffer_thread;// create thread
		new_sock = malloc(1);
		*new_sock = new_socket;
		if(pthread_create(&sniffer_thread, NULL, connection_handler,(void*)new_sock) < 0)// new thread inter connection
		{
			return 1;
		}		
		
	}
	
	sem_destroy(&semaforeList);	
	sem_destroy(&semaforeFollow);
	sem_destroy(&semaforeMessage);

}


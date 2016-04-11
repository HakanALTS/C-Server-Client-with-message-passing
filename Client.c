#include<stdio.h> //printf
#include<string.h>//strlen
#include<sys/socket.h>//socket
#include<arpa/inet.h> //inet_addr
#include<semaphore.h>
#include<pthread.h>


sem_t semafore;
pthread_t tid[2];

int sock;
struct sockaddr_in server;
char message[1000] , server_reply[2000];



void sendFunction()//send message for command
{
	while(1)
	{
		sem_wait(&semafore);

		printf("\nEnter message :");
		scanf("%s" , message);	
    		
		//Send some data
		if( send(sock , message , strlen(message) , 0) < 0)
		{
			puts("Send failed");
			//return 1;
		}

	}
}


void recvFunction()//listen to server
{

	int recv_size;
	while(1)
	{	
		if((recv_size=recv(sock , server_reply , 2000 , 0))>0 )
		{
 			printf("%s \n",server_reply);			
		}
		sem_post(&semafore);

	}

}






int main(int argc , char *argv[])
{

	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("deduKodu Client");
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");
	puts("code example: command:login,username:name,pwd:12121(pwd is unnecessary)\n");	
	puts("code example: command:getListMyFollowers\n");
	while(1)
	{	
		
		int temp =strcmp("True",server_reply);		

		
		sem_init(&semafore, 0, 1);

		pthread_create(&(tid[0]), NULL, &recvFunction,NULL);//create thread 1
		pthread_create(&(tid[1]), NULL, &sendFunction,NULL);//create thread 2
		

		pthread_join(tid[0], NULL);
		pthread_join(tid[1], NULL);
   
    		sem_destroy(&semafore);




		

	}

}

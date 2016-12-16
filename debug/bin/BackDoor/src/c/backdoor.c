/* ==================================================================== */
/* This file is released into the public domain */
/* ==================================================================== */
#include "backdoor.h"
#include <stdio.h>
/* ==================================================================== */

#define TCP_PORT 27020
#define UPD_PORT 27020
#define BIND_IP "127.0.0.1"
#define BUFF_SIZE 8092
#define RESULT_VAR "bd_result"
#define DONE_STR "done\n"

#ifdef  __linux__
#define COMMAND_EXECSTR  "Err_Job = execstr(TMP_EXEC_STRING,\"errcatch\",\"n\");quit;"
#define COMMAND_CLEAR "clear TMP_EXEC_STRING;clear Err_Job;quit;"
void SendScilabJobBD(char *job)
{
  SciErr sciErr;
  int lencommand = 0;
  char *command = NULL;
  command = strdup(job);
  lencommand = (int)strlen(command);
  C2F(scirun)(COMMAND_CLEAR, (long int)strlen(COMMAND_CLEAR));
  sciErr = createNamedMatrixOfString(pvApiCtx, "TMP_EXEC_STRING", 1, 1, &command);
  C2F(scirun)(COMMAND_EXECSTR, (long int)strlen(COMMAND_EXECSTR));
}

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

pthread_t threadTCP, threadUDP;
int listenTCP;

void *waitTCPRequests();

void openBackDoor()
{
	pthread_create( &threadTCP, NULL, waitTCPRequests, (void*) NULL);
}

void closeBackDoor()
{
	pthread_kill(threadTCP,SIGKILL);
}


void takeRequests(void* arg)
{
  	char buff[BUFF_SIZE];
  	char result[BUFF_SIZE];
	  char cmd[BUFF_SIZE];
  	char *eol,*c;
	  struct sockaddr_in addr,client;
  	int socksize;
  	int m=1,n=1;
	  int commandComplete;
	  ssize_t received=0;
    int socket = (int)arg;
	
		sprintf(cmd,"mprintf(\"\tBackDoor: new connection accepted\n\");");
		SendScilabJobBD(cmd);
		commandComplete=0;
		strcpy(cmd,""); // Clear the command
		do  { // Accept commands through the backdoor
			received=recv(socket,buff,BUFF_SIZE,0);		
			buff[received]='\0'; // Terminate string with NULL char
			c=buff;
			while ((eol = strchr(c,'\n'))!=NULL) { // A end-of-line char has been received
				eol[0]='\0'; // Terminate the string
				if (eol[-1]=='\r') 
					eol[-1]='\0'; // Remove \r too
				strcat(cmd,c);
				//printf("Run command '%s'\n",cmd);
				if (cmd[0]=='@') { // If the first char is the '@' we need to send the result (ans var)
				  ReadMatrix(RESULT_VAR,&m,&n,(double*)result);
					//printf("Sending to client '%g'\n",((double*)result)[0]);
					send(socket,result,sizeof(double),0);
				} else if (cmd[0]=='!') { // The '!' denotes that the client needs a confirmation that the job is ready
					if (cmd[1]=='\\')
            sprintf(result,"%s",cmd+2);
          else
					  sprintf(result,"%s=%s",RESULT_VAR,cmd+1);
					SendScilabJobBD(result);
					send(socket,DONE_STR,strlen(DONE_STR),0);
				} else if (cmd[0]=='\\') { // The '!' denotes that the client needs a confirmation that the job is ready
					sprintf(result,"%s",cmd+1);
					SendScilabJobBD(result);
				} else {
					sprintf(result,"%s=%s",RESULT_VAR,cmd);// run the job
				  //printf("Run command '%s'\n",result);
					SendScilabJobBD(result);
				}
			  strcpy(cmd,""); // Blank for new command
				c=eol+1;
			}
			strcat(cmd,c); // Copy the last part for the new command
		} while (received);
}

void *waitTCPRequests()
{
	sleep(1); // Wait a little bit to scilab to be ready 
	char buff[BUFF_SIZE];
	char result[BUFF_SIZE];
	char cmd[BUFF_SIZE];
	char *eol,*c;
	struct sockaddr_in addr,client;
	int socksize;
	int m=1,n=1;
	listenTCP=socket(AF_INET,SOCK_STREAM,0);
	ssize_t received=0;

	addr.sin_family = AF_INET;
  int actual_port = TCP_PORT + getuid() % 10000;
	addr.sin_port = htons(actual_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (listenTCP, (struct sockaddr *) &addr, sizeof (addr))<0) {
		sprintf(cmd,"mprintf(\"\tBackDoor: TCP port %d is in use\");",actual_port);
		SendScilabJobBD(cmd);
		pthread_exit(NULL);
		return;
	}
	
	if (listen(listenTCP,5)==0) {
		sprintf(cmd,"mprintf(\"\tBackDoor: listening to connections on TCP port %d\");",actual_port);
		SendScilabJobBD(cmd);
    do {
		  int socket=accept(listenTCP,(struct sockaddr *)&client, &socksize);
      pthread_t t;
	    pthread_create( &t, NULL, takeRequests, (void*) socket);
    } while (socket>0);
	}
	close(listenTCP);
}
#else
// Put Windows includes
#endif
/* ==================================================================== */

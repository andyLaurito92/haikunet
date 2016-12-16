
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "winsock2.h"
#include <unistd.h>
#include <sys/types.h> 

OS getOs() 
{
	return WINDOWS;
}


void initLib()
{
  //struct timeval tv;
  //gettimeofday(&tv,NULL);
  realTiSimulation = GetTickCount()*1e-3;
}
void cleanLib()
{
}
double getRealSimulationTime()
{
	return GetTickCount()*1e-3 - realTiSimulation;
}

double getTime()
{
	return 1.0*clock()/CLOCKS_PER_SEC;
}

int waitFor(Time t, RealTimeMode m) 
{
	if (t<=0) return 0;
	double ti=getRealSimulationTime();
	while ((getRealSimulationTime()-ti)<t) ;
	return 0;
}

void printLog(const char *fmt,...) {
  static int init=0;
  if (!init) {
    init=1;
    fclose(fopen("pdevs.log","w"));
  }
	va_list va;
	va_start(va,fmt);
	FILE *fd=fopen("pdevs.log","a");
	vfprintf(fd,fmt,va);
	fclose(fd);
	va_end(va);
}

void spawnProcess(const char *path, char *arg) {
	spawnl(P_NOWAIT , path, " ", arg,NULL);;
}

void writeToPort(short v,int port) {
}

short readFromPort(int port) {
}

/* Scilab Interface */

SOCKET SendSocket;
struct sockaddr_in service;
bool scilabActive;


double getScilabVar(const char *varname) {
  // Try to solve it locally
  char *s;
  double d=strtod(varname,&s);
  if (varname+strlen(varname)==s)
    return d;
  initScilab();
  char buf[1024];
  double f;
  if (!scilabActive) {
     printLog("There is not a Scilab instance running. Returning atof(%s)\n",varname); 
     exitStatus = -1;
     return atof(varname); 
  }	  
  sprintf(buf,"anss=%s",varname);
  executeScilabJob(buf,false);
  executeScilabJob("exists anss",false);
  getAns(&f,1,1);
  if (!scilabActive) {
     printLog("There is not a Scilab instance running. Returning atof(%s)\n",varname); 
     exitStatus = -1;
     return atof(varname); 
  }	  
  if (f==0) {
     printLog("Variable %s does not exists!\n",varname); 
     exitStatus = -1;
     return 0.0; 
  }
  executeScilabJob("ans=anss",false);
  getAns(&f,1,1);
  executeScilabJob("clear anss",false);
  return f;
}

void putScilabVar(const char *varname, double var) {
   char buf[1024];
   sprintf(buf,"%s=%g",varname,var);
   executeScilabJob(buf,false);
}

double executeVoidScilabJob(char *cmd,bool blocking) {
  executeScilabJob(cmd,blocking);
}
double executeScilabJob(char *cmd,bool blocking) {
  initScilab();
  double ans=0;
  int timeout = 2000;
  char buff[1024];
  int SenderAddrSize = sizeof(sockaddr_in);
  int result = -1;

  if (!scilabActive) {
    exitStatus = -1;
    return 0.0; 
  }
  strcpy(buff,"");
  if (blocking) 
    strcpy(buff,"!");
  strcat(buff,cmd);
  sendto(SendSocket,buff,strlen(buff),0,(SOCKADDR *) & service,sizeof(struct sockaddr_in));
  if (blocking) {
    fd_set read_set;
    timeval tv;
    tv.tv_sec=2;
    tv.tv_usec=0;
    read_set.fd_count=1;
    read_set.fd_array[0] = SendSocket;
  
    if (select(0,&read_set,NULL,NULL,&tv)!=0) {
      result = recvfrom(SendSocket, (char*)buff, sizeof(double), 0,(SOCKADDR *) & service ,& SenderAddrSize);
      if (result < 0) {
        scilabActive = false;
        printLog("There is not a Scilab instance running.1\n"); 
    	exitStatus = -1;
        return 0.0; 
      }
    } else {
      scilabActive = false;
      printLog("There is not a Scilab instance running.2\n"); 
      exitStatus = -1;
      return 0.0; 
    }
  }
  return ans;
}

void initScilab() {
  static int init=0;
  char cmd[1024];
  scilabActive=true;
  if (init++ == 0) {
	  unsigned long bio = 1;
	  WSADATA wsaData;
	  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	  SendSocket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);
	  service.sin_family = AF_INET;
	  service.sin_addr.s_addr = inet_addr("127.0.0.1");
	  service.sin_port = htons(27015);
	  //ioctlsocket(SendSocket,FIONBIO,&bio);
  }
}
void cleanScilab() {
 static int clean=0;
 if (clean++ == 0) {
    closesocket(SendSocket);
    WSACleanup();
 }
}

void getAns(double *ans, int rows, int cols) {
  char cmd[124] = "@";
  int timeout = 2000;
  int iResult;
  int SenderAddrSize = sizeof(sockaddr_in);
  sendto(SendSocket,cmd,strlen(cmd),0,(SOCKADDR *) & service,sizeof(struct sockaddr_in));
  ans[0]=0;

  fd_set read_set;
  timeval tv;
  tv.tv_sec=2;
  tv.tv_usec=0;
  read_set.fd_count=1;
  read_set.fd_array[0] = SendSocket;
  
  if (select(0,&read_set,NULL,NULL,&tv)!=0) {
    iResult = recvfrom(SendSocket, (char*)ans, sizeof(double)*rows*cols, 0,(SOCKADDR *) & service ,& SenderAddrSize);
    if (iResult < 0) {
      scilabActive = false;
      printLog("There is not a Scilab instance running.3\n"); 
      exitStatus = -1;
      return; 
    }
  } else {
      printLog("There is not a Scilab instance running.4\n"); 
      exitStatus = -1;
      scilabActive = false;
      return; 
  }
}

extern double tf;
double getFinalTime()
{
	return tf;
}
void setFinalTime(double t)
{
	tf = t;
}


long int PDFileOpen(char* name,char mode) {
	char strMode[2];
	strMode[0]=mode;
	strMode[1]='\0';
	return (long int)fopen(name,strMode);
};
long int PDFileWrite(long int file ,char* buf,int size) {
	fwrite(buf,size,1,(FILE*)file);	
  fflush((FILE*)file);
};

long int PDFileRead(long int file ,char* buf ,int size){
	fread(buf,size,1,(FILE*)file);
}
void PDFileClose(long int file){
	fclose((FILE*)file);
}


void getScilabVector(const char* varname, int *length, double *data) {
  int rows,cols;
  char buf[1024];
  if (!scilabActive) {
    exitStatus = -1;
    return; 
  }
  sprintf(buf,"tempvar=%s",varname);
  executeVoidScilabJob(buf,true);
  sprintf(buf,"save(char([116 101 109 112 46 100 97 116]),tempvar)");
  executeVoidScilabJob(buf,true);
  FILE *FOpen;
  FOpen=fopen("temp.dat","rb");
  char name[24];
  fread(&name,24,1,FOpen);
  int varint;
  fread(&varint,sizeof(int),1,FOpen);
  fread(&rows,sizeof(int),1,FOpen);
  fread(&cols,sizeof(int),1,FOpen);
  fread(&varint,sizeof(int),1,FOpen);
  if (rows>cols) *length=rows; else *length=cols;
  double lastdata;
  for (int i=0;i<*length;i++) {
     fread(&lastdata,sizeof(double),1,FOpen);
     data[i]=lastdata;
  }
  fclose(FOpen);  
}

void getScilabMatrix(const char* varname, int *rows, int *cols, double **data) {
  char buf[1024];
  if (!scilabActive) {
    exitStatus = -1;
    return;
  }
  sprintf(buf,"tempvar=%s",varname);
  executeVoidScilabJob(buf,true);
  sprintf(buf,"save(char([116 101 109 112 46 100 97 116]),tempvar)");
  executeVoidScilabJob(buf,true);
  FILE *FOpen=fopen("temp.dat","rb");
  char name[24];
  fread(&name,24,1,FOpen);
  int varint;
  fread(&varint,sizeof(int),1,FOpen);
  fread(rows,sizeof(int),1,FOpen);
  fread(cols,sizeof(int),1,FOpen);
  fread(&varint,sizeof(int),1,FOpen);
  int nrows=*rows;
  int ncols=*cols;
  int msize=nrows*ncols;
  double lastdata;
  for (int j=0;j<ncols;j++) {
    for (int i=0;i<nrows;i++) {
       fread(&lastdata,sizeof(double),1,FOpen);
       data[i][j]=lastdata;
    }
  }
  fclose(FOpen); 
}


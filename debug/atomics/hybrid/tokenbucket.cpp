#include "tokenbucket.h"
void tokenbucket::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//      %Type% is the parameter type
maxbucketsize= va_arg(parameters,double);
maxrate=0;
sigma=INF;
elapsed=0;
Tickets=1;    // Starts with 1 available Ticket]

for (int i=0;i<10;i++){
  y[i]=0;
};
}
double tokenbucket::ta(double t) {
//This function returns a double.
return sigma;
}
void tokenbucket::dint(double t) {
Tickets--;
sigma=INF;
}
void tokenbucket::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
double *xv;
double newtickets;
xv=(double*)x.value;
newtickets=floor((e+elapsed)*maxrate);
printLog("t=%g  maxrate=%g Tickets=%g newtickets=%g elapsed=%g \n",t,maxrate,Tickets,newtickets,elapsed);
Tickets=Tickets+newtickets;  
if (Tickets>maxbucketsize) Tickets=maxbucketsize;
if (maxrate==0) {
  elapsed=e+elapsed;
} else {
  elapsed=e+elapsed-newtickets/maxrate; 
};
if (x.port==0) {   
  if (Tickets>0) {
     sigma = 0;
  } else {
    sigma=INF;
  };
} else {
  maxrate=xv[0];
  if (maxrate<0) maxrate=0;
  elapsed=0;
  sigma=INF;
};
}
Event tokenbucket::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)
y[0]=Tickets;
return Event(&y[0],0);
}
void tokenbucket::exit() {
//Code executed at the end of the simulation.
}

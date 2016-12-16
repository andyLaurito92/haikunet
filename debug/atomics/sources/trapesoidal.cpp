#include "trapesoidal.h"
void trapesoidal::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//      %Type% is the parameter type
Vu = va_arg(parameters,double);
Vl = va_arg(parameters,double);
Tu = va_arg(parameters,double);
Tl = va_arg(parameters,double);
Tr = va_arg(parameters,double);
Tf = va_arg(parameters,double);
Method=va_arg(parameters,char*);
dq=va_arg(parameters,double);
for(int i=0;i<10;i++){y[i]=0;};
v[0]=Vl;
v[1]=Vu;
v[2]=Vu;
v[3]=Vl;
tv[0]=Tr;
tv[1]=Tu;
tv[2]=Tf;
tv[3]=Tl;
if (Tr>0){mv[0]=(Vu-Vl)/Tr;};
if (Tf>0){mv[2]=(Vl-Vu)/Tf;};
mv[1]=0;
mv[3]=0;
k=0;
sigma=0;
}
double trapesoidal::ta(double t) {
//This function returns a double.
return sigma;
}
void trapesoidal::dint(double t) {
sigma=tv[k];
k=(k+1)%4;
if( (strcmp(Method,"QSS")==0)&&( (k==1)||(k==3) ) ){
	k=k-1;
y[5]=10;
y[6]=20;
	if( ( ((v[k]+dq)<Vu)&&(mv[k]>0) )|| ( ((v[k]-dq)>Vl) && (mv[k]<0) ) ){
		if(mv[k]>0){
			v[k]=v[k]+dq;
			sigma=dq/mv[k];
		}else{
			v[k]=v[k]-dq;
			sigma=-dq/mv[k];
		}
	}else{
y[5]=v[k];
y[6]=mv[k];
		if(mv[k]>0){sigma=(Vu-v[k])/mv[k];}else{sigma=-(v[k]-Vl)/mv[k];};
		v[0]=Vl;
		v[2]=Vu;
		y[4]=1000;
		k=(k+1)%4;
y[7]=sigma;
	};
}else{y[4]=2000;};
}
void trapesoidal::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
}
Event trapesoidal::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)
y[0]=v[k];
y[1]=mv[k];
y[2]=0;
y[3]=k;
return Event(&y[0],0);
}
void trapesoidal::exit() {

}

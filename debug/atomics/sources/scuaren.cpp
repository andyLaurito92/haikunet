#include "scuaren.h"
void scuaren::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//      %Type% is the parameter type
A=va_arg(parameters,double);    //Amplitude
F=va_arg(parameters,double);    //frecuency
D=va_arg(parameters,double);    //duty cycle
for(int i=0;i<10;i++){y[i]=0;};
lev=1;
sigma=0;
}
double scuaren::ta(double t) {
return sigma;
}
void scuaren::dint(double t) {
sigma=(D*lev+(1-lev)*(100-D))/(100*F);
lev=1-lev;
}
void scuaren::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
}
Event scuaren::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)
y[0]=lev*A;
return Event(&y[0],0);
}
void scuaren::exit() {

}

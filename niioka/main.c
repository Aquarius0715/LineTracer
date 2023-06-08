#include<stdio.h>
#include<stdlib.h>
#include "motor.h"
#include "light.h"
int main(){
    int ms,ls,rs;
    int fd = motor_init();
    int bi = 0b00000;
    //5ビット目L(Left) 
    //4ビット目ML(Mid-left)
    //3ビット目M(Mid)
    //2ビット目MR(Mid-right)
    //1ビット目R(Right)
    motor_drive(fd,0,0);
    while(1){
      ms=0,ls=0,rs=0;
        bi=sensor();
	if((bi&(1<<0))==1){rs=5;}
	if((bi&(1<<1))==1){rs=2;}
	if((bi&(1<<2))==1){ms=5;}
	if((bi&(1<<3))==1){ms=2;}
	if((bi&(1<<4))==1){ls=5;}
	if(bi==0)
	motor_drive(fd,ms+rs,ms+ls);
        delay(100);
    }
    return 0;
}

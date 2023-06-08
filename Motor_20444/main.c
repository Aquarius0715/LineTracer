#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "motor.h"
#include "light.h"
int main(){
    int ls,rs,state;
    int p_ls,p_rs;
    int fd = motor_init();
    int bi = 0b00000;
    int p_bi;
    motor_drive(fd,0,0);
    while(1){
        ls=0;
        rs=0;
        bi=sensor();
        if(state == 0 || bi != 0b00000){
            if(bi&(1<<0)){
                ls+=8;
                rs+=16;
                state=1;
            }
            if(bi&(1<<1)){
		ls+=6;
		rs+=10;
            if(bi&(1<<2)){
                ls+=12;
                rs+=12;
            }
            if(bi&(1<<3)){
		ls+=12;
		rs+=6;
            if(bi&(1<<4)){
                ls+=16;
                rs+=8;
                state=2;
            }
        }else{
            if(state==1)rs=16,ls=-8;
            if(state==2)rs=-8,ls=16;
	    if(bi&(1<<2))state=0;
	    if(bi&(1<<0))state=1;
	    if(bi&(1<<4))state=2;
        }
        if(ls>16)ls=16;
        if(rs>16)rs=16;
        if(p_ls!=ls){
	    motor_l(fd,ls);
	    p_ls=ls;
	}
	if(p_rs!=rs){
	    motor_r(fd,rs);
	    p_rs=rs;
	}
	p_bi=bi;
    }
    return 0;
}

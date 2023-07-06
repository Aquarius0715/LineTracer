#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "motor.h"
#include "light.h"
int main(){
    int ls,rs,p_ls,p_rs;
    int state=3,chg=500,cnt=0;
    int fd = motor_init();
    int bi = 0b00000;
    motor_drive(fd,0,0);
    while(1){
        cnt++;
        ls=0;
        rs=0;
        bi=sensor();
        if(bi != 0b00000 && state!=3){
            if(bi&(1<<0)){
                ls+=4;
                rs+=16;
		printf("L,");
                state=1;
            }
            if(bi&(1<<1)){
	      if(cnt<chg){
		ls+=12;
		rs+=4;
	      }else{
		ls+=4;
		rs+=12;
	      }
	      printf("ML,");
	      state=1;
	    }
            if(bi&(1<<2)){
                ls+=16;
                rs+=16;
		printf("M,");
		state=0;
            }
            if(bi&(1<<3)){
	      if(cnt<chg){
		ls+=4;
		rs+=12;
	      }else{
		ls+=12;
		rs+=4;
	      }
	      printf("MR");
	      state=2;
	    }
            if(bi&(1<<4)){
                ls+=16;
                rs+=4;
		printf("R");
                state=2;
            }
	    if(bi==0b11011)state=3;
        }else{
	    if((state==0||state==3)&&cnt>chg){
	      motor_drive(fd,0,0);
	      while(bi!=0b00100)bi=sensor();
	    }
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
	    printf("ls=%d\n",ls);
	    cnt=0;
	}
	if(p_rs!=rs){
	    motor_r(fd,rs);
	    p_rs=rs;
	    printf("rs=%d\n",rs);
	    cnt=0;
	}
	printf("state=%d\n",state);
    }
    return 0;
}

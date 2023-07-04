#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "motor.h"
#include "light.h"
void run(int fd,int ls,int rs,int chg){
  int fd = motor_init();
  int cnt=0;
  motor_drive(fd,ls,rs);
  while(cnt<chg)cnt++;
  motor_drive(fd,0,0);
}
void move_4(){
int main(){
    int ls,rs,p_ls,p_rs;
    int state=3,chg=300,cnt=0;
    int fd = motor_init();
    int bi = 0b00000;
    motor_drive(fd,0,0);
    while(1){
        cnt++
        ls=0;
        rs=0;
        bi=sensor();
        if(bi != 0b00000 && state!=3){
            if(bi&(1<<0)){
                ls+=8;
                rs+=16;
                state=1;
            }
            if(bi&(1<<1)){
	      if(cnt<chg){
		ls+=16;
		rs+=10;
	      }else{
		ls+=10;
		rs+=16;
	      }
	    }
            if(bi&(1<<2)){
                ls+=16;
                rs+=16;
		state=0;
            }
            if(bi&(1<<3)){
	      if(cnt<chg){
		ls+=10;
		rs+=16;
	      }else{
		ls+=16;
		rs+=10;
	      }
	    }
            if(bi&(1<<4)){
                ls+=16;
                rs+=8;
                state=2;
            }
	    if(bi==0b11011)state=3;
        }else{
	    if(state==3 && cnt>chg){
	      motor_drive(fd,0,0);
	      while(bi!=0b00100)bi=sensor();
	      move_4();
	    }
	    if(state==0 && cnt>chg){
	      motor_drive(fd,16,4);
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
	    cnt=0;
	}
	if(p_rs!=rs){
	    motor_r(fd,rs);
	    p_rs=rs;
	    cnt=0;
	}
    }
    return 0;
}
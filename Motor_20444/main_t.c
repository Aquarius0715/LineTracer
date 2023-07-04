#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "motor.h"
#include "light.h"

void move_t(int fd,int bi){
  static int state=0;
  int cnt=0;
  motor_drive(fd,16,16);
  while(cnt<500)cnt++;
  if(state==0 || state==2){
    motor_drive(fd,-16,16);
    while(bi&(1<<2))bi=sensor();
    while(bi!=0b00100)bi=sensor();
    motor_drive(fd,0,0);
  }else if(state==3){
    motor_drive(fd,16,-16);
    while(bi&(1<<2))bi=sensor();
    while(bi!=0b00100)bi=sensor();
    motor_drive(fd,0,0);
  }
  state++;
}

int main(){
    int ls,rs,p_ls,p_rs;
    int state=3,chg=300,cnt=0,lights=0;
    int fd = motor_init();
    int bi = 0b00000;
    motor_drive(fd,0,0);
    while(1){
        cnt++;
        ls=0;
        rs=0;
	lights=0;
        bi=sensor();
        if(bi != 0b00000 && state!=3){
            if(bi&(1<<0)){
                ls+=8;
                rs+=16;
                state=1;
		lights++;
            }
            if(bi&(1<<1)){
	      if(cnt<chg){
		ls+=16;
		rs+=10;
	      }else{
		ls+=10;
		rs+=16;
	      }
	      lights++;
	    }
            if(bi&(1<<2)){
                ls+=16;
                rs+=16;
		state=0;
		lights++;
            }
            if(bi&(1<<3)){
	      if(cnt<chg){
		ls+=10;
		rs+=16;
	      }else{
		ls+=16;
		rs+=10;
	      }
	      lights++;
	    }
            if(bi&(1<<4)){
                ls+=16;
                rs+=8;
                state=2;
		lights++;
            }
	    if(bi==0b11011)state=3;
        }else{
	    if(state==3 && cnt>chg){
	      motor_drive(fd,0,0);
	      while(bi!=0b00100)bi=sensor();
	      cnt=0;
	    }
	    if(state==0 && cnt>chg){
	      motor_drive(fd,-16,16);
	      while(bi!=0b00100)bi=sensor();
	      cnt=0;
	    }
            if(state==1)rs=16,ls=-8;
            if(state==2)rs=-8,ls=16;
	    if(bi&(1<<2))state=0;
	    if(bi&(1<<0))state=1;
	    if(bi&(1<<4))state=2;
        }
	if(lights>3)move_t(fd,bi);
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

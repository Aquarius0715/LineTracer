#include<stdio.h>
#include<stdlib.h>
#include "motor.h"
#include "light.h"

int state=3;
long vcnt=0,vchg=600000;

void move_4(int fd){
  static int IV=0,bi=0;
  int cnt=0;
  motor_drive(fd,16,16);
  while(cnt<50000)cnt++;
  if(IV%3==0){
    motor_drive(fd,4,12);
    while(!(bi&(1<<4)))bi=sensor();
    while(bi!=0b00100)bi=sensor();
    motor_drive(fd,0,0);
    vchg=100000;
  }else if(IV%3==1){
    motor_drive(fd,12,4);
    while(!(bi&(1<<0)))bi=sensor();
    while(bi!=0b00100)bi=sensor();
    motor_drive(fd,0,0);
    vchg=300000;
  }else if(IV%3==2){
    motor_drive(fd,12,4);
    while(!(bi&(1<<0)))bi=sensor();
    while(cnt<50000)cnt++;
    motor_drive(fd,4,12);
    while(!(bi&(1<<2)))bi=sensor();
    while(cnt<50000)cnt++;
    motor_drive(fd,0,0);
    vchg=500000;
    state=1;
  }
  IV++;
}

int main(){
    int ls,rs,p_ls,p_rs;
    long chg=50000;
    long cnt=chg;
    int fd = motor_init();
    int bi = 0b00000;
    motor_drive(fd,0,0);
    while(1){
      ls=0;
      rs=0;
      bi=sensor();
      if(bi == 0b11011)state=3;
      if(bi > 0 && state != 3){
	if(bi&(1<<0)){
	  ls-=4;
	  rs+=8;
	  //printf("L,");
	  state=1;
	  cnt=0;
        }
	if(bi&(1<<1)){
	  if(cnt<chg){
	    ls+=12;
	    rs+=6;
	  }else{
	    ls+=6;
	    rs+=12;
	  }
	  //printf("ML,");
	  state=1;
	}
	if(bi&(1<<2)){
	  ls+=12;
	  rs+=12;
	  //printf("M,");
	  state=0;
	}
	if(bi&(1<<3)){
	  if(cnt<chg){
	    ls+=6;
	    rs+=12;
	  }else{
	    ls+=12;
	    rs+=6;
	  }
	  //printf("MR,");
	  state=2;
	}
	if(bi&(1<<4)){
	  ls+=8;
	  rs-=4;
	  //printf("R");
	  state=2;
	  cnt=0;
	}
	//printf(" ");
      }else{
	if(state==1)rs=16,ls=-8;
	if(state==2)rs=-8,ls=16;
	if(state==3 && cnt>=chg){
	  motor_drive(fd,0,0);
	  while(bi!=0b00100)bi=sensor();
	  state=0;
	  cnt=0;
	}
      }
      if(ls>16)ls=16;
      if(rs>16)rs=16;
      if(p_ls!=ls){
	motor_l(fd,ls);
	p_ls=ls;
	//printf(" ls=%d ",ls);
      }
      if(p_rs!=rs){
	motor_r(fd,rs);
	p_rs=rs;
	//printf(" rs=%d ",rs);
      }
      //printf(" state=%d ",state);
      //printf(" cnt=%d\n",cnt);
      cnt++;
      vcnt++;
      //printf("%ld\n",vcnt);
      if(vcnt>vchg){
	vcnt=0;
	move_4(fd);
      }
    }
    return 0;
}

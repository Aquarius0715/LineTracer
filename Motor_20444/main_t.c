#include<stdio.h>
#include<stdlib.h>
#include "motor.h"
#include "light.h"

void move_t(int fd){
  static int state=0,bi=0;
  int cnt=0;
  motor_drive(fd,16,16);
  while(cnt<50000)cnt++;
  if(state%3==0){
    motor_drive(fd,-8,8);
    while(bi&(1<<4))bi=sensor();
    while(bi!=0b00100)bi=sensor();
    motor_drive(fd,0,0);
  }else if(state%3==2){
    motor_drive(fd,8,-8);
    while(bi&(1<<0))bi=sensor();
    while(bi!=0b00100)bi=sensor();
    motor_drive(fd,0,0);
  }
  state++;
}

int main(){
    int ls,rs,p_ls,p_rs;
    int state=3;
    long chg=50000;
    long cnt=chg;
    int lt;
    int fd = motor_init();
    int bi = 0b00000;
    motor_drive(fd,0,0);
    while(1){
      ls=0;
      rs=0;
      lt=0;
      bi=sensor();
      if(bi == 0b11011)state=3;
      if(bi > 0 && state != 3){
	if(bi&(1<<0)){
	  ls-=4;
	  rs+=8;
	  //printf("L,");
	  state=1;
	  lt++;
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
	  lt++;
	}
	if(bi&(1<<2)){
	  ls+=12;
	  rs+=12;
	  //printf("M,");
	  state=0;
	  lt++;
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
	  lt++;
	}
	if(bi&(1<<4)){
	  ls+=8;
	  rs-=4;
	  //printf("R");
	  state=2;
	  lt++;
	  cnt=0;
	}
	//printf(" ");
	cnt++;
      }else{
	if(state==0 && cnt>=chg){
	  motor_drive(fd,8,-8);
	  while(bi!=0b00100)bi=sensor();
	  cnt=0;
	}
	if(state==1)rs=16,ls=-8;
	if(state==2)rs=-8,ls=16;
	if(state==3 && cnt>=chg){
	  motor_drive(fd,0,0);
	  while(bi!=0b00100)bi=sensor();
	  cnt=0;
	}
	if(lt>3)move_t(fd);
	cnt++;
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
    }
    return 0;
}

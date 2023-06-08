#ifndef stdio
#include<stdio.h>
#endif
#ifndef stdlib
#include<stdlib.h>
#endif
#ifndef wiringPi
#include "wiringPi.h"
#endif
#ifndef wiringPiI2C
#include "wiringPiI2C.h"
#endif
#define OS_L 5
#define OS_ML 6
#define OS_M 13
#define OS_MR 19
#define OS_R 26

int sensor(){
    int bi = 0b00000;
    if(digitalRead(OS_L)==1) bi |= (1<<0);//bit0 turns 1
    if(digitalRead(OS_ML)==1)bi |= (1<<1);//bit1 turns 1
    if(digitalRead(OS_M)==1) bi |= (1<<2);//bit2 turns 1
    if(digitalRead(OS_MR)==1)bi |= (1<<3);//bit3 turns 1
    if(digitalRead(OS_R)==1) bi |= (1<<4);//bit4 turns 1
    return bi;
}

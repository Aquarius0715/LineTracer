#include<stdio.h>
#include<stdlib.h>
#include "motor.h"
#include "light.h"
int main(){
    int fd = motor_init();
    motor_drive(fd,15,15);
    return 0;
}

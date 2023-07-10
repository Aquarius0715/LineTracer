#include<stdio.h>
#include<stdlib.h>
#include "wiringPi.h"
#include "wiringPiI2C.h"

#define OS_L 5
#define OS_ML 6
#define OS_M 13
#define OS_MR 19
#define OS_R 26

// 以下、定数宣言です
// PWMユニットのI2Cアドレス
// i2cdetect で確認可能、違っていたら修正して下さい

#define PWMI2CADR 0x40

// モータードライバの各入力が接続されているPWMユニットのチャネル番号
// 右側のモーター：パワーユニットのK1またはK2に接続（説明書は誤り）
// ENAはPWM駆動に使う（1でブリッジ動作、0はブリッジオフ）
// IN1とIN2は右車輪の回転方向を決める（後進：0,1、前進：1,0）（0,0 と1,1 はブレーキ）
#define ENA_PWM 8
#define IN1_PWM 9
#define IN2_PWM 10
// 左側のモーター：パワーユニットのK3またはK4に接続（説明書は誤り）
// ENBはPWM駆動に使う（1でブリッジ動作、0はブリッジオフ）
// IN3とIN4は左車輪の回転方向を決める（後進：0,1、前進：1,0）（0,0 と1,1 はブレーキ）
#define ENB_PWM 13
#define IN3_PWM 11
#define IN4_PWM 12 // PWMモジュールのレジスタ番号
#define PWM_MODE1 0
#define PWM_MODE2 1
#define PWM_SUBADR1 2
#define PWM_SUBADR2 3
#define PWM_SUBADR3 4
#define PWM_ALLCALL 5
// PWM番号×4+PWM_0_??_? でレジスタ番号は求まる
#define PWM_0_ON_L 6
#define PWM_0_ON_H 7
#define PWM_0_OFF_L 8
#define PWM_0_OFF_H 9
// PWM出力定数
#define PWMFULLON 16
#define PWMFULLOFF 0
// プリスケーラのレジスタ番号
// PWM周波数を決めるレジスタ番号、100Hzなら61をセット
#define PWM_PRESCALE 254

int set_pwm_output(int fd, int pwmch, int outval){
//motor_drive( )から呼ばれる関数、PWMユニットへの書き込みをやっています
//直接、他から呼び出す必要はないと思われますが、必要ならどうぞ

  int ef = 0;
  int regno;
  if ((pwmch < 0) || (pwmch > 15)) ef = 1;// チャネルの指定違反チェック
  if ((outval < 0) || (outval > 16)) ef = ef + 2;// 出力値の指定違反チェック
  if (ef == 0){regno = PWM_0_ON_L + pwmch * 4; // 1chあたり4レジスタで16ch分あるので
    if (outval == 16){
	wiringPiI2CWriteReg8(fd,regno+3,0);
	wiringPiI2CWriteReg8(fd,regno+1,0x10);
    } else {
	wiringPiI2CWriteReg8(fd,regno+1,0);
	wiringPiI2CWriteReg8(fd,regno+3,outval);
    }
  }
  return ef; // エラーがなければ0が返る
}

int motor_drive(int fd, int rm, int lm){ // モーターを制御するための関数
// fdはI2C初期化時のファイルディスクプリタ（デバイス番号のようなもの）
// lmは左モーター、rmは右モーターの駆動数値で、-16～+16 の範囲で指定
// 負の場合は後方向に回転、正の場合は前方向に回転
// 絶対値が大きいほど、パワーが大きくなる
// PWMユニット自体は12ビット精度だが、上位4ビット分を制御
// あまり細かく制御しても、ロボカーの動きとしては大差ないと考えられるため
// 必要と思うなら、自分でマニュアルを見てプログラムを書いて下さい
  set_pwm_output(fd, ENA_PWM, 0); // Right motor disable
  set_pwm_output(fd, ENB_PWM, 0); // Left motor disable
  // Right motor PWM control
  if (rm < 0){
    set_pwm_output(fd, IN1_PWM, 0); // OUT1->GND
    set_pwm_output(fd, IN2_PWM, 16); // OUT2->+Vs
    rm = abs(rm);
  } else {
    set_pwm_output(fd, IN1_PWM, 16); // OUT1->+Vs
    set_pwm_output(fd, IN2_PWM, 0); // OUT2->GND
  }
  // Left motor PWM control
  if (lm < 0){
    set_pwm_output(fd, IN3_PWM, 0); // OUT3->GND
    set_pwm_output(fd, IN4_PWM, 16); // OUT4->+Vs
    lm = abs(lm);
  } else {set_pwm_output(fd, IN3_PWM, 16); // OUT3->+Vs
    set_pwm_output(fd, IN4_PWM, 0); // OUT4->GND
  }
  if (lm > 16) lm = 16;
  if (rm > 16) rm = 16;
  set_pwm_output(fd, ENA_PWM, rm); // Right motor PWM start
  set_pwm_output(fd, ENB_PWM, lm); // Left motor PWM start
  return 0;// 戻り値は常に0
}//この他に、プログラムの最初の方で以下のPWMユニットの初期化が必要。

int motor_r(int fd, int rm){ // モーターを制御するための関数
// fdはI2C初期化時のファイルディスクプリタ（デバイス番号のようなもの）
// lmは左モーター、rmは右モーターの駆動数値で、-16～+16 の範囲で指定
// 負の場合は後方向に回転、正の場合は前方向に回転
// 絶対値が大きいほど、パワーが大きくなる
// PWMユニット自体は12ビット精度だが、上位4ビット分を制御
// あまり細かく制御しても、ロボカーの動きとしては大差ないと考えられるため
// 必要と思うなら、自分でマニュアルを見てプログラムを書いて下さい
  set_pwm_output(fd, ENA_PWM, 0); // Right motor disable
  // Right motor PWM control
  if (rm < 0){
    set_pwm_output(fd, IN1_PWM, 0); // OUT1->GND
    set_pwm_output(fd, IN2_PWM, 16); // OUT2->+Vs
    rm = abs(rm);
  } else {
    set_pwm_output(fd, IN1_PWM, 16); // OUT1->+Vs
    set_pwm_output(fd, IN2_PWM, 0); // OUT2->GND
  }
  if (rm > 16) rm = 16;
  set_pwm_output(fd, ENA_PWM, rm); // Right motor PWM start
  return 0;// 戻り値は常に0
}//この他に、プログラムの最初の方で以下のPWMユニットの初期化が必要。

int motor_l(int fd, int lm){ // モーターを制御するための関数
// fdはI2C初期化時のファイルディスクプリタ（デバイス番号のようなもの）
// lmは左モーター、rmは右モーターの駆動数値で、-16～+16 の範囲で指定
// 負の場合は後方向に回転、正の場合は前方向に回転
// 絶対値が大きいほど、パワーが大きくなる
// PWMユニット自体は12ビット精度だが、上位4ビット分を制御
// あまり細かく制御しても、ロボカーの動きとしては大差ないと考えられるため
// 必要と思うなら、自分でマニュアルを見てプログラムを書いて下さい
  set_pwm_output(fd, ENB_PWM, 0); // Left motor disable
  // Left motor PWM control
  if (lm < 0){
    set_pwm_output(fd, IN3_PWM, 0); // OUT3->GND
    set_pwm_output(fd, IN4_PWM, 16); // OUT4->+Vs
    lm = abs(lm);
  } else {set_pwm_output(fd, IN3_PWM, 16); // OUT3->+Vs
    set_pwm_output(fd, IN4_PWM, 0); // OUT4->GND
  }
  if (lm > 16) lm = 16;
  set_pwm_output(fd, ENB_PWM, lm); // Left motor PWM start
  return 0;// 戻り値は常に0
}//この他に、プログラムの最初の方で以下のPWMユニットの初期化が必要。

int motor_init(){
  int fd;
  wiringPiSetupGpio(); /* BCM_GPIOピン番号で指定*/
  fd = wiringPiI2CSetup(PWMI2CADR); // このfdがファイルディスクプリタ
  if (fd < 0){
    printf("I2Cの初期化に失敗しました。終了します。¥n");
    exit(EXIT_FAILURE);
  }
  wiringPiI2CWriteReg8(fd,PWM_PRESCALE,61);    //PWM周期10msに設定
  wiringPiI2CWriteReg8(fd,PWM_MODE1,0x10);    //SLEEPmode
  wiringPiI2CWriteReg8(fd,PWM_MODE1,0);   //NORMALmode 
  delay(1); // wait for stabilizing internal oscillator
  wiringPiI2CWriteReg8(fd,PWM_MODE1,0x80);    //Restart all PWM ch
  return fd;
}

int sensor(){
    int bi = 0b00000;
    if(digitalRead(OS_L)==1) bi |= (1<<0);//bit0 turns 1
    if(digitalRead(OS_ML)==1)bi |= (1<<1);//bit1 turns 1
    if(digitalRead(OS_M)==1) bi |= (1<<2);//bit2 turns 1
    if(digitalRead(OS_MR)==1)bi |= (1<<3);//bit3 turns 1
    if(digitalRead(OS_R)==1) bi |= (1<<4);//bit4 turns 1
    return bi;
}

int main(){
    int ls,rs,p_ls,p_rs;
    int state=3,chg=50000;
    int cnt=chg;
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
	  printf("L,");
	  state=1;
	  cnt=0;
        }
	if(bi&(1<<1)){
	  if(cnt<chg){
	    ls+=6;
	    rs+=4;
	  }else{
	    ls+=4;
	    rs+=6;
	  }
	  printf("ML,");
	  state=1;
	}
	if(bi&(1<<2)){
	  ls+=8;
	  rs+=8;
	  printf("M,");
	  state=0;
	}
	if(bi&(1<<3)){
	  if(cnt<chg){
	    ls+=4;
	    rs+=6;
	  }else{
	    ls+=6;
	    rs+=4;
	  }
	  printf("MR,");
	  state=2;
	}
	if(bi&(1<<4)){
	  ls+=8;
	  rs-=4;
	  printf("R");
	  state=2;
	  cnt=0;
	}
	printf(" ");
	cnt++;
      }else{
	if(state==1)rs=16,ls=-8;
	if(state==2)rs=-8,ls=16;
	if(state==3 && cnt>=chg){
	  motor_drive(fd,0,0);
	  while(bi!=0b00100)bi=sensor();
	  state=0;
	  cnt=0;
	}
	cnt++;
      }
      if(ls>16)ls=16;
      if(rs>16)rs=16;
      if(p_ls!=ls){
	motor_l(fd,ls);
	p_ls=ls;
	printf(" ls=%d ",ls);
      }
      if(p_rs!=rs){
	motor_r(fd,rs);
	p_rs=rs;
	printf(" rs=%d ",rs);
      }
      printf(" state=%d ",state);
      printf(" cnt=%d\n",cnt);
    }
    return 0;
}

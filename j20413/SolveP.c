#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

// PWMユニットのI2Cアドレス
#define PWMI2CADR 0x40

// PWM制御に使う。（１でブリッジ動作、0はブリッジオフ）
#define ENA_PWM 8

// IN1とIN2は右車輪の回転方向を決める（後進：0, 1, 前進：1, 0）（0, 0と1, 1はブレーキ）
#define IN1_PWM 9
#define IN2_PWM 10

// 左側のモーター：パワーユニットのK3またはK4に接続
// ENBはPWM駆動に使う（1でブリッジ動作、0はブリッジオフ）
#define ENB_PWM 13

// IN3とIN4は左車輪の回転方向を決める（後進：0, 1、前進：1, 0）（0, 0と1, 1はブレーキ）
#define IN3_PWM 11
#define IN4_PWM 12

// PWMモジュールのレジスタ番号
#define PWM_MODE1 0
#define PWM_MODE2 1
#define PWM_SUBADR1 2
#define PWM_SUBADR2 3
#define PWM_SUBADR3 4
#define PWM_ALLCALL 5

// PWM番号＊4＋PWM_0_??_?でレジスタ番号は求まる
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

// 光センサーのピン番号
#define GPIO_L 5
#define GPIO_ML 6
#define GPIO_M 13
#define GPIO_MR 19
#define GPIO_R 26

// motor_drive()から呼ばれる関数、PWMユニットへの書き込みをする。
int set_pwm_output(int fd, int pwmch, int outval) {
  int ef = 0;
  int regno;

  if ((pwmch < 0) || (pwmch > 15)) ef = 1; // チャネルの指定違反チェック
  if ((outval < 0) || (outval > 16)) ef = ef + 2; // 出力値の指定違反チェック
  if (ef == 0) {
    regno = PWM_0_ON_L + pwmch * 4; // 1chあたり4レジスタで16ch分あるので
    if (outval == 16) {
      wiringPiI2CWriteReg8(fd, regno + 3, 0);
      wiringPiI2CWriteReg8(fd, regno + 1, 0x10);
    } else {
      wiringPiI2CWriteReg8(fd, regno + 1, 0);
      wiringPiI2CWriteReg8(fd, regno + 3, outval);
    }
  }
  return ef; // エラーがなければ０が返る
}


// モーターを制御するための関数。
// fdはI2C初期化時のファイルディスクリプタ（デバイス番号のようなもの）
// lmは左モーター、rmは右モーターの駆動数値で、-16~+16の範囲で指定
// 負の場合は後ろ方向に回転、生の場合は前方向に回転
// 全体値が大きいほど、パワーが大きくなる
// PWMユニット自体は12ビット制度だが、上位４ビット分を制御
int motor_drive(int fd, int lm, int rm) {
  set_pwm_output(fd, ENA_PWM, 0); // 右のモーター無効化
  set_pwm_output(fd, ENB_PWM, 0); // 左のモーター有効化
  // 右モーターの制御
  if (rm < 0) {
    set_pwm_output(fd, IN1_PWM, 0); // OUT -> GND
    set_pwm_output(fd, IN2_PWM, 16); // OUT2 -> +Vs
    rm = abs(rm);
  } else {
    set_pwm_output(fd, IN1_PWM, 16); // OUT1 -> +Vs
    set_pwm_output(fd, IN2_PWM, 0);  // OUT2 -> GND
  }

  // 左モーターの制御
  if (lm < 0) {
    set_pwm_output(fd, IN3_PWM, 0); // OUT3 -> GND
    set_pwm_output(fd, IN4_PWM, 16); // OUT -> +Vs
    lm = abs(lm);
  } else {
    set_pwm_output(fd, IN3_PWM, 16); // OUT3 -> +Vs
    set_pwm_output(fd, IN4_PWM, 0); // OUT4 -> GND
  }
  if (lm > 16) lm = 16;
  if (rm > 16) rm = 16;
  set_pwm_output(fd, ENA_PWM, rm); // 右モータースタート
  set_pwm_output(fd, ENB_PWM, lm); // 左モータースタート
  return 0;
}

int sensor(char* c) {
  int i = 0;
  const int pin[] = {GPIO_L, GPIO_ML, GPIO_M, GPIO_MR, GPIO_R};
  if (digitalRead(GPIO_L) == (c[0] - '0') && digitalRead(GPIO_ML) == (c[1] - '0') && digitalRead(GPIO_M) == (c[2] - '0') && digitalRead(GPIO_MR) == (c[3] - '0' && digitalRead(GPIO_R) == (c[4] - '0')) {
      return 1;
	} else {
      return 0;
    }
    /*  while (c[i] != '\0') {
    if (i > 4) {
      printf("Out of Argument Error\n");
      exit(-1);
    }
    if (c[i] == '0') {
      continue;
    }
    if ((c[i] - '0') == digitalRead(pin[i])) {
      i++;
      return 1;
    }
    else {
      return 0;
    }
  }
  return 0;
    */
}


int main() {
  int fd;
  wiringPiSetupGpio();
  fd = wiringPiI2CSetup(PWMI2CADR);
  if (fd < 0) {
    printf("I2Cの初期化に失敗しました。終了します。\n");
    exit(EXIT_FAILURE);
  }
  wiringPiI2CWriteReg8(fd, PWM_PRESCALE, 61);
  wiringPiI2CWriteReg8(fd, PWM_MODE1, 0x10);
  wiringPiI2CWriteReg8(fd, PWM_MODE1, 0);
  delay(1);
  wiringPiI2CWriteReg8(fd, PWM_MODE1, 0x80);

  while (sensor("11111")) delay(1000);

  int ms, ls, rs;
  while (1) {
    ms = 0;
    ls = 0;
    rs = 0;
    if (digitalRead(GPIO_L)) {
      rs = 7;
    }
    if (digitalRead(GPIO_ML)) {
      rs = 5; ms = 5;
    }
    if (digitalRead(GPIO_M)) {
      ms = 10;
    }
    if (digitalRead(GPIO_MR)) {
      ls = 5; ms = 5;
    }
    if (digitalRead(GPIO_R)) {
      ls = 7;
    }
    if (sensor("00000")) {
      ls = 10;
      rs = -10;
    }
    motor_drive(fd, ms+ls, ms+rs);
    delay(25);
  }
  return 0;
}

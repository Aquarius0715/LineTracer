#include <stdio.h>

#define GPIO_L 0
#define GPIO_ML 0
#define GPIO_M 0
#define GPIO_MR 0
#define GPIO_R 0

int sensor(char* c) {
  int i = 0;
  const int pin[] = {GPIO_L, GPIO_ML, GPIO_M, GPIO_MR, GPIO_R};
  while (c[i] != '\0') {
    if (i > 4) {
      printf("Out of Argument Error\n");
      exit(-1);
    }
    if ((c[i] - '0') == digitalRead(pin[i]) {
      continue;
    }
    else {
      return 0;
    }
  }
  return 1;
}

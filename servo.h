#ifndef servo_h
#define servo_h

#include <stdio.h>
#include <stdint.h>

void initServo(int pin, float startDegree);
void initDegree(int pin, float degree);

#endif
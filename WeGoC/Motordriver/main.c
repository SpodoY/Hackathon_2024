#include <stdio.h>
#include <lgpio.h>
#include <unistd.h>

#define IN1 21
#define IN2 20
#define IN3 16
#define IN4 12

void step(int handler,int in1, int in2, int in3, int in4) {
    lgGpioWrite(handler,IN1, in1);
    lgGpioWrite(handler,IN2, in2);
    lgGpioWrite(handler,IN3, in3);
    lgGpioWrite(handler,IN4, in4);
    usleep(2000); // Adjust delay as needed for your motor
}

void forward(int handler,int steps, int delay) {
    for (int i = 0; i < steps; i++) {
        step(handler,1, 0, 0, 0);
        step(handler,0, 1, 0, 0);
        step(handler,0, 0, 1, 0);
        step(handler,0, 0, 0, 1);
    }
}

void backward(int handler,int steps, int delay) {
    for (int i = 0; i < steps; i++) {
        step(handler,0, 0, 0, 1);
        step(handler,0, 0, 1, 0);
        step(handler,0, 1, 0, 0);
        step(handler,1, 0, 0, 0);
    }
}

int main() {
    int gpioHandler = lgGpiochipOpen(4);
    lgGpioClaimOutput(gpioHandler,0,IN1,0);
    lgGpioClaimOutput(gpioHandler,0,IN2,0);
    lgGpioClaimOutput(gpioHandler,0,IN3,0);
    lgGpioClaimOutput(gpioHandler,0,IN4,0);


    // Rotate motor forward for 512 steps
    forward(gpioHandler,512, 2000);

    // Rotate motor backward for 512 steps
    backward(gpioHandler,512, 2000);
    return 0;
}
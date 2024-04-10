//
// Created by chronikle on 4/10/24.
//
#include <lgpio.h>
#include <stdio.h>
lgChipInfo_t chipInfo;
int main(){
    int gpioHandler = lgGpiochipOpen(4);
    if (gpioHandler>0){
        printf("We got a handler");
    }else{ printf("Something went wrong, error: %d\n", gpioHandler);}
    lgGpioGetChipInfo(gpioHandler, &chipInfo);
    printf("Label is: %s\nLines are: %d\nName is: %s\n ",chipInfo.label, chipInfo.lines, chipInfo.name);
}
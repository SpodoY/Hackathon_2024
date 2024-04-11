#include <stdio.h>
#include <lgpio.h>
#include <unistd.h>
#include <mosquitto.h>
#include <wiringPi.h>
#include <errno.h>
#include <string.h>


#define LHIN1 21
#define LHIN2 20
#define LVIN1 16
#define LVIN2 12

#define RHIN1 25
#define RHIN2 8
#define RVIN1 7
#define RVIN2 1

#define ECHO1 24
#define TRIG1 23

#define ECHO2 17
#define TRIG2 27

#define ECHO3 5
#define TRIG3 6

int power = 60;
float minDistance = 10.0;
int gpioHandler;
int timeout = 10000;

int lH[4] ={21,20,16,12};
int rH[4] = {25,8,7,1};

struct DistanceData {
    float distance1;
    float distance2;
    float distance3;
};

struct DistanceData distances;

void forward(int handler);
void stop(int handler);
void reverse(int handler);
void curveLeft(int handler);
void curveRight(int handler);
void right(int handler);
void left(int handler);
void circleRight(int handler);


struct mosquitto *mosqittoConnect() {
    mosquitto_lib_init();

    struct mosquitto *mosq = mosquitto_new("publisher-test", true, NULL);
    if (!mosq) {
        printf("Failed by mosquitto_new()!\n");
    }

    // Set the username and password
    if (mosquitto_username_pw_set(mosq, "prod", "Traktor")) {
        printf("Failed by mosquitto_username_pw_set() ! ");
        // Handle error
    }

    // Connect to the MQTT broker
    int rc = mosquitto_connect(mosq, "192.168.1.50", 1883, 60);
    if (rc != 0) {
        printf("Client could not connect to broker! Error Code: %d\n", rc);
        mosquitto_destroy(mosq);
    }
    printf("We are now connected to the broker!\n");

    return mosq;
}
void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    printf("Received message: %s\n", (char*)message->payload);
    const char *payload_str = (const char*)message->payload;
    /*if (distances.distance1 < minDistance || distances.distance2 < minDistance || distances.distance3 < minDistance) {
        stop(gpioHandler);
        printf("TO CLOSE");

    }else {
     */
        switch (payload_str[0]) {
            case ('F'):
                forward(gpioHandler);
                break;
            case ('L'):
                curveRight(gpioHandler);
                break;
            case('R'):
                curveLeft(gpioHandler);
                break;
            case ('V'):
                right(gpioHandler);
                break;
            case ('B'):
                left(gpioHandler);
                break;
            case('N'):
                reverse(gpioHandler);
                break;
        }
    }
//}
void mosquittoReceiveMessage(struct mosquitto *mosq, int handler){
    int mid = 0; // Message ID, not used in this example
    if (mosquitto_subscribe(mosq, &mid, "car/control", 0)) {
        printf("cant subscribe");
    }
    mosquitto_message_callback_set(mosq, message_callback);

    mosquitto_loop_start(mosq);
};




struct DistanceData getDistances(int handler) {
    struct DistanceData distances;
    lgGpioWrite(handler, TRIG1, 1);
    delayMicroseconds(10);
    lgGpioWrite(handler, TRIG1, 0);
    while (lgGpioRead(handler, ECHO1) == 0);
    long startTime1 = micros();
    while (lgGpioRead(handler, ECHO1) == 1);
    long travelTimer1 = micros() - startTime1;
    distances.distance1 = travelTimer1 / 58.0;

    // Sensor 2
    lgGpioWrite(handler, TRIG2, 1);
    delayMicroseconds(10);
    lgGpioWrite(handler, TRIG2, 0);
    while (lgGpioRead(handler, ECHO2) == 0);
    long startTime2 = micros();
    while (lgGpioRead(handler, ECHO2) == 1);
    long travelTimer2 = micros() - startTime2;
    distances.distance2 = travelTimer2 / 58.0;

    // Sensor 3
    lgGpioWrite(handler, TRIG3, 1);
    delayMicroseconds(10);
    lgGpioWrite(handler, TRIG3, 0);
    while (lgGpioRead(handler, ECHO3) == 0);
    long startTime3 = micros();
    while (lgGpioRead(handler, ECHO3) == 1);
    long travelTimer3 = micros() - startTime3;
    distances.distance3 = travelTimer3 / 58.0;

    return distances;
}


void wheelForward(int handler, int wheel1, int wheel2 ){
    lgTxPwm(handler,wheel1,10000,power,1,timeout);
    lgTxPwm(handler,wheel2,10000,0,1,timeout);
}
void wheelBack(int handler, int wheel1, int wheel2 ){
    lgTxPwm(handler,wheel1,10000,0,1,timeout);
    lgTxPwm(handler,wheel2,10000,power,1,timeout);
}
void wheelStop(int handler, int wheel1, int wheel2 ){
    lgGpioWrite(handler,wheel1,0);
    lgGpioWrite(handler,wheel2,0);
}

void stopAfterOneSec(int handler){
    usleep(1000000); // Sleep for 1 second
    stop(handler); // Stop after 1 second
}

void forward(int handler){
    wheelForward(handler, RVIN1, RVIN2);
    wheelForward(handler, RHIN1, RHIN2);
    wheelForward(handler, LVIN1, LVIN2);
    wheelForward(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);
}

void stop(int handler){
    wheelStop(handler, RVIN1, RVIN2);
    wheelStop(handler, RHIN1, RHIN2);
    wheelStop(handler, LVIN1, LVIN2);
    wheelStop(handler, LHIN1, LHIN2);
}

void reverse(int handler){
    wheelBack(handler, RVIN1, RVIN2);
    wheelBack(handler, RHIN1, RHIN2);
    wheelBack(handler, LVIN1, LVIN2);
    wheelBack(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);

}

void curveRight(int handler){
    wheelStop(handler, RVIN1, RVIN2);
    wheelStop(handler, RHIN1, RHIN2);
    wheelForward(handler, LVIN1, LVIN2);
    wheelForward(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);

}

void curveLeft(int handler){
    wheelStop(handler, LVIN1, LVIN2);
    wheelStop(handler, LHIN1, LHIN2);
    wheelForward(handler, RVIN1, RVIN2);
    wheelForward(handler, RHIN1, RHIN2);
    stopAfterOneSec(handler);
}

void right(int handler){
    wheelBack(handler, RVIN1, RVIN2);
    wheelForward(handler, RHIN1, RHIN2);
    wheelForward(handler, LVIN1, LVIN2);
    wheelBack(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);

}

void left(int handler){
    wheelForward(handler, RVIN1, RVIN2);
    wheelBack(handler, RHIN1, RHIN2);
    wheelBack(handler, LVIN1, LVIN2);
    wheelForward(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);

}

void turnLeft(int handler){
    wheelForward(handler, RVIN1, RVIN2);
    wheelStop(handler, RHIN1, RHIN2);
    wheelBack(handler, LVIN1, LVIN2);
    wheelStop(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);
}

void turnRight(int handler){
    wheelBack(handler, RVIN1, RVIN2);
    wheelStop(handler, RHIN1, RHIN2);
    wheelForward(handler, LVIN1, LVIN2);
    wheelStop(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);
}

void circleRight(int handler){
    wheelForward(handler, LVIN1, LVIN2);
    wheelForward(handler, LHIN1, LHIN2);
    wheelBack(handler, RVIN1, RVIN2);
    wheelBack(handler, RHIN1, RHIN2);
    stopAfterOneSec(handler);
}

void circleLeft(int handler){
    wheelForward(handler, RVIN1, RVIN2);
    wheelForward(handler, RHIN1, RHIN2);
    wheelBack(handler, LVIN1, LVIN2);
    wheelBack(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);

}
int main() {
    gpioHandler = lgGpiochipOpen(4);
    if (gpioHandler < 0) {
        printf("Failed to open GPIO chip: %s\n", strerror(errno));
        return 1;
    }
    lgGpioClaimOutput(gpioHandler, 0, LHIN1, 0);
    lgGpioClaimOutput(gpioHandler, 0, LHIN2, 0);
    lgGpioClaimOutput(gpioHandler, 0, RHIN1, 0);
    lgGpioClaimOutput(gpioHandler, 0, RHIN2, 0);
    lgGpioClaimOutput(gpioHandler, 0, LVIN1, 0);
    lgGpioClaimOutput(gpioHandler, 0, LVIN2, 0);
    lgGpioClaimOutput(gpioHandler, 0, RVIN1, 0);
    lgGpioClaimOutput(gpioHandler, 0, RVIN2, 0);
    lgGpioClaimOutput(gpioHandler, 0, TRIG1, 0);
    lgGpioClaimOutput(gpioHandler, 0, TRIG2, 0);
    lgGpioClaimOutput(gpioHandler, 0, TRIG3, 0);
    lgGpioClaimInput(gpioHandler, 0, ECHO1);
    lgGpioClaimInput(gpioHandler, 0, ECHO2);
    lgGpioClaimInput(gpioHandler, 0, ECHO3);

    struct mosquitto *mosq = mosqittoConnect();
    printf("Trying to connect\n");
    mosquittoReceiveMessage(mosq, gpioHandler);

    while (1) {
        // Continuously receive MQTT messages
        mosquitto_loop(mosq, 0, 1);

        // Get distance readings
        distances = getDistances(gpioHandler);

        // Print the distances
        printf("Distance 1: %.2f\n", distances.distance1);
        printf("Distance 2: %.2f\n", distances.distance2);
        printf("Distance 3: %.2f\n", distances.distance3);

        // Process distance readings and perform actions based on them
        if (distances.distance1 < minDistance || distances.distance2 < minDistance || distances.distance3 < minDistance) {
            stop(gpioHandler);
            printf("TOO CLOSE\n");
        } else {
            forward(gpioHandler);
            printf("RUNNING\n");
        }

        // Introduce a delay to control the loop rate
        usleep(100000); // Sleep for 100 milliseconds
    }

    return 0;
}
    

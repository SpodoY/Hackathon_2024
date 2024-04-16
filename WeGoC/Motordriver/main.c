#include <stdio.h>
#include <lgpio.h>
#include <unistd.h>
#include <mosquitto.h>
#include <wiringPi.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>

#define LHIN1 21
#define LHIN2 20
#define LVIN1 16
#define LVIN2 12

#define RHIN1 25
#define RHIN2 8
#define RVIN1 7
#define RVIN2 1

#define ECHO1 17
#define TRIG1 27

#define ECHO2 24
#define TRIG2 23

#define ECHO3 5
#define TRIG3 6

int power = 40;
float minDistance = 10.0;
int gpioHandler;
int howLongMovement = 5000000;
volatile sig_atomic_t flag = 0;

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

void sig_handler(int signum) {
    if (distances.distance1 < minDistance || distances.distance2 < minDistance || distances.distance3 < minDistance) {
        printf("WE STOP NOW\n");
        stop(gpioHandler);
    }
}

struct mosquitto *mosqittoConnect() {
    mosquitto_lib_init();

    struct mosquitto *mosq = mosquitto_new("publisher-test", true, NULL);
    if (!mosq) {
        printf("Failed by mosquitto_new()!\n");
    }

    // Set the username and password
    if (mosquitto_username_pw_set(mosq, "prod", "Traktor")) {
        printf("Failed by mosquitto_username_pw_set() ! ");
        // Handle error-
    }

    // Connect to the MQTT broker
    int rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 60);
    if (rc != 0) {
        printf("Client could not connect to broker! Error Code: %d\n", rc);
        mosquitto_destroy(mosq);
    }
    printf("We are now connected to the broker!\n");

    return mosq;
}

float getDistance(int handler, int TRIG, int ECHO) {
    // Trigger the sensor
    lgGpioWrite(handler, TRIG, 1);
    delayMicroseconds(10);
    lgGpioWrite(handler, TRIG, 0);

    // Wait for the echo signal
    unsigned long startTime = micros();
    unsigned long MAX_TIMEOUT_US = 50000;
    while (lgGpioRead(handler, ECHO) == 0) {
        if ((micros() - startTime) > MAX_TIMEOUT_US) {
            printf("Error: Timeout waiting for echo signal from sensor.\n");
            return -1.0; // Return -1.0 to indicate an error
        }
    }
    unsigned long startEchoTime = micros(); // Record the time when the echo signal starts

    while (lgGpioRead(handler, ECHO) == 1) {
        if ((micros() - startTime) > MAX_TIMEOUT_US) {
            printf("Error: Timeout while receiving echo signal from sensor.\n");
            return -1.0; // Return -1.0 to indicate an error
        }
    }
    unsigned long endEchoTime = micros(); // Record the time when the echo signal ends

    // Calculate travel time of the echo signal
    unsigned long travelTime = endEchoTime - startEchoTime;

    // Check if travel time exceeds maximum timeout
    if (travelTime > MAX_TIMEOUT_US) {
        printf("Error: Travel time exceeds maximum timeout. Possible sensor malfunction or obstruction.\n");
        return -1.0; // Return -1.0 to indicate an error
    }

    // Calculate distance
    float distance = travelTime / 58.0; // Speed of sound is approximately 343 m/s or 58 us/cm
    if (1 < distance < minDistance) raise(SIGUSR1);
    return distance;
}


void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    printf("Received message: %s\n", (char *) message->payload);
    const char *payload_str = (const char *) message->payload;
    switch (payload_str[0]) {
        case ('F'):
            forward(gpioHandler);
            break;
        case ('L'):
            curveRight(gpioHandler);
            break;
        case ('R'):
            curveLeft(gpioHandler);
            break;
        case ('V'):
            right(gpioHandler);
            break;
        case ('B'):
            left(gpioHandler);
            break;
        case ('N'):
            reverse(gpioHandler);
            break;
    }
}

void mosquittoReceiveMessage(struct mosquitto *mosq, int handler) {
    int mid = 0; // Message ID, not used in this example
    if (mosquitto_subscribe(mosq, &mid, "car/control", 0)) {
        printf("cant subscribe");
    }
    mosquitto_message_callback_set(mosq, message_callback);

    mosquitto_loop_start(mosq);
};


void wheelForward(int handler, int wheel1, int wheel2) {
    lgTxPwm(handler, wheel1, 10000, power, 1, 0);
    lgTxPwm(handler, wheel2, 10000, 0, 1, 0);
}

void wheelBack(int handler, int wheel1, int wheel2) {
    lgTxPwm(handler, wheel1, 10000, 0, 1, 0);
    lgTxPwm(handler, wheel2, 10000, power, 1, 0);
}

void wheelStop(int handler, int wheel1, int wheel2) {
    lgTxPwm(handler, wheel1, 10000, 0, 1, 0);
    lgTxPwm(handler, wheel2, 10000, 0, 1, 0);
}

void stopAfterOneSec(int handler) {
    usleep(howLongMovement); // Sleep for 1 second
    stop(handler); // Stop after 1 second
}

void forward(int handler) {
    wheelForward(handler, RVIN1, RVIN2);
    wheelForward(handler, RHIN1, RHIN2);
    wheelForward(handler, LVIN1, LVIN2);
    wheelForward(handler, LHIN1, LHIN2);
    stopAfterOneSec(gpioHandler);

}

void stop(int handler) {
    wheelStop(handler, RVIN1, RVIN2);
    wheelStop(handler, RHIN1, RHIN2);
    wheelStop(handler, LVIN1, LVIN2);
    wheelStop(handler, LHIN1, LHIN2);
}

void reverse(int handler) {
    wheelBack(handler, RVIN1, RVIN2);
    wheelBack(handler, RHIN1, RHIN2);
    wheelBack(handler, LVIN1, LVIN2);
    wheelBack(handler, LHIN1, LHIN2);
    stopAfterOneSec(gpioHandler);
}

void curveRight(int handler) {
    wheelStop(handler, RVIN1, RVIN2);
    wheelStop(handler, RHIN1, RHIN2);
    wheelForward(handler, LVIN1, LVIN2);
    wheelForward(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);

}

void curveLeft(int handler) {
    wheelStop(handler, LVIN1, LVIN2);
    wheelStop(handler, LHIN1, LHIN2);
    wheelForward(handler, RVIN1, RVIN2);
    wheelForward(handler, RHIN1, RHIN2);
    stopAfterOneSec(handler);
}

void right(int handler) {
    wheelBack(handler, RVIN1, RVIN2);
    wheelForward(handler, RHIN1, RHIN2);
    wheelForward(handler, LVIN1, LVIN2);
    wheelBack(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);

}

void left(int handler) {
    wheelForward(handler, RVIN1, RVIN2);
    wheelBack(handler, RHIN1, RHIN2);
    wheelBack(handler, LVIN1, LVIN2);
    wheelForward(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);

}

void turnLeft(int handler) {
    wheelForward(handler, RVIN1, RVIN2);
    wheelStop(handler, RHIN1, RHIN2);
    wheelBack(handler, LVIN1, LVIN2);
    wheelStop(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);
}

void turnRight(int handler) {
    wheelBack(handler, RVIN1, RVIN2);
    wheelStop(handler, RHIN1, RHIN2);
    wheelForward(handler, LVIN1, LVIN2);
    wheelStop(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);
}

void circleRight(int handler) {
    wheelForward(handler, LVIN1, LVIN2);
    wheelForward(handler, LHIN1, LHIN2);
    wheelBack(handler, RVIN1, RVIN2);
    wheelBack(handler, RHIN1, RHIN2);
    stopAfterOneSec(handler);
}

void circleLeft(int handler) {
    wheelForward(handler, RVIN1, RVIN2);
    wheelForward(handler, RHIN1, RHIN2);
    wheelBack(handler, LVIN1, LVIN2);
    wheelBack(handler, LHIN1, LHIN2);
    stopAfterOneSec(handler);

}

void *mqttThread(void *arg) {
    struct mosquitto *mosq = (struct mosquitto *) arg;
    mosquittoReceiveMessage(mosq, gpioHandler);
    return NULL;
}

void *distanceThread1(void *arg) {
    while (1) {
        float distance = getDistance(gpioHandler, TRIG1, ECHO1);
        distances.distance1 = distance;
        usleep(100000);
    }
    return NULL;
}

// Function to continuously read distance for sensor 2
void *distanceThread2(void *arg) {
    while (1) {
        float distance = getDistance(gpioHandler, TRIG2, ECHO2);
        distances.distance2 = distance;
        usleep(100000);
    }
    return NULL;
}

// Function to continuously read distance for sensor 3
void *distanceThread3(void *arg) {
    while (1) {
        float distance = getDistance(gpioHandler, TRIG3, ECHO3);
        distances.distance3 = distance;
        usleep(100000);
    }
    return NULL;
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
    printf("Claimed all GPIOs\n");
    struct mosquitto *mosq = mosqittoConnect();
    printf("Trying to connect\n");
    pthread_t mqtt_tid;
    pthread_create(&mqtt_tid, NULL, mqttThread, mosq);
    pthread_join(mqtt_tid, NULL);
    printf("Connected\n");

    pthread_t thread1, thread2, thread3; // Declare thread variables

    // Create threads for distance measurements
    pthread_create(&thread1, NULL, distanceThread1, NULL);
    pthread_create(&thread2, NULL, distanceThread2, NULL);
    pthread_create(&thread3, NULL, distanceThread3, NULL);
    signal(SIGUSR1, sig_handler);

    while (1) {
        printf("Distance 1: %f\n", distances.distance1);
        printf("Distance 2: %f\n", distances.distance2);
        printf("Distance 3: %f\n", distances.distance3);
        usleep(100000); // Sleep for 100 milliseconds
    }

    return 0;
}

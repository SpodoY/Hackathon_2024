import random
from Model.SensorResponse import SensorResponse
import paho.mqtt.client as mqtt
import json

broker = "192.168.1.50"
port = 1883
topic = "tele/Sensor1_C11CE5/SENSOR"
client_id = f'python-mqtt-{random.randint(0, 1000)}'
username = 'prod'
password = 'Traktor'
mqttc = mqtt.Client()

lastMessagePayload: SensorResponse


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc, properties=None):
    print(f"Connected with result code {rc}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global lastMessagePayload
    global mqttc

    # Decodes the response into a 'SensorResponse' Model
    response = json.loads(msg.payload.decode())
    sensor_response = SensorResponse()
    lastMessagePayload = sensor_response.parseResponse(response)

    print(sensor_response)
    if lastMessagePayload is not None: publish(mqttc, lastMessagePayload)
    # print(f"{msg.topic} {msg.payload.decode()}")


def on_disconnect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("Disconnected")
    if reason_code > 0:
        print(f"Disconnected with reason {reason_code}")


def subscribe(client: mqtt):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")


def publish(client, data: SensorResponse):
    COMMAND_TOPIC = 'cmnd/car'
    if data.veml7700.illuminance < 30:
        print("It's dark here")
        result = client.publish(f"{COMMAND_TOPIC}/dark", "It's dark :(")


# Set usr + pwd
mqttc.username_pw_set(username=username, password=password)

# Assign on_connect & on_message callbacks
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.on_disconnect = on_disconnect

mqttc.connect(broker, port)

mqttc.subscribe(topic)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
mqttc.loop_forever()

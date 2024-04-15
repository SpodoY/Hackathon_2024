import random

from Model.LightLevels import LightLevel
from Model.PlantSettings import PlantSettings
from Model.SensorResponse import SensorResponse
from Model.MoistureLevels import MoistureLevel
import paho.mqtt.client as mqtt
import json

broker = "192.168.178.172"
port = 1883

# MQTT Topics
sensor_topic = "tele/Sensor1_C11CE5/SENSOR"
plant_settings_topic = "plant/settings"
car_response_topic = "car/response"
car_env_topic = "car/envEval"

client_id = f'python-mqtt-{random.randint(0, 1000)}'
username = 'prod'
password = 'Traktor'
mqttc = mqtt.Client()

lastMessagePayload: SensorResponse
plantSettings: PlantSettings


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc, properties=None):
    print(f"Connected with result code {rc}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.


# The callback for when a PUBLISH message is received from the server.
def decodePlantSettings(settings: str):
    global plantSettings
    newPlantSettings = json.loads(settings)
    plantSettings.decode(newPlantSettings)
    print(f"Plant settings: {newPlantSettings}")


def on_message(client, userdata, msg):
    global lastMessagePayload
    global plantSettings
    global mqttc

    if msg.topic == plant_settings_topic:
        print(msg.payload.decode())
        decodePlantSettings(msg.payload.decode("utf-8"))
        return

    if msg.topic == car_response_topic:
        print(f"Car response: {msg.payload.decode('utf-8')}")
        return



    # Decodes the response into a 'SensorResponse' Model
    response = json.loads(msg.payload.decode())
    sensor_response = SensorResponse()
    lastMessagePayload = sensor_response.parseResponse(response)

    if plantSettings is not None:
        plantConditions = calculatePlantCondition(lastMessagePayload)
        client.publish(topic=car_env_topic, payload=json.dumps(plantConditions))
    else:
        print("No plant settings yet")

    print(sensor_response)
    if lastMessagePayload is not None: publish(mqttc, lastMessagePayload)


def on_disconnect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("Disconnected")
    if reason_code > 0:
        print(f"Disconnected with reason {reason_code}")


def subscribe(client: mqtt):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.sensor_topic}` topic")


def publish(client, data: SensorResponse):
    COMMAND_TOPIC = 'cmnd/car'
    if data.veml7700.illuminance < 30:
        print("It's dark here")
        result = client.publish(f"{COMMAND_TOPIC}/dark", "It's dark :(")


def calculatePlantCondition(sensorData: SensorResponse):
    global plantSettings
    lightLevels = [e.value for e in LightLevel]
    moistLevels = [e.value for e in MoistureLevel]

    currentPlantConditions: dict = {}
    optiLiLvl = plantSettings.optimalLightLevel.value
    curLiLvl = sensorData.veml7700.illuminance

    optiMoist = plantSettings.optimalMoisture.value
    curMoist = sensorData.moistureSensor.moisture

    optiTemp = plantSettings.optimalTemp
    curTemp = sensorData.am2301.temperature

    print(f"Optimal values Light: {optiLiLvl}, Moist: {optiMoist}, Temp: {optiTemp}")

    # Calculate light action
    try:
        if optiLiLvl <= curLiLvl <= lightLevels[lightLevels.index(optiLiLvl) + 1]:
            currentPlantConditions["lightAction"] = "gucci"
        elif curLiLvl >= lightLevels[lightLevels.index(optiLiLvl) + 1]:
            currentPlantConditions["lightAction"] = "less"
        else:
            currentPlantConditions["lightAction"] = "more"
    except:
        currentPlantConditions["lightAction"] = "dead"
        print("No light sensor data available")

    # Calculate moisture needs
    try:
        if optiMoist <= curMoist <= moistLevels[moistLevels.index(optiMoist) + 1]:
            currentPlantConditions["moistureAction"] = "gucci"
        elif curMoist >= moistLevels[moistLevels.index(optiMoist) + 1]:
            currentPlantConditions["moistureAction"] = "less"
        else:
            currentPlantConditions["moistureAction"] = "more"
    except:
        currentPlantConditions["moistureAction"] = "dead"
        print("No moisture sensor data available")

    # Calculate temp needs
    try:
        currentPlantConditions["tempAction"] = "gucci" if optiTemp < curTemp else "more"
        print(f"Calculated plant condition - {currentPlantConditions}")
    except:
        currentPlantConditions["tempAction"] = "dead"
        print("No temperature sensor data available")
        
    return currentPlantConditions


# Set usr + pwd
mqttc.username_pw_set(username=username, password=password)

plantSettings = PlantSettings()

# Assign on_connect & on_message callbacks
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.on_disconnect = on_disconnect

mqttc.connect(broker, port)

mqttc.subscribe(sensor_topic)
mqttc.subscribe(plant_settings_topic)
mqttc.subscribe(car_response_topic)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
mqttc.loop_forever()

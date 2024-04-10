class AM2301:
    temperature: float
    humidity: float
    dewpoint: float

    def __init__(self):
        self.temperature = 0
        self.humidity = 0
        self.dewpoint = 0


class VEML7700:
    illuminance: float
    whiteContent: float

    def __init__(self):
        self.illuminance = 0
        self.whiteContent = 0


class MoistureSensor:
    moisture: int

    def __init__(self):
        self.moisture = 0


class SensorResponse:
    am2301: AM2301
    veml7700: VEML7700
    moistureSensor: MoistureSensor

    def __init__(self):
        self.am2301 = AM2301()
        self.veml7700 = VEML7700()
        self.moistureSensor = MoistureSensor()

    def parseResponse(self, msg):
        # Moisture Sensor
        self.moistureSensor.moisture = msg['ANALOG']['A0']

        # Humidity Sensor
        self.am2301.temperature = msg['AM2301']['Temperature']
        self.am2301.humidity = msg['AM2301']['Humidity']
        self.am2301.dewpoint = msg['AM2301']['DewPoint']

        # Light Sensor
        self.veml7700.illuminance = msg['VEML7700']['Illuminance']
        self.veml7700.whiteContent = msg['VEML7700']['WhiteContent']
        return self

    def __str__(self):
        return (f"Moisture: {self.moistureSensor.moisture}\r\n"
                f"Humidity: Temp: {self.am2301.temperature}, Humi: {self.am2301.humidity}, DewP: {self.am2301.dewpoint}\r\n"
                f"Light: Illu: {self.veml7700.illuminance}, WhiteCon: {self.veml7700.whiteContent}")

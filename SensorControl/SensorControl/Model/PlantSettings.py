from Model import LightLevels
from Model.LightLevels import LightLevel
from Model.MoistureLevels import MoistureLevel


class PlantSettings:
    optimalTemp: float
    optimalLightLevel: LightLevels
    optimalMoisture: MoistureLevel

    def __init__(self):
        self.optimalTemp = 20
        self.optimalLightLevel = LightLevel.LOW
        self.optimalMoisture = MoistureLevel.LOW

    def decode(self, newPlantSettingsString):
        self.optimalTemp = int(newPlantSettingsString['temp'])
        self.optimalLightLevel = LightLevel[newPlantSettingsString['light']]
        self.optimalMoisture = MoistureLevel[newPlantSettingsString['moisture']]

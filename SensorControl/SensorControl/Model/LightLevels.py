from enum import Enum


class LightLevel(Enum):
    TO_LOW = 0
    LOW = 500
    MEDIUM = 2500
    BRIGHT = 10000
    VERY_BRIGHT = 20000
    TOO_BRIGHT = 50000

    @classmethod
    def _missing_(cls, value):
        return cls.LOW



from enum import Enum


class MoistureLevel(Enum):
    TOO_LOW = 50
    LOW = 100
    MEDIUM = 200
    HIGH = 400
    TOO_HIGH = 600

    @classmethod
    def _missing_(cls, value):
        return cls.LOW




from dataclasses import dataclass, field

# Fictional sensor: educational simulation only.
DEVICE_ADDR = 0x1D
WHO_AM_I = 0x0F
TEMP_OUT_L = 0x2A
TEMP_OUT_H = 0x2B

EXPECTED_ID = 0xBC
TEMP_SCALE_C_PER_COUNT = 0.1


class I2CStatus:
    OK = "OK"
    NACK = "NACK"
    TIMEOUT = "TIMEOUT"
    INVALID_DEVICE = "INVALID_DEVICE"
    BUS_FAILURE = "BUS_FAILURE"
    INVALID_REGISTER = "INVALID_REGISTER"


@dataclass
class Sensor:
    registers: dict = field(default_factory=lambda: {
        WHO_AM_I: EXPECTED_ID,
        TEMP_OUT_L: 0xF6,  # Low byte of 246
        TEMP_OUT_H: 0x00,  # High byte of 246
    })


class SimulatedI2CBus:
    def __init__(self):
        self.devices = {}
        self.next_error = None

    def attach(self, address, sensor):
        self.devices[address] = sensor

    def inject_error(self, error):
        """Inject one error for the next transaction."""
        self.next_error = error

    def consume_error(self):
        error = self.next_error
        self.next_error = None
        return error


class SensorDriver:
    def __init__(self, bus, address):
        self.bus = bus
        self.address = address

    def read_registers(self, start_reg, length):
        """Read consecutive registers; return (status, bytes)."""
        if length <= 0:
            return I2CStatus.INVALID_REGISTER, None

        error = self.bus.consume_error()
        if error is not None:
            print(f"Transaction failed: {error}")
            return error, None

        sensor = self.bus.devices.get(self.address)
        if sensor is None:
            return I2CStatus.NACK, None

        values = []
        for offset in range(length):
            reg = start_reg + offset
            if reg not in sensor.registers:
                return I2CStatus.INVALID_REGISTER, None
            values.append(sensor.registers[reg])

        print(
            f"I2C burst read: address=0x{self.address:02X}, "
            f"start=0x{start_reg:02X}, length={length}"
        )
        print("START -> ADDRESS+WRITE -> REGISTER -> REPEATED START")
        print("ADDRESS+READ -> DATA bytes -> NACK(final) -> STOP")
        print("Received:", " ".join(f"0x{x:02X}" for x in values))

        return I2CStatus.OK, bytes(values)

    def initialize(self):
        status, data = self.read_registers(WHO_AM_I, 1)

        if status != I2CStatus.OK:
            print("Initialization failed:", status)
            return status

        if data[0] != EXPECTED_ID:
            print(f"Unexpected ID: 0x{data[0]:02X}")
            return I2CStatus.INVALID_DEVICE

        print("Sensor identity verified.")
        return I2CStatus.OK

    def read_temperature_c(self):
        status, data = self.read_registers(TEMP_OUT_L, 2)

        # Never convert data after a failed transaction.
        if status != I2CStatus.OK:
            return status, None

        low, high = data

        # Little-endian assembly, then signed 16-bit interpretation.
        raw_unsigned = (high << 8) | low
        raw_signed = (
            raw_unsigned - 0x10000
            if raw_unsigned & 0x8000
            else raw_unsigned
        )

        temperature_c = raw_signed * TEMP_SCALE_C_PER_COUNT
        return I2CStatus.OK, temperature_c


def main():
    bus = SimulatedI2CBus()
    sensor = Sensor()
    bus.attach(DEVICE_ADDR, sensor)
    driver = SensorDriver(bus, DEVICE_ADDR)

    print("=== SENSOR INITIALIZATION ===")
    status = driver.initialize()
    if status != I2CStatus.OK:
        return

    print("\n=== I2C-04 / I2C-06: TEMPERATURE READ ===")
    status, temp = driver.read_temperature_c()
    if status == I2CStatus.OK:
        print(f"Temperature = {temp:.1f} deg C")
    else:
        print(f"Temperature unavailable: {status}")

    print("\n=== I2C-05: ERROR HANDLING TESTS ===")
    for error in [
        I2CStatus.NACK,
        I2CStatus.TIMEOUT,
        I2CStatus.BUS_FAILURE,
    ]:
        bus.inject_error(error)
        status, temp = driver.read_temperature_c()
        print(f"Result={status}; measurement={temp}")

    print("\n=== SIGNED CONVERSION TEST ===")
    sensor.registers[TEMP_OUT_L] = 0x9C
    sensor.registers[TEMP_OUT_H] = 0xFF
    status, temp = driver.read_temperature_c()
    if status == I2CStatus.OK:
        print(f"Raw 0xFF9C -> {temp:.1f} deg C")


if __name__ == "__main__":
    main()

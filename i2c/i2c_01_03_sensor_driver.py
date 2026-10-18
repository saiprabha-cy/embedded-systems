
from dataclasses import dataclass, field

# ------------------------------------------------------------
# I2C-01: Software transaction model
# I2C-02: Generic register access
# I2C-03: Sensor initialization
# ------------------------------------------------------------

DEVICE_ADDR = 0x1D

WHO_AM_I = 0x0F
CTRL1 = 0x20
CTRL2 = 0x21
STATUS = 0x27

EXPECTED_ID = 0xBC

OK = "OK"
NACK = "NACK"
INVALID_DEVICE = "INVALID_DEVICE"
BUS_ERROR = "BUS_ERROR"


@dataclass
class SensorModel:
    """Fictional sensor: a software model, not real hardware."""
    registers: dict = field(default_factory=lambda: {
        WHO_AM_I: EXPECTED_ID,
        CTRL1: 0x00,
        CTRL2: 0x00,
        STATUS: 0x01,
    })
    pointer: int = 0


class I2CBus:
    def __init__(self):
        self.devices = {}
        self.events = []

    def attach(self, address, device):
        self.devices[address] = device

    def log(self, event):
        self.events.append(event)
        print("  " + event)

    # I2C-01: Model transaction primitives
    def start(self):
        self.log("START")

    def repeated_start(self):
        self.log("REPEATED START")

    def stop(self):
        self.log("STOP")

    def address_device(self, address, read=False):
        direction = "READ" if read else "WRITE"
        self.log(f"ADDRESS 0x{address:02X} + {direction}")

        device = self.devices.get(address)
        if device is None:
            self.log("NACK: no device at this address")
            return None

        self.log("ACK")
        return device

    def write_byte(self, device, value):
        self.log(f"WRITE 0x{value:02X}")
        self.log("ACK")
        return OK

    def read_byte(self, device, value, ack):
        response = "ACK" if ack else "NACK (final byte)"
        self.log(f"READ 0x{value:02X} -> {response}")
        return value


# I2C-02: Generic register driver
class SensorDriver:
    def __init__(self, bus, address):
        self.bus = bus
        self.address = address

    def write_register(self, reg, value):
        device = None
        self.bus.start()
        device = self.bus.address_device(self.address, read=False)

        if device is None:
            self.bus.stop()
            return NACK

        self.bus.write_byte(device, reg)
        self.bus.write_byte(device, value)

        # Apply the write to our simulated register map.
        if reg in device.registers:
            device.registers[reg] = value
        else:
            self.bus.log(f"Invalid register 0x{reg:02X}")
            self.bus.stop()
            return BUS_ERROR

        self.bus.stop()
        return OK

    def read_register(self, reg):
        self.bus.start()
        device = self.bus.address_device(self.address, read=False)

        if device is None:
            self.bus.stop()
            return NACK, None

        self.bus.write_byte(device, reg)

        if reg not in device.registers:
            self.bus.log(f"Invalid register 0x{reg:02X}")
            self.bus.stop()
            return BUS_ERROR, None

        device.pointer = reg

        # Real register reads commonly use a repeated START.
        self.bus.repeated_start()
        device = self.bus.address_device(self.address, read=True)

        if device is None:
            self.bus.stop()
            return NACK, None

        value = device.registers[device.pointer]
        self.bus.read_byte(device, value, ack=False)
        self.bus.stop()
        return OK, value

    # I2C-03: Sensor initialization
    def initialize(self):
        print("\nChecking sensor identity...")
        status, device_id = self.read_register(WHO_AM_I)

        if status != OK:
            print("Initialization failed: cannot read device ID.")
            return status

        print(f"WHO_AM_I = 0x{device_id:02X}")

        if device_id != EXPECTED_ID:
            print("Initialization failed: unexpected device ID.")
            return INVALID_DEVICE

        # Register/value pairs: configuration table.
        configuration = [
            (CTRL1, 0x57),
            (CTRL2, 0x10),
        ]

        print("\nWriting configuration table...")
        for reg, value in configuration:
            print(f"\nConfigure register 0x{reg:02X}")
            status = self.write_register(reg, value)
            if status != OK:
                print("Initialization failed during configuration.")
                return status

        print("\nVerifying configuration...")
        for reg, expected in configuration:
            status, actual = self.read_register(reg)

            if status != OK or actual != expected:
                print(f"Readback failed at register 0x{reg:02X}")
                return BUS_ERROR

            print(f"Register 0x{reg:02X}: 0x{actual:02X} verified")

        print("\nSensor initialization successful.")
        return OK


def main():
    bus = I2CBus()
    simulated_sensor = SensorModel()
    bus.attach(DEVICE_ADDR, simulated_sensor)

    driver = SensorDriver(bus, DEVICE_ADDR)

    print("=== I2C-01/02/03 SOFTWARE SIMULATION ===")
    result = driver.initialize()

    print(f"\nFinal result: {result}")
    print("\nFinal simulated register values:")
    for reg in [WHO_AM_I, CTRL1, CTRL2, STATUS]:
        print(f"  0x{reg:02X} = 0x{simulated_sensor.registers[reg]:02X}")


if __name__ == "__main__":
    main()

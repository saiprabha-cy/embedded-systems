
from dataclasses import dataclass, field

# SPI-01 through SPI-05
# Fictional register-based sensor; no hardware required.

DEVICE_ID = 0xBC
WHO_AM_I = 0x0F
CTRL1 = 0x20
DATA_L = 0x2A
DATA_H = 0x2B

READ_BIT = 0x00
WRITE_BIT = 0x80
MAX_CLOCK_HZ = 5_000_000  # Fictional sensor limit for this exercise


class SPIStatus:
    OK = "OK"
    TIMEOUT = "TIMEOUT"
    BUS_ERROR = "BUS_ERROR"
    INVALID_ARGUMENT = "INVALID_ARGUMENT"
    INVALID_DEVICE = "INVALID_DEVICE"
    INVALID_REGISTER = "INVALID_REGISTER"


@dataclass
class SPISensor:
    registers: dict = field(default_factory=lambda: {
        WHO_AM_I: DEVICE_ID,
        CTRL1: 0x00,
        DATA_L: 0xF6,
        DATA_H: 0x00,
        0x2C: 0x01,
    })


class SPIMaster:
    def __init__(self, mode=0, clock_hz=1_000_000):
        self.devices = {}
        self.mode = mode
        self.clock_hz = clock_hz
        self.cs_active = False
        self.next_error = None
        self.total_clock_cycles = 0

    def attach(self, sensor):
        self.devices[0] = sensor  # One simulated CS line.

    def configure(self, mode, clock_hz):
        if mode not in (0, 1, 2, 3) or clock_hz <= 0:
            return SPIStatus.INVALID_ARGUMENT

        if clock_hz > MAX_CLOCK_HZ:
            print("Clock exceeds the fictional sensor's limit.")
            return SPIStatus.INVALID_ARGUMENT

        self.mode = mode
        self.clock_hz = clock_hz
        cpol = mode // 2
        cpha = mode % 2
        print(
            f"Configured SPI mode {mode}: CPOL={cpol}, "
            f"CPHA={cpha}, SCLK={clock_hz / 1e6:.2f} MHz"
        )
        return SPIStatus.OK

    def inject_error(self, error):
        self.next_error = error

    def select(self):
        if self.cs_active:
            return SPIStatus.BUS_ERROR

        self.cs_active = True
        print("CS LOW: peripheral selected")
        return SPIStatus.OK

    def deselect(self):
        self.cs_active = False
        print("CS HIGH: peripheral deselected")

    def transfer(self, tx_bytes, timeout_ms=100):
        if not self.cs_active or not tx_bytes:
            return SPIStatus.INVALID_ARGUMENT, None

        if timeout_ms <= 0:
            return SPIStatus.INVALID_ARGUMENT, None

        if self.next_error is not None:
            error = self.next_error
            self.next_error = None
            print(f"Injected communication error: {error}")
            return error, None

        rx_bytes = []
        for tx in tx_bytes:
            if not 0 <= tx <= 0xFF:
                return SPIStatus.INVALID_ARGUMENT, None

            self.total_clock_cycles += 8
            rx_bytes.append(0x00)  # Replaced by device behavior below.

        ideal_time_s = (len(tx_bytes) * 8) / self.clock_hz
        if ideal_time_s > timeout_ms / 1000:
            return SPIStatus.TIMEOUT, None

        print(f"MOSI TX: {[f'0x{x:02X}' for x in tx_bytes]}")
        print(f"Clock cycles: {len(tx_bytes) * 8}")
        print(f"Ideal clock time: {ideal_time_s * 1e6:.2f} us")

        return SPIStatus.OK, rx_bytes


class SPISensorDriver:
    def __init__(self, master):
        self.master = master
        self.sensor = master.devices[0]

    def read_registers(self, start_reg, length, timeout_ms=100):
        if length <= 0 or start_reg + length > 256:
            return SPIStatus.INVALID_ARGUMENT, None

        # This fictional protocol uses bit 7 = write, 0 = read.
        command = start_reg | READ_BIT
        tx = [command] + [0x00] * length

        status = self.master.select()
        if status != SPIStatus.OK:
            return status, None

        try:
            status, _ = self.master.transfer(tx, timeout_ms)
            if status != SPIStatus.OK:
                return status, None

            # Simulate the peripheral's response during the dummy bytes.
            if any(reg not in self.sensor.registers
                   for reg in range(start_reg, start_reg + length)):
                return SPIStatus.INVALID_REGISTER, None

            data = [
                self.sensor.registers[reg]
                for reg in range(start_reg, start_reg + length)
            ]
            print("MISO RX:", [f"0x{x:02X}" for x in [0x00] + data])
            return SPIStatus.OK, data
        finally:
            # This fictional device ends a transaction on CS HIGH.
            self.master.deselect()

    def write_register(self, reg, value, timeout_ms=100):
        if not 0 <= reg <= 0x7F or not 0 <= value <= 0xFF:
            return SPIStatus.INVALID_ARGUMENT

        tx = [reg | WRITE_BIT, value]

        status = self.master.select()
        if status != SPIStatus.OK:
            return status

        try:
            status, _ = self.master.transfer(tx, timeout_ms)
            if status != SPIStatus.OK:
                return status

            if reg not in self.sensor.registers:
                return SPIStatus.INVALID_REGISTER

            self.sensor.registers[reg] = value
            return SPIStatus.OK
        finally:
            self.master.deselect()

    def initialize(self):
        status, data = self.read_registers(WHO_AM_I, 1)
        if status != SPIStatus.OK:
            return status

        if data[0] != DEVICE_ID:
            return SPIStatus.INVALID_DEVICE

        print(f"Device ID verified: 0x{data[0]:02X}")
        return self.write_register(CTRL1, 0x57)


def main():
    sensor = SPISensor()
    master = SPIMaster()
    master.attach(sensor)
    driver = SPISensorDriver(master)

    print("=== SPI-01/05 SENSOR DRIVER SIMULATION ===")

    print("\n1. Configure SPI")
    print("Configuration status:",
          master.configure(mode=0, clock_hz=1_000_000))

    print("\n2. Initialize sensor")
    print("Initialization status:", driver.initialize())

    print("\n3. Multi-byte register read")
    status, data = driver.read_registers(DATA_L, 3)
    if status == SPIStatus.OK:
        low, high, status_byte = data
        raw = (high << 8) | low
        signed_raw = raw - 65536 if raw & 0x8000 else raw
        print(f"Data bytes: {data}")
        print(f"Little-endian raw value: {signed_raw}")
        print(f"Status register: 0x{status_byte:02X}")
    else:
        print("Read failed:", status)

    print("\n4. Compare all four SPI modes")
    for mode in range(4):
        print(f"\nMode {mode}")
        master.configure(mode, 1_000_000)
        status, data = driver.read_registers(WHO_AM_I, 1)
        print("Read status:", status, "data:", data)

    print("\n5. Test timeout and bus errors")
    for error in [SPIStatus.TIMEOUT, SPIStatus.BUS_ERROR]:
        master.inject_error(error)
        status, data = driver.read_registers(WHO_AM_I, 1)
        print(f"Injected {error} -> returned {status}, data={data}")

    print("\n6. Test invalid clock")
    print("Configuration status:",
          master.configure(mode=0, clock_hz=10_000_000))

    print("\nTotal simulated clock cycles:", master.total_clock_cycles)


if __name__ == "__main__":
    main()

from enum import Enum


class ParserState(Enum):
    WAIT_SYNC = 0
    READ_TYPE = 1
    READ_LENGTH = 2
    READ_PAYLOAD = 3
    READ_CRC = 4


class PacketParser:
    def __init__(self, timeout_ms):
        self.state = ParserState.WAIT_SYNC
        self.timeout_ms = timeout_ms

        self.last_byte_time = None

        self.packet_type = None
        self.payload_length = 0
        self.payload = []

    def reset(self):
        print("PARSER RESET -> Timeout")

        self.state = ParserState.WAIT_SYNC
        self.last_byte_time = None

        self.packet_type = None
        self.payload_length = 0
        self.payload = []

    def receive_byte(self, byte, current_time):
        # Check timeout before processing new byte
        if (
            self.last_byte_time is not None
            and current_time - self.last_byte_time > self.timeout_ms
        ):
            self.reset()

        print(
            f"{current_time:4} ms -> "
            f"RX: {byte:02X} | State: {self.state.name}"
        )

        self.last_byte_time = current_time

        if self.state == ParserState.WAIT_SYNC:

            if byte == 0xAA:
                self.state = ParserState.READ_TYPE

        elif self.state == ParserState.READ_TYPE:

            self.packet_type = byte
            self.state = ParserState.READ_LENGTH

        elif self.state == ParserState.READ_LENGTH:

            self.payload_length = byte
            self.payload = []

            if self.payload_length == 0:
                self.state = ParserState.READ_CRC
            else:
                self.state = ParserState.READ_PAYLOAD

        elif self.state == ParserState.READ_PAYLOAD:

            self.payload.append(byte)

            if len(self.payload) >= self.payload_length:
                self.state = ParserState.READ_CRC

        elif self.state == ParserState.READ_CRC:

            print("PACKET COMPLETE")

            self.state = ParserState.WAIT_SYNC


def main():

    parser = PacketParser(timeout_ms=100)

    # A packet starts arriving...
    packet = [
        (0,  0xAA),   # SYNC
        (20, 0x01),   # TYPE
        (40, 0x04),   # LENGTH
        (60, 0x12),   # PAYLOAD
        (80, 0x34),   # PAYLOAD
    ]

    for current_time, byte in packet:
        parser.receive_byte(byte, current_time)

    # Large gap occurs here.
    #
    # Next byte arrives after 250 ms.
    parser.receive_byte(0x56, 250)


if __name__ == "__main__":
    main()
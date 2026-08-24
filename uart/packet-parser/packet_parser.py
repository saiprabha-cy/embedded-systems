from enum import Enum


class ParserState(Enum):
    WAIT_SYNC = 0
    READ_TYPE = 1
    READ_LENGTH = 2
    READ_PAYLOAD = 3
    READ_CHECKSUM = 4


class PacketParser:

    def __init__(self):
        self.state = ParserState.WAIT_SYNC

        self.packet_type = None
        self.length = 0
        self.payload = []

    def reset(self):
        self.state = ParserState.WAIT_SYNC

        self.packet_type = None
        self.length = 0
        self.payload = []

    def process_byte(self, byte):

        if self.state == ParserState.WAIT_SYNC:

            if byte == 0xAA:
                self.state = ParserState.READ_TYPE

        elif self.state == ParserState.READ_TYPE:

            self.packet_type = byte
            self.state = ParserState.READ_LENGTH

        elif self.state == ParserState.READ_LENGTH:

            self.length = byte
            self.payload = []

            if self.length == 0:
                self.state = ParserState.READ_CHECKSUM
            else:
                self.state = ParserState.READ_PAYLOAD

        elif self.state == ParserState.READ_PAYLOAD:

            self.payload.append(byte)

            if len(self.payload) >= self.length:
                self.state = ParserState.READ_CHECKSUM

        elif self.state == ParserState.READ_CHECKSUM:

            print("PACKET RECEIVED")
            print(f"Type    : 0x{self.packet_type:02X}")
            print(f"Length  : {self.length}")
            print(f"Payload : {self.payload}")
            print(f"Checksum: 0x{byte:02X}")
            print()

            self.reset()


parser = PacketParser()


packet = [
    0xAA,
    0x01,
    0x03,
    0x10,
    0x20,
    0x30,
    0x5B
]


for byte in packet:
    parser.process_byte(byte)
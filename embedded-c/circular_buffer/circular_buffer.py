class CircularBuffer:

    def __init__(self, size):
        self.buffer = [None] * size
        self.size = size

        self.write_index = 0
        self.read_index = 0

        self.count = 0

    def is_empty(self):
        return self.count == 0

    def is_full(self):
        return self.count == self.size

    def write(self, data):

        if self.is_full():
            print("Buffer FULL")
            return False

        self.buffer[self.write_index] = data

        self.write_index = (
            (self.write_index + 1) % self.size
        )

        self.count += 1

        return True

    def read(self):

        if self.is_empty():
            print("Buffer EMPTY")
            return None

        data = self.buffer[self.read_index]

        self.read_index = (
            (self.read_index + 1) % self.size
        )

        self.count -= 1

        return data


buffer = CircularBuffer(4)

buffer.write("A")
buffer.write("B")
buffer.write("C")

print("Read:", buffer.read())
print("Read:", buffer.read())

buffer.write("D")
buffer.write("E")

print("Read:", buffer.read())
print("Read:", buffer.read())
print("Read:", buffer.read())
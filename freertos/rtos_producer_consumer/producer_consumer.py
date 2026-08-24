from collections import deque


class Queue:
    def __init__(self, size):
        self.buffer = deque(maxlen=size)

    def send(self, data):
        if len(self.buffer) >= self.buffer.maxlen:
            print(f"QUEUE FULL -> Cannot add {data}")
            return False

        self.buffer.append(data)
        print(f"QUEUE <- {data}")
        return True

    def receive(self):
        if not self.buffer:
            print("QUEUE EMPTY")
            return None

        data = self.buffer.popleft()
        print(f"QUEUE -> {data}")
        return data


class Producer:
    def __init__(self, queue):
        self.queue = queue
        self.value = 0

    def produce(self):
        self.value += 1
        print(f"Producer generated: {self.value}")
        self.queue.send(self.value)


class Consumer:
    def __init__(self, queue):
        self.queue = queue

    def consume(self):
        data = self.queue.receive()

        if data is not None:
            print(f"Consumer processed: {data}")


queue = Queue(size=3)

producer = Producer(queue)
consumer = Consumer(queue)


for cycle in range(5):

    print(f"\n--- Cycle {cycle} ---")

    producer.produce()
    consumer.consume()
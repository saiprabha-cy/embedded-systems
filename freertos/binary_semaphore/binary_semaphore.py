class BinarySemaphore:
    def __init__(self):
        self.available = False

    def give(self):
        if self.available:
            print("Semaphore already available")
            return

        self.available = True
        print("Semaphore GIVEN")

    def take(self):
        if not self.available:
            print("Semaphore NOT available")
            return False

        self.available = False
        print("Semaphore TAKEN")
        return True


class EventProducer:
    def __init__(self, semaphore):
        self.semaphore = semaphore

    def generate_event(self):
        print("Producer: EVENT OCCURRED")
        self.semaphore.give()


class EventConsumer:
    def __init__(self, semaphore):
        self.semaphore = semaphore

    def wait_for_event(self):
        if self.semaphore.take():
            print("Consumer: Event handled")
        else:
            print("Consumer: Waiting...")


semaphore = BinarySemaphore()

producer = EventProducer(semaphore)
consumer = EventConsumer(semaphore)


print("\n--- Initial state ---")
consumer.wait_for_event()

print("\n--- Event occurs ---")
producer.generate_event()

print("\n--- Consumer checks ---")
consumer.wait_for_event()

print("\n--- Consumer checks again ---")
consumer.wait_for_event()
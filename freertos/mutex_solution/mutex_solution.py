class SharedCounter:
    def __init__(self):
        self.value = 0


class Mutex:
    def __init__(self):
        self.locked = False

    def acquire(self, task_name):
        if self.locked:
            print(f"{task_name} blocked - mutex already locked")
            return False

        self.locked = True
        print(f"{task_name} acquired mutex")
        return True

    def release(self, task_name):
        self.locked = False
        print(f"{task_name} released mutex")


def sensor_task(counter, mutex):
    if mutex.acquire("Sensor"):

        print(f"Sensor reads counter = {counter.value}")

        temp = counter.value
        temp += 10

        print(f"Sensor calculates = {temp}")

        counter.value = temp

        print(f"Sensor writes counter = {counter.value}")

        mutex.release("Sensor")


def telemetry_task(counter, mutex):
    if mutex.acquire("Telemetry"):

        print(f"Telemetry reads counter = {counter.value}")

        temp = counter.value
        temp += 1

        print(f"Telemetry calculates = {temp}")

        counter.value = temp

        print(f"Telemetry writes counter = {counter.value}")

        mutex.release("Telemetry")


counter = SharedCounter()
mutex = Mutex()

print("Initial counter =", counter.value)
print()

sensor_task(counter, mutex)

print()

telemetry_task(counter, mutex)

print()

print("Final counter =", counter.value)
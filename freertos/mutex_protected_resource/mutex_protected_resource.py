class Mutex:
    def __init__(self):
        self.locked = False

    def acquire(self, task_name):
        if not self.locked:
            self.locked = True
            print(f"{task_name} acquired mutex")
            return True

        print(f"{task_name} blocked - mutex already locked")
        return False

    def release(self, task_name):
        if self.locked:
            self.locked = False
            print(f"{task_name} released mutex")


class Task:
    def __init__(self, name):
        self.name = name

    def write_telemetry(self, mutex, log):
        if mutex.acquire(self.name):

            print(f"{self.name} writing telemetry...")

            log.append(f"{self.name}: telemetry packet")

            print(f"{self.name} finished writing")

            mutex.release(self.name)


mutex = Mutex()
telemetry_log = []

sensor = Task("Sensor")
telemetry = Task("Telemetry")

# sensor.write_telemetry(mutex, telemetry_log)
# telemetry.write_telemetry(mutex, telemetry_log)

#Telemetry doesn't get access to the resource while Sensor owns the mutex.This is mutual exclusion.

mutex.acquire("Sensor")

print("Sensor writing telemetry...")
telemetry_log.append("Sensor: telemetry packet")

telemetry.write_telemetry(mutex, telemetry_log)

print("Sensor finished writing")
mutex.release("Sensor")

print("\nFinal telemetry log:")
for item in telemetry_log:
    print(item)
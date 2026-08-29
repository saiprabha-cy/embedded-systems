class Mutex:
    def __init__(self):
        self.owner = None

    def acquire(self, task):
        if self.owner is None:
            self.owner = task
            print(f"{task} acquired mutex")
            return True

        print(f"{task} blocked - mutex owned by {self.owner}")
        return False

    def release(self, task):
        if self.owner == task:
            self.owner = None
            print(f"{task} released mutex")


class Task:
    def __init__(self, name, priority):
        self.name = name
        self.priority = priority
        self.state = "READY"
    def __str__(self):
        return self.name


mutex = Mutex()

sensor = Task("Sensor", 1)
processing = Task("Processing", 2)
telemetry = Task("Telemetry", 3)


print("\n--- Step 1 ---")
mutex.acquire(sensor)

print("\n--- Step 2 ---")
mutex.acquire(telemetry)

print("\n--- Step 3 ---")
print("Processing becomes ready")
print("Processing preempts Sensor")

print("\n--- Step 4 ---")
print("Sensor cannot run")
print("Sensor cannot release mutex")

print("\n--- Step 5 ---")
print("Telemetry remains blocked")

print("\n--- Result ---")
print("Priority inversion occurred!")
class SharedCounter:
    def __init__(self):
        self.value = 0

    def increment(self, task_name):
        print(f"{task_name}: reading counter = {self.value}")

        temp = self.value

        print(f"{task_name}: adding 1")

        temp = temp + 1

        print(f"{task_name}: writing counter = {temp}")

        self.value = temp


counter = SharedCounter()

counter.increment("Sensor")
counter.increment("Telemetry")

print("\nFinal counter:", counter.value)
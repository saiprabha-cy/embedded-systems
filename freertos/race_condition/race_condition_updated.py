class SharedCounter:
    def __init__(self):
        self.value = 0


counter = SharedCounter()

print("Initial counter:", counter.value)

# Sensor reads
sensor_temp = counter.value 
print("Sensor reads:", sensor_temp)

# Context switch happens here

# Telemetry reads
telemetry_temp = counter.value
print("Telemetry reads:", telemetry_temp)

# Sensor continues
sensor_temp = sensor_temp + 1
counter.value = sensor_temp
print("Sensor writes:", counter.value)

# Telemetry continues using its OLD copy
telemetry_temp = telemetry_temp + 1
counter.value = telemetry_temp
print("Telemetry writes:", counter.value)

print("\nFinal counter:", counter.value)
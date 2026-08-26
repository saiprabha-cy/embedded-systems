class CountingSemaphore:
    def __init__(self, maximum, initial):
        self.maximum = maximum
        self.count = initial

    def give(self):
        if self.count >= self.maximum:
            print("Semaphore FULL")
            return False

        self.count += 1
        print(f"Semaphore GIVEN -> count = {self.count}")
        return True

    def take(self):
        if self.count <= 0:
            print("Semaphore EMPTY")
            return False

        self.count -= 1
        print(f"Semaphore TAKEN -> count = {self.count}")
        return True


semaphore = CountingSemaphore(maximum=3, initial=0)


print("\n--- Producer generates events ---")

semaphore.give()
semaphore.give()
semaphore.give()
semaphore.give()


print("\n--- Consumer handles events ---")

semaphore.take()
semaphore.take()
semaphore.take()
semaphore.take()
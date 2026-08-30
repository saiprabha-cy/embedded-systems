import threading
import time


resource_a = threading.Lock()
resource_b = threading.Lock()


def task_a():
    print("Task A: trying to acquire Resource A")
    resource_a.acquire()

    print("Task A: acquired Resource A")

    time.sleep(1)

    print("Task A: trying to acquire Resource B")
    resource_b.acquire()

    print("Task A: acquired Resource B")

    print("Task A: using both resources")

    resource_b.release()
    resource_a.release()

    print("Task A: released A and B")
    print("Task A: completed")


def task_b():
    print("Task B: trying to acquire Resource A")
    resource_a.acquire()

    print("Task B: acquired Resource A")

    time.sleep(1)

    print("Task B: trying to acquire Resource B")
    resource_b.acquire()

    print("Task B: acquired Resource B")

    print("Task B: using both resources")

    resource_b.release()
    resource_a.release()

    print("Task B: released A and B")
    print("Task B: completed")


thread_a = threading.Thread(target=task_a)
thread_b = threading.Thread(target=task_b)

thread_a.start()
thread_b.start()

thread_a.join()
thread_b.join()

print("\nMain: no deadlock occurred.")
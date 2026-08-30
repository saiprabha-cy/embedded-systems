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
#These lines are never reached by either thread. Because both tasks are stuck waiting at their second .acquire() call, neither thread ever reaches the lines that release the locks or print "completed".
    print("Task A: acquired Resource B")

    resource_b.release()
    resource_a.release()

    print("Task A: completed")


def task_b():
    print("Task B: trying to acquire Resource B")
    resource_b.acquire()

    print("Task B: acquired Resource B")

    time.sleep(1)

    print("Task B: trying to acquire Resource A")
    resource_a.acquire()
#These lines are never reached by either thread. Because both tasks are stuck waiting at their second .acquire() call, neither thread ever reaches the lines that release the locks or print "completed".
    print("Task B: acquired Resource A")

    resource_a.release()
    resource_b.release()

    print("Task B: completed")


thread_a = threading.Thread(target=task_a)
thread_b = threading.Thread(target=task_b)

thread_a.start()
thread_b.start()

thread_a.join(timeout=3)
thread_b.join(timeout=3)

print("\nMain: simulation finished.")
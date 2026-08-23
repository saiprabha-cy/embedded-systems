class Task:
    def __init__(self, name, period, priority, execution_time):
        self.name = name
        self.period = period
        self.priority = priority
        self.execution_time = execution_time

        self.next_run = 0
        self.remaining_time = 0

        self.state = "WAITING"

    def release(self, current_time):
        if current_time >= self.next_run and self.remaining_time == 0:
            self.remaining_time = self.execution_time
            self.next_run += self.period
            self.state = "READY"

    def ready(self):
        return self.remaining_time > 0

    def execute(self, time_step):
        self.remaining_time -= time_step
        self.state = "RUNNING"

        if self.remaining_time <= 0:
            self.remaining_time = 0
            self.state = "WAITING"


tasks = [
    Task("Sensor", 100, 2, 20),
    Task("Processing", 500, 1, 100),
    Task("Telemetry", 1000, 3, 50),
]


current_task = None

for current_time in range(0, 2001, 10):

    # -------------------------------------------------
    # 1. Release periodic tasks
    # -------------------------------------------------

    for task in tasks:
        task.release(current_time)

    # -------------------------------------------------
    # 2. Find READY tasks
    # -------------------------------------------------

    ready_tasks = [
        task for task in tasks
        if task.ready()
    ]

    # -------------------------------------------------
    # 3. Select highest-priority READY task
    # -------------------------------------------------

    if ready_tasks:

        selected_task = max(
            ready_tasks,
            key=lambda task: task.priority
        )

        # -------------------------------------------------
        # 4. Check for preemption
        # -------------------------------------------------

        if current_task != selected_task:

            if current_task is not None:
                if current_task.state == "RUNNING":
                    current_task.state = "READY"

            current_task = selected_task

            print(
                f"{current_time:4} ms -> "
                f"SWITCH TO {current_task.name}"
            )

    # -------------------------------------------------
    # 5. Execute selected task for one time step
    # -------------------------------------------------

    if current_task is not None:

        print(
            f"{current_time:4} ms -> "
            f"RUN {current_task.name} "
            f"(remaining: {current_task.remaining_time} ms)"
        )

        current_task.execute(10)

        # -------------------------------------------------
        # 6. Check whether task completed
        # -------------------------------------------------

        if current_task.state == "WAITING":

            print(
                f"{current_time + 10:4} ms -> "
                f"{current_task.name} COMPLETE"
            )

            current_task = None
from enum import Enum


class State(Enum):
    IDLE = 0
    RUNNING = 1
    COMPLETE = 2
    ERROR = 3
    PAUSED= 4


state = State.IDLE


def handle_event(event):
    global state

    if state == State.IDLE:

        if event == "START":
            state = State.RUNNING

        elif event == "ERROR":
            state = State.ERROR

    elif state == State.RUNNING:

        if event == "DONE":
            state = State.COMPLETE

        elif event == "ERROR":
            state = State.ERROR

    elif state == State.COMPLETE:

        if event == "RESET":
            state = State.IDLE

    elif state == State.ERROR:

        if event == "RESET":
            state = State.IDLE


events = [
    "START",
    "DONE",
    "RESET",
    "START",
    "ERROR",
    "RESET",
]

for event in events:

    print(
        f"Event: {event:6} | "
        f"State before: {state.name:9}",
        end=""
    )

    handle_event(event)

    print(
        f" | State after: {state.name}"
    )
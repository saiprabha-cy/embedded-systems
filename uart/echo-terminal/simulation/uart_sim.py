def send_to_uart(data):
    print(f"PC TX: {data}")


def receive_from_uart(data):
    print(f"PC RX: {data}")


message = "HELLO\r\n"

send_to_uart(message)

# Simulate firmware echoing the received data
echoed_data = message

receive_from_uart(echoed_data)
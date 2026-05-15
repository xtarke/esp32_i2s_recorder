
import socket

# Define IP and Port
UDP_IP = "0.0.0.0"
UDP_PORT = 3333

# 1. Create a UDP socket (SOCK_DGRAM)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# 2. Bind the socket to the IP and Port
sock.bind((UDP_IP, UDP_PORT))

print(f"UDP server listening on {UDP_IP}:{UDP_PORT}")

while True:
    # 3. Receive data (1024 is the buffer size)
    # recvfrom returns the data and the (address, port) of the sender
    data, addr = sock.recvfrom(1024)

    print(f"Received message from {addr}: {data.decode()}")

    # 4. Optional: Send a response back to the sender
    response = b"Message received!"
    sock.sendto(response, addr)

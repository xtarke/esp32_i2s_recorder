import socket

# Configuration
UDP_IP = "0.0.0.0" # Listen on localhost (change to "0.0.0.0" to listen on all interfaces)
UDP_PORT = 3333        # Port to listen on
BUFFER_SIZE = 32768     # Maximum packet size to receive at once
OUTPUT_FILENAME = "received_file.wav"

def start_udp_server():
    # 1. Create a UDP socket
    # socket.AF_INET specifies IPv4, socket.SOCK_DGRAM specifies UDP
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # 2. Bind the socket to the IP and port
    sock.bind((UDP_IP, UDP_PORT))
    print(f"UDP server listening on {UDP_IP}:{UDP_PORT}...")

    blocks = 0

    # 3. Open the output file in binary write mode ('wb')
    with open(OUTPUT_FILENAME, "wb") as f:
        print(f"Ready to record data into '{OUTPUT_FILENAME}'...")
        
        try:
            while True:
                # Receive data from the socket
                # data is a bytes object, addr is a tuple (ip, port)
                data, addr = sock.recvfrom(BUFFER_SIZE)
                
                if not data:
                    # Optional: Break if an empty packet is received (often used as an EOF signal)
                    print("Received empty packet. Stopping.")
                    break
                
                # 4. Write the binary chunk directly to the file
                f.write(data)
                print(f"Received {len(data)} bytes from {addr} ({blocks})")

                blocks = blocks + 1
                
        except KeyboardInterrupt:
            print("\nServer stopped manually by user.")
        finally:
            sock.close()
            print("Socket closed.")

if __name__ == "__main__":
    start_udp_server()
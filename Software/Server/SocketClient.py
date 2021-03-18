import socket
import numpy as np
import cv2
import time

HOST = '192.168.1.140'  # The server's hostname or IP address
PORT = 65432        # The port used by the server

while True:
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            s.sendall(b'Request to view Frame')
            data = s.recv(999999) # large size so that this doesn't close the connection in the middle of the image (prob wrong)
        print(data)
        frame = cv2.imdecode(np.frombuffer(data, dtype=np.uint8), cv2.IMREAD_COLOR)
        try:
            cv2.imshow('Video', frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        except:
            print("Dropped frame detected")
    except:
        print("Crashed")
import asyncio
import asyncws
import cv2
import urllib.request
import numpy as np


img = cv2.imread('C:\\Users\\hazem\\Desktop\\1.jpg')

@asyncio.coroutine
def echo(websocket):
    global img
    img_str = cv2.imencode('.jpg', img)[1]
    igme_str = bytearray(img_str)
    while True:
        #frame = yield from websocket.recv()

        yield from websocket.send(igme_str)

server = asyncws.start_server(echo, '192.168.1.140', 65432)
loop = asyncio.get_event_loop()
loop.run_until_complete(server)
asyncio.get_event_loop().run_forever()

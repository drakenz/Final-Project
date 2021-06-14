import os
import numpy as np
import face_recognition
import pickle
import cv2
import paho.mqtt.client as mqtt
from datetime import datetime, timedelta
import glob

import base64


def findFace():
    cap = cv2.VideoCapture("http://192.168.1.12:8080/video")

    files = []
    for file in glob.glob("known/*.jpg"):
        files.append(file)

    known_face_encodings = []  # array of known face encodings
    known_face_names = []  # and their names

    # Get all photos in known folder and
    for file in files:
        img = face_recognition.load_image_file(file)
        img_encoding = face_recognition.face_encodings(img)[0]
        known_face_encodings.append(img_encoding)
        known_face_names.append(file)

    files = []
    for file in glob.glob("unknown/*.jpg"):
        files.append(file)

    unknown_face_encodings = []
    for file in files:
        img = face_recognition.load_image_file(file)
        img_encoding = face_recognition.face_encodings(img)[0]
        unknown_face_encodings.append(img_encoding)

    count_number = len(unknown_face_encodings)

    print('We have : ', count_number, 'Unknown Images')

    stopTime = datetime.now() + timedelta(seconds=30)

    while True:
        if(datetime.now() > stopTime):
            cv2.destroyAllWindows()
            return

        ret, frame = cap.read()

        frame_grey = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        frame_grey = np.dstack([frame_grey, frame_grey, frame_grey])
        rgb_frame = frame_grey[:, :, ::-1]

        # Find all the faces and face enqcodings in the frame of video
        face_locations = face_recognition.face_locations(rgb_frame)
        face_encodings = face_recognition.face_encodings(
            rgb_frame, face_locations)

        # Loop through each face in this frame of video
        for (top, right, bottom, left), face_encoding in zip(face_locations, face_encodings):
            # See if the face is a match for the known face(s)
            matches = face_recognition.compare_faces(
                known_face_encodings, face_encoding)
            name = "Unknown"

            # If a match was found in known_face_encodings, just use the first one.
            # if True in matches:
            #    #     first_match_index = matches.index(True)
            #    #     name = known_face_names[first_match_index]
            #    # Publish to MQTT server to open Door
            #    client.publish("Door/State", "1")
            #    cv2.destroyAllWindows()
            #    return

            # Or instead, use the known face with the smallest distance to the new face
            face_distances = face_recognition.face_distance(
                known_face_encodings, face_encoding)
            best_match_index = np.argmin(face_distances)
            if matches[best_match_index]:
                name = known_face_names[best_match_index]
                print("UNLOCKED DOOR TO ", name)
                client.publish("Door/State", "1")
                cv2.destroyAllWindows()
                return

            else:
                unmatches = face_recognition.compare_faces(
                    unknown_face_encodings, face_encoding)
                unknown_face_distances = face_recognition.face_distance(
                    unknown_face_encodings, face_encoding)
                if unknown_face_distances.size != 0:
                    best_match_index = np.argmin(unknown_face_distances)
                    if unmatches[best_match_index] == False:
                        unknown_face_encodings.append(face_encoding)
                        fileName = 'unknown/' + str(
                            count_number) + '.jpg'
                        cv2.imwrite(fileName, frame)
                        count_number += 1

                else:
                    unknown_face_encodings.append(face_encoding)
                    fileName = 'unknown/' + \
                        str(count_number) + '.jpg'
                    cv2.imwrite(fileName, frame)
                    count_number += 1

            # Draw a box around the face
            cv2.rectangle(frame, (left - 20, top - 20),
                          (right + 20, bottom + 20), (0, 0, 255), 2)

            # Draw a label with a name below the face
            cv2.rectangle(frame, (left - 20, bottom + 25),
                          (right + 20, bottom + 20), (0, 0, 255), cv2.FILLED)
            font = cv2.FONT_HERSHEY_DUPLEX
            cv2.putText(frame, name, (left - 14, bottom + 24),
                        font, 1.0, (255, 255, 255), 1)

        cv2.imshow('Video', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            cv2.destroyAllWindows()


def sendPhotos():
    for file in glob.glob("unknown/*.jpg"):
        f = open(file, "rb")
        fileContent = f.read()
        byteArr = base64.encodebytes(fileContent)
        byteArr = byteArr.strip()
        byteArr = byteArr.decode()
        byteArr.replace('\n', '')

        client.publish("Door/Photos", byteArr)


def on_connect(client, userdata, flags, rc):
    print("Connected with result code ", str(rc))
    client.subscribe('Door/PIR')
    client.subscribe('Door/requestPhotos')
    client.subscribe('Door/Approve')
    client.subscribe('Door/Delete')


def Approve(index):
    print("Approving user ", index)

    files = []
    for file in glob.glob("unknown/*.jpg"):
        files.append(file)

    index = int(index)
    mahindex = 0
    
    for file in files:
        if(mahindex == index):
            file = str(file)
            newfile = file.replace("unknown\\", "")
            print(newfile)
            os.rename(file, "known\\" + newfile)  # DELETE IMAGE

        mahindex += 1


def Delete(index):
    print("Deleting user ", index)

    files = []
    for file in glob.glob("unknown/*.jpg"):
        files.append(file)

    mahindex = 0
    index = int(index)
    for file in files:
        if(mahindex == index):
            os.remove(file)  # DELETE IMAGE
        mahindex += 1


def on_message(client, userdata, msg):
    if(msg.topic == 'Door/PIR'):
        if(msg.payload.decode("utf-8") == '1'):
            findFace()
    if(msg.topic == 'Door/requestPhotos'):
        if(msg.payload.decode("utf-8") == '1'):
            sendPhotos()
    if(msg.topic == 'Door/Approve'):
        Approve(msg.payload.decode("utf-8"))
    if(msg.topic == 'Door/Delete'):
        Delete(msg.payload.decode("utf-8"))


broker_address = "192.168.1.19"

client = mqtt.Client("DoorServer")  # create new instance
client.username_pw_set(username="admin", password="admin")

client.on_connect = on_connect
client.on_message = on_message
client.connect(broker_address)

# client.loop()
print("LOOPING!")
client.loop_forever()

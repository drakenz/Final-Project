# import the opencv library 
import cv2
import urllib3
import urllib
import urllib.request
import numpy as np
# define a video capture object
import face_recognition
import imutils
from imutils.video import FPS
import pickle

with urllib.request.urlopen('http://192.168.1.133:81/stream') as stream:
    
    bytes = bytearray()
    #imagePath = "C:/Users/hazem/Desktop/FaceDetection/FaceDetect-master/FaceDetect-master/mo2a1.jpg"
    #cascPath = "C:/Users/hazem/Desktop/FaceDetection/FaceDetect-master/FaceDetect-master/haarcascade_frontalface_default.xml"
    #faceCascade = cv2.CascadeClassifier(cascPath)
    mo2a_image = face_recognition.load_image_file("C:/Users/hazem/Desktop/FaceDetection/FaceDetect-master/FaceDetect-master/mo2a2.jpg")
    mo2a_face_encoding = face_recognition.face_encodings(mo2a_image)[0]

    # Load a second sample picture and learn how to recognize it.
    biden_image = face_recognition.load_image_file("C:/Users/hazem/Desktop/FaceDetection/FaceDetect-master/FaceDetect-master/biden.jpg")
    biden_face_encoding = face_recognition.face_encodings(biden_image)[0]

    # Create arrays of known face encodings and their names
    known_face_encodings = [
        mo2a_face_encoding,
        biden_face_encoding
    ]
    known_face_names = [
        "Muayad",
        "Joe Biden"
    ]

    unknown_face_encodings = []
    with open('C:/Users/hazem/Desktop/FaceDetection/FaceDetect-master/FaceDetect-master/Unknown/unknown_face_encodings.dat', 'rb') as f:
        unknown_face_encodings = pickle.load(f)
    count_number = 0;

    while True:
        bytes += stream.read(1024)
        a = bytes.find(b'\xff\xd8')
        b = bytes.find(b'\xff\xd9')
        if a != -1 and b != -1:
            jpg = bytes[a:b+2]
            bytes = bytes[b+2:]
            try:
                img = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            except:
                #print("Lost Frame")
                continue

            # Create the haar cascade


            # Read the image

            #gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

            # Detect faces in the image
            # faces = faceCascade.detectMultiScale(
            #     gray,
            #     scaleFactor=1.01,
            #     minNeighbors=5,
            #     minSize=(30, 30),
            #     flags=cv2.CASCADE_SCALE_IMAGE
            # )
            #
            # print("Found {0} faces!".format(len(faces)))
            #
            # # Draw a rectangle around the faces
            # for (x, y, w, h) in faces:
            #     cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
            frame = imutils.resize(img, width=460)
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            frame = np.dstack([frame, frame, frame])
            rgb_frame = frame[:, :, ::-1]

            # Find all the faces and face enqcodings in the frame of video
            face_locations = face_recognition.face_locations(rgb_frame)
            face_encodings = face_recognition.face_encodings(rgb_frame, face_locations)

            # Loop through each face in this frame of video
            for (top, right, bottom, left), face_encoding in zip(face_locations, face_encodings):
                # See if the face is a match for the known face(s)
                matches = face_recognition.compare_faces(known_face_encodings, face_encoding)

                name = "Unknown"

                # If a match was found in known_face_encodings, just use the first one.
                # if True in matches:
                #     first_match_index = matches.index(True)
                #     name = known_face_names[first_match_index]

                # Or instead, use the known face with the smallest distance to the new face
                face_distances = face_recognition.face_distance(known_face_encodings, face_encoding)
                best_match_index = np.argmin(face_distances)
                if matches[best_match_index]:
                    name = known_face_names[best_match_index]


                else:
                    unmatches = face_recognition.compare_faces(unknown_face_encodings, face_encoding)
                    unknown_face_distances = face_recognition.face_distance(unknown_face_encodings, face_encoding)
                    if unknown_face_distances.size != 0:
                        best_match_index = np.argmin(unknown_face_distances)
                        if unmatches[best_match_index] == False:
                            unknown_face_encodings.append(face_encoding)
                            fileName = 'C:/Users/hazem/Desktop/FaceDetection/FaceDetect-master/FaceDetect-master\\Unknown\\' + str(count_number) + '.jpg'
                            roiGray = rgb_frame[top:bottom, left:right]
                            cv2.imwrite(fileName, roiGray)
                            count_number += 1
                            with open('C:/Users/hazem/Desktop/FaceDetection/FaceDetect-master/FaceDetect-master\\Unknown\\unknown_face_encodings.dat',
                                      'wb') as f:
                                pickle.dump(unknown_face_encodings, f)
                    else:
                        unknown_face_encodings.append(face_encoding)
                        fileName = 'C:/Users/hazem/Desktop/FaceDetection/FaceDetect-master/FaceDetect-master\\Unknown\\' + str(count_number) + '.jpg'
                        roiGray = rgb_frame[top:bottom, left:right]
                        cv2.imwrite(fileName, roiGray)
                        count_number += 1
                        with open('C:/Users/hazem/Desktop/FaceDetection/FaceDetect-master/FaceDetect-master\\Unknown\\unknown_face_encodings.dat',
                                  'wb') as f:
                            pickle.dump(unknown_face_encodings, f)

                # Draw a box around the face
                cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)

                # Draw a label with a name below the face
                cv2.rectangle(frame, (left, bottom - 35), (right, bottom), (0, 0, 255), cv2.FILLED)
                font = cv2.FONT_HERSHEY_DUPLEX
                cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)

            cv2.imshow('Video', frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

cv2.destroyAllWindows()


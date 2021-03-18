import face_recognition
import cv2
import numpy as np
import imutils
from imutils.video import FPS
import pickle

# This is a super simple (but slow) example of running face recognition on live video from your webcam.
# There's a second example that's a little more complicated but runs faster.

# PLEASE NOTE: This example requires OpenCV (the `cv2` library) to be installed only to read from your webcam.
# OpenCV is *not* required to use the face_recognition library. It's only required if you want to run this
# specific demo. If you have trouble installing it, try any of the other demos that don't require it instead.

# Get a reference to webcam #0 (the default one)
video_capture = cv2.VideoCapture(0)
fps = FPS().start()
# Load a sample picture and learn how to recognize it.
mo2a_image = face_recognition.load_image_file("F:\FaceDetect-master\FaceDetect-master\mo2a1.jpg")
mo2a_face_encoding = face_recognition.face_encodings(mo2a_image)[0]

# Load a second sample picture and learn how to recognize it.
biden_image = face_recognition.load_image_file("F:/FaceDetect-master/FaceDetect-master/biden.jpg")
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
with open('F:\FaceDetect-master\FaceDetect-master\\Unknown\\unknown_face_encodings.dat', 'rb') as f:
	unknown_face_encodings = pickle.load(f)
count_number=0;

while True:
    # Grab a single frame of video
    video_capture.set(cv2.CAP_PROP_FPS,30)
    ret, frame = video_capture.read()

    # Convert the image from BGR color (which OpenCV uses) to RGB color (which face_recognition uses)
    frame = imutils.resize(frame, width=450)
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
                    fileName = 'F:\FaceDetect-master\FaceDetect-master\\Unknown\\' + str(count_number) + '.jpg'
                    roiGray = rgb_frame[top:bottom,left:right]
                    cv2.imwrite(fileName, roiGray)
                    count_number+=1
                    with open('F:\FaceDetect-master\FaceDetect-master\\Unknown\\unknown_face_encodings.dat', 'wb') as f:
                        pickle.dump(unknown_face_encodings, f)
            else:
                unknown_face_encodings.append(face_encoding)
                fileName = 'F:\FaceDetect-master\FaceDetect-master\\Unknown\\' + str(count_number) + '.jpg'
                roiGray = rgb_frame[top:bottom, left:right]
                cv2.imwrite(fileName, roiGray)
                count_number += 1
                with open('F:\FaceDetect-master\FaceDetect-master\\Unknown\\unknown_face_encodings.dat', 'wb') as f:
                    pickle.dump(unknown_face_encodings, f)

        # Draw a box around the face
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)

        # Draw a label with a name below the face
        cv2.rectangle(frame, (left, bottom - 35), (right, bottom), (0, 0, 255), cv2.FILLED)
        font = cv2.FONT_HERSHEY_DUPLEX
        cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)

    # Display the resulting image
    cv2.imshow('Video', frame)

    # Hit 'qqq' on the keyboard to quit!
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    fps.update()
# Release handle to the webcam
video_capture.release()
cv2.destroyAllWindows()
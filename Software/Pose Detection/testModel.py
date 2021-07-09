import pandas as pd
import numpy as np
from tensorflow.keras.models import load_model
import tensorflow as tf
import cv2
import time
import argparse
import posenet
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from tensorflow.python.keras import backend as K
import os
import paho.mqtt.client as mqtt


def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT server with result code ", str(rc))


broker_address = "homeassistant.local"
client = mqtt.Client("Pose Server")
client.username_pw_set(username="homeassistant",
                       password="ahhah9Mio6Oingaeweithihohsh0ieGhai4cua0yi9Xah0ya4poY3aeC4ozei6el")
client.on_connect = on_connect
client.connect(broker_address, port=1883)

print(__file__)
dirname = os.path.dirname(__file__)
pose_dict = {0: "Sitting", 1: "Standing"}
data = np.loadtxt(os.path.join(dirname, 'data.txt')).reshape(1599, 52)

column_names = ['pose', 'nose', 'nose_coordX', 'nose_coordY', 'L_eye', 'L_eye_coordX', 'L_eye_coordY', 'R_eye',
                'R_eye_coordX', 'R_eye_coordY', 'L_ear',
                'L_ear_coordX', 'L_ear_coordY', 'R_ear', 'R_ear_coordX', 'R_ear_coordY', 'L_shoulder',
                'L_shoulder_coordX', 'L_shoulder_coordY', 'R_shoulder',
                'R_shoulder_coordX', 'R_shoulder_coordY', 'L_elbow', 'L_elbow_coordX', 'L_elbow_coordY', 'R_elbow',
                'R_elbow_coordX', 'R_elbow_coordY', 'L_wrist',
                'L_wrist_coordX', 'L_wrist_coordY', 'R_wrist', 'R_wrist_coordX', 'R_wrist_coordY', 'L_hip',
                'L_hip_coordX', 'L_hip_coordY', 'R_hip', 'R_hip_coordX', 'R_hip_coordY',
                'L_knee', 'L_knee_coordX', 'L_knee_coordY', 'R_knee', 'R_knee_coordX', 'R_knee_coordY', 'L_ankle',
                'L_ankle_coordX', 'L_ankle_coordY', 'R_ankle',
                'R_ankle_coordX', 'R_ankle_coordY']
df = pd.DataFrame(columns=column_names)

#cap = cv2.VideoCapture("rtsp://Kimo123:passwordgdeed@aykalamcam.duckdns.org:554/stream2")
cap = cv2.VideoCapture("rtsp://192.168.1.104:8080/h264_pcm.sdp")
cap.set(3, 513)
cap.set(4, 513)

start = time.time()
frame_count = 0
count = 1600
sitting = 0
with tf.Graph().as_default():
    with tf.compat.v1.Session() as sess:
        K.set_session(sess)
        model_mlp = load_model(os.path.join(dirname, 'Model4.h5'))
        model_cfg, model_outputs = posenet.load_model(101, sess)
        output_stride = model_cfg['output_stride']
        while True:
            try:
                input_image, display_image, output_scale = posenet.read_cap(
                    cap, scale_factor=0.7125, output_stride=output_stride)
            except Exception as e:
                print(e)
                continue

            heatmaps_result, offsets_result, displacement_fwd_result, displacement_bwd_result = sess.run(
                model_outputs, feed_dict={'image:0': input_image})

            pose_scores, keypoint_scores, keypoint_coords = posenet.decode_multi.decode_multiple_poses(
                heatmaps_result.squeeze(axis=0),
                offsets_result.squeeze(axis=0),
                displacement_fwd_result.squeeze(axis=0),
                displacement_bwd_result.squeeze(axis=0),
                output_stride=output_stride,
                max_pose_detections=10,
                min_pose_score=0.4
            )
            keypoint_coords *= output_scale

            if frame_count >= 0:
                i = 0
                for pi in range(len(pose_scores)):
                    if pose_scores[pi] == 0.:
                        break

                    df.at[i, 'pose'] = pose_scores[pi]
                    j = 1
                    for ki, (s, c) in enumerate(zip(keypoint_scores[pi, :], keypoint_coords[pi, :, :])):
                        df.iat[i, j] = s
                        df.iat[i, (j + 1)] = keypoint_coords[pi, ki, 0]
                        df.iat[i, (j + 2)] = keypoint_coords[pi, ki, 1]
                        j = j + 3
                    i = i + 1

                poses = len(df.index)
                prediction = []
                x = df.values
                x = np.asarray(x)
                data_new = data
                data_new = np.append(data_new, x, axis=0)
                scaler = StandardScaler().fit(data_new)
                X_scaled = scaler.transform(data_new)
                for i in range(0, poses):
                    x = X_scaled[-1-i]
                    x = np.asarray(x)
                    x = x.reshape(-1, 52)
                    prediction.append(model_mlp.predict(x))

                i = 1
                for it in prediction:
                    label = pose_dict[int(it)]

                    if label == "Sitting" and sitting == 0:
                        print("Case 1")
                        sitting = 1
                        s_time = time.time()
                    elif label == "Sitting" and sitting == 1:
                        print("Case 2")
                        p_time = time.time() - s_time
                        print("Sitting for: ", int(p_time), " Seconds")
                        if (p_time >= 60):
                            print("Stand up or whatever!")
                            client.publish(
                                "Mobile/Notification", "You have been still for too long, Stand up and move a little")
                            sitting = 0
                            label = "Stand up Man!!"
                        if (p_time >= 120):
                            client.publish(
                                "Mobile/Notification", "Person is too still, Initiating Emergency")
                            client.publish("Emergency", "1")
                    else:
                        print("Case 3")
                        print("Hurray! You stood up")
                        client.publish("Emergency", "0")
                        sitting = 0

                    print("Person ", i, ": ", pose_dict[int(it)])
                    cv2.putText(display_image, label, (int(df.at[i-1, 'R_hip_coordX']), int(
                        df.at[i-1, 'R_hip_coordY'])), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
                    print()
                    i += 1

            cv2.imshow('pose-detection', display_image)
            frame_count += 1
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

    print('Average FPS: ', frame_count / (time.time() - start))
    print('Frame Count: ', frame_count)

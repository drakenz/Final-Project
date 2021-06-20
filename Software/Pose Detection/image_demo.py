import tensorflow as tf
import cv2
import time
import argparse
import os
import csv
import pandas as pd
import posenet
import numpy as np


parser = argparse.ArgumentParser()
parser.add_argument('--model', type=int, default=101)
parser.add_argument('--scale_factor', type=float, default=1.0)
parser.add_argument('--notxt', action='store_true')
parser.add_argument('--image_dir', type=str, default='./images')
parser.add_argument('--output_dir', type=str, default='./output')
args = parser.parse_args()


def main():

    with  tf.compat.v1.Session()as sess:
        model_cfg, model_outputs = posenet.load_model(args.model, sess)
        output_stride = model_cfg['output_stride']

        if args.output_dir:
            if not os.path.exists(args.output_dir):
                os.makedirs(args.output_dir)
        '''
        filenames = [
            f.path for f in os.scandir('F:\Dataset') if f.is_file() and f.path.endswith(('.png', '.jpg'))]
'''
        filenames=[]
        for root, dirs, files in os.walk('F:\Dataset'):
            for filename in files:
                filenames.append(os.path.join(root, filename))

        start = time.time()
        column_names=['pose', 'nose', 'nose_coordX', 'nose_coordY', 'L_eye', 'L_eye_coordX', 'L_eye_coordY', 'R_eye', 'R_eye_coordX', 'R_eye_coordY', 'L_ear',
                          'L_ear_coordX', 'L_ear_coordY', 'R_ear', 'R_ear_coordX', 'R_ear_coordY', 'L_shoulder', 'L_shoulder_coordX', 'L_shoulder_coordY', 'R_shoulder',
                          'R_shoulder_coordX', 'R_shoulder_coordY', 'L_elbow', 'L_elbow_coordX', 'L_elbow_coordY', 'R_elbow', 'R_elbow_coordX', 'R_elbow_coordY', 'L_wrist',
                          'L_wrist_coordX', 'L_wrist_coordY', 'R_wrist', 'R_wrist_coordX', 'R_wrist_coordY', 'L_hip', 'L_hip_coordX', 'L_hip_coordY', 'R_hip', 'R_hip_coordX', 'R_hip_coordY',
                          'L_knee', 'L_knee_coordX', 'L_knee_coordY', 'R_knee', 'R_knee_coordX', 'R_knee_coordY', 'L_ankle', 'L_ankle_coordX', 'L_ankle_coordY', 'R_ankle',
                          'R_ankle_coordX', 'R_ankle_coordY']
        df = pd.DataFrame(columns=column_names)
        i=0
        for f in filenames:
            input_image, draw_image, output_scale = posenet.read_imgfile(
                f, scale_factor=args.scale_factor, output_stride=output_stride)

            heatmaps_result, offsets_result, displacement_fwd_result, displacement_bwd_result = sess.run(
                model_outputs,
                feed_dict={'image:0': input_image}
            )

            pose_scores, keypoint_scores, keypoint_coords = posenet.decode_multiple_poses(
                heatmaps_result.squeeze(axis=0),
                offsets_result.squeeze(axis=0),
                displacement_fwd_result.squeeze(axis=0),
                displacement_bwd_result.squeeze(axis=0),
                output_stride=output_stride,
                max_pose_detections=10,
                min_pose_score=0.25)

            keypoint_coords *= output_scale

            if args.output_dir:
                draw_image = posenet.draw_skel_and_kp(
                    draw_image, pose_scores, keypoint_scores, keypoint_coords,
                    min_pose_score=0.25, min_part_score=0.25)

                #cv2.imwrite(os.path.join(args.output_dir, os.path.relpath(f, args.image_dir)), draw_image)


            if not args.notxt:
                print()
                print("Results for image: %s" % f)

                for pi in range(len(pose_scores)):
                    if pose_scores[pi] == 0.:
                        break

                    df.at[i, 'Label'] = f.split('\\')[2]
                    print('Pose #%d, score = %f' % (pi, pose_scores[pi]))
                    df.at[i, 'pose'] = pose_scores[pi]
                    j=2
                    for ki, (s, c) in enumerate(zip(keypoint_scores[pi, :], keypoint_coords[pi, :, :])):
                        print('Keypoint %s, score = %f, coord = %s' % (posenet.PART_NAMES[ki], s, c))
                        df.iat[i, j] = s
                        df.iat[i, (j+1)] = keypoint_coords[pi, ki, 0]
                        df.iat[i, (j+2)] = keypoint_coords[pi, ki, 1]
                        j=j+3
                    i=i+1

        print('Average FPS:', len(filenames) / (time.time() - start))
        df.to_csv("images_scores.csv")
        print(df.shape)


if __name__ == "__main__":
    main()

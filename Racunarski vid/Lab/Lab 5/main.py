import cv2
import glob
import numpy as np
from cv2 import aruco
import yaml
import os

calibrate_camera=not os.path.isfile('calibration.yaml')
ARUCO_DICT = aruco.Dictionary_get(aruco.DICT_6X6_1000)
board = aruco.GridBoard_create(markersX=5,markersY=7,markerLength=2,markerSeparation=0.4,dictionary=ARUCO_DICT)

arucoParams = aruco.DetectorParameters_create()

if calibrate_camera:
    img_list=[]
    calib_fnms = glob.glob('./calib_image_*.jpg')
    for idx,fn in enumerate(calib_fnms):
        img = cv2.imread(fn)
        img_list.append(img)
        h,w,c = img.shape
    
    counter, corners_list, id_list = [],[],[]
    first = True
    for im in img_list:
        img_gray = cv2.cvtColor(im,cv2.COLOR_RGB2GRAY)
        corners, ids, rejectedImgPoints = aruco.detectMarkers(img_gray, ARUCO_DICT,parameters=arucoParams)
        if first:
            corners_list=corners
            id_list=ids
            first=False
        else:
            corners_list=np.vstack((corners_list,corners))
            id_list=np.vstack((id_list,ids))
        counter.append(len(ids))
    counter=np.array(counter)

    ret,mtx,dist,rvecs,tvecs = aruco.calibrateCameraAruco(corners_list,id_list, counter, board, img_gray.shape, None,None)
    data = {'camera_matrix':np.asarray(mtx).tolist(),'dist_coeff':np.asarray(dist).tolist()}
    with open("calibration.yaml","w") as file:
        yaml.dump(data,file)
else:
    camera = cv2.VideoCapture('Aruco_board.mp4')
    with open('calibration.yaml') as file:
        loadeddict = yaml.safe_load(file)
    camera_matrix = loadeddict.get('camera_matrix')
    dist = loadeddict.get('dist_coeff')
    camera_matrix = np.array(camera_matrix)
    dist_coeffs = np.array(dist)

    while True:
        ret, frame = camera.read()
        if not ret:
            break

        frame = np.array(frame)
        frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        h,w=frame_gray.shape[:2]
        new_camera_matrix, roi = cv2.getOptimalNewCameraMatrix(camera_matrix, dist_coeffs, (w,h), 1,(w,h))

        corners, ids, rejectedImgPoints = aruco.detectMarkers(frame_gray, ARUCO_DICT, parameters=arucoParams)
        aruco.refineDetectedMarkers(frame_gray, board, corners, ids, rejectedImgPoints)

        im_with_aruco_board = frame

        if type(ids) is np.ndarray and ids.any() != None:
            im_with_aruco_board = aruco.drawDetectedMarkers(frame, corners, ids, (0,255,0))
            rvec,tvec = (),()
            retval, rvec, tvec = aruco.estimatePoseBoard(corners, ids, board, camera_matrix, dist_coeffs, rvec, tvec)
            if retval != 0:
                im_with_aruco_board = cv2.drawFrameAxes(im_with_aruco_board, camera_matrix, dist_coeffs, rvec, tvec, 5)

        undst = cv2.undistort(im_with_aruco_board, camera_matrix, dist_coeffs, None, new_camera_matrix)

        width = int(undst.shape[1] * 0.5)
        height = int(undst.shape[0] * 0.5)
        dim = (width, height)
        undst = cv2.resize(undst, dim, interpolation = cv2.INTER_AREA)

        cv2.imshow("Video", undst)
        if cv2.waitKey(2) & 0xFF == ord('q'):
            break
    camera.release()

cv2.destroyAllWindows()
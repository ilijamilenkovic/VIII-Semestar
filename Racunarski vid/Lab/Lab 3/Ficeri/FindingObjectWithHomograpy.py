import numpy as np
import cv2 as cv

MIN_MATCH_COUNT = 10



# # Initiate SIFT detector
# detector = cv.SIFT_create()
# #detector = cv.xfeatures2d.SURF_create()

# # find the keypoints and descriptors with SIFT
# kp1, des1 = detector.detectAndCompute(img1, None)
# kp2, des2 = detector.detectAndCompute(img2, None)

# FLANN_INDEX_KDTREE = 1
# index_params = dict(algorithm=FLANN_INDEX_KDTREE, trees=5)
# search_params = dict(checks=50)
# flann = cv.FlannBasedMatcher(index_params, search_params)
# matches = flann.knnMatch(des1, des2, k=2)

# # store all the good matches as per Lowe's ratio test.
# good = []
# for m, n in matches:
#     if m.distance < 0.7 * n.distance:
#         good.append(m)

# if len(good) > MIN_MATCH_COUNT:
#     src_pts = np.float32([kp1[m.queryIdx].pt for m in good]).reshape(-1, 1, 2)
#     dst_pts = np.float32([kp2[m.trainIdx].pt for m in good]).reshape(-1, 1, 2)

#     M, mask = cv.findHomography(src_pts, dst_pts, cv.RANSAC, 5.0)
    
    
#     #matchesMask = mask.ravel().tolist()
#     h1, w1 = img1.shape
#     h2,w2 = img2.shape
#     img2 = cv.cvtColor(img2, cv.COLOR_GRAY2BGR)
#     img2 = cv.warpPerspective(img2, M, (w1+w2,h1+h2))
    
    
# else:
#     print("Not enough matches are found - {}/{}".format(len(good), MIN_MATCH_COUNT))
#     matchesMask = None





# draw_params = dict(matchColor=(0, 255, 0),  # draw matches in green color
#                    singlePointColor=None,
#                    matchesMask=matchesMask,  # draw only inliers
#                    flags=2)
# img3 = cv.drawMatches(img1, kp1, img2, kp2, good, None, **draw_params)

# cv.imshow("Output1", img1)
# cv.imshow("Output2", img2)
# cv.waitKey(0)    


 
def merge_imgs(img1, img2):
    MIN_MATCH_COUNT = 10
    detector = cv.SIFT_create()
    kp1, des1 = detector.detectAndCompute(img1, None)
    kp2, des2 = detector.detectAndCompute(img2, None)

    FLANN_INDEX_KDTREE = 1
    index_params = dict(algorithm=FLANN_INDEX_KDTREE, trees=5)
    search_params = dict(checks=50)
    flann = cv.FlannBasedMatcher(index_params, search_params)
    matches = flann.knnMatch(des1, des2, k=2)
    good = list()
    for m, n in matches:
        if m.distance < 0.7 * n.distance:
            good.append(m)

    if len(good) <= MIN_MATCH_COUNT:
        return None

    src_pts = np.float32([kp1[m.queryIdx].pt for m in good]).reshape(-1, 1, 2)
    dst_pts = np.float32([kp2[m.trainIdx].pt for m in good]).reshape(-1, 1, 2)

    M, mask = cv.findHomography(dst_pts, src_pts, cv.RANSAC, 5.0)
    h, w = img1.shape[0], img1.shape[1]
    pts = np.float32([[0, 0], [0, h - 1], [w - 1, h - 1], [w - 1, 0]]).reshape(-1, 1, 2)
    dst = cv.perspectiveTransform(pts, M)
    dst += (w, 0)  # shift the transformed image to the right by the width of img1


    h_new, w_new = max(img1.shape[0], img2.shape[0]), img1.shape[1] + img2.shape[1]
    result = np.ones((h_new, w_new, 3), np.uint8)

    # Merge the images onto the canvas
    
    result[0:img1.shape[0], 0:img1.shape[1]] = img1
    warp_img = cv.warpPerspective(img2, M, (w_new, h_new))
    result = np.where(warp_img != 0, warp_img, result)

 

    return result



img1 = cv.imread('1.JPG', cv.IMREAD_GRAYSCALE)          # queryImage
img2 = cv.imread('2.JPG', cv.IMREAD_GRAYSCALE)  # trainImage



result = merge_imgs(img1,img2)

cv.imshow("Output2", result)
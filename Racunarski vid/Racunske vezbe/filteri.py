import cv2

img = cv2.imread("./vending_machine.jpg", cv2.IMREAD_GRAYSCALE)

blur = cv2.GaussianBlur(img, (11,11), 0)

sobX = cv2.Sobel(img,cv2.CV_8U, 1,0)
sobY = cv2.Sobel(img,cv2.CV_8U, 0,1)

cv2.imshow("Image", sobY)

cv2.waitKey(0)
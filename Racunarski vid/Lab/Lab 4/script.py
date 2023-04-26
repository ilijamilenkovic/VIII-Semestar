import cv2 as cv
import numpy as np
import time
import imutils

def pyramid(image, scale=2, minSize=(30, 30)):
	# yield the original image
	yield image
	# keep looping over the pyramid
	while True:
		# compute the new dimensions of the image and resize it
		w = int(image.shape[1] / scale)
		image = imutils.resize(image, width=w)
		# if the resized image does not meet the supplied minimum
		# size, then stop constructing the pyramid
		if image.shape[0] < minSize[1] or image.shape[1] < minSize[0]:
			break
		# yield the next image in the pyramid
		yield image


def sliding_window(image, stepSize, windowSize):
	# slide a window across the image
	for y in range(0, image.shape[0], stepSize):
		for x in range(0, image.shape[1], stepSize):
			# yield the current window
			yield (x, y, image[y:y + windowSize[1], x:x + windowSize[0]])

def prepoznaj(blob, net):
    net.setInput(blob)
    preds = net.forward()

    idxs = np.argsort(preds[0])[::-1][:5]
    text = "{}".format(classes[idxs[0]])
    return (preds[0][idxs[0]] * 100, text)
     
            

    
       




if __name__ == '__main__':
    # load the input image
    image = cv.imread("download.png")

    #Crop loaded image
    image = image[296:1016,365:1805]


    #for y = 0, y < 
    #

    #velicina prozora za sliding window
    (winW, winH) = (180, 180)
    (winWScaled,winHScaled) = (180,180)

    rows = open("synset_words.txt").read().strip().split("\n")
    classes = [r[r.find(" ") + 1:].split(",")[0] for r in rows]
    net = cv.dnn.readNetFromCaffe("bvlc_googlenet.prototxt", "bvlc_googlenet.caffemodel")
    scale = 2
    i = 0
 
    for resized in pyramid(image, scale=2):
        for (x, y, window) in sliding_window(resized, stepSize=180, windowSize=(winW, winH)):
            blob = cv.dnn.blobFromImage(window, 1, (224, 224), (104, 117, 123))
            (confidence, text) = prepoznaj(blob,net)
            winWScaled = winW
            winHScaled = winH
            if(confidence > 70):
                for s in range(0,i):
                    x = int(x * scale)
                    y = int(y * scale)
                    winWScaled = int(winWScaled * scale)
                    winHScaled = int(winHScaled * scale)
                #print(winWScaled, winHScaled)
                clone = image.copy()
                if text == "Maltese dog":
                    cv.rectangle(image, (x, y), (x + winWScaled, y + winHScaled), (0, 0, 255), 2)
                    text = "DOG"
                elif text == "Siamese cat":
                    cv.rectangle(image, (x, y), (x + winWScaled, y + winHScaled), (0, 255, 255), 2)
                    text = "CAT"
                
                cv.putText(image, text, (x+5, y+25),  cv.FONT_HERSHEY_SIMPLEX,0.7, (0, 0, 255), 2)
                #cv.imshow("Window2", clone)
                #cv.waitKey(0)
        i = i + 1
        #print("i je " + str(i))
        
		   

           
        
       
    
    
    # display the output image
    cv.imshow("Image", image)
    cv.waitKey(0)

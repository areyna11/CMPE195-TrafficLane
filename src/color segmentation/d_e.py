
import cv2
import math
import numpy as np
from PIL import Image

img = cv2.imread('5.jpg')

kernel = np.ones((5,5),np.uint8)

img_erosion = cv2.erode(img, kernel, iterations=3)
im = Image.fromarray(img_erosion)

img_dilation = cv2.dilate(img, kernel, iterations=3)
im2 = Image.fromarray(img_dilation)

#difference between erosion and dilation

img_diff = abs(img_dilation[:] - img_erosion[:])
img_diff = Image.fromarray(img_diff)
pe= img_diff.load()
(w,h) = img_diff.size
h1 = int(h*0.4)

for i in range(w):
	for j in range(h1):
		pe[i,j] = 0,0,0

img_diff = np.array(img_diff)
im3 = cv2.cvtColor(img_diff, cv2.COLOR_BGR2GRAY)


img_diff_erosion =  cv2.erode(im3, kernel, iterations=3)

thresh = 70
maxValue = 255
th,diff_thredhold = cv2.threshold(img_diff,thresh,maxValue,cv2.THRESH_BINARY)
th2,diff_e_thredhold = cv2.threshold(img_diff_erosion,thresh,maxValue,cv2.THRESH_BINARY)


#img_erosion_dilation = cv2.dilate(img_erosion, kernel, iterations=3)
#img_dilation_erosion = cv2.erode(img_dilation, kernel, iterations=3)
#floodfill
'''
t1 = t.copy()
h, w = t.shape[:2]
mask = np.zeros((h+2, w+2), np.uint8)
cv2.floodFill(t, mask, (0,0), 255)

th3,t3 = cv2.threshold(t1,255,maxValue,cv2.THRESH_BINARY)
'''
cv2.imshow('Input', img)

cv2.imshow('Erosion', img_erosion)
cv2.imshow('Dilation', img_dilation)
cv2.imshow('Difference of Dilation and Erosion', img_diff)
#cv2.imshow('Diff->Erosion',img_diff_erosion)
#cv2.imshow('Erosion->Dilation', img_erosion_dilation)
#cv2.imshow('Dilation->Erosion', img_dilation_erosion)
#cv2.imshow("Floodfilled Image W/O E", t3)

cv2.imshow('Threshold Diff',diff_thredhold)
cv2.imshow('Threshold Diff->Erosion',diff_e_thredhold)
cv2.waitKey(0)

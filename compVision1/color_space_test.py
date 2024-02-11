# Justin Fellows
# 1001865403
# Computer Vision assignment 1

import numpy as np
import sys
from PIL import Image

def clampInput(hsvMod):
    if hsvMod[0] > 360 or hsvMod[0] < 0:
        raise ValueError("Hue must be between 0 and 360")
    if hsvMod[1] > 1.0 or hsvMod[1] < 0.0:
        raise ValueError("Saturation must be between 0.0 and 1.0")
    if hsvMod[2] > 1.0 or hsvMod[2] < 0.0:
        raise ValueError("Value must be between 0.0 and 1.0")
    
# Pass in the image as a numpy array
# Changing this to work on only one pixel
def RGBtoHSV(img):
    V = np.max(img)
    C = V - np.min(img)
    if V == 0:
        S = 0
    else:
        S = float(C / V)
    # Calculating Hue now
    if C == 0:
        H = 0
    elif img[0] == V:
        H = float((img[1] - img[2]) / C)
        H %= 6
    elif img[1] == V:
        H = 2 + float((img[2] - img[0]) / C)
    else:
        H = 4 + float((img[0] - img[1]) / C)
    H *= 60

    return [H,S,V/255.0] # Hack solution, but it works. Turns out its because i forgot to normalize the rgb values at the beginning

def HSVtoRGB(pix):
    H = pix[0]
    S = pix[1]
    V = pix[2]
    C = V * S
    Hnot = H / 60
    X = C * (1 - abs(Hnot % 2 - 1))
    if Hnot >= 0 and Hnot < 1:
        R = C
        G = X
        B = 0
    elif Hnot >= 1 and Hnot < 2:
        R = X
        G = C
        B = 0
    elif Hnot >= 2 and Hnot < 3:
        R = 0
        G = C
        B = X
    elif Hnot >= 3 and Hnot < 4:
        R = 0
        G = X
        B = C
    elif Hnot >= 4 and Hnot < 5:
        R = X
        G = 0
        B = C
    else:
        R = C
        G = 0
        B = X
    m = V - C
    return np.array([int((R + m)*255), int((G + m)*255), int((B + m)*255)]).astype('uint8')

def hsvModify(pix, hsvMod):
    H = (pix[0] + hsvMod[0]) % 360.0
    S = (pix[1] + hsvMod[1]) % 1.0
    V = (pix[2] + hsvMod[2]) % 1.0
    return [H,S,V]

# This is the function to call to do the conversion and modification
# Pass it an image as a numpy array and it will return the modified image
# as another numpy array
def modifyImage(img, hsvMod):
    imgArray = np.apply_along_axis(RGBtoHSV, 2, img)
    imgArray = np.apply_along_axis(hsvModify, 2, imgArray, hsvMod)
    imgArray = np.apply_along_axis(HSVtoRGB, 2, imgArray)    
    return imgArray

   
if __name__ == "__main__":
    args = sys.argv
    if len(args) != 5:
        raise ValueError("Provide image file name and HSV modifiers")
    hsvMod = np.array([float(args[2]), float(args[3]), float(args[4])])
    clampInput(hsvMod)
    inputFile = args[1]
    img = Image.open(inputFile)
    imgArray = np.asarray(img).astype('int64')
    img.close()
    modifiedImage = modifyImage(imgArray, hsvMod)
    outputFileName = "shifted_" + inputFile
    modifiedImage.save(outputFileName)
    img.close()

'''
Justin Fellows 1001865403
'''
import img_transforms as it
from PIL import Image
import numpy as np
import sys

def pyramid_helper(img, height, filename, steps):
    if steps <= 0:
        return
    scaledImgArr = it.resize_img(img, 2)
    scaledImg = Image.fromarray(scaledImgArr)
    splitname = filename.split('.', 1)
    newFilename = splitname[0] + "_" + str(2 ** (-1*(steps-height))) + "x." + splitname[1]
    scaledImg.save(newFilename)
    scaledImg.close()
    pyramid_helper(scaledImgArr, height, filename, steps - 1)

def create_pyramid(img, height, filename):
    steps = height - 1
    if steps <= 0:
        raise ValueError("Height must be greater than one.")
    pyramid_helper(img, height, filename, steps)

# Testing pyramid creation
if __name__ == "__main__":
    args = sys.argv
    if len(args) != 3:
        raise ValueError("Provide image file name and height.")
    inputFile = args[1]
    img = Image.open(inputFile)
    imgArray = np.asarray(img).astype('uint8')
    create_pyramid(imgArray, int(args[2]), inputFile)

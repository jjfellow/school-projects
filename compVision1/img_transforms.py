'''
Justin Fellows 1001865403
Computer Vision Assignment 1
'''
import numpy as np
from numpy.lib import stride_tricks
from PIL import Image
import sys
import random
import color_space_test as cst

# Assume the supplied image is already a numpy array
def random_crop(img, size):
    dim = img.shape
    # dim has the shape of the image, in width, height, and channels
    # Check given size to make sure it is possible
    if size > min(dim[0],dim[1]) or size < 0:
        raise ValueError("Invalid size.")
    # pick the top left corner of the crop, using a psuedorandom number gen
    origin_width = random.randrange(0, dim[0] - (size + 1))
    origin_height = random.randrange(0, dim[1] - (size + 1))
    newImg = np.empty((size, size, dim[2]), dtype='uint8')
    i = 0
    j = 0
    for ii in range(origin_width, origin_width + size):
        for jj in range(origin_height, origin_height + size):
            newImg[i][j] = img[ii][jj]
            j += 1
        j = 0
        i += 1
    
    return newImg

# This works off the assumption the provided image is square
def extract_patch(img, num_patches):
    height, width, channels = img.shape
    size = height //  num_patches
    shape = [height // size, width // size] + [size, size]

    strides = [size * s for s in img.strides[:2]] + list(img.strides)
    print("strides= " + str(strides))
    print("size= " + str(size))
    patches = stride_tricks.as_strided(img, shape=shape, strides=strides)
    return patches

# Factor needs to be an integer
def resize_img(img, factor):
    height, width, channels = img.shape
    reHeight, reWidth = height * factor, width * factor
    newImg = np.empty(shape=(reHeight, reWidth, channels), dtype="uint8")
    i=0
    j=0
    for ii in range(0, height - 1):
        for jj in range(0, width - 1):
            newImg[i*factor][j*factor] = img[ii][jj]
            for iii in range(i*factor, i*factor +factor):
                for jjj in range(j*factor, j*factor+factor):
                    newImg[iii][jjj] = img[ii][jj]
            j += 1
        j = 0
        i += 1
    return newImg

def color_jitter(img, hue, saturation, value):
    hueJitter = random.uniform(0, hue)
    satJitter = random.uniform(0, saturation)
    valJitter = random.uniform(0, value)
    return cst.modifyImage(img, [hueJitter, satJitter, valJitter])
'''
# Test for random crop
if __name__ == "__main__":
    args = sys.argv
    if len(args) != 3:
        raise ValueError("Provide image file name and crop size.")
    inputFile = args[1]
    img = Image.open(inputFile)
    imgArray = np.asarray(img).astype('uint8')
    img.close()
    croppedImg = random_crop(imgArray, int(args[2]))
    croppedImg = Image.fromarray(croppedImg)
    croppedImg.save("cropped_" + inputFile)
    croppedImg.close()
'''
'''
# Test for patch extraction
if __name__ == "__main__":
    args = sys.argv
    if len(args) != 3:
        raise ValueError("Provide image file name and number of patches per row.")
    inputFile = args[1]
    img = Image.open(inputFile)
    imgArray = np.asarray(img).astype('uint8')
    img.close()
    patches = extract_patch(imgArray, int(args[2]))
    j = 0
    for i in patches:
        outfileName = "patch" + str(j) + inputFile
        outputfile = Image.fromarray(i)
        outputfile.save(outfileName)
        outputfile.close()
        j += 1
'''
'''
# Test for resize
if __name__ == "__main__":
    args = sys.argv
    if len(args) != 3:
        raise ValueError("Provide image file name and scale factor.")
    inputFile = args[1]
    img = Image.open(inputFile)
    imgArray = np.asarray(img).astype('uint8')
    img.close()
    newImage = resize_img(imgArray, int(args[2]))
    newImage = Image.fromarray(newImage)
    newImage.save("resized_" + inputFile)
    newImage.close()
'''

# Test for color jitter
if __name__ == "__main__":
    args = sys.argv
    if len(args) != 5:
        raise ValueError("Provide image file name and HSV modifiers")
    hsvMod = np.array([float(args[2]), float(args[3]), float(args[4])])
    cst.clampInput(hsvMod)
    inputFile = args[1]
    img = Image.open(inputFile)
    imgArray = np.asarray(img).astype('int64')
    img.close()
    jitteredImg = color_jitter(imgArray, hsvMod[0], hsvMod[1], hsvMod[2])
    jitteredImg = Image.fromarray(jitteredImg)
    jitteredImg.save("jittered_" + inputFile)
    jitteredImg.close()
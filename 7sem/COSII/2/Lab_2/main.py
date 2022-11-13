import sys
import random

import matplotlib.pyplot as plt
from PIL import Image, ImageDraw
from matplotlib import image as mpimg

from ImageLib.Image import Image
from ImageLib.ImageHandler import ImageHandler
from ImageLib.ObjectHandler import ObjectHandler


def main():
    sys.setrecursionlimit(1048576)

    im = Image()
    im.open("./images/P0001460.jpg")

    greyscale = ImageHandler.convert_to_greyscale(im)
    greyscale.save_image("./result/greyscale")
    div = ImageHandler.devide_classes(greyscale)
    print("devision: ", div)
    bin_im = ImageHandler.get_binorized_image(greyscale, div)
    bin_im.save_image("./result/bin")
    after_erosion = ImageHandler.image_erosion(bin_im, 2)
    after_erosion.save_image("./result/after_erosion")
    after_building = ImageHandler.image_building(after_erosion, 2)

    result_im = ImageHandler.median_filter(after_building)

    objects = ObjectHandler.find_object(after_building)
    objectDraw = []
    vectors = []
    for i in range(len(objects)):
        vectors.append((i, objects[i].get_vector()))
        objectDraw.append(objects[i].get_vector())
        print(vectors[i])

    print(str(len(vectors)) + " Objects found ")

    #normalization
    normalize_vectors = []
    for v in vectors:
        normalize_vectors.append((v[0], []))

    for i in range(len(vectors[0][1])):
        array = []
        for v in vectors:
            array.append(v[1][i])

        min_value = min(array)
        max_value = max(array)

        for j in range(len(vectors)):
            normalize_vectors[j][1].append((vectors[j][1][i] - min_value) / (max_value - min_value))

    print(objectDraw[0])

    result_im.save_image("1")
    #img = Image().open('1.jpg')
    img = mpimg.imread('1.jpg')

    #draw = ImageDraw.Draw(img)
    for i in range(len(objects)):
        stra = []
        stra = str(objectDraw[i]).replace("(", "").replace(")", "").split(", ")
        #draw.point(xy=(float(stra[3], float(stra[4]))), fill="red")
        plt.imshow(img)
        plt.scatter([float(stra[0])/10], [float(stra[1])/10], c='r')

    plt.show()
    img.save("./result/colored.jpg")

if __name__ == "__main__":
    main()

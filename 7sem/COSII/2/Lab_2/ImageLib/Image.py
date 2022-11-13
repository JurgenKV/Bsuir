import PIL.Image
import copy
import numpy


class Image:

    def __init__(self, pixels = None, width = None, height = None, mode = None):
        self.___pixels = pixels
        self.__width = width
        self.__height = height
        self.__mode = mode

    def open(self, path):
        image = PIL.Image.open(path)
        self.__width, self.__height = image.size
        self.___pixels = self.__get_channels(list(image.getdata()), image.mode)
        self.__mode = image.mode

    def __get_channels(self, pix, mode):
        if mode == 'RGB':
            channels = [list(), list(), list()]
            for i in range(3):
                for j in range(len(pix)):
                    channels[i].append(pix[j][i])
        else:
            channels = [copy.copy(pix)]
        return channels

    def __pixel_converter(self):
        pix = []

        if self.__mode == "RGB":
            for i in range(self.__width * self.__height):
                pix.append((self.___pixels[0][i],
                            self.___pixels[1][i],
                            self.___pixels[2][i],
                            255))
        else:
            pix = copy.copy(self.___pixels[0])

        return pix

    def save_image(self, file_name):
        im = PIL.Image.new(self.__mode, (self.__width, self.__height))
        pix = self.__pixel_converter()
        im.putdata(pix)
        im.save(file_name + ".jpg", 'JPEG')

    def get_histogram(self):
        histograms = []
        for channel in self.___pixels:
            histogram = numpy.zeros(256)
            for brightness in channel:
                histogram[brightness] += 1

            histograms.append(histogram)

        return histograms

    def get_width(self):
        return self.__width

    def get_height(self):
        return self.__height

    def get_mode(self):
        return self.__mode

    def get_pixels(self):
        return self.___pixels

import math
import numpy as np
import random


class KMedoids:

    @staticmethod
    def __evclid_distance(a, b):
        distance = 0
        for i in range(len(a)):
            distance = distance + (b[i] - a[i]) ** 2
        return math.sqrt(distance)

    @staticmethod
    def __city_distance(a, b):
        distance = 0
        for i in range(len(a)):
            distance = distance + abs(b[i] - a[i])
        return math.sqrt(distance)

    @staticmethod
    def __choose_medoids(k, vectors):
        used = [-1]
        medoids = []

        for i in range(k):
            while (True):
                index = random.randint(0, len(vectors) - 1)

                if index not in used:
                    break

            used.append(index)
            medoids.append(vectors[index])

        return medoids

    @staticmethod
    def clustering(vectors, k, distance=__city_distance, iterations=100):
        if k > len(vectors):
            raise ValueError('Incorrect K value.')

        clusters = [None] * k

        for i in range(iterations):
            distances = []
            medoids = KMedoids.__choose_medoids(k, vectors)

            for vector in vectors:
                distances.extend(list(map(lambda x: distance(x, vector))))

import numpy as np
import math
import copy


def median(arr, mask):
    variational_series = []
    for i in range(0, len(mask), 1):
        for j in range(0, len(mask[i]), 1):
            for _ in range(0, mask[i][j], 1):
                variational_series.append(arr[i][j])
    variational_series.sort()
    return variational_series[math.floor(len(variational_series) / 2)]


def median_filter(noise_img, mask):
    if len(mask) % 2 != 1:
        return "Mask has not odd sum!"
    copy_img = copy.copy(noise_img)
    for i in range(0, len(copy_img), 1):
        for j in range(0, len(copy_img[i]), 1):
            cut = np.zeros(np.shape(mask))
            for p in range(0, len(cut), 1):
                for q in range(0, len(cut[p]), 1):
                    if i - 1 + p > len(copy_img) - 1:
                        p -= 1
                    if j - 1 + q > len(copy_img[i]) - 1:
                        q -= 1
                    cut[p][q] = copy_img[i - 1 + p][j - 1 + q]
            copy_img[i][j] = median(cut, mask)
    return copy_img


if __name__ == "noise.py":
    with open("f.txt", "r") as f:
        data = [int(x) for line in f for x in line.split()]
        size = (data[0], data[1])
        image = np.array(data[2:]).reshape(size)
        W = np.array([[1, 1, 1], [1, 1, 1], [1, 1, 1]], dtype='uint8')
        image = median_filter(W, image)
        print(image)

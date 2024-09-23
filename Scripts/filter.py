import numpy as np
import math
import copy
import cv2


def median(arr, mask):
    variational_series = []
    for i in range(0, len(mask), 1):
        for j in range(0, len(mask[i]), 1):
            for _ in range(0, mask[i][j], 1):
                variational_series.append(arr[i][j])
    variational_series.sort()
    return variational_series[math.floor(len(variational_series) / 2)]


def median_filter(noise_img, mask):
    if mask.shape[0] % 2 != 1:
        return "Mask has not odd sum!"
    copy_img = noise_img.copy()
    for i in range(0, copy_img.shape[0], 1):
        for j in range(0, copy_img.shape[1], 1):
            cut = np.zeros(np.shape(mask))
            for p in range(0, cut.shape[0], 1):
                for q in range(0, cut.shape[1], 1):
                    if i - 1 + p > copy_img.shape[0] - 1:
                        p -= 1
                    if j - 1 + q > copy_img.shape[1] - 1:
                        q -= 1
                    cut[p][q] = copy_img[i - 1 + p][j - 1 + q]
            copy_img[i][j] = median(cut, mask)
    return copy_img


if __name__ == "__main__":
    with open("/Users/vladimirskobcov/Desktop/Labs/TPZRP/Txt/RECEIVED_EYES.txt", "r") as f:
        data = [int(x) for line in f for x in line.split()]
        size = (data[0], data[1])
        image = np.array(data[2:]).reshape(size)
        W = np.array([[1, 1, 1], [1, 1, 1], [1, 1, 1]], dtype='uint8')
        image = median_filter(image, W)
    cv2.imwrite("/Users/vladimirskobcov/Desktop/Labs/TPZRP/Img/RESTORED_EYES.PNG", image)

import numpy as np
import cv2


def get_impulse_noise(arr: np.ndarray, p):
    noise_img = arr.copy()
    noise = np.full(arr.shape, 255)
    for i in range(128):
        for j in range(128):
            if np.random.uniform() < p:
                noise_img[i, j] = np.random.uniform(0, 255)
    return noise_img


if __name__ == "noise.py":
    result = []
    #Откуда читаем изображение
    with open("f.txt", "r") as f:
        data = [int(x) for line in f for x in line.split()]
        size = (data[0], data[1])
        image = np.array(data[2:]).reshape(size)
        noise_img = get_impulse_noise(image, 0.3)
        result.append(noise_img.shape)
        result.append(noise_img.flatten().tolist())
    #куда записываем
    with open('ff.txt', 'w') as f:
        for px in result:
            f.write(str(px) + '\n')








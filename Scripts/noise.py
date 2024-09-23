import numpy as np
import cv2


def get_impulse_noise(arr: np.ndarray, p):
    noise_img = arr.copy()
    noise = np.full(arr.shape, 255)
    for i in range(arr.shape[0]):
        for j in range(arr.shape[1]):
            if np.random.uniform() < p:
                noise_img[i, j] = np.random.uniform(0, 255)
    return noise_img


if __name__ == "__main__":
    image = cv2.imread("/Users/vladimirskobcov/Desktop/Labs/TPZRP/Img/SWEETIE_EYES.PNG", cv2.IMREAD_GRAYSCALE)
    noise_img = get_impulse_noise(image, 0.3)
    result = [image.shape[0], image.shape[1]]
    result.extend(noise_img.flatten().tolist())
    cv2.imwrite("/Users/vladimirskobcov/Desktop/Labs/TPZRP/Img/SWEETIE_EYES_NOISE.PNG", noise_img)
    with open('/Users/vladimirskobcov/Desktop/Labs/TPZRP/Txt/SCREW_EYES.txt', 'w') as f:
        for px in result:
            f.write(str(px) + '\n')
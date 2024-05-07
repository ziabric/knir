import numpy as np
import cv2

def add_gaussian_noise(image, mean=0, std=2):
    """
    Добавляет гауссовский шум к изображению.
    :param image: исходное изображение
    :param mean: среднее значение шума
    :param std: стандартное отклонение шума
    :return: изображение с шумом
    """
    gaussian_noise = np.random.normal(mean, std, image.shape)
    gaussian_noise = gaussian_noise.reshape(image.shape)
    noisy_image = cv2.add(image, gaussian_noise.astype(image.dtype))
    return noisy_image

image_path = './examples/face/face3.png'
image = cv2.imread(image_path)

noisy_image = add_gaussian_noise(image)

cv2.imwrite('./examples/city/g_face3.png', noisy_image)
import cv2
import numpy as np

def add_poisson_noise(image):
    vals = image.astype(float)    
    noisy = np.random.poisson(vals).astype(float)
    noisy_normalized = (255 * (noisy - np.min(noisy)) / np.ptp(noisy)).astype(np.uint8)
    return noisy_normalized

image = cv2.imread('examples/mono/mono4.png')

noisy_image = add_poisson_noise(image)

cv2.imwrite('examples/mono/p_mono4.png', noisy_image)
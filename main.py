import cv2
import numpy as np


def create_blank(width, height, rgb_color=(255, 255, 255)):
    img = np.zeros((height, width, 3), np.uint8)
    color = tuple(reversed(rgb_color))
    img[:] = color
    return img


def rotate_image(image, angle):
    image_center = tuple(np.array(image.shape[1::-1]) / 2)
    rot_mat = cv2.getRotationMatrix2D(image_center, angle, 1.0)
    result = cv2.warpAffine(image, rot_mat, image.shape[1::-1], flags=cv2.INTER_LINEAR)
    return result


img = create_blank(640, 480)
cv2.imshow("blank img", img)
cv2.waitKey(0)
img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
cv2.imshow("blank img", img)
cv2.waitKey(0)

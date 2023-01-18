import cv2
import numpy as np

WHOLE_IMAGE_WIDTH = 710
WHOLE_IMAGE_HEIGHT = 710
FRAME_WIDTH = 500
FRAME_HEIGHT = 500
STEP = 2
PATH_WIDTH = 2
ROTATE_SCALE = 1
FPS = int(1000 / 30)
# FPS = 10
WINDOW_NAME = "Whole Image"
cv2.namedWindow(WINDOW_NAME)
# fourcc = cv2.cv.CV_FOURCC()
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
# fourcc = cv2.VideoWriter_fourcc('H','2','6','4')
VIDEO = cv2.VideoWriter('output.mp4', fourcc, 30.0, (FRAME_HEIGHT, FRAME_WIDTH))


def create_blank(width, height, rgb_color=(0, 0, 0)):
    img = np.zeros((height, width, 3), np.uint8)
    color = tuple(reversed(rgb_color))
    img[:] = color
    return img


def rotate_image(image, angle):
    image_center = tuple(np.array(image.shape[1::-1]) / 2)
    rot_mat = cv2.getRotationMatrix2D(image_center, angle, 1.0)
    result = cv2.warpAffine(image, rot_mat, image.shape[1::-1], flags=cv2.INTER_LINEAR)
    return result


def move_forward(whole_image):
    whole_image[STEP:, :] = whole_image[:-STEP, :]
    whole_image[:STEP, :] = (0, 0, 0)
    frame = whole_image[int(WHOLE_IMAGE_WIDTH / 2 - FRAME_WIDTH / 2):int(WHOLE_IMAGE_WIDTH / 2 + FRAME_WIDTH / 2),
            int(WHOLE_IMAGE_HEIGHT / 2 - FRAME_HEIGHT / 2):int(WHOLE_IMAGE_HEIGHT / 2 + FRAME_HEIGHT / 2)]
    return frame, whole_image


def get_frame(whole_image):
    frame = whole_image[int(WHOLE_IMAGE_WIDTH / 2 - FRAME_WIDTH / 2):int(WHOLE_IMAGE_WIDTH / 2 + FRAME_WIDTH / 2),
            int(WHOLE_IMAGE_HEIGHT / 2 - FRAME_HEIGHT / 2):int(WHOLE_IMAGE_HEIGHT / 2 + FRAME_HEIGHT / 2)]
    return frame


def place_left_corner(whole_image):
    whole_image = cv2.rectangle(whole_image, (1, 1),
                                (int(WHOLE_IMAGE_WIDTH / 2 + PATH_WIDTH / 2 - 1), int(PATH_WIDTH + 1)),
                                (255, 0, 0),
                                thickness=-1)
    return whole_image


def place_right_corner(whole_image):
    whole_image = cv2.rectangle(whole_image, (int(WHOLE_IMAGE_WIDTH / 2 - PATH_WIDTH / 2 - 1), 1),
                                (int(WHOLE_IMAGE_WIDTH - 2), int(PATH_WIDTH + 1)),
                                (255, 0, 0),
                                thickness=-1)
    return whole_image


def place_forward(whole_image):
    whole_image = cv2.rectangle(whole_image, (int(WHOLE_IMAGE_WIDTH / 2 - PATH_WIDTH / 2 - 1), 1),
                                (int(WHOLE_IMAGE_WIDTH / 2 + PATH_WIDTH / 2 - 1), int(WHOLE_IMAGE_HEIGHT / 2)),
                                (255, 0, 0),
                                -1)
    return whole_image


def rotate_left(whole_image):
    global VIDEO
    angle = 90 / (FPS * ROTATE_SCALE)
    counter = 90 / angle
    for count in range(int(counter)):
        whole_image = rotate_image(whole_image, -angle)
        frame = get_frame(whole_image)
        VIDEO.write(frame)
        cv2.imshow(WINDOW_NAME, whole_image)
        cv2.waitKey(FPS)
    return whole_image


def rotate_right(whole_image):
    global VIDEO
    angle = 90 / (FPS * ROTATE_SCALE)
    counter = 90 / angle
    for count in range(int(counter)):
        whole_image = rotate_image(whole_image, angle)
        frame = get_frame(whole_image)
        VIDEO.write(frame)
        cv2.imshow(WINDOW_NAME, whole_image)
        cv2.waitKey(FPS)
    return whole_image


def keep_moving_forward(whole_image, seconds):
    global VIDEO
    distance = seconds * FPS
    for step in range(int(distance)):
        _, whole_image = move_forward(whole_image)
        whole_image = place_forward(whole_image)
        frame = get_frame(whole_image)
        VIDEO.write(frame)
        cv2.imshow(WINDOW_NAME, whole_image)
        cv2.waitKey(FPS)
    return whole_image


def keep_going(whole_image):
    global VIDEO
    while True:
        if whole_image[353, 1][0] == 255 and \
                whole_image[354, 1][0] == 255 and \
                whole_image[355, 1][0] == 255:
            whole_image = rotate_left(whole_image)
            break
        if whole_image[353, 708][0] == 255 and \
                whole_image[354, 708][0] == 255 and \
                whole_image[355, 708][0] == 255:
            whole_image = rotate_right(whole_image)
            break
        frame, whole_image = move_forward(whole_image)
        VIDEO.write(frame)
        cv2.imshow(WINDOW_NAME, whole_image)
        cv2.waitKey(FPS)
    return whole_image

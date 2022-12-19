from DrawingUtils import *
import DrawingUtils as DU
import time

def main():
    whole_image = create_blank(DU.WHOLE_IMAGE_WIDTH, DU.WHOLE_IMAGE_HEIGHT)
    whole_image = place_forward(whole_image)
    whole_image = place_right_corner(whole_image)
    whole_image = DU.keep_going(whole_image)
    # start = time.time()
    whole_image = DU.keep_moving_forward(whole_image, 2)
    # end = time.time()
    # print(end - start)
    whole_image = DU.place_left_corner(whole_image)
    whole_image = DU.keep_going(whole_image)
    whole_image = DU.keep_moving_forward(whole_image, 10)
    whole_image = DU.place_left_corner(whole_image)
    whole_image = DU.keep_going(whole_image)
    whole_image = DU.place_left_corner(whole_image)
    whole_image = DU.keep_going(whole_image)
    whole_image = DU.place_right_corner(whole_image)
    whole_image = DU.keep_going(whole_image)
    whole_image = DU.place_right_corner(whole_image)
    whole_image = DU.keep_going(whole_image)
    whole_image = DU.keep_moving_forward(whole_image, 10)
    for i in range(int(710 / (2 * 2)) - 1):
        _, whole_image = DU.move_forward(whole_image)
        cv2.imshow(WINDOW_NAME, whole_image)
        cv2.waitKey(FPS)
    cv2.waitKey(0)
    return 0


if __name__ == "__main__":
    main()

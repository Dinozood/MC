import cv2
import numpy as np
import time
import spidev
#//			GPIOA->ODR ^= (2 << 8); // for right 10 is backward, so 01 is forward;
#//			GPIOA->ODR ^= (2 << 10); // same for left

states = {"forward, right, left"}
state = None


class Analiser:
    def __init__(self):
        pass
        self.spi = spidev.SpiDev()
        self.spi.open(0, 0)
        self.spi.max_speed_hz = 1 * (10 ** 6)

        # to_send = [1, 2, 3, 5, 5, 6, 7, 8]
        # spi.xfer(to_send)

    def say_forward(self):
        global state
        to_send = [1, 1]
        self.spi.xfer(to_send)
        if state != "forward":
            state = "forward"
            print("forward")

    def say_left(self):
        global state
        to_send = [2, 1]
        self.spi.xfer(to_send)
        if state != "left":
            state = "left"
            print("left")

    def say_right(self):
        global state
        to_send = [1, 2]
        self.spi.xfer(to_send)
        if state != "right":
            state = "right"
            print("right")

    def go_forward(self):
        self.say_forward()

    def rotate_right(self, cap):
        self.say_right()
        while cap.isOpened():
            ret, frame = cap.read()
            if ret:
                # Display the resulting frame
                # cv2.imshow('Frame', frame)
                time.sleep(0.033)
                cv2.waitKey(25)
                tmp = frame[0][250:252, 0]
                sum = tmp[0] + tmp[1]
                if sum > 150:
                    self.go_forward()
                    break

    def rotate_left(self, cap):
        self.say_left()
        while cap.isOpened():
            ret, frame = cap.read()
            if ret:
                # Display the resulting frame
                # cv2.imshow('F
                # rame', frame)
                time.sleep(0.033)
                cv2.waitKey(25)
                tmp = frame[0][250:252, 0]
                sum = int(tmp[0]) + int(tmp[1])
                if sum > 150:
                    self.go_forward()
                    break

    def start_analize(self, cap):
        global state
        while cap.isOpened():
            ret, frame = cap.read()
            if ret:
                # Display the resulting frame
#                 # cv2.imshow('Frame', frame)
                tmp = frame[0][250:252, 0]
                sum = int(tmp[0]) + int(tmp[1])
                if sum > 150:
                    self.go_forward()
                else:
                    tmp_r = frame[248:250, 499, 0]
                    sum_r = tmp_r[0] + tmp_r[1]
                    if sum_r > 150:
                        self.rotate_right(cap)
                    tmp_l = frame[248:250, 0, 0]
                    sum_l = tmp_l[0] + tmp_l[1]
                    if sum_l > 150:
                        self.rotate_left(cap)
                self.go_forward()
                # Press Q on keyboard to  exit

                time.sleep(0.033)
                if cv2.waitKey(25000) & 0xFF == ord('q'):
                    break
            else:
                break
                # When everything done, release the video capture object


def main():
    visor = Analiser()
    cap = cv2.VideoCapture("/home/pi/Projects/MC/output.avi")
    # cap = cv2.VideoCapture("/home/dinozood/Projects/IFMO/MC/output.avi")

    if not cap.isOpened():
        print("Error opening video stream or file")

        # Capture frame-by-frame
    visor.start_analize(cap)
    cap.release()

    # Closes all the frames
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()

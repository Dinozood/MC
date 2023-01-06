import cv2
import numpy as np

states = {"forward, right, left"}
state = None


def say_forward():
    global state
    if state is not "forward":
        state = "forward"
        print("forward")


def say_left():
    global state
    if state is not "left":
        state = "left"
        print("left")


def say_right():
    global state
    if state is not "right":
        state = "right"
        print("right")


def go_forward():
    say_forward()


def rotate_right(cap):
    say_right()
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            # Display the resulting frame
            cv2.imshow('Frame', frame)
            cv2.waitKey(25)
            tmp = frame[0][250:252, 0]
            sum = tmp[0] + tmp[1]
            if sum > 150:
                go_forward()
                break


def rotate_left(cap):
    say_left()
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            # Display the resulting frame
            cv2.imshow('Frame', frame)
            cv2.waitKey(25)
            tmp = frame[0][250:252, 0]
            sum = int(tmp[0]) + int(tmp[1])
            if sum > 150:
                go_forward()
                break


def start_analize(cap):
    global state
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            # Display the resulting frame
            cv2.imshow('Frame', frame)
            tmp = frame[0][250:252, 0]
            sum = int(tmp[0]) + int(tmp[1])
            if sum > 150:
                go_forward()
            else:
                tmp_r = frame[248:250, 499, 0]
                sum_r = tmp_r[0] + tmp_r[1]
                if sum_r > 150:
                    rotate_right(cap)
                tmp_l = frame[248:250, 0, 0]
                sum_l = tmp_l[0] + tmp_l[1]
                if sum_l > 150:
                    rotate_left(cap)
            if state is not "forward":
                state = "forward"
                print("forward")
            # Press Q on keyboard to  exit
            if cv2.waitKey(25) & 0xFF == ord('q'):
                break
        else:
            break
            # When everything done, release the video capture object


def main():
    cap = cv2.VideoCapture("/home/dinozood/Projects/IFMO/MC/output.avi")
    if not cap.isOpened():
        print("Error opening video stream or file")

        # Capture frame-by-frame
    start_analize(cap)
    cap.release()

    # Closes all the frames
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()

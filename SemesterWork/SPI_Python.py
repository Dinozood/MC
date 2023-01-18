import spidev

spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 1 * (10 ** 6)
to_send = [1, 2, 3, 5, 5, 6, 7, 8]

spi.xfer(to_send)

print()



# import the opencv library
# import cv2
# import os
# print("here")
# define a video capture object
# vid = cv2.VideoCapture(0)
#print("Couldn't get videostream")
#raise BlockingIOError

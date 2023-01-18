import spidev

spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 1 * (10 ** 6)
to_send = [1, 2, 3, 5, 5, 6, 7, 8, 9, 10]

spi.xfer(to_send)
rec = spi.readbytes(10)

print(rec)
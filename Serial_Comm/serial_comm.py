from numpy import datetime_as_string
import serial
import matplotlib.pyplot as plt
from drawnow import *
import time

data = []
t = []
sensor_res = 1024
serial_port = "COM5"
baud_rate = 9600
arduino = serial.Serial(serial_port, baud_rate)

temp_t = 0


def make_figure():
    plt.ylim(-0.1, 6)
    plt.grid(True)
    plt.plot(t, data, 'Clo--')
    return


# Numero de muestras
N = 30

for i in range(N):
    try:
        data_ard = arduino.readline()
        data_str = str(data_ard)
        temp_data = float(data_str) * (5.0 / sensor_res)
        print(temp_data)
        t.append(temp_t)
        temp_t += 0.5
        data.append(temp_data)
        drawnow(make_figure)
    except:
        print("Error leyendo puerto serie.")
        pass
    time.sleep(0.5)  # seg

arduino.close()

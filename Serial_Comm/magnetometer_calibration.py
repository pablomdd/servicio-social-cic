import matplotlib.pyplot as plt
import numpy as np
import csv

def getMagnetometerCalibrationParameters(x, y):
    x_min = x.min()
    x_max = x.max()
    y_min = y.min()
    y_max = y.max()

    # Scale Factor
    x_sf = y_max - y_min / x_max - x_min
    y_sf = x_max - x_min / y_max - y_min
    # Offset
    x_off = ((x_max - x_min / 2) - x_max) * x_sf
    y_off = ((y_max - y_min / 2) - y_max) * y_sf

    return x_sf, y_sf, x_off, y_off
def getMagnetometerCalibrationValues(x, y):
    x_sf, y_sf, x_off, y_off = getMagnetometerCalibrationParameters(x, y)
    mx = np.array([])
    my = np.array([])
    for x_i, y_i in np.nditer([x, y]):
        mx_i = x_sf * x_i + x_off
        my_i = y_sf * y_i + y_off
        mx = np.append(mx, mx_i)
        my = np.append(my, my_i)
    return mx, my

#def draw_calibration(x, y):


if __name__ == "__main__":
    x = np.array([])
    y = np.array([])
    with open('data.csv', mode='r') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=",")
        line_count = 0
        for row in csv_reader:
            if line_count == 0:
                print(f'Column names are {", ".join(row)}')
                line_count += 1
                continue
            x = np.append(x, float(row[0]))
            y = np.append(y, float(row[1]))

            print(f'\t{row[0]} , {row[1]}')
            line_count += 1
        print(f'Processed {line_count} lines.')
    mx,my = getMagnetometerCalibrationValues(x, y)
    print(mx, my)

    plt.style.use('ggplot')
    fig, ax = plt.subplots(1, 2, figsize=(12, 7))
    
    ax[0].scatter(x, y)
    ax[1].scatter(mx, my)
    ax[0].set_xlabel("Mx(mT)")
    ax[0].set_ylabel("My(mT)")
    ax[0].set_title("Before Calibration")
    ax[1].set_xlabel("Mx(mT)")
    ax[1].set_ylabel("My(mT)")
    ax[1].set_title("After Calibration")

    # Add Graph to Canvas
    plt.show()
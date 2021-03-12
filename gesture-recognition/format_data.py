# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import pandas as pd
import numpy as np
import csv
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import sys


# Takes in a gesture entry and interpolates 150 data points

def interpolate(entry):
    time = entry[:, 0]
    x, y, z = entry[:, 1], entry[:, 2], entry[:, 3]

    fx = interp1d(time, x, kind='cubic')
    fy = interp1d(time, y, kind='cubic')
    fz = interp1d(time, z, kind='cubic')
    new_time = np.linspace(time[0], time[-1], 150)
    interpolated_x = fx(new_time)
    interpolated_y = fy(new_time)
    interpolated_z = fz(new_time)
    interpolated_entry = np.array([interpolated_x, interpolated_y, interpolated_z]).T
    return interpolated_entry


def format_data(file_path):
    data = []
    size = 0
    with open(file_path) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        entry = []
        for row in csv_reader:
            if row[0] == 'EXIT':
                try:
                    entry = interpolate(np.array(entry))
                    data.append(entry)
                except:
                    print('error')
                size += 1
            elif row[0] == 'BEGIN':
                entry = []
            else:
                entry.append([float(i) for i in row[:4]])
    print(f'Number of entries: {size}')
    return data


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    data_O = format_data(input_path)
    data_example = pd.DataFrame(data_O[0], columns=['x', 'y', 'z'])
    np.save(output_path, data_example)
    print(data_example)

# See PyCharm help at https://www.jetbrains.com/help/pycharm/

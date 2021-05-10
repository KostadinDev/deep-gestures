
import pandas as pd
import numpy as np
import csv
from scipy.interpolate import interp1d
from mpl_toolkits import mplot3d
import sys
import matplotlib.pyplot as plt

date = True


# Takes in a gesture entry and interpolates 128 data points
def interpolate(entry):
    time = entry[:, 0]
    x, y, z = entry[:, 1], entry[:, 2], entry[:, 3]

    fx = interp1d(time, x, kind='cubic')
    fy = interp1d(time, y, kind='cubic')
    fz = interp1d(time, z, kind='cubic')
    new_time = np.linspace(time[0], time[-1], 128)
    interpolated_x = fx(new_time)
    interpolated_y = fy(new_time)
    interpolated_z = fz(new_time)
    interpolated_entry = np.array([interpolated_x, interpolated_y, interpolated_z]).T
    return interpolated_entry


def format_data(file_path, label):
    data = []
    size = 0
    with open(file_path) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        entry = []
        for row in csv_reader:
            if date:
                row[0] = row[0][16:]
            if row[0] == 'EXIT':
                try:
                    entry = interpolate(np.array(entry))
                    data.append(np.array([entry, label]))
                except:
                    print('')
                size += 1
            elif row[0] == 'BEGIN':
                entry = []
            else:
                entry.append([float(i) for i in row[:4]])
    return data


# python format_data.py input_location output_location label
if __name__ == '__main__':
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    label = sys.argv[3]
    data = format_data(input_path, label)
    np.save(output_path, data, allow_pickle=True)
    print(data)



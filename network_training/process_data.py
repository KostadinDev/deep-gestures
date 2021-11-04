
import pandas as pd
import numpy as np
import csv
from scipy.interpolate import interp1d
import sys
import matplotlib.pyplot as plt
from data_augmentation import data_augment

date = True

# This file is intended to process the raw .csv file of accelerometer data

# Takes in a gesture entry and interpolates 128 data points
def interpolate(entry):
    time = entry[:, 0]
    x, y, z = entry[:, 1], entry[:, 2], entry[:, 3]

    # Uncomment to plot Acceleration vs Time
    
    # plt.subplot(1, 2, 1)
    # plt.scatter(time, x, label = "x", s=10)
    # plt.scatter(time, y, label = "y", s=10)
    # plt.scatter(time, z, label = "z", s=10)
    # plt.title('Gesture: LR')
    # plt.xlabel('Time (ms)')
    # plt.ylabel('Acceleration m/s^2')
    # plt.legend()

    fx = interp1d(time, x, kind='cubic')
    fy = interp1d(time, y, kind='cubic')
    fz = interp1d(time, z, kind='cubic')
    new_time = np.linspace(time[0], time[-1], 128)
    interpolated_x = fx(new_time)
    interpolated_y = fy(new_time)
    interpolated_z = fz(new_time)

    # Uncomment to plot Acceleration vs Time (Interpolated data)

    # plt.subplot(1, 2, 2)
    # plt.scatter(new_time, interpolated_x, label = "x", s=10)
    # plt.scatter(new_time, interpolated_y, label = "y", s=10)
    # plt.scatter(new_time, interpolated_z, label = "z", s=10)
    # plt.title('Interpolated Gesture: LR')
    # plt.xlabel('Time (ms)')
    # plt.ylabel('Acceleration m/s^2')
    # plt.legend()
    # plt.show()

    interpolated_entry = np.array([interpolated_x, interpolated_y, interpolated_z]).T
    return interpolated_entry


def format_data(file_path, label, data_format, data_augment_flag):
    data = []
    # size = 0
    with open(file_path) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        entry = []
        for row in csv_reader:
            if data_format == 'Category':
                #Remove the unneccessary beginning characters
                row[0] = row[0][16:]
            if row[0] == 'EXIT':
                try:
                    raw_entry = entry.copy()
                    entry = interpolate(np.array(entry))
                
                    data.append(np.array([entry, label]))
                    # Apply data augmentation from the current entry
                    if data_augment_flag:
                        for _ in range(10):
                            augment_entry = data_augment(raw_entry)
                            data.append(np.array([augment_entry, label]))
                except:
                    pass
                # size += 1
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
    data_format = 'Category'
    try:
        data_format = sys.argv[4]
    except:
        pass
    data_augment_flag = True
    try:
        data_augment_flag = False if sys.argv[5] == "False" else True
    except:
        pass
    data = format_data(input_path, label, data_format, data_augment_flag)
    np.save(output_path, data, allow_pickle=True)
import numpy as np
from scipy.interpolate import interp1d
import random

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

# Takes an entry for data and randomizes the points within a small range
def randomize_features_within_range(entry):
    time = entry[:, 0]
    # percentages_arr = [(1/15), (1/20), (1/12), (1/10), (1/17), (1/11), (1/13), (1/16)]
    # x_perc = random.choice(percentages_arr)
    # y_perc = random.choice(percentages_arr)
    # z_perc = random.choice(percentages_arr)
    x, y, z = entry[:, 1], entry[:, 2], entry[:, 3]
    for i in range(len(x)):
        x[i] += random.uniform(-(1/15)*x[i], (1/15)*x[i])
        y[i] += random.uniform(-(1/15)*y[i], (1/15)*y[i])
        z[i] += random.uniform(-(1/15)*z[i], (1/15)*z[i])
    entry[:,1] = x
    entry[:,2] = y
    entry[:,3] = z
    return entry
    

# Augment the given data entry to produce new data points from existing data
def data_augment(entry):
    entry_list = entry.tolist()
    percent_arr = [(4//5), (5//6), (3//4), (6//7)]
    index_arr = random.sample(range(len(entry_list)), random.choice(percent_arr)*len(entry_list))
    new_entry_list = entry_list[index_arr]
    adjusted_entry_list = randomize_features_within_range(new_entry_list)
    return interpolate(np.array(adjusted_entry_list))


import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras import layers, models, backend as K
from tensorflow import keras
import matplotlib.pyplot as plt
from sklearn.metrics import f1_score, precision_score, recall_score, classification_report
import os
import sys


# MEANT FOR DATA EVALUATION PURPOSES ONLY
if __name__ == '__main__':

    data_folder, model_input = sys.argv[1:3]

    data = []
    # Load data from folder
    for root, dirs, files in os.walk(data_folder):
        for file in files:
            current = np.load(root + '/' + file, allow_pickle=True)
            try:
                if current.shape[1] == 2:
                    data.append(current)
            except IndexError:
                print(f'Improper data for {file}: should be of shape (n, 2)')

    data = np.vstack(data)
    np.random.shuffle(data)

    # Get X data and format into training format
    X = data[:, 0]
    X = np.vstack(X).astype('float32')
    X = np.reshape(X, (int(len(X) / 128), 128, 3, -1))

    # Get y data
    y = data[:, 1].astype(np.float)

    model = tf.keras.models.load_model(model_input)
    y_pred = np.argmax(model.predict(X), axis=1)

    # Prints a Metric Report of the model
    print(classification_report(y, y_pred, target_names=['0','1','2']))

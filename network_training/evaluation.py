import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras import layers, models, backend as K
from tensorflow import keras
import matplotlib.pyplot as plt
import os
import sys

if __name__ == '__main__':

    data_folder, model_output = sys.argv[1:3]

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
    # print("Standard deviation")
    # np.std(X)
    # print("MEAN: ")
    # np.mean(X)
    # # Normalize the data
    # print("Before:")
    # print(X)
    # keras.utils.normalize(
    #     X, axis=-1, order=2
    # )
    # print("After")
    # print(X)

    X = np.reshape(X, (int(len(X) / 128), 128, 3, -1))

    # Get y data
    y = data[:, 1].astype(np.float)

    # Split into 60% for training, 20% for validation, 20% testing
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=1)
    X_train, X_val, y_train, y_val = train_test_split(X_train, y_train, test_size=0.25, random_state=1)


    def recall_m(y_true, y_pred):
        true_positives = K.sum(K.round(K.clip(y_true * y_pred, 0, 1)))
        possible_positives = K.sum(K.round(K.clip(y_true, 0, 1)))
        recall = true_positives / (possible_positives + K.epsilon())
        return recall


    def precision_m(y_true, y_pred):
        true_positives = K.sum(K.round(K.clip(y_true * y_pred, 0, 1)))
        predicted_positives = K.sum(K.round(K.clip(y_pred, 0, 1)))
        precision = true_positives / (predicted_positives + K.epsilon())
        return precision


    def f1_m(y_true, y_pred):
        precision = precision_m(y_true, y_pred)
        recall = recall_m(y_true, y_pred)
        return 2 * ((precision * recall) / (precision + recall + K.epsilon()))



    model = tf.keras.models.load_model('lite_models/model_keras.h5')
    model.summary()
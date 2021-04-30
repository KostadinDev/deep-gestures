import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras import layers, models
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
    X = np.reshape(X, (int(len(X) / 128), 128, 3, -1))

    # Get y data
    y = data[:, 1].astype(np.float)

    # Split into 60% for training, 20% for validation, 20% testing
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=1)
    X_train, X_val, y_train, y_val = train_test_split(X_train, y_train, test_size=0.25, random_state=1)

    # CNN architecture
    model = models.Sequential([layers.Conv2D(8, (4, 3), padding='same', activation='relu', input_shape=(128, 3, 1)),
                               layers.MaxPool2D((3, 3)),
                               layers.Conv2D(16, (4, 1), padding='same', activation='relu'),
                               layers.MaxPool2D((3, 1), padding='same'),
                               layers.Flatten(),
                               layers.Dense(16, activation='relu'),
                               layers.Dense(5, activation='softmax')])

    # Set up hyper parameters
    model.compile(optimizer='adam', loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
                  metrics=['accuracy'])

    # Start training
    history = model.fit(X_train, y_train, epochs=50, validation_data=(X_val, y_val))

    # Evaluate
    test_loss, test_acc = model.evaluate(X_test, y_test, verbose=2)

    # Convert model to Tensorflow Lite
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()

    # Save model
    if not os.path.exists(model_output):
        os.makedirs(model_output)

    with open(model_output + "/" + 'model.tflite', 'wb') as f:
        f.write(tflite_model)

import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras import datasets, layers, models
import matplotlib.pyplot as plt

if __name__ == '__main__':
    # load in formatted data
    data_O = np.load('output/data_O_formatted.npy', allow_pickle=True)
    data_LR = np.load('output/data_LR_formatted.npy', allow_pickle=True)

    # join and shuffle formatted data
    data = np.vstack((data_O, data_LR))
    np.random.shuffle(data)
    X = data[:, 0]
    y = data[:, 1].astype(np.float)
    X = np.vstack(X).astype('float32')
    X = np.reshape(X, (int(len(X) / 128), 128, 3, -1))

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=1)

    X_train, X_val, y_train, y_val = train_test_split(X_train, y_train, test_size=0.25, random_state=1)

    print("Training shapes: ", X_train.shape, X_val.shape, X_test.shape)

    print("Testing shapes: ", y_train.shape, y_val.shape, y_test.shape)

    model = models.Sequential([layers.Conv2D(8, (4, 3), padding='same', activation='relu', input_shape=(128, 3, 1)),
                               layers.MaxPool2D((3, 3)),
                               layers.Conv2D(16, (4, 1), padding='same', activation='relu'),
                               layers.MaxPool2D((3, 1), padding='same'),
                               layers.Flatten(),
                               layers.Dense(16, activation='relu'),
                               layers.Dense(4, activation='softmax')])

    model.compile(optimizer='adam', loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
                  metrics=['accuracy'])

    history = model.fit(X_train, y_train, epochs=200, validation_data=(X_val, y_val))

    plt.plot(history.history['accuracy'], label='accuracy')
    plt.plot(history.history['val_accuracy'], label='val_accuracy')
    plt.xlabel('Epoch')
    plt.ylabel('Accuracy')
    plt.ylim([0.5, 1])
    plt.legend(loc='lower right')

    test_loss, test_acc = model.evaluate(X_test, y_test, verbose=2)

import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras import layers, models, backend as K
from tensorflow import keras
import matplotlib.pyplot as plt
import os
import sys
from sklearn.metrics import f1_score, precision_score, recall_score, classification_report


if __name__ == '__main__':

    data_folder, model_output, file_type = sys.argv[1:4]

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
    model = models.Sequential([layers.LayerNormalization(axis=1, center=True, scale=True),
                               layers.Conv2D(8, (3, 3), padding='same', input_shape=(128, 3, 1)),
                               layers.BatchNormalization(),
                               layers.Activation("relu"),
                               layers.MaxPool2D((2, 2), padding='same', strides=2),
                               layers.Conv2D(16, (3, 3), padding='same', activation='relu'),
                               layers.BatchNormalization(),
                               layers.MaxPool2D((2, 2), padding='same', strides=2),
                               layers.Flatten(),
                               layers.Dense(16, activation='relu'),
                               layers.BatchNormalization(),
                               layers.Dense(5, activation='softmax')])

    # Set up hyper parameters
    optimizer = keras.optimizers.Adam(learning_rate=0.001)

    model.compile(optimizer=optimizer, loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True), metrics=['accuracy'])

    # Start training
    history = model.fit(X_train, y_train, epochs=20, validation_data=(X_val, y_val))

    # Evaluate

    y_pred = np.argmax(model.predict(X_test), axis=1)

    # Prints the Classification Metrics Report
    print(classification_report(y_test, y_pred, target_names=['0','1','2','3','4']))


    # 
    # training_loss = history.history['loss']
    # val_loss = history.history['val_loss']
    #
    # training_accuracy = history.history['accuracy']
    # test_accuracy = history.history['val_accuracy']

    # Create count of the number of epochs
    # epoch_count = range(1, len(training_loss) + 1)
    

    # # Visualize loss history
    # plt.plot(epoch_count, training_loss, 'r--')
    # plt.plot(epoch_count, val_loss, 'b-')
    # plt.legend(['Training Loss', 'Test Loss'])
    # plt.xlabel('Epoch')
    # plt.ylabel('Loss')
    # plt.show()

    # epoch_count = range(1, len(training_accuracy) + 1)
    # plt.plot(epoch_count, training_accuracy, 'r--')
    # plt.plot(epoch_count, test_accuracy, 'b-')
    # plt.legend(['Training Accuracy', 'Test Accuracy'])
    # plt.xlabel('Epoch')
    # plt.ylabel('Accuracy')
    # plt.show()

    # Saving the kera model 
    if str(file_type) == '0':
        # Save as a .h5 file
        model_output = model_output + "/" + "model.h5"
        model.save(model_output, save_format="h5")
    else:
        # Convert model to Tensorflow Lite
        converter = tf.lite.TFLiteConverter.from_keras_model(model)
        tflite_model = converter.convert()
        
        # Save model
        if not os.path.exists(model_output):
            os.makedirs(model_output)
        
        with open(model_output + "/" + 'model.tflite', 'wb') as f:
            f.write(tflite_model)
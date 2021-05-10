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

    X = np.reshape(X, (int(len(X) / 128), 128, 3, -1))
    

    # Get y data
    y = data[:, 1].astype(np.float)
    
    # Split into 80% for training, 20% for validation
    X_train, X_val, y_train, y_val = train_test_split(X, y, test_size=0.2, random_state=1)

    #Custom metric functions

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
        return 2*((precision*recall)/(precision+recall+K.epsilon()))

    ###

    #Load in our previously saved Kera model
    model = tf.keras.models.load_model('lite_models/model_keras.h5')
    
    #Prints out a summary of the loaded in model
    model.summary()

    #Remove the last layer and add in a new Dense layer that matches the amount of output channels that we want
    model.pop()
    model.add(layers.Dense(3, activation = 'softmax', name='dense_tune'))
    

    # Set up hyper parameters
    optimizer = keras.optimizers.Adam(learning_rate=0.001)
    model.compile(optimizer=optimizer, loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
                  metrics=['accuracy', f1_m, precision_m, recall_m])

    # Start training
    history = model.fit(X_train, y_train, epochs=20, validation_data=(X_val, y_val))
    # Evaluate validation data
    test_loss, test_acc, test_f1, test_prec, test_recall = model.evaluate(X_val, y_val, verbose=2)
    
    training_loss = history.history['loss']
    val_loss = history.history['val_loss']
    
    training_f1m = history.history['f1_m']
    val_f1m = history.history['val_f1_m']

    # Create count of the number of epochs
    epoch_count = range(1, len(training_loss) + 1)
    
    # Visualize loss history
    plt.plot(epoch_count, training_loss, 'r--')
    plt.plot(epoch_count, val_loss, 'b-')
    plt.legend(['Training Loss', 'Validation Loss'])
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.show()

    epoch_count = range(1, len(training_f1m) + 1)
    plt.plot(epoch_count, training_f1m, 'r--')
    plt.plot(epoch_count, val_f1m, 'b-')
    plt.legend(['Training F1', 'Validation F1'])
    plt.xlabel('Epoch')
    plt.ylabel('F1')
    plt.show()

    # Convert model to Tensorflow Lite
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()

    # Save model
    if not os.path.exists(model_output):
        os.makedirs(model_output)

    with open(model_output + "/" + 'model_testing_data.tflite', 'wb') as f:
        f.write(tflite_model)

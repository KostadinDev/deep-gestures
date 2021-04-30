# Gesture Recognition Pipeline

Complete pipeline to create an application that can recognize custom gestures made on an Arduino Nano BLE Sense. 

The pipeline includes data collection on an Arduino, data processing, neural network training, model conversion, model deployment on Arduino, and sending the gesure prediction to a server. 

## Collect Data

In order to collect data, we used a basic triggering system in order to tell the arduino when to
collect the data. From there it was printed to the Serial Port, detected and saved as a csv using PuTTY.

#### Steps
1. Using an Arduino Nano 33 BLE Sense and a button, create a circuit that will connect the vOut pin
to an input datapin whenever the button is depressed. This acts as a trigger for the start of the movement.
2. Run the script provided within 'datacollection'. This script prints the output of the IMU within the
Arduino Nano to the Serial Monitor.
3. Download Putty, setting the 'Connection Type' to 'Serial', the 'Serial line' box to the same 
Serial Port that the Arduino Nano is set to, and the 'Speed' to your Arduino Nano's 'Baud Rate'
4.  Collect Data and save it within a csv file.

## Process Data
The data processing script <b>network_training/format_data.py</b> converts the output of the Arduino sensors into numpy arrays that are later used to train the machine learning model.

### Steps:
1. Put the collected data csv files in network_training/collected_data
2. Open the network training folder
	>cd network_training/
3. For each different gesture run the command below specifying th collected data, where to save the processed output, and  the label of the gesture. Keep in mind the label should be numeric. There are example files in network_training/collected_data and network_training/processed_data.
	>python format_data.py <collected_data_csv> <saving_location> \<label>
	
	Example:
	>python format_data.py collected_data/collected_LRdash.csv processed_data/processed_LRdash.npy 0


## Train Model
By this point you should have a folder with processed data with .npy files. Now we will train a model with this data. The model is trained with a convolutional neural network (CNN) in Tensorflow. The data is split into training (60%) , validation (20%), and testing (20%). 
 
### Steps:
1. Open the network training folder
	>cd network_training/
2. Run the training script
    >python train.py <processed_data_folder> <model_saving_location_folder> \<label>
    
	Example:
	>python train.py processed_data/ lite_models/

## Model Conversion
After obtaining the Tensforflow Lite model as .tflite file you have to convert it to a c++ file and put it in your Arduino project.

### Steps:
1. Place the folder with tensorflow lite models in Google Drive
2. Open the convert_model.ipynb jupyter notebook in Google Colab
3. Run the code blocks in the Colab notebook and a model.cc file will be generated
4. Place the model.cc file in your Arduino project

## Deployment

For Deployment are running a script similar to the datacollection script. The input in discrete chunks
using a button trigger, interpolated linearly, and then inferenced.

#### Steps
1. Use the exact same circuit at the one used within the data collection phase.
2. Add five LED's wired to light if any of the pins 3, 4, 5, 6 and 7 output `HIGH` signals
3. Open the 'arduino-imp.ino' script located at deployment\arduino-imp\arduino-imp.ino
4. The arduino editor should open up with four tabs:
    - 'arduino-imp.ino' - This is the main script that will be uploaded
    - 'primer.h' - header file for primer.cpp file that outlines helper methods
    - 'primer.cpp' - cpp file that contains the linear interpolation funcions and array compiler.
    - 'model.h' - header file that contains the TensorFlowLite model
5. Upload the code and test.

## Connect to a Server

TBD

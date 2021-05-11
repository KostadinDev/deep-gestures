READ ME

# Gesture Recognition Pipeline

Complete pipeline to create an application that can recognize custom gestures made on an Arduino Nano BLE Sense.

The gestures are detected with deep learning using accelerometer data. 

The pipeline includes data collection on an Arduino, data processing, neural network training, model conversion, model deployment on Arduino. Additionally, there is a section on how to send the predicted gestures to a server via bluetooth.

## Collect Data

In order to collect data, we used a basic triggering system in order to tell the arduino when to
collect the data. From there it was printed to the Serial Port, detected and saved as a csv using PuTTY.

### Steps
1. Using an Arduino Nano 33 BLE Sense and a button, create a circuit that will connect the vOut pin to an input datapin whenever the button is depressed. This acts as a trigger for the start of the movement.
2. Run the script provided within 'datacollection'. This script prints the output of the IMU within the Arduino Nano to the Serial Monitor.
3. Download Putty, setting the 'Connection Type' to 'Serial', the 'Serial line' box to the same 
Serial Port that the Arduino Nano is set to, and the 'Speed' to your Arduino Nano's 'Baud Rate'
4.  Collect Data and save it within a csv file.

## Process Data
The data processing script network_training/process_data.py converts the output of the Arduino sensors into numpy arrays that are later used to train the machine learning model.

### Steps:
1. Put the collected data csv files in network_training/collected_data
2. Open the network training folder
	cd network_training/
3. For each different gesture run the command below specifying the collected data, where to save the processed output, and the label of the gesture. Keep in mind the label should be numeric. There are example files in network_training/collected_data and network_training/processed_data.
	To process data, run: python process_data.py <collected_data.csv> <saving_location> <label>
	
	Example:
		python process_data.py collected_data/collected_LRdash.csv processed_data/processed_LRdash.npy 0
		
	Note: process_data.py takes additional optional args <data_format> and <data_augment_flag>, where <data_format> is either 'Category' or 'MAGICWAND' to accomodate for the 2 different formats of csv data we currently have, and <data_augment_flag> is a boolean either True or False telling the program to perform data augmentation on the input.


## Train Model
By this point you should have a folder with processed data with .npy files. Now we will train a model with this data. The model is trained with a convolutional neural network (CNN) in Tensorflow. The data is split into training (60%) , validation (20%), and testing (20%). 
 
### Steps:
1. Open the network training folder
	cd network_training/
2. Run the training script
    	python train.py <processed_data_folder> <model_saving_location_folder> <0 or 1>
    	-To save as a .h5 file, the last argument should be 0
	-To save as a .tflite file, the argument should be 1
	Or for the fine tuned model:

	python evaluation.py <processed_data_folder> <model_saving_location_folder>

	Example:
		python train.py processed_data/ lite_models/ 0

## Model Conversion
After obtaining the Tensforflow Lite model as a “.tflite” file you have to convert it to a c++ file and put it in your Arduino project.

### Steps:
1. Place the folder with tensorflow lite models in Google Drive
2. Open the convert_model.ipynb jupyter notebook in Google Colab
3. Run the code blocks in the Colab notebook and a model.cc file will be generated
4. Now you should have a model.cc file containing a `model_tflite[]` unsigned char array and its size. You will need to create a new file named `model.h` inside the same folder as your arduino sketch.
5. Copy the entire contents of model.cc into model.h
6. Create header guards that define the file as `MODEL_TFLITE_H`

## Deployment

For Deployment, we are running a script similar to the data collection script. The input in discrete chunks using a button trigger, interpolated linearly, and then inferenced.

### Steps
1. Use the exact same circuit at the one used within the data collection phase.
2. Add five LED's wired to light if any of the pins 3, 4, 5, 6 and 7 output `HIGH` signals
3. Open the 'arduino-imp.ino' script located at deployment\arduino-imp\arduino-imp.ino
4. The arduino editor should open up with four tabs:
    - 'arduino-imp.ino' - This is the main script that will be uploaded
    - 'primer.h' - header file for primer.cpp file that outlines helper methods
    - 'primer.cpp' - cpp file that contains the linear interpolation functions and array compiler.
    - 'model.h' - header file that contains the TensorFlowLite model
5. Upload the code and test.

## Connect to a computer via bluetooth

We took advantage of the Bluetooth Low-Energy capabilities of the Arduino Nano 33 BLE in conjunction with the Bleak python library in order to allow for the Arduino to communicate with the outside world.  Our hopes are that the system can be used as a gesture controller 
that can be used to trigger other processes.

### Steps
1. Download python.
2. Use pip install to download the Bleak and Asyncio libraries (python version must be < 3.6)
3. Plug in the Arduino Nano 33 BLE with the Deployment code running
4. Navigate to the bleak [documentation page](https://bleak.readthedocs.io/en/latest/scanning.html) and run the code used to discover other devices
5. Copy the address of the Arduino Nano. It should either be `GesturePredictor` or `Arduino`
6. load the python file `bluetooth\ble-testing` paste in the address detected and check that the Model number matches the model number within the arduino code
7. Run the ble code via command line and test


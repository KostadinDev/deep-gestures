# Gesture Recognition Pipeline

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

* Put the collected data csv files in network_training/collected_data
* cd network_training/ 
* For each different gesture run the command below specifying the label of the gesture. The label should be numeric.
* _python format_data.py <input_location> <output_location> <label>_
	>python format_data.py collected_data/collected_LRdash processed_data/processed_LRdash 0
## Train Model

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

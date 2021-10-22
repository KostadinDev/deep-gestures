import sys
import os
import numpy as np
import json

if __name__ == '__main__':
    data_folder = sys.argv[1]

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
    print(data[0])
    # json object structure:
    # json = {user: String, label: Int, data:{x:[],y:[], z:[]}}
    json_data = json.dumps({'user': 'kostadin', 'label': data[0][1], 'data':
        {'x': list(data[0][0][:, 0]), 'y': list(data[0][0][:, 1]), 'z': list(data[0][0][:, 2])}}, indent=4)

    print(json_data)
    with open('json_data/json_example.json', 'w') as outfile:
        json.dump(json_data, outfile)

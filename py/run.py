import numpy as np
import network
import classifier

# Load the map of keys_values
key = np.load('dataset_mapping.npz')['key']

# Load the neural network
model = np.load('models/model_29.npz')
we = model['weights']
bi = model['biases']

net = network.Network([784, 300, 100, 68])
net.load_model(we,bi)

# Load the classifier
clsfr = classifier.classifier()
code = clsfr.classify_lines(net, key)

print (code)

import network
import numpy as np

from PIL import Image

class classifier(object):

	def __init__(self, path = '../p.png', invert = False):
		self.prepare_input(path, invert)

	def classify(self, net):
		
		# Feed the network
		res = net.feedforward(self.y)

		# Print result
		print (np.argmax(res))

	#def classify_line(self):

		# Read folder

		# iterate, prepare_input, classify, string concatenate, print


	def prepare_input(self, path, invert):

		# Import the image
		self.x = Image.open(path, 'r')

		# Convert to Gray
		self.x = np.array(self.x.convert('L'))

		# Transform x to meet neural network specs
		#self.x = np.fliplr(self.x)

		# Convert to one column image, float (0.0->1.0), invert colors
		if (invert): self.x = 255 - self.x
		print (self.x)
		self.y = self.x.reshape((28 * 28, -1)).astype(np.float32)/255.0


import network
import math
import os
from os import listdir
from os.path import isfile, join, basename

import numpy as np
import scipy.misc as sp

from PIL import Image

PATH = '../data/4.CharacterOutput_ClassifierInput/'

class classifier(object):

	def classify(self, net):
		
		# Feed the network
		res = net.feedforward(self.y)

		# Print result
		return (res)

	def classify_lines(self, net, key):
		code = ''

		for line in sorted(np.array(get_files_list(PATH))):
			for word in sorted(np.array(get_files_list(PATH+str(line)+"/"))):
				for char in sorted(np.array(get_files_list(PATH+str(line)+"/"+str(word)+"/", True))):
					self.prepare_input(PATH+str(line)+"/"+str(word)+"/"+str(char)+".jpg", True)
					res = self.classify(net)

					code += chr(int(key[np.argmax(res)][1]))
					print (PATH+str(line)+"/"+str(word)+"/"+str(char)+".jpg", chr(int(key[np.argmax(res)][0])), np.argsort(-res, axis=None)[:10], np.array(sorted(-res)[:10]).tolist())

				print (' ')
				code += ' '

			print('\n')
			code += '\n'
		return code

	def prepare_input(self, path, invert):

		# Import the image
		self.x = Image.open(path, 'r')

		# Transform x to meet neural network specs
		self.x = np.rot90(np.fliplr(np.array(self.x)))
		
		# Invert colors
		if (invert): self.x = 255 - self.x

		# Resize to match dataset
		rows = len(self.x)
		cols = len(self.x[0])

		if (rows > cols):
			ratio = 20.0/rows
			new_dimes = 20, int(cols * ratio)
		else:
			ratio = 20.0/cols
			new_dimes = int(rows * ratio), 20
			
		self.x = sp.imresize(self.x, new_dimes, 'nearest', 'L')

		out = np.zeros((28,28), dtype=np.int)
		out[14-int(math.floor(new_dimes[0]/2.0)):14+int(math.ceil(new_dimes[0]/2.0)), 14-int(math.floor(new_dimes[1]/2.0)):14+int(math.ceil(new_dimes[1]/2.0))] = self.x

		self.y = out.reshape((28 * 28, -1)).astype(np.float32)/255.0

#### Miscellaneous functions
def get_files_list(path, remove_ext = False):
	li = list()
	for t in (listdir(path)):
		if (not isinstance(t, int) and t[0] == '.'): continue
		if (remove_ext): li.append(int(t.split('.')[0]))
		else : li.append(int(t))
	return li
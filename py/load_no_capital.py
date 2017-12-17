import numpy as np
from os import listdir
from os.path import isfile, join
import numbers
import scipy.ndimage

import PIL.ImageOps    
from PIL import Image   

np.set_printoptions(threshold=np.nan)
np.set_printoptions(linewidth=1000)

org_set = '/Users/ibrahimradwan/Desktop/org_set/'

def load_no_capital():
	images = []
	labels = []

	# Read digits
	for i in range(0, 10):
		for img in listdir((org_set+str(i))):
			if (img[0] == '.'): continue
			x = Image.open((org_set + str(i) + '/' + img), 'r')
			images.append(255-np.array(x))
			labels.append(i)

	# Read digits
	for i in range(36, 62):
		for img in listdir((org_set+str(i))):
			if (img[0] == '.'): continue
			x = Image.open((org_set + str(i) + '/' + img), 'r')
			images.append(255-np.array(x))
			labels.append(i-26)

	return images, labels
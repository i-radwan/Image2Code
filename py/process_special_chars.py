import numpy as np
from os import listdir
from os.path import isfile, join
import numbers

import PIL.ImageOps    
from PIL import Image    

size = (20,20)
new_size = (28, 28)
PATH = '/Users/ibrahimradwan/Desktop/symbols/'

def add_chars(training_set, training_results, test_set, test_results):
	# Add general sybmols
	training_set, training_results, test_set, test_results = add_special_chars_to_training_set(training_set, training_results, test_set, test_results)
	
	# Rotate > && < to get ^
	cnt = len(listdir((PATH + 'ls')))
	i = 0
	for img in listdir((PATH + 'ls')):
		if img == '.DS_Store': continue
		
		# Read the image, invert it, resize it to 20*20
		x = Image.open((PATH + 'ls' + '/' + img), 'r')
		x = PIL.ImageOps.invert(x)
		x.thumbnail(size, Image.ANTIALIAS)

		# Add the frame -> new image 28*28
		new_im = Image.new("L", new_size)
		new_im.paste(x, (int((new_size[0]-size[0])/2), int((new_size[1]-size[1])/2)))

		img = np.array(new_im)
		img = np.rot90(np.rot90(np.rot90(img)))
		# Apply transformations to add to the traning set
		img = np.rot90(np.fliplr(img))

		if (i < (5.0/6) * cnt):
			# Append to dataset
			training_set.append(img)
			training_results.append(87)
		else:
			# Append to testset
			test_set.append(img)
			test_results.append(87)

		i+=1
	
	cnt = len(listdir((PATH + 'gt')))
	i = 0
	for img in listdir((PATH + 'gt')):
		if img == '.DS_Store': continue
		
		# Read the image, invert it, resize it to 20*20
		x = Image.open((PATH + 'gt' + '/' + img), 'r')
		x = PIL.ImageOps.invert(x)
		x.thumbnail(size, Image.ANTIALIAS)

		# Add the frame -> new image 28*28
		new_im = Image.new("L", new_size)
		new_im.paste(x, (int((new_size[0]-size[0])/2), int((new_size[1]-size[1])/2)))

		img = np.array(new_im)
		img = np.rot90(img)

		# Apply transformations to add to the traning set
		img = np.rot90(np.fliplr(img))

		if (i < (5.0/6) * cnt):
			# Append to dataset
			training_set.append(img)
			training_results.append(87)
		else:
			# Append to testset
			test_set.append(img)
			test_results.append(87)

		i+=1
		
	cnt = len(listdir((PATH + 'dash')))
	i = 0
	# Drag - down to get _
	for img in listdir((PATH + 'dash')):
		if img == '.DS_Store': continue

		# Read the image, invert it, resize it to 20*20
		x = Image.open((PATH + 'dash' + '/' + img), 'r')
		x = PIL.ImageOps.invert(x)
		x.thumbnail(size, Image.ANTIALIAS)

		new_x = Image.new("L", size)
		new_x.paste(x, (0, 7))

		# Add the frame -> new image 28*28
		new_im = Image.new("L", new_size)
		new_im.paste(new_x, (int((new_size[0]-size[0])/2), int((new_size[1]-size[1])/2)))

		#print np.array(new_im)
		img = np.array(new_im)

		# Apply transformations to add to the traning set
		img = np.rot90(np.fliplr(img))

		if (i < (5.0/6) * cnt):
			# Append to dataset
			training_set.append(img)
			training_results.append(88)
		else:
			# Append to testset
			test_set.append(img)
			test_results.append(88)
		i+=1
	
	return training_set, training_results, test_set, test_results	

def add_special_chars_to_training_set(training_set, training_results, test_set, test_results):
	# Loop over all symbols in the folder -> Loop over all symbol images
	for symbol in listdir(PATH):
		if symbol == '.DS_Store': continue

		cnt = len(listdir((PATH + symbol)))
		i = 0
		for img in listdir((PATH + symbol)):
			if img == '.DS_Store': continue
			
			# Read the image, invert it, resize it to 20*20
			x = Image.open(((PATH + str(symbol)) + '/' + img), 'r')
			x = PIL.ImageOps.invert(x)

			# Resize and add frame if not ready already
			if (symbol == 'bslash' or symbol == 'dash' or symbol == 'eq' or symbol == 'exclamation' or symbol == 'forwards' or symbol == 'gt'or symbol == 'lb'or symbol == 'lcb'or symbol == 'ls'or symbol == 'lsb'or symbol == 'or'or symbol == 'plus'or symbol == 'rb'or symbol == 'rcb'or symbol == 'rsb'):
				x.thumbnail(size, Image.ANTIALIAS)

				# Add the frame -> new image 28*28
				new_im = Image.new("L", new_size)
				new_im.paste(x, (int((new_size[0]-size[0])/2), int((new_size[1]-size[1])/2)))
			else: new_im = x

			img = np.array(new_im)

			# Apply transformations to add to the traning set
			img = np.rot90(np.fliplr(img))

			if (symbol == 'forwards'):
				symbolKey = 76
			elif (symbol == 'colons'):
				symbolKey = 77
			elif (symbol == 'dots'):
				symbolKey = 75
			elif (symbol == 'and'):
				symbolKey = 67
			elif (symbol == 'or'):
				symbolKey = 91
			elif (symbol == 'at'):
				symbolKey = 83
			elif (symbol == 'hash'):
				symbolKey = 64
			elif (symbol == 'mod'):
				symbolKey = 66
			elif (symbol == 'bslash'):
				symbolKey = 85
			elif (symbol == 'btick'):
				symbolKey = 89
			elif (symbol == 'comma'):
				symbolKey = 73
			elif (symbol == 'dash'):
				symbolKey = 74
			elif (symbol == 'dollar'):
				symbolKey = 65
			elif (symbol == 'dquote'):
				symbolKey = 63
			elif (symbol == 'quote'):
				symbolKey = 68
			elif (symbol == 'eq'):
				symbolKey = 80
			elif (symbol == 'exclamation'):
				symbolKey = 62
			elif (symbol == 'gt'):
				symbolKey = 81
			elif (symbol == 'lb'):
				symbolKey = 69
			elif (symbol == 'rb'):
				symbolKey = 70
			elif (symbol == 'lcb'):
				symbolKey = 90
			elif (symbol == 'rcb'):
				symbolKey = 92
			elif (symbol == 'rsb'):
				symbolKey = 86
			elif (symbol == 'lsb'):
				symbolKey = 84
			elif (symbol == 'ls'):
				symbolKey = 79
			elif (symbol == 'multi'):
				symbolKey = 71
			elif (symbol == 'plus'):
				symbolKey = 72
			elif (symbol == 'question'):
				symbolKey = 82
			elif (symbol == 'semicolon'):
				symbolKey = 78
			elif (symbol == 'tilde'):
				symbolKey = 93
			else:
				symbolKey = (ord(symbol))
			
            # Append to dataset
			if (i < (5.0/6) * cnt):
				training_set.append(img)
				training_results.append(symbolKey)
			else:
				# Append to testset
				test_set.append(img)
				test_results.append(symbolKey)
			i+=1
	return training_set, training_results, test_set, test_results

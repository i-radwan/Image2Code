from os import listdir
import math
import difflib

import numpy as np
import scipy.misc as sp
from PIL import Image

import network

class Classifier(object):
	DATA_PATH = "preprocessing/tmp/Characters/"

	keywords = [
        'auto',
        'bool',
        'break',
        'case',
        'catch',
        'char',
        'class',
        'const',
        'continue',
        'default',
        'delete',
        'do',
        'double',
        'dynamic_cast',
        'else',
        'enum',
        'false',
        'float',
        'for',
        'friend',
        'goto',
        'if',
        'inline',
        'int',
        'long',
        'mutable',
        'namespace',
        'new',
        'nullptr',
        'operator',
        'or',
        'or_eq',
        'private',
        'protected',
        'public',
        'return',
        'short',
        'signed',
        'sizeof',
        'static',
        'static_cast',
        'struct',
        'switch',
        'template',
        'this',
        'throw',
        'true',
        'try',
        'typedef',
        'typename',
        'union',
        'unsigned',
        'using',
        'virtual',
        'void',
        'while',
        'cout',
        'cin',
        'vector',
        'map',
        'pair',
        'push_back',
        'endl',
        'std',
        'main'
    ]

	def __init__(self):

		# Load the map of keys_values
		self.small_key = np.load('utilities/classifier/models/small_mapping.npz')['key']
		self.special_key = np.load('utilities/classifier/models/special_mapping.npz')['key']

		# Load the neural networks
		self.small_model = np.load('utilities/classifier/models/small_model.npz')
		self.small_we = self.small_model['weights']
		self.small_bi = self.small_model['biases']

		self.special_model = np.load('utilities/classifier/models/special_model.npz')
		self.special_we = self.special_model['weights']
		self.special_bi = self.special_model['biases']

		# Initialize the networks
		self.small_net = network.Network([784, 300, 100, 36])
		self.small_net.load_model(self.small_we, self.small_bi)

		self.special_net = network.Network([784, 300, 23])
		self.special_net.load_model(self.special_we, self.special_bi)

		# Result vectors
		self.result = []

	def classifiy(self, path = DATA_PATH):

		get_small_vec_values = np.vectorize(self.get_small_value)
		get_special_vec_values = np.vectorize(self.get_special_value)

		code = ''

		last_line_id = "00"
		last_word_id = "00"

		# Append place for first line -> first word
		self.result.append([[]])
		
		for char in sorted(np.array(listdir(path))):
			if (char[0]=='.'): continue

			img = self.prepare_input(path + char, True)

			if (char.split(".")[0][0:2] != last_line_id):
				self.result.append([[]])
			elif (char.split(".")[0][2:4] != last_word_id): 
				self.result[-1].append([])

			if (char.split(".")[0][-1] == "x"): # Special char
				res = self.special_net.feedforward(img)
				# print (path + char, chr(int(self.special_key[np.argmax(res)])), get_special_vec_values(np.argsort(-res, axis=None)[:10]), np.array(sorted(-res)[:10]).tolist(), '\n')
				self.result[-1][-1].append((get_special_vec_values(np.argsort(-res, axis=None)[:5]), np.array(sorted(-res)[:5]).tolist()))

			else :
				res = self.small_net.feedforward(img)
				# print (path + char, chr(int(self.small_key[np.argmax(res)])), get_small_vec_values(np.argsort(-res, axis=None)[:10]), np.array(sorted(-res)[:10]).tolist(), '\n')
				self.result[-1][-1].append((get_small_vec_values(np.argsort(-res, axis=None)[:5]), np.array(sorted(-res)[:5]).tolist()))

			last_line_id = char.split(".")[0][0:2]
			last_word_id = char.split(".")[0][2:4]

		return self.post_processing()

	def post_processing(self):
		result = np.array(self.result)

		processed_result = []

		code = ''

		# Initial prost-processing
		for l, line in enumerate(result):
			processed_result.append([])
			for w, word in enumerate(line):
				new_word = ""
				for c, char in enumerate(word):
					fc = (c-1 < 0) # first character
					lc = (c+1 == len(word)) # last character
					# Handle:
					# Single quotation
					if (char[0][0] == '\''):
						if (char[0][1] == '(' or char[0][1] == ')'):
							new_word += char[0][1]
						else: new_word += ','
					
					# 5 -> s
					elif (char[0][0] == '5' and((not fc and is_char(word[c-1][0][0])) or \
						(not lc and is_char(word[c+1][0][0])))):
						new_word += 's' 
					
					# 9 -> g
					elif ((char[0][0] == '9' or char[0][0] == '3') and ('g' in char[0]) and ((not fc and is_char(word[c-1][0][0])) or \
						(not lc and is_char(word[c+1][0][0])))):
						new_word += 'g' 
					
					# 0 -> o
					elif (char[0][0] == '0' and ((not fc and is_char(word[c-1][0][0]) and word[c-1][0][0] != 'n') or \
						(not lc and is_char(word[c+1][0][0])))):
						new_word += 'o' 
					
					# integer to char
					elif (is_digit(char[0][0]) and is_char(char[0][1]) and((not fc and is_char(word[c-1][0][0])) or \
						(not lc and is_char(word[c+1][0][0])))):
						new_word += char[0][1]
					
					# },],) -> ;
					elif ((char[0][0] == '}' or char[0][0] == ')' or char[0][0] == ']') and char[0][1] == ';' and ((char[1][0][0] / char[1][1][0]) < 3 or (lc and w == len(line) - 1))):
						new_word += ';'

					# ; -> },],)
					elif ((char[0][1] == '}' or char[0][1] == ')' or char[0][1] == ']') and char[0][0] == ';' and ((char[1][0][0] / char[1][1][0]) < 3 and (w != len(line) - 1 or not lc))):
						new_word += char[0][1]
					
					# . * -> +
					elif ((char[0][0] == '.' or char[0][0] == '*') and char[0][1] == '+' and ((char[1][0][0] / char[1][1][0]) < 3 and ((not fc and word[c-1][0][0] == '+') or (not lc and (word[c+1][0][0] == '=' or word[c+1][0][0] == '+'))))):
						new_word += '+'
					
					# any -> >
					elif (char[0][0] != '>' and (not is_char(char[0][0])) and (not is_digit(char[0][0])) and not lc and (word[c+1][0][0] == '>')):
						new_word += '>'

					# any -> <
					elif (char[0][0] != '<' and (not is_char(char[0][0])) and (not is_digit(char[0][0])) and not lc and (word[c+1][0][0] == '<')):
						new_word += '<'

					# any -> &
					elif (char[0][0] != '&' and (not is_char(char[0][0])) and (not is_digit(char[0][0])) and not lc and (word[c+1][0][0] == '&')):
						new_word += '&'

					# ++ or -- no +- or -+
					elif (not lc and (char[0][0] == '-' or char[0][0] == '+') and ((word[c+1][0][0] == '+' or word[c+1][0][0] == '-') and word[c+1][0][0] != char[0][0])):
						if (char[1][0] > word[c+1][1][0]): word[c+1][0][0] = char[0][0]
						else: 
							char[0][0] = word[c+1][0][0]

						new_word += char[0][0]
					
					# End bracket
					elif (lc and w == len(line) - 1 and l == len(result) - 1):
						new_word += '}'

					# Other chars
					else:
						new_word += char[0][0]
				
				processed_result[-1].append(new_word)

		# Repalce with keywords
		for line in processed_result:
			for w, word in enumerate(line):

				near_words = difflib.get_close_matches(word, Classifier.keywords, cutoff=0.75)
				if (len(near_words) > 0 and len(near_words[0]) == len(word)):
					code += near_words[0]
				else: code += word

				if (w != len(line) - 1): code += ' '
			code += '\n'
		
		return code

	def prepare_input(self, path, invert):

		# Import the image
		x = Image.open(path, 'r')

		# Transform x to meet neural network specs
		x = np.rot90(np.fliplr(np.array(x)))
		
		# Invert colors
		if (invert): x = 255 - x

		# Resize to match dataset
		rows = len(x)
		cols = len(x[0])

		if (rows > cols):
			ratio = 20.0/rows
			new_dimes = 20, int(cols * ratio)
		else:
			ratio = 20.0/cols
			new_dimes = int(rows * ratio), 20
			
		x = sp.imresize(x, new_dimes, 'nearest', 'L')

		out = np.zeros((28,28), dtype=np.int)
		out[14-int(math.floor(new_dimes[0]/2.0)):14+int(math.ceil(new_dimes[0]/2.0)), 14-int(math.floor(new_dimes[1]/2.0)):14+int(math.ceil(new_dimes[1]/2.0))] = x

		return out.reshape((28 * 28, -1)).astype(np.float32)/255.0

	def get_small_value(self, k):
		return chr(self.small_key[k])

	def get_special_value(self, k):
		return chr(self.special_key[k])

def is_char(c):
	return c >= 'a' and c <= 'z'

def is_digit(d):
	return d >= '1' and d <= '9' 

if __name__ == '__main__':
	clsfr = Classifier()
	print(clsfr.classifiy())

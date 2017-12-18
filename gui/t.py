import sys
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import QApplication, QWidget, QInputDialog, QLineEdit, QFileDialog
from PyQt5.QtGui import *
from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import *
from PyQt5.QtCore import Qt, QTimer, QPoint

class sliderdemo(QWidget):
   def __init__(self, parent = None):
      super(sliderdemo, self).__init__(parent)

      layout = QVBoxLayout()
      self.l1 = QLabel("Hello")
      self.l1.setAlignment(Qt.AlignCenter)
      layout.addWidget(self.l1)
		
      self.sl = QSlider(Qt.Horizontal)
      self.sl.setMinimum(0)
      self.sl.setMaximum(5)
      self.sl.setValue(0)
      self.sl.setTickPosition(QSlider.TicksBelow)
      self.sl.setTickInterval(1)
		
      layout.addWidget(self.sl)
      self.sl.valueChanged.connect(self.valuechange)
      self.setLayout(layout)
      self.setWindowTitle("SpinBox demo")

   def valuechange(self):
      size = self.sl.value()
      self.l1.setFont(QFont("Arial",size))
		
def main():
   app = QApplication(sys.argv)
   ex = sliderdemo()
   ex.show()
   sys.exit(app.exec_())
	
if __name__ == '__main__':
   main()
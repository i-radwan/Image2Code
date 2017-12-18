import sys
# import from utilities folder
sys.path.insert(0, 'utilities')
sys.path.insert(0, 'utilities/classifier')

import os
import subprocess
from subprocess import Popen, PIPE, STDOUT
from PyQt5.QtWidgets import QApplication, QWidget, QInputDialog, QLineEdit, QFileDialog
from PyQt5.QtGui import *
from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import *
from PyQt5.QtCore import Qt, QTimer, QPoint

from Overlay import Overlay
from code_editor import CodeEditor
import syntax

from classifier import Classifier

class Image2Code(QWidget):
    PLACEHOLDER_IMG_PATH = "utilities/images/placeholder.png"

    OUTPUT_EXE_PATH = "output/code.out"
    OUTPUT_SRC_PATH = "output/output.cpp"
    OUTPUT_TXT_PATH = "output/output"
    OUTPUT_TXT_FILE = "output/output.txt"

    SEGMENTATION_IMG = "preprocessing/tmp/Preprocess/segmented.jpg"
    SEGMENTATION_EXE_FILE = "preprocessing/Image2Code"

    MODE_PRINTED = 0
    MODE_HANDWRITING = 1

    def __init__(self):
        super().__init__()

        self.title = 'I2C - Image to Code'
        self.left = 0
        self.top = 0
        self.width = self.getWindowSize()[1]
        self.height = int(self.getWindowSize()[0] * 0.925)
        self.allowedImgWidth = int(self.width * 0.55)
        self.allowedImgHeight = int(self.height * 0.88)

        self.allowedEditorWidth = int(self.width * 0.402)
        self.allowedEditorHeight = int(self.height * 0.401)
        self.allowedInputWidth = int(self.width * 0.402)
        self.allowedInputHeight = int(self.height * 0.2)
        self.allowedConsoleWidth = int(self.width * 0.402)
        self.allowedConsoleHeight = int(self.height * 0.2)

        self.mode = Image2Code.MODE_HANDWRITING
        self.segmetedPicReady = False

        self.setFixedSize(self.width, self.height)
        self.initUI()
 
    def initUI(self):

        self.setupWindow()

        self.setupWidnowTitle()

        self.setupWidnowExitBtn()

        self.setupImageLabel()
        
        self.setupCodeTextEdit()

        self.setupInputTextEdit()

        self.setupCompilationResult()

        self.setupSelectImageBtn()

        self.setupModeBtn()

        self.setupRunBtn()

        self.setupConvertBtn()
        
        self.setupLoadingAnimation()

        self.show()

    def resizeEvent(self, event):

        self.overlay.resize(event.size())
        event.accept()

    def center(self):

        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    def mousePressEvent(self, event):

        self.oldPos = event.globalPos()

    def mouseMoveEvent(self, event):

        if (hasattr(self, 'oldPos')):
            delta = QPoint (event.globalPos() - self.oldPos)
            self.move(self.x() + delta.x(), self.y() + delta.y())
            self.oldPos = event.globalPos()

    def setupWindow(self):

        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)

        # Set window background color
        self.setAutoFillBackground(True)
        p = self.palette()
        p.setColor(self.backgroundRole(), QColor(255, 255, 255))
        self.setPalette(p)

        self.setWindowFlags(QtCore.Qt.FramelessWindowHint)

    def setupWidnowTitle(self):

        self.title = QLabel(self)
        self.title.setText("Image2Code")
        self.title.setStyleSheet("font-size: 25px; padding: 12px; font-weight: lighter; color: #ffffff; background: #3498db; font-family: Courier New;")
        self.title.setGeometry(QtCore.QRect(20, 10, self.width - 40, 55))
        self.title.setAlignment(QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter)

    def setupLoadingAnimation(self):

        self.overlay = Overlay(self)
        self.overlay.hide()

    def setupImageLabel(self):

        self.picLabel = QLabel(self)
        self.picLabel.setStyleSheet("background: #ffffff")
        self.picLabel.setGeometry(QtCore.QRect(20, 80, self.allowedImgWidth, self.allowedImgHeight - 60))
        self.picLabel.setAlignment(QtCore.Qt.AlignHCenter | QtCore.Qt.AlignVCenter)

        # Set placeholder image
        self.pic = QPixmap(Image2Code.PLACEHOLDER_IMG_PATH)
        self.scaledPic = self.pic.scaled(self.allowedImgWidth, self.allowedImgHeight, QtCore.Qt.KeepAspectRatio, transformMode=QtCore.Qt.SmoothTransformation)
        self.picLabel.setPixmap(self.scaledPic)

    def setupWidnowExitBtn(self):

        self.windowExitBtn = QPushButton('×', parent=self)
        self.windowExitBtn.move(self.width - 55, 18)
        self.windowExitBtn.setCursor(Qt.PointingHandCursor)
        self.windowExitBtn.setStyleSheet("border-radius: 0px; font-family: Courier New; font-size: 36px; color: #ffffff; background: #00ffffff; text-decoration: none;")

        self.windowExitBtn.clicked.connect(app.quit)

    def setupModeBtn(self):

        self.selectHandModeBtn = HoverButton('Handwritten', color="#ffffff", BKColor="#50ae55", hoverBKColor="#c850ae55", parent=self)
        self.selectHandModeBtn.move(94 + self.selectImgBtn.width(), self.height - 60)
        self.selectHandModeBtn.setCursor(Qt.PointingHandCursor)
        self.selectHandModeBtn.setStyleSheet("border-radius: 0px; font-family: Courier New; color: #ffffff; font-size: 18px; background: #50ae55; padding: 8px 18px 8px 18px; text-decoration: none;")

        self.selectPrintModeBtn = HoverButton('Printed', color="#000000", BKColor="#e0e0e0", hoverBKColor="#c8e0e0e0", parent=self)
        self.selectPrintModeBtn.move(174 + self.selectImgBtn.width() + self.selectHandModeBtn.width(), self.height - 60)
        self.selectPrintModeBtn.setCursor(Qt.PointingHandCursor)
        self.selectPrintModeBtn.setStyleSheet("border-radius: 0px;  font-family: Courier New; color: #000000; font-size: 18px; background: #e0e0e0; padding: 8px 18px 8px 18px; text-decoration: none;")

        self.selectHandModeBtn.clicked.connect(self.selectHandMode)
        self.selectPrintModeBtn.clicked.connect(self.selectPrintMode)

    def setupSelectImageBtn(self):

        self.selectImgBtn = HoverButton('Select', parent=self)
        self.selectImgBtn.move(20, self.height - 60)
        self.selectImgBtn.setCursor(Qt.PointingHandCursor)
        self.selectImgBtn.setStyleSheet("border-radius: 0px;  font-family: Courier New; color: #ffffff; font-size: 18px; background: #3498db; padding: 8px 18px 8px 18px; text-decoration: none;")

        self.selectImgBtn.clicked.connect(self.selectImage)

    def setupRunBtn(self):

        self.runBtn = HoverButton('Run', parent=self)
        self.runBtn.move((self.width - self.runBtn.width() - 15), self.height - 60)
        self.runBtn.setCursor(Qt.PointingHandCursor)
        self.runBtn.setStyleSheet("border-radius: 0px;  font-family: Courier New; color: #ffffff; font-size: 18px; background: #3498db; padding: 8px 18px 8px 18px; text-decoration: none;")

        self.runBtn.clicked.connect(self.compile)

    def setupConvertBtn(self):

        self.convertBtn = HoverButton('Convert', parent=self)
        self.convertBtn.move((self.width - self.convertBtn.width() - 200), self.height - 60)
        self.convertBtn.setCursor(Qt.PointingHandCursor)
        self.convertBtn.setStyleSheet("border-radius: 0px;  font-family: Courier New; color: #ffffff; font-size: 18px; background: #3498db; padding: 8px 18px 8px 18px; text-decoration: none;")

        self.convertBtn.clicked.connect(self.convertToCode)

    def setupCodeTextEdit(self):

        font = self.getFont()
        self.editor = CodeEditor(self)
        self.editor.setFont(font)
        self.editor.setPlaceholderText("Code...")
        self.editor.setGeometry(QtCore.QRect(self.allowedImgWidth + 50, 80, self.allowedEditorWidth, self.allowedEditorHeight - 60))
        self.editor.setGraphicsEffect(self.getShadow());

        self.editor.show()
        
        self.highlight = syntax.CPPHighlighter(self.editor.document())

    def setupInputTextEdit(self):

        font = self.getFont()
        self.inputEditor = CodeEditor(self)
        self.inputEditor.setFont(font)
        self.inputEditor.setPlaceholderText("Input...")
        self.inputEditor.setGeometry(QtCore.QRect(self.allowedImgWidth + 50, 52 + self.allowedEditorHeight, self.allowedInputWidth, self.allowedInputHeight))
        self.inputEditor.setGraphicsEffect(self.getShadow());
        self.inputEditor.show()

    def setupCompilationResult(self):

        font = self.getFont()
        self.console = CodeEditor(self)
        self.console.setReadOnly(True)
        self.console.setFont(font)
        self.console.setPlaceholderText("Output...")
        self.console.setGeometry(QtCore.QRect(self.allowedImgWidth + 50, 40 + self.allowedEditorHeight + self.allowedInputHeight + 42, self.allowedConsoleWidth, self.allowedConsoleHeight + 3))
        self.console.setGraphicsEffect(self.getShadow());
        self.console.show()
        

    def convertToCode(self):

        if (len(self.imagePath.strip()) == 0): return

        if (self.mode == Image2Code.MODE_PRINTED):
            self.convertPrinted()
        else:
            self.convertHandwritten()


    def convertPrinted(self):

        # Run Tesseract
        result = subprocess.run(['tesseract', self.imagePath, Image2Code.OUTPUT_TXT_PATH], stdout=subprocess.PIPE)

        with open(Image2Code.OUTPUT_TXT_FILE, 'r') as codeFile:
            code = codeFile.read()

        # Post processing
        code = code.replace("—", "-")
        
        self.editor.clear()
        self.editor.insertPlainText(code)


    def convertHandwritten(self):
        
        # Segmentation
        subprocess.run(['./' + Image2Code.SEGMENTATION_EXE_FILE, self.imagePath], stdout=subprocess.PIPE)

        # Run classifier
        result = Classifier().classifiy()

        self.editor.clear()
        self.editor.insertPlainText(result)

        # Display segmented image
        self.displaySegmentedImage()
        self.segmetedPicReady = True

        return

    def compile(self):

        with open(Image2Code.OUTPUT_SRC_PATH, 'w') as codeFile:
            codeFile.write(self.editor.toPlainText())

        result = subprocess.run(['g++', Image2Code.OUTPUT_SRC_PATH, '-o', Image2Code.OUTPUT_EXE_PATH], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        self.console.clear()
        if (len(result.stderr.decode('utf-8').strip())==0):
            result = Popen(['./' + Image2Code.OUTPUT_EXE_PATH], stdin=subprocess.PIPE, stdout=subprocess.PIPE).communicate(input=(self.inputEditor.toPlainText()).encode())[0].decode("utf-8")
            
            self.console.insertPlainText(result)
            self.console.setStyleSheet("color: #000000; font-family: Courier New; font-size: 13px;")
        else:
            self.console.insertPlainText(result.stderr.decode("utf-8"))
            self.console.setStyleSheet("color: #ff0000; font-family: Courier New; font-size: 13px;")

    def getFont(self):

        font = QFont()
        font.setFamily('Courier New')
        font.setFixedPitch(True)
        font.setPointSize(12)
        
        return font

    def getShadow(self):

        shadow = QGraphicsDropShadowEffect(self)
        shadow.setBlurRadius(0)
        shadow.setOffset(0, 0)
        
        return shadow

    def selectImage(self, args=None):

        path = QFileDialog.getOpenFileName()[0]
        
        if (len(path.strip()) != 0):
            self.imagePath = path
            self.displayOriginalImage()
            self.segmetedPicReady = False

    def displayOriginalImage(self, args=None):

        self.displayImage(self.imagePath, 'Click to view segmented image')
        self.picLabel.mousePressEvent = self.displaySegmentedImage

    def displaySegmentedImage(self, args=None):

        if (self.segmetedPicReady):
            self.displayImage(Image2Code.SEGMENTATION_IMG, 'Click to view original image')
            self.picLabel.mousePressEvent = self.displayOriginalImage
        
        if (args!= None):
            args.accept()

    def displayImage(self, path, tooltipTitle= None):
        
        self.pic = QPixmap(path)
        self.scaledPic = self.pic.scaled(self.allowedImgWidth - 20, self.allowedImgHeight - 60, QtCore.Qt.KeepAspectRatio, transformMode=QtCore.Qt.SmoothTransformation)
        self.picLabel.setPixmap(self.scaledPic)
        self.picLabel.setGraphicsEffect(self.getShadow());

        if (tooltipTitle != None):
            self.picLabel.setToolTip(tooltipTitle)
        
        self.picLabel.setCursor(Qt.PointingHandCursor)

    def getWindowSize(self):

        sizeObject = QtWidgets.QDesktopWidget().screenGeometry(-1)
        return (sizeObject.height(), sizeObject.width())
 
    def selectHandMode(self):

        self.mode = Image2Code.MODE_HANDWRITING
        self.switchModeBtns(self.selectHandModeBtn, self.selectPrintModeBtn)

    def selectPrintMode(self):

        self.mode = Image2Code.MODE_PRINTED
        self.switchModeBtns(self.selectPrintModeBtn, self.selectHandModeBtn)

    def switchModeBtns(self, active, inactive):

        active.color ="#ffffff"
        active.BKColor="#50ae55"
        active.hoverBKColor="#c850ae55"
        active.setStyleSheet("border-radius: 0px; font-family: Courier New; color: #ffffff; font-size: 18px; background: #50ae55; padding: 8px 18px 8px 18px; text-decoration: none;")

        inactive.color ="#000000"
        inactive.BKColor="#e0e0e0"
        inactive.hoverBKColor="#c8e0e0e0"
        inactive.setStyleSheet("border-radius: 0px;  font-family: Courier New; color: #000000; font-size: 18px; background: #e0e0e0; padding: 8px 18px 8px 18px; text-decoration: none;")


class HoverButton(QToolButton):

    def __init__(self, text, BKColor = "#3498db", hoverBKColor = "#3cb0fd", color = "#ffffff", padding = "8px 18px 8px 18px", fontSize="18px", parent=None):
        super(HoverButton, self).__init__(parent)
        
        self.setMouseTracking(True)
        self.setText(text)
        self.BKColor = BKColor
        self.hoverBKColor = hoverBKColor
        self.color = color
        self.fontSize = fontSize
        self.padding = padding

    def enterEvent(self,event):

        self.setStyleSheet("border-radius: 0px;  font-family: Courier New; color: " + self.color + "; font-size: "+self.fontSize+"; background: " + self.hoverBKColor + "; padding: " + self.padding + "; text-decoration: none;")

    def leaveEvent(self,event):

        self.setStyleSheet("border-radius: 0px;  font-family: Courier New; color: " + self.color + "; font-size: "+self.fontSize+"; background: " + self.BKColor + "; padding: " + self.padding + "; text-decoration: none;")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = Image2Code()
    sys.exit(app.exec_())
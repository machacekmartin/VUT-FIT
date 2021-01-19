from PyQt5.QtWidgets import QMainWindow, QWidget, QVBoxLayout,  QHBoxLayout, QLineEdit, QPushButton, QGridLayout
from PyQt5 import QtCore
from buttons_template import ButtonsLayout

class MainWindow(QMainWindow):
    ##  Main Window of the app
    def __init__(self):
        super().__init__(None)

        self.setMaximumSize(1200, 800)
        self.setWindowTitle('Calculator')

        self.centralWidget = QWidget(self)
        self.mainLayout = QVBoxLayout()
        self.displayRow = QHBoxLayout()
        self.expressionDisplay = QLineEdit()
        self.entryDisplay = QLineEdit()
        self.buttonsGrid = QGridLayout()
        
        self.buttons = ButtonsLayout.buttons
        self.clear_button = ButtonsLayout.clearButton
        self.assignLayout()

        self.fontSize = 50

    def resizeEvent(self, event):
        while self.calcTextWidth() > self.calcDisplayWidth() - 30:
                self.fontSize -= 1
                self.updateFontSize()

    ##  Creates a layout and corresponding buttons for main windo and 
    def assignLayout(self):
        self.addClass(self.centralWidget, "body")
        self.addClass(self.entryDisplay, "display")
        self.addClass(self.expressionDisplay, "expression-display")
        self.entryDisplay.setAlignment(QtCore.Qt.AlignRight)
        self.entryDisplay.setReadOnly(True)
        self.entryDisplay.setPlaceholderText("0") 
        self.expressionDisplay.setAlignment(QtCore.Qt.AlignRight)
        self.expressionDisplay.setReadOnly(True)

        self.clear_button = self.newButton(self.clear_button['val'], self.clear_button['name'])

        self.displayRow.addWidget(self.clear_button)
        self.displayRow.addWidget(self.entryDisplay)
        
        for x in range(len(self.buttons)):
            for y in range(len(self.buttons[x])):
                button = self.newButton(self.buttons[x][y]['val'], self.buttons[x][y]['name'])
                self.buttonsGrid.addWidget(button, x, y)

        self.mainLayout.addWidget(self.expressionDisplay)
        self.mainLayout.addLayout(self.displayRow)
        self.mainLayout.addStretch()
        self.mainLayout.addLayout(self.buttonsGrid)
        self.centralWidget.setLayout(self.mainLayout)
        self.setCentralWidget(self.centralWidget)
        
    ##  Changes current font size of input display to number stored in fontSize variable
    def updateFontSize(self):
        self.entryDisplay.setStyleSheet("*[cssClass='display']{ font-size: "+ str(self.fontSize) +"px; }")

    ##  Resets the font size to default "50"
    def resetFontSize(self):
        self.fontSize = 50
        self.updateFontSize()

    ##  Returns width of text inside input display
    def calcTextWidth(self):
        return self.entryDisplay.fontMetrics().boundingRect(self.entryDisplay.text()).width()

    ##  Returns width of input display 
    def calcDisplayWidth(self):
        return self.entryDisplay.frameGeometry().width()

    ##  Scales font depending on text width and display width.
    #
    #   @param value - value of button that was pressed.
    def scaleFont(self, value):
        if value == "c":
            self.resetFontSize()
        elif value == "d":
            while self.calcTextWidth() < self.calcDisplayWidth() - 50 and self.fontSize < 50:
                self.fontSize += 1
                self.updateFontSize()

        else:
            while self.calcTextWidth() > self.calcDisplayWidth() - 30:
                self.fontSize -= 1
                self.updateFontSize()

    ##  Clears both input and expression displays
    def clearDisplay(self):
        self.expressionDisplay.setText('')
        self.entryDisplay.setText('')

    ##  Inserts a number or operation symbol to the display
    #
    #   @param value - number or a symbol that will be added to the display
    def addInput(self, value):
        self.entryDisplay.setText(self.entryDisplay.text() + str(value))

    ## Sets the value to input display
    #
    # @param value - string to be shown on display
    def setInput(self, value):
        self.entryDisplay.setText(str(value))

    ## Takes input display's content and puts it inside expression display
    #
    # @param error - additional error string from evaluate function, can be empty
    def setExpressionDisplay(self):
        self.expressionDisplay.setText(self.getInput()+"=")

    ##  Retrieves content of a display
    # 
    #   return Contents of the display
    def getInput(self):
        return self.entryDisplay.text()

    ##  Adds a class to given object
    #
    #   @param item - object to which the class will be added to
    #   @param _class - class name to be added
    #
    #   return New QPushButton
    def addClass(self, item, _class):
        item.setProperty("cssClass", _class)

    ##  Returns new button with value
    #
    #   @param x - number to be subtracted from 
    #
    #   return New QPushButton
    def newButton(self, value, name):
        return QPushButton(text = value, objectName = name)
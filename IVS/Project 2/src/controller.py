from functools import partial
from model import ExpressionError

class CalcController:
    ##  Connects model with view, handles all user interaction and application logic
    def __init__(self, model, view):
        self.model = model
        self.view = view
        self.bindViewButtons()
        self.error = False

    ##  Connects buttons to their functionality, adds them on click events
    def bindViewButtons(self):
        self.view.clear_button.clicked.connect(self.clearDisplay)

        grid = self.view.buttonsGrid
        for x in range(len(self.view.buttons)):
            for y in range(len(self.view.buttons[x])):
                item = grid.itemAtPosition(x,y)

                if item is None:
                    break

                item = item.widget()

                if item.objectName() == "clear":
                    item.clicked.connect(self.clearDisplay)
                elif item.objectName() == "delete":
                    item.clicked.connect(self.deleteLastInput)
                elif item.objectName() == "eval":
                    item.clicked.connect(self.evaluateInput)
                else:
                    item.clicked.connect(partial(self.input, item.text()))

    ##  Deletes error message inside error display
    def removeError(self):
        self.error = False
        self.view.setInput("")
        
    ##  Adds an input symbol to the calculator display. 
    #   If last symbol and param value are both + or -, the symbol is replaced instead.
    #
    #   @param value - symbol which will be added to the display
    def input(self, value):
        if self.error == True: 
            self.removeError()
        if(
            (value == "+" or value == "-") and 
            self.view.getInput() != "" and 
            (self.view.getInput()[-1] == "+" or
            self.view.getInput()[-1] == "-")
        ):
            self.view.entryDisplay.backspace()

        self.view.addInput(value)

        self.view.scaleFont(value)
       
        
    ##  Clears the display
    def clearDisplay(self):
        if self.error == True: 
            self.removeError()
        self.view.clearDisplay()

        self.view.scaleFont("c")

        
    ##  Deletes last input symbol
    def deleteLastInput(self):
        if self.error == True: 
            self.removeError()
        if self.view.getInput() != "" and self.view.getInput()[-1] == "n":
            self.view.entryDisplay.backspace()
            self.view.entryDisplay.backspace()
        self.view.entryDisplay.backspace()

        self.view.scaleFont("d")

    ##  Sends the expression inside display to app model, which will evaluate the expression
    def evaluateInput(self):
        if self.error == True: 
            self.removeError()
        input = self.view.getInput()
        self.view.setExpressionDisplay()
        if input != "" and self.error != True:
            try: 
                self.view.setInput(self.model.evaluate(input))
            except ExpressionError as e:
                self.view.setInput(e)
                self.error = True

        self.view.scaleFont("eval")

        
        

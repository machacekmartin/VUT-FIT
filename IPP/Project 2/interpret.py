import sys
import argparse
import xml.etree.ElementTree as ET
import re

threeOperands = ["ADD","SUB","MUL","IDIV","LT","GT","EQ","AND","OR","STRI2INT","CONCAT","GETCHAR","SETCHAR","JUMPIFEQ","JUMPIFNEQ"]
twoOperands = ["MOVE","INT2CHAR","READ","STRLEN","TYPE","NOT"]
oneOperand = ["DEFVAR","CALL","PUSHS","POPS","WRITE","LABEL","JUMP","EXIT","DPRINT"]
noOperand = ["CREATEFRAME","PUSHFRAME","POPFRAME","RETURN","BREAK"]

labelRegex ="^(label@([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*))$"
varRegex = "^(var@)(GF|LF|TF)(@)([a-zA-Z_\-$&%*!?]+[a-zA-Z0-9_\-$&%*!?]*)$"
symbRegex ="^(nil)(@)(nil)$|^(bool)(@)(true|false)$|^(int)(@)(\S+)$|^(string)(@)([\S]*)$|^(var@)(GF|LF|TF)(@)([a-zA-Z_\-$&%*!?]+[a-zA-Z0-9_\-$&%*!?]*)$"
typeRegex ="^type@(bool|int|string)$"

tFrame= None
lFrame= None
gFrame= {}
dataTray = []

inputCode = None
contentFromInput = None

##
#   Overrides the classic error message of argparse
##
class ArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        raise

##
#   Checks the input arguments
#   argparse used, function has to check if the same --arg hasnt been used yet 
#   @return argsArray   array with correct arguments
##
def parseArgs():
    argParser = ArgumentParser(description="argument parser")
    argsArray = {} 

    try:
        argParser.add_argument('--input', action='append')
        argParser.add_argument('--source', action='append')
        args = argParser.parse_args()  

        if (args.input is not None):
            if (len(args.input) > 1):
                argParser.error(1)
            argsArray["input"] = args.input[0]

        if (args.source is not None):
            if (len(args.source) > 1 or (not args.source[0])):
                argParser.error(1)
            argsArray["source"] = args.source[0]

    except:
        sys.exit(10) 
          
    return argsArray

##
#   Prints help to the user.. DUH
##
def printHelpAndExit():
    print(
    '''
    How to run this script:
        python3.6 interpret.py <args>
        where <args> is field of available arguments listed below
    
            Arguments
                --help      
                        prints out this help
                        cannot be used in combination with any other argument
      
                --source=<path>      
                        sets path to a source file to this script
                        if not set, will be loaded from stdin

                --input=<path>
                        sets path to an input file to this script
                        if not set, will be loaded from stdin when needed
                    
                (--input or --source must be set)            
    ''')
    sys.exit(0)

##
#   Checks the file we want to open (if it exists and if we are able to open it)
#   @param  file    file to be checked and opened
#   @return openedFile  opened file
##
def checkAndAssignFile(file):
    try:
        openedFile = open(file, 'r').read()
        return openedFile
    except:
        sys.exit(11) 

##
#   Loads a string with user keyboard input
#   @return code    user's input in string variable
##
def loadFromStdin():
    code = ""
    for line in sys.stdin:
        code += line 

    return code

##
#   Elementtree library checks the syntax for us
#   Check, if main "program" tag exists
#   Also, it converts the xml code input into dictionary structure
#   @param  code    XML code to be converted into dictionary
#   @return eleDict Dictionary made of xml input
##
def lexCheckAndConvert(code):
    try: tree = ET.fromstring(code)
    except: 
        exit(31)

    if ('language' not in tree.attrib):
        sys.exit(32)

    if (tree.tag != 'program' or tree.attrib['language'] != "IPPcode19"):
        sys.exit(32)

    size = 1
    if ('description' in tree.attrib):
        size+=1
    if ('name' in tree.attrib):
        size+=1

    if (len(tree.attrib) != size):
        sys.exit(32)

    eleDict = dict()

    for instr in tree: 
        if (instr.tag != "instruction"):
            sys.exit(32)

        if ((len(instr.attrib) != 2) or ('order' not in instr.attrib) or ('opcode' not in instr.attrib)):
            sys.exit(32)

        order = int(instr.attrib['order'].strip())

        if (order in eleDict):
            sys.exit(32)

        eleDict[order] = {}
        eleDict[order] = instr.attrib
        eleDict[order]['args'] = {}
        
        argTags = {}
        for arg in instr:
            if arg.tag in argTags:
                sys.exit(32)
            argTags[arg.tag] = 1
            eleDict[order]['args'][arg.tag] = {}

            if (len(arg.attrib) != 1 or arg.attrib['type'] is None):
                sys.exit(32)

            if (arg.text is None):
                if arg.attrib['type'] == "string":
                    arg.text = ""
                else:
                    sys.exit(32)
            if ("#" in arg.text):
                sys.exit(32)
                    
            eleDict[order]['args'][arg.tag]['type'] = arg.attrib['type'].strip()
            eleDict[order]['args'][arg.tag]['value'] = arg.text.strip()           

    return eleDict

##
#   Checks if order parameter in instruction elements is correct
#   @param  c   code from syntax analyzator
#   @return     just leave function
##
def checkOrder(c):
    array = []
    orderArray = []

    for x in range(1, len(c)+1):
        array.append(x)
    try:
        for value, key in c.items():
            if (int(key['order']) not in array):
                raise
            else:
                if (int(key['order']) in orderArray):
                    raise

                orderArray.append(int(key['order']))
    except:
        sys.exit(32)
    return

##
#   Checks if all instructions' arguments are in correct form via regex function re.match
#   @param  args    arguments in instruction
#   @param  detail  detailed info of instructions arguments
#                   f.e. lss is short for label, symb, symb (each argument)
##
def checkRegex(args, detail):
    if (len(detail) == 3):
        if (detail == "lss"):
            if (re.match(labelRegex, args[0]) is None):
                sys.exit(32)
            
        elif (detail == "vss"):
            if (re.match(varRegex, args[0]) is None):
                sys.exit(32) 
        
        if (re.match(symbRegex, args[1]) is None or
            re.match(symbRegex, args[2]) is None):

            sys.exit(32)
    elif (len(detail) == 2):
        if (re.match(varRegex, args[0]) is None):
            sys.exit(32)

        if (detail == "vs"):
            if (re.match(symbRegex, args[1]) is None):
                sys.exit(32)

        if (detail == "vt"):
            if (re.match(typeRegex, args[1]) is None):
                sys.exit(32)
    elif (len(detail) == 1):
        if (detail == "l"):
            if (re.match(labelRegex, args[0]) is None):
                sys.exit(32)

        if (detail == "v"):
            if (re.match(varRegex, args[0]) is None):
                sys.exit(32)

        if (detail == "s"):
            if (re.match(symbRegex, args[0]) is None):
                sys.exit(32)
    else:
        sys.exit(32)

##
#   Syntax analyzator
#   @param  code    lexicaly checked and clean dictionary code
#   @return interpretCode   Code ready for interpret formatting
##
def syntaxCheck(code):
    checkOrder(code)
    interpretCode = {}
    
    for value, key in sorted(code.items()):
        key['opcode'] = key['opcode'].strip()
        if (len(key['args']) > 3 ):
           sys.exit(32)
        interpretCode[key['order']] = key['order']
        interpretCode[key['order']] = {}
        interpretCode[key['order']][0] = key['opcode']

        if (key['opcode'] in threeOperands):
            if (len(key['args']) != 3):
                sys.exit(32)

            if ('arg1' not in key['args'] or 'arg2' not in key['args'] or 'arg3' not in key['args']):
                sys.exit(32)

            typesAndValues = [key['args']['arg1']['type']+'@'+key['args']['arg1']['value'],
                              key['args']['arg2']['type']+'@'+key['args']['arg2']['value'],
                              key['args']['arg3']['type']+'@'+key['args']['arg3']['value']
                              ]
            if (key['opcode'] == "JUMPIFEQ" or key['opcode'] == "JUMPIFNEQ"):
                checkRegex(typesAndValues, "lss")
            else:
                checkRegex(typesAndValues, "vss")

            interpretCode[key['order']][1] = typesAndValues[0]
            interpretCode[key['order']][2] = typesAndValues[1]
            interpretCode[key['order']][3] = typesAndValues[2]
            
        elif (key['opcode'] in twoOperands):
            if (len(key['args']) != 2):
                sys.exit(32)

            if ('arg1' not in key['args'] or 'arg2' not in key['args']):
                sys.exit(32)

            typesAndValues = [key['args']['arg1']['type']+'@'+key['args']['arg1']['value'],
                              key['args']['arg2']['type']+'@'+key['args']['arg2']['value']
                              ]
            if (key['opcode'] == "READ"):
                checkRegex(typesAndValues, "vt")
            else:
                checkRegex(typesAndValues, "vs")

            interpretCode[key['order']][1] = typesAndValues[0]
            interpretCode[key['order']][2] = typesAndValues[1]

        elif (key['opcode'] in oneOperand):
            if (len(key['args']) != 1):
                sys.exit(32)

            if ('arg1' not in key['args']):
                sys.exit(32)

            typesAndValues = [key['args']['arg1']['type']+'@'+key['args']['arg1']['value']]
            if (key['opcode'] == "PUSHS" or key['opcode'] == "WRITE" or
                key['opcode'] == "EXIT" or key['opcode'] == "DPRINT"):
                
                checkRegex(typesAndValues, "s")
            elif(key['opcode'] == "CALL" or key['opcode'] == "LABEL" or
                 key['opcode'] == "JUMP"):

                checkRegex(typesAndValues, "l")
            else:
                checkRegex(typesAndValues, "v")

            interpretCode[key['order']][1] = typesAndValues[0]

        elif (key['opcode'] in noOperand):
            if (len(key['args']) != 0):
                sys.exit(32)
        else:
            sys.exit(32)

    return interpretCode

##
#   Makes a code ready for interpret
#   @param  code    syntax checked code
#   @return data    code ready for interpretation
##
def setupCodeForInterpret(code): 
    data = list(code.values())
    for command in data:
        i = 0
        while i != len(command):
            if (command[i].count("@") == 2):
                command[i] = command[i].split("@")
                del command[i][0]
                command[i] = command[i][0]+"@"+command[i][1]
                
            i+=1    
    return data

##
#   Finds a variable in a scope given by user cycling through frames
#   @param  v       variable to be found
#   @param  f       frame to search in
#   @return item    found item
##
def findVariable(v, f):
    global tFrame, lFrame, gFrame
    if (f == None):
        sys.exit(55)

    if isinstance(f, dict):
        for key, item in f.items():
            if item[0] == v[1]:
                return item
    else:
        for item in f:
            if (item[0] == v[1]):
                return item
    sys.exit(54)

##
#   Returns type and value of given item (symbol).
#   Set second argument "needNone" to True and it wont exit when type or value is None 
#   @param  symb            item of which the function returns type and value
#   @param  NeedNone=False  default definition for ignoring None types and values
#   @return type, value     type and value of param symb
##
def getTypeAndValue(symb, needNone=False):
    try:
        if symb[0] == "TF":
            value = findVariable(symb, tFrame)[2]
            type = findVariable(symb, tFrame)[1]
        elif symb[0] == "LF":
            value = findVariable(symb, lFrame[-1])[2]
            type = findVariable(symb, lFrame[-1])[1]
        elif symb[0] == "GF":
            value = findVariable(symb, gFrame)[2]
            type = findVariable(symb, gFrame)[1]
        else:
            value = symb[1]
            type = symb[0]
    except (TypeError, IndexError, AttributeError):
        sys.exit(55)

    if (needNone is False and (type is None or value is None)):
        sys.exit(56)

    return type, value

##
#   Updates information of variable (type, value)
#   This function uses function findVariable to search for the variable
#   @param  var     variable to be updated
#   @param  type    new type
#   @param  value   new value
##
def updateVar(var, type, value):
    try:
        if var[0] == "TF":
            index = tFrame.index(findVariable(var, tFrame))
            tFrame[index][1] = type
            tFrame[index][2] = value
        elif var[0] == "LF":
            index = lFrame[-1].index(findVariable(var, lFrame[-1]))
            lFrame[-1][index][1] = type
            lFrame[-1][index][2] = value
        elif var[0] == "GF":
            index = findVariable(var, gFrame)
            gFrame[gFrame[index[0]][0]][1] = type
            gFrame[gFrame[index[0]][0]][2] = value
        else:
            sys.exit(32)
    except (TypeError, IndexError, AttributeError):
        sys.exit(55)

##
#   Moves (copies) a certain value of symbol to the given variable 
#   @param  var     variable to which the value will be copied
#   @param  symb    symbol from which the value will be copied
##
def move(var, symb):
    global tFrame, lFrame, gFrame
    sourceType, sourceValue = getTypeAndValue(symb)
    updateVar(var, sourceType, sourceValue)    

##
#   Checks if given frame exists and if given variable is already in it
#   @param  var     variable to look for
#   @param  frame   frame to check
#   @return         returns back
##
def checkFrameAndVarSpace(var, frame):
    if (frame is None):
        sys.exit(55)
    if (var in frame):
        sys.exit(52)
    return

##
#   Creates variable in given frame
#   @param  frame       where the variable will be created
#   @param  var_name    variable name
##
def defvar(frame, var_name):
    global lFrame, tFrame, gFrame
    try:
        if (frame == "TF"):
            checkFrameAndVarSpace(var_name, tFrame)   
            tFrame.append([var_name, None, None]) 
        elif (frame == "LF"):
            checkFrameAndVarSpace(var_name, lFrame) 
            lFrame[-1].append([var_name, None, None]) 
        elif (frame == "GF"):
            checkFrameAndVarSpace(var_name, gFrame)   
            gFrame[var_name] = [var_name, None, None]
        else:
            sys.exit(55)
    except (TypeError, IndexError, AttributeError):
        sys.exit(55)

##
#   Creates new temporary frame / clears existing temporary frame
##
def createFrame():
    global tFrame
    if (tFrame is None):
        tFrame = []
    tFrame.clear()

##
#   Moves temporary frame to local frame if they exist
##
def pushFrame():
    global tFrame, lFrame

    if (tFrame is None):
        sys.exit(55)
    if (lFrame is None):
        lFrame = []
        
    lFrame.append(tFrame)
    tFrame = None

##
#   Moves the first frame in tray of frames(LF) into temporary fram
##
def popFrame():
    global lFrame, tFrame
    if (lFrame is None):
        sys.exit(55)

    if (tFrame is None):
        tFrame = []
        
    try: tFrame = lFrame[-1]
    except IndexError: sys.exit(55)
    del lFrame[-1]

##
#   Adds a value(and its type) into data tray
#   @param  symb    symbol from which the value will be copied
##
def pushs(symb):
    type, value = getTypeAndValue(symb)
    dataTray.append([type, value])

##
#   Removes the first value in data tray and saves it into given variable
#   @param  var     chosen var to which the value from data tray will be saved
##
def pops(var):
    if not dataTray:
        sys.exit(56)
    
    updateVar(var, dataTray[-1][0], dataTray[-1][1]) 
    del dataTray[-1]

##
#   Sets up variables needed for arithmetic operations
#   @params s1,s2               variables to be initialized
#   @return s1t, s1v, s2t, s2v  types and values of two variables
##
def setupSymbols(s1, s2):
    s1t,s1v = getTypeAndValue(s1) 
    s2t,s2v = getTypeAndValue(s2)
    return s1t, s1v, s2t, s2v

##
#   Checks types of given variables
#   @params s1,s2               variables to be checked
#   @param  type                desired type
#   @return s1_value, s2_value  values of 2 variables
##
def arithmeticTypeCheck(s1, s2, type):
    s1_type, s1_value, s2_type, s2_value = setupSymbols(s1, s2)
    if (s1_type != s2_type or s1_type != type):
        sys.exit(53)

    return s1_value, s2_value

##
#   Arithmetic functions
#   @param  var             variable to which the result will be saved 
#   @params symb1, symb2    operators to which functions will apply the operations
##
def add(var, symb1, symb2):
    s1_value, s2_value = arithmeticTypeCheck(symb1, symb2, "int")
    updateVar(var, "int", int(s1_value) + int(s2_value))

def sub(var, symb1, symb2):
    s1_value, s2_value = arithmeticTypeCheck(symb1, symb2, "int")
    updateVar(var, "int", int(s1_value) - int(s2_value))   

def mul(var, symb1, symb2):
    s1_value, s2_value = arithmeticTypeCheck(symb1, symb2, "int")
    updateVar(var, "int", int(s1_value) * int(s2_value))  

def idiv(var, symb1, symb2):
    s1_value, s2_value = arithmeticTypeCheck(symb1, symb2, "int")
    if int(s2_value) == 0:
        sys.exit(57)
    updateVar(var, "int", int(s1_value) / int(s2_value)) 

##
#   Converts string to bool
#   @param  s   string  
##
def strToBool(s):
    if s == "true":
        return True
    elif s == "false":
        return False

##
#   Compares special connections given in function ltgteq(var, symb1, symb2, operator)
##
def compare(s1, s2, operator):
    value = "false"
    if (operator == "LT"):
        if s1 < s2:
            value = "true"
    elif (operator == "GT"):
        if s1 > s2:
            value = "true"
    elif (operator == "EQ"):
        if s1 == s2:
            value = "true"

    return value

##
#   Does less than, greater than, equals on symb1 and symb2 with given operation and saves result to variable var
#   @param  var             variable to which functions will save result to  
#   @params symb1,symb2     compared symbols
#   @param  operator        one of mentioned operators
##
def ltgteq(var, symb1, symb2, operator):
    s1_type, s1_value = getTypeAndValue(symb1)
    s2_type, s2_value = getTypeAndValue(symb2)
    if (s1_type != "nil" and s2_type != "nil" and s1_type != s2_type):
        sys.exit(53)
    else:
        value = ""
        if s1_type == "nil" or s2_type == "nil":
            if (operator != "EQ"):
                sys.exit(53)
            value = compare(s1_value, s2_value, "EQ")
        elif s1_type == "int":
            try:
                s1_value = int(s1_value)
                s2_value = int(s2_value)
                value = compare(s1_value, s2_value, operator)
            except: sys.exit(32)
        elif s1_type == "bool":
            s1_value = strToBool(s1_value)
            s2_value = strToBool(s2_value)   
            value = compare(s1_value, s2_value, operator)

    updateVar(var, "bool", value)

##
#   Converts symbol's value into integer and saves the result to variable
#   @param  var     variable to which functions will save result to  
#   @param  symb    symbol from which the value will be taken 
##
def int2char(var, symb):
    type, value = getTypeAndValue(symb)
    if type != "int":
        sys.exit(53)
    value = int(value)
    try: result = chr(value)
    except: sys.exit(58)

    updateVar(var, "int", result)

##
#   Takes a character at a given position inside a symbol and converts it into integer
#   @param  var     variable to which functions will save result to  
#   @param  symb1   symbol from which the value will be taken (at a symb2 position) 
#   @param  symb2   character position (integer)
##
def stri2int(var, symb1, symb2):
    s1_type, s1_value = getTypeAndValue(symb1)
    s2_type, s2_value = getTypeAndValue(symb2)
    if s1_type != "string" or s2_type != "int":
        sys.exit(53)
    try:
        result = s1_value[int(s2_value)]
        result = ord(result)
    except: sys.exit(58)
    updateVar(var, "int", result)

##
#   Prints given value of symbol to the user
#   @param  symb    given symbol from which the value will be taken
##
def write(symb):
    type, value = getTypeAndValue(symb)
    if type == "nil":
        value = ""
    print(value)

##
#   Connects two strings and saves the result into chosen variable
#   @param  var         chosen variable
#   @param  symb1,symb2 symbols to be connected  
##
def concat(var, symb1, symb2):
    s1_type, s1_value = getTypeAndValue(symb1)
    s2_type, s2_value = getTypeAndValue(symb2)
    if (s1_type != "string" or s2_type != "string"):
        sys.exit(53)
    try: result = s1_value + s2_value
    except: sys.exit(58)
    updateVar(var, "string", result)

##
#   Calculates length of given symbol and saves the value into variable
#   @param  var     chosen variable
#   @param  symb    symbol to be measured
##
def strlen(var, symb):
    type, value = getTypeAndValue(symb)
    if (type != "string"):
        sys.exit(53)
    updateVar(var, "int", len(value))

##
#   Retrieves a character at a specific position and saves it to given variable
#   @param  var     chosen variable
#   @param  symb1   symbol from which the character will be taken (at a symb2 position) 
#   @param  symb2   character position (integer)  
##
def getchar(var, symb1, symb2):
    s1_type, s1_value = getTypeAndValue(symb1)
    s2_type, s2_value = getTypeAndValue(symb2)
    if (s1_type != "string" or s2_type != "int"):
        sys.exit(53)
    try: value = s1_value[int(s2_value)]
    except: sys.exit(58)
    updateVar(var, "string", value)

##
#   Replaces character at a given position into given character
#   @param  var     chosen variable to replace character inside
#   @param  symb1   symbol with position of variable character
#   @param  symb2   symbol with new character
##
def setchar(var, symb1, symb2):
    var_type, var_value = getTypeAndValue(var)
    s1_type, s1_value = getTypeAndValue(symb1)
    s2_type, s2_value = getTypeAndValue(symb2)
    if (var_type != "string" or s1_type != "int" or s2_type != "string"):
        sys.exit(53)
    try:
        value = list(var_value)
        value[int(s1_value)] = s2_value[0]
        value = ''.join(value)
    except: sys.exit(58)    
    updateVar(var, "string", value)

##
#   Retrieves type of given symbol and saves it as a string into variable
#   @param  var     chosen variable to which the type will be saved
#   @param  symb    symbol with type
##
def itype(var, symb):
    type, value = getTypeAndValue(symb, True)
    if type == None or value == None:
        type = ""
    updateVar(var, "string", type)

##
#   Exits program with return value of given symbol's value
#   @param  symb    chosen symbol from which the value will be taken
##
def iexit(symb):
    type, value = getTypeAndValue(symb)
    if type != "int":
        sys.exit(53)
    if int(value) not in range(0,50):
        sys.exit(57)

    sys.exit(int(value))

##
#   Prints a value of symbol into stderr
#   @param  symb    chosen symbol from which the value will be taken
##
def dprint(symb):
    type, value = getTypeAndValue(symb, True)
    sys.stderr.write(value)
  
##
#   Logical operations OR or AND
#   @param  var             variable to which functions will save result to  
#   @params symb1,symb2     compared symbols
#   @param  operator        one of mentioned operators
##
def andor(var, symb1, symb2, operator):
    s1_type, s1_value = getTypeAndValue(symb1)
    s2_type, s2_value = getTypeAndValue(symb2)
    if (s1_type != "bool" or s2_type != "bool"):
        sys.exit(53)
    value ="false"
    if operator == "AND":
        if s1_value == "true" and s2_value == "true":
            value = "true"
    elif operator == "OR":
        if s1_value == "true" or s2_value == "true":
            value = "true"

    updateVar(var, "bool", value)

##
#   Logical operations OR or AND
#   @param  var             variable to which functions will save result to  
#   @params symb1,symb2     compared symbols
#   @param  operator        one of mentioned operators
##
def inot(var, symb):
    type, value = getTypeAndValue(symb)
    if type != "bool":
        sys.exit(53)
    if value == "false":
        updateVar(var, "bool", "true") 
    else:
        updateVar(var, "bool", "false") 

##
#   Reads a line input from stdin or from a file and saves value with given type to variable
#   @param  var     variable to which the 
#   @param  type    given type by user inside attribute 
##
def read(var, type):
    global inputCode, contentFromInput
    type, value = getTypeAndValue(type)
    if inputCode is None:
        try: line = input()
        except: line=""
    else:
        if (contentFromInput is None):
            contentFromInput = open(inputCode, 'r')
        
        line = contentFromInput.readline().rstrip() 
            
    if value == "string":
        updateVar(var, "string", line)
    elif value == "bool":
        if line.lower() == "true":
            updateVar(var, "bool", "true")
        else:
            updateVar(var, "bool", "false")
    elif value == "int":
        try: updateVar(var, "int", str(int(line)))
        except: updateVar(var, "int", "0")

##
#   Runs the whole interpretation of this project, decides to which function the 
#   program will be sent to by instruction name
#   @param  code    code ready for interpretation
##
def interpret(code):
    for instr in code:
        i = 1
        while i != len(instr):
            instr[i]=instr[i].split("@")
            i+=1
        if (instr[0] == "MOVE"):
            move(instr[1], instr[2])

        elif (instr[0] == "DEFVAR"):
            defvar(instr[1][0], instr[1][1])

        elif (instr[0] == "CREATEFRAME"):
            createFrame()
        
        elif (instr[0] == "PUSHFRAME"):
            pushFrame()

        elif (instr[0] == "POPFRAME"):
            popFrame()

        elif (instr[0] == "CALL"):
            continue

        elif (instr[0] == "RETURN"):
            continue

        elif (instr[0] == "PUSHS"):
            pushs(instr[1])

        elif (instr[0] == "POPS"):
            pops(instr[1])

        elif (instr[0] == "ADD"):
            add(instr[1], instr[2], instr[3])

        elif (instr[0] == "SUB"):
            sub(instr[1], instr[2], instr[3])

        elif (instr[0] == "MUL"):
            mul(instr[1], instr[2], instr[3])

        elif (instr[0] == "IDIV"):
            idiv(instr[1], instr[2], instr[3])

        elif (instr[0] == "LT"):
            ltgteq(instr[1], instr[2], instr[3], "LT")

        elif (instr[0] == "GT"):
            ltgteq(instr[1], instr[2], instr[3], "GT")

        elif (instr[0] == "EQ"):
            ltgteq(instr[1], instr[2], instr[3], "EQ")

        elif (instr[0] == "AND"):
            andor(instr[1], instr[2], instr[3], "AND")

        elif (instr[0] == "OR"):
            andor(instr[1], instr[2], instr[3], "OR")

        elif (instr[0] == "NOT"):
            inot(instr[1], instr[2])

        elif (instr[0] == "INT2CHAR"):
            int2char(instr[1], instr[2])

        elif (instr[0] == "STRI2INT"):
            stri2int(instr[1], instr[2], instr[3])

        elif (instr[0] == "READ"):
            read(instr[1], instr[2]);

        elif (instr[0] == "WRITE"):
            write(instr[1])

        elif (instr[0] == "CONCAT"):
            concat(instr[1], instr[2], instr[3])

        elif (instr[0] == "STRLEN"):
            strlen(instr[1], instr[2])

        elif (instr[0] == "GETCHAR"):
            getchar(instr[1], instr[2], instr[3])

        elif (instr[0] == "SETCHAR"):
            setchar(instr[1], instr[2], instr[3])

        elif(instr[0] == "TYPE"):
            itype(instr[1], instr[2])

        elif (instr[0] == "LABEL"):
            continue

        elif (instr[0] == "JUMP"):
            continue

        elif (instr[0] == "JUMPIFEQ"):
            continue

        elif (instr[0] == "JUMPIFNEQ"):
            continue

        elif (instr[0] == "EXIT"):
            iexit(instr[1])

        elif (instr[0] == "DPRINT"):
            dprint(instr[1])

        elif (instr[0] == "BREAK"):
            continue

        else:
            sys.exit(32)
##
#   MAIN FUNCTION - checks --help and assigns source and input codes from files or stdin
##
def main():
    global inputCode;

    if ("--help" in sys.argv):
        if (len(sys.argv) != 2):
            exit(10)
        printHelpAndExit()    

    if (len(sys.argv) == 1):
        exit(10)
    else:
        args = parseArgs()
        if ('source' in args):
            sourceCode = checkAndAssignFile(args["source"])
        if ('input' in args):
            inputCode = args["input"]
        if ('source' not in args):
            sourceCode = loadFromStdin()  

        dictLexOk = lexCheckAndConvert(sourceCode)
        syntaxOk = syntaxCheck(dictLexOk)
        interpretReady = setupCodeForInterpret(syntaxOk)
        interpret(interpretReady)
main()
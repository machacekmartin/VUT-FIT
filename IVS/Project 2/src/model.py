## Calculator model
# @package model
#
# @brief Evaluation model, which provides method to evaluate expression from given input and uses mathematical library
#
# @author Zdenek Dolezal (xdolez82@stud.fit.vutbr.cz)
#
# @version 1.0

import matlib
import re

class ExpressionError(Exception):
    def __init__(self, *args):
        if len(args) == 0:
            self.message = 'Syntax error'
        else:
            self.message = args[0]

    def __str__(self):
        return self.message         

class Model:
    ## Evaluates expression - https://en.wikipedia.org/wiki/Shunting-yard_algorithm
    # @param str_expr - expression as a string
    # 
    # @exception ExpressionError - if expression cannot be evalidated or is invalid
    #
    # @return Evaluated expression (result of expression) as digit (float or int)
    def evaluate(self, strExpr):
        values = []
        ops = []

        i = 0

        wasNumberOrPar = False
        while i < len(strExpr):

            # ignore whitespaces
            if strExpr[i].isspace():
                i += 1
                continue

            # input is (
            elif strExpr[i] == '(':
                ops.append(strExpr[i])
                wasNumberOrPar = False

            # input is digit or pi
            elif strExpr[i].isdigit() or strExpr[i] == "\u03c0" or ((i == 0 or strExpr[i - 1] == '(') and strExpr[i] in ('-', '+') and i + 1 < len(strExpr) and strExpr[i + 1].isdigit()):   
                numStr = ''
                if strExpr[i] in ('-', '+'):
                    numStr += strExpr[i]
                    i += 1

                while i < len(strExpr) and (strExpr[i].isdigit() or strExpr[i] == '.' or strExpr[i] == "\u03c0"):
                    numStr += strExpr[i]
                    i += 1
                i -= 1 # index is increased by one for the whole while cycle 
                num = self.__convertNumber(numStr)
                values.append(num)
                wasNumberOrPar = True

            # input is )
            elif strExpr[i] == ')':
                while len(ops) != 0 and ops[-1] != '(':
                    self.__output(ops, values)
                if len(ops) == 0:
                    raise ExpressionError('Missing parenthesis')
                ops.pop()
                wasNumberOrPar = True

            # input has to be operator
            else:
                op = self.__parseOperator(strExpr, i)
                if op == 'sin':
                    if wasNumberOrPar:
                        raise ExpressionError()
                    i += 2
                elif op == '!' and not wasNumberOrPar:
                    raise ExpressionError()

                while len(ops) != 0 and self.__getPrecedence(ops[-1]) >= self.__getPrecedence(op):
                    self.__output(ops, values)
                ops.append(op)

                wasNumberOrPar = False
            i += 1

        while len(ops) != 0:
            self.__output(ops, values)

        if len(values) > 1:
            raise ExpressionError()

        return values[-1]

    def __parseOperator(self, strExpr, i):
        op = strExpr[i]
        # parsing of sinus
        if op == 's':
            if i + 2 < len(strExpr):
                op += strExpr[i + 1] + strExpr[i + 2]
                if op != 'sin':
                    raise ExpressionError()
        return op

    def __output(self, ops, values):
        currOp = ops.pop()
        if self.__isUnary(currOp):
            if len(values) < 1:
                raise ExpressionError()
            val = values.pop()
            values.append(self.__applyOperator(currOp, val))
        else:
            # Behave differently if value is left with unary minus
            if currOp == '-' and len(values) == 1:
                val = values.pop()
                values.append(self.__applyOperator(currOp, val))
            else:
                if len(values) < 2:
                    raise ExpressionError()
                val2 = values.pop()
                val1 = values.pop()

                values.append(self.__applyOperator(currOp, val1, val2))

    def __convertNumber(self, strNum):
        if re.match(r'^(\+|-)?\d+\.\d+$', strNum):
            return float(strNum)
        elif re.match(r'^(\+|-)?\d+$', strNum):
            return int(strNum)
        elif strNum == "\u03c0":
            return matlib.PI
        else:
            raise ExpressionError('Invalid number')

    def __applyOperator(self, op, val1, val2=None):
        if op in ('+', '-', '\u00f7', '\u00d7', '\u221a', '^'):
            if op == '-' and val2 is None:
                val2 = matlib.multiply(2, val1)
            if val2 is None:
                raise ExpressionError('Operator: {} expects 2 values'.format(op))

            if op == '+':
                return matlib.add(val1, val2)
            elif op == '-':
                return matlib.subtract(val1, val2)
            elif op == '\u00f7':
                try:
                    return matlib.divide(val1, val2)
                except ValueError:
                    raise ExpressionError('Cannot divide by zero!')
            elif op == '\u00d7':
                return matlib.multiply(val1, val2)
            elif op == '\u221a':
                try:
                    return matlib.nthRoot(val2, val1)
                except (ValueError, TypeError):
                    raise ExpressionError('Math error')
            elif op == '^':
                try:
                    return matlib.pow(val1, val2)
                except (ValueError, TypeError):
                    raise ExpressionError('Math error')

        elif op in ('!', 'sin'):
            if val2 is not None:
                raise ExpressionError('Operator: {} expects 1 values'.format(op))

            if op == '!':
                try:
                    return matlib.factorial(val1)
                except (ValueError, TypeError):
                    raise ExpressionError('Math error')
            elif op == 'sin':
                return matlib.sin(val1)
        else:
            raise ExpressionError('Unknown operator: {}'.format(op))

    def __isUnary(self, op):
        return op in ('!', 'sin')

    def __getPrecedence(self, op):
        if op in ('+', '-'):
            return 1
        elif op in ('\u00d7', '\u00f7'):
            return 2
        elif op in ('\u221a', '^'):
            return 3
        elif op in ('!', 'sin'):
            return 4
        elif op in ('(', ')'):
            return 0
        else:
            raise ExpressionError('Unknown precedence for operator: {}'.format(op))

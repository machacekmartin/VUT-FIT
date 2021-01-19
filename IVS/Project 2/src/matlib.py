## Mathematical library
# @package matlib
#
# @brief Simple math library supporting basic math operations, factorial, exponentiation, nth root, sin
#
# @author David Gajdos (xgajdo22@stud.fit.vutbr.cz)
#
# @version 1.0

import math

PI = math.pi

## Adds two numbers and returns their result
#
# @param x - first number to be added
# @param y - second number to be added
#
# @exception TypeError - if other than int/float parameters are given
#
# @return Sum of parameters
def add(x, y):
    allowedTypes = (int, float)

    if not isinstance(x,allowedTypes) or not isinstance(y,allowedTypes):
        raise TypeError('Wrong type of parameter(s), only integers and floats are allowed!')

    return x+y

## Subtracts second number from first and returns result
#
#  @param x - number to be subtracted from 
#  @param y - number to be subtracted
#
#  @exception TypeError - if other than int/float parameters are given
#
#  @return Result of x-y
def subtract(x, y):
    allowedTypes = (int, float)

    if not isinstance(x,allowedTypes) or not isinstance(y,allowedTypes):
        raise TypeError('Wrong type of parameter(s), only integers and floats are allowed!')

    return x-y

## Multiplies two numbers and returns their result 
#
#  @param x - first number to be multiplied 
#  @param y - second number to be multiplied
#
#  @exception TypeError - if other than int/float parameters are given
#
#  @return Result of parameters multiplied
def multiply(x, y):
    allowedTypes = (int, float)

    if not isinstance(x,allowedTypes) or not isinstance(y,allowedTypes):
        raise TypeError('Wrong type of parameter(s), only integers and floats are allowed!')

    return x*y


## Divides first number by second and returns result
#
#  @param x - number to be divided
#  @param y - number to be divided by
#
#  @exception TypeError - if other than int/float parameters are given
#  @exception ValueError - if second parameter is 0
#
#  @return Result of first parameter divided by second
def divide(x, y):
    allowedTypes = (int, float)

    if not isinstance(x,allowedTypes) or not isinstance(y,allowedTypes):
        raise TypeError('Wrong type of parameter(s), only integers and floats are allowed!')

    if y == 0:
        raise ValueError('Division by zero!')

    return x/y

## Recursive factorial function
#
#  @param n - natural number to be calculated
#
#  @exception TypeError - if other than int value is given as parameter
#  @exception ValueError - if parameter is negative integer
#
#  @return Result of factorial
def factorial(n):
    if not isinstance(n,int):
        raise TypeError('Wrong type of parameter, only integers allowed!')

    if n < 0:
        raise ValueError('Factorial can only be computed from number greater or equal to 0!')

    if n == 0:
        return 1
    elif n == 1:
        return 1
    else:
        return (n * factorial(n-1))

## Exponentiation function
#
#  @param x - base
#  @param y - exponent
#
#  @exception TypeError - if other than int/float value is given for base or other than int value for exponent
#  @exception ValueError - if exponent is negative
#
#  @return Base to the power of exponent
def pow(x, y):
    if not isinstance(x,(int, float)):
        raise TypeError('Wrong type of base, only integers and floats allowed!')
    
    if not isinstance(y,int):
        raise TypeError('Wrong type of exponent, only integers allowed!')

    if y < 0:
        raise ValueError('Exponent can only be positive integer!')

    return x**y

## Calculates nth root of a number. Function considers only real numbers
#
#  @param x - base of root
#  @param n - root degree
#
#  @exception TypeError - if other than int/float parameters are given
#  @exception ValueError - if degreee is 0 or if degree is even and base is negative
#
#  @warning In case you pass negative base and odd degree, returns only one real root (no imaginary ones) 
#
#  @return Nth root of a number
def nthRoot(x, n):
    allowedTypes = (int, float)

    if not isinstance(x,allowedTypes) or not isinstance(n,allowedTypes):
        raise TypeError('Wrong type of parameters, only integers and floats allowed!')

    if x < 0 and (n%2) == 0:
        raise ValueError('Even root of negative number is not defined in real numbers!')

    if n == 0:
        raise ValueError('\"Zeroth\" root of number doesn\'t exist.')

    #if odd root of negative number => just flip sign and then return with minus || returns only real root and ignores imaginary ones
    if x < 0:
        root = (-x)**(1.0/n)
        return -root
    else:
        return x**(1.0/n)

## Calculates sin of angle
#
#  @param angle - angle in radians
#
#  @exception TypeError - if angle is of different type than int/float
#
#  @return Sinus of angle
def sin(angle):
    if not isinstance(angle,(int, float)):
        raise TypeError('Sin of angle can only be calculated from integer or float!')

    return math.sin(angle)

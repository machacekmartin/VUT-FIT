# Tests for math library (Test driven development)
# Author: David Gajdos (xgajdo22@stud.fit.vutbr.cz)

import unittest
import matlib

class TestMatLib(unittest.TestCase):
    intNums1 = [10, 0, 100, -5, -1, -100]
    intNums2 = [5, 1, 200, 5, -1, -200]

    floatNums1 = [1.5, 0.5, 1.23456, -10.59, -10.59, -100.123]
    floatNums2 = [2.5, 0.15, 2.987654, 10.58, -15.41, -200.456]

    def test_add(self):
        intResults = [15, 1, 300, 0, -2, -300]
        floatResults = [4.0, 0.65, 4.222214, -0.01, -26.0, -300.579]

        length = len(intResults)

        #Integers
        for i in range(length):
            self.assertEqual(matlib.add(self.intNums1[i],self.intNums2[i]),intResults[i])

        #Floats
        for i in range(length):
            self.assertAlmostEqual(matlib.add(self.floatNums1[i],self.floatNums2[i]),floatResults[i])

        #Wrong data types
        self.assertRaises(TypeError,matlib.add,'textext','somemoretext')
        self.assertRaises(TypeError,matlib.add,'textext',5)
        self.assertRaises(TypeError,matlib.add,['ovoce','zelenina'],'nejakyText')
        self.assertRaises(TypeError,matlib.add,{'apple','banana'},10)
        self.assertRaises(TypeError,matlib.add,[5],5)
        self.assertRaises(TypeError,matlib.add,{5},5)

    def test_subtract(self):
        intResults = [5, -1, -100, -10, 0, 100]
        floatResults = [-1.0, 0.35, -1.753094, -21.17, 4.82, 100.333]

        length = len(intResults)

        #Integers
        for i in range(length):
            self.assertEqual(matlib.subtract(self.intNums1[i],self.intNums2[i]),intResults[i])
        
        #Floats
        for i in range(length):
            self.assertAlmostEqual(matlib.subtract(self.floatNums1[i],self.floatNums2[i]),floatResults[i])

        #Wrong data types
        self.assertRaises(TypeError,matlib.subtract,'textext','somemoretext')
        self.assertRaises(TypeError,matlib.subtract,'textext',5)
        self.assertRaises(TypeError,matlib.subtract,['ovoce','zelenina'],'nejakyText')
        self.assertRaises(TypeError,matlib.subtract,{'apple','banana'},10)
        self.assertRaises(TypeError,matlib.subtract,[5],5)
        self.assertRaises(TypeError,matlib.subtract,{5},5)

    def test_multiply(self):
        intResults = [50, 0, 20000, -25, 1, 20000]
        floatResults = [3.75, 0.075, 3.688438122, -112.0422, 163.1919, 20070.256088]

        length = len(intResults)

        #Integers
        for i in range(length):
            self.assertEqual(matlib.multiply(self.intNums1[i],self.intNums2[i]),intResults[i])

        #Floats
        for i in range(length):
            self.assertAlmostEqual(matlib.multiply(self.floatNums1[i],self.floatNums2[i]),floatResults[i])

        #Wrong data types
        self.assertRaises(TypeError,matlib.multiply,'textext','somemoretext')
        self.assertRaises(TypeError,matlib.multiply,'textext',5)
        self.assertRaises(TypeError,matlib.multiply,['ovoce','zelenina'],'nejakyText')
        self.assertRaises(TypeError,matlib.multiply,{'apple','banana'},10)
        self.assertRaises(TypeError,matlib.multiply,[5],5)
        self.assertRaises(TypeError,matlib.multiply,{5},5)

    def test_divide(self):
        intResults = [2, 0, 0.5, -1, 1, 0.5]
        floatResults = [0.6, 3.3333333, 0.4132205, -1.00094517, 0.68721609, 0.49947619]

        length = len(intResults)

        #Integers
        for i in range(length):
            self.assertAlmostEqual(matlib.divide(self.intNums1[i],self.intNums2[i]),intResults[i])

        #Floats
        for i in range(length):
            self.assertAlmostEqual(matlib.divide(self.floatNums1[i],self.floatNums2[i]),floatResults[i])

        #Wrong data types
        self.assertRaises(TypeError,matlib.divide,'textext','somemoretext')
        self.assertRaises(TypeError,matlib.divide,'textext',5)
        self.assertRaises(TypeError,matlib.divide,['ovoce','zelenina'],'nejakyText')
        self.assertRaises(TypeError,matlib.divide,{'apple','banana'},10)
        self.assertRaises(TypeError,matlib.divide,[5],5)
        self.assertRaises(TypeError,matlib.divide,{5},5)

        #Division by zero
        self.assertRaises(ValueError,matlib.divide,10,0)

    def test_factorial(self):
        faktResults = [1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800]

        #Factorial for natural numbers 0...10
        for i in range(11):
            self.assertEqual(matlib.factorial(i),faktResults[i])

        #Trying with higher numbers
        self.assertEqual(matlib.factorial(12),479001600)
        self.assertEqual(matlib.factorial(15),1307674368000)
        self.assertEqual(matlib.factorial(20),2432902008176640000)

        #Expecting value error
        self.assertRaises(ValueError,matlib.factorial,-1)
        self.assertRaises(ValueError,matlib.factorial,-10)
        
        #Wrong data types
        self.assertRaises(TypeError,matlib.factorial,'textext')
        self.assertRaises(TypeError,matlib.factorial,[4,4,9,8])
        self.assertRaises(TypeError,matlib.factorial,0.5)
        self.assertRaises(TypeError,matlib.factorial,10.123)
        self.assertRaises(TypeError,matlib.factorial,-1.234)

    def test_pow(self):
        pow2Results = [0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100]
        pow3Results = [0, 1, 8, 27, 64, 125, 216, 343, 512, 729, 1000]
        pow4Results = [0, 1, 16, 81, 256, 625, 1296, 2401, 4096, 6561, 10000]

        #Checking natural numbers 0...10 to the power of ^2,^3 and ^4
        for i in range(11):
            self.assertEqual(matlib.pow(i,2),pow2Results[i])
            self.assertEqual(matlib.pow(i,3),pow3Results[i])
            self.assertEqual(matlib.pow(i,4),pow4Results[i])

        #Higher powers of integers
        self.assertEqual(matlib.pow(0,100),0)
        self.assertEqual(matlib.pow(0,100000),0)
        self.assertEqual(matlib.pow(0,123456),0)
        self.assertEqual(matlib.pow(1,10),1)
        self.assertEqual(matlib.pow(1,100),1)
        self.assertEqual(matlib.pow(1,1000),1)
        self.assertEqual(matlib.pow(2,10),1024)
        self.assertEqual(matlib.pow(2,20),1048576)
        self.assertEqual(matlib.pow(5,7),78125)
        self.assertEqual(matlib.pow(7,8),5764801)
        self.assertEqual(matlib.pow(10,6),1000000)

        self.assertEqual(matlib.pow(-1,2),1)
        self.assertEqual(matlib.pow(-1,15),-1)
        self.assertEqual(matlib.pow(-5,3),-125)
        self.assertEqual(matlib.pow(-5,4),625)
        self.assertEqual(matlib.pow(-10,7),-10000000)
        self.assertEqual(matlib.pow(-10,8),100000000)

        #Floats
        self.assertAlmostEqual(matlib.pow(0.0026,2),0.00000676)
        self.assertAlmostEqual(matlib.pow(0.1,5),0.00001)
        self.assertAlmostEqual(matlib.pow(0.5,2),0.25)
        self.assertAlmostEqual(matlib.pow(1.5,10),57.66503906)
        self.assertAlmostEqual(matlib.pow(5.35,7),125451.677849877)

        self.assertAlmostEqual(matlib.pow(-0.1,5),-0.00001)
        self.assertAlmostEqual(matlib.pow(-0.5,2),0.25)
        self.assertAlmostEqual(matlib.pow(-2.5,3),-15.625)
        self.assertAlmostEqual(matlib.pow(-2.758,7),-1213.83549469)
        self.assertAlmostEqual(matlib.pow(-10.84,4),13807.56603136)

        #Expecting value error
        self.assertRaises(ValueError,matlib.pow,0,-2)
        self.assertRaises(ValueError,matlib.pow,15,-4)
        self.assertRaises(ValueError,matlib.pow,42,-42)
        
        #Wrong data types
        self.assertRaises(TypeError,matlib.pow,'textext',80)
        self.assertRaises(TypeError,matlib.pow,{'car','tree'},[58,9])
        self.assertRaises(TypeError,matlib.pow,42,-0.001)
        self.assertRaises(TypeError,matlib.pow,625,1.05)

    def test_nthRoot(self):
        sqrtNumbers = [0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100]
        cubeRootNumbers = [0, 1, 8, 27, 64, 125, 216, 343, 512, 729, 1000]
        fourthRootNumbers = [0, 1, 16, 81, 256, 625, 1296, 2401, 4096, 6561, 10000]

        for i in range(11):
            self.assertEqual(matlib.nthRoot(sqrtNumbers[i],2),i)
            self.assertAlmostEqual(matlib.nthRoot(cubeRootNumbers[i],3),i)
            self.assertEqual(matlib.nthRoot(fourthRootNumbers[i],4),i)

        #Other integer roots
        self.assertEqual(matlib.nthRoot(0,2),0)
        self.assertEqual(matlib.nthRoot(1,2),1)
        self.assertEqual(matlib.nthRoot(4,2),2)
        self.assertEqual(matlib.nthRoot(9,2),3)
        self.assertEqual(matlib.nthRoot(16,2),4)

        self.assertAlmostEqual(matlib.nthRoot(25,-2),0.2)
        self.assertAlmostEqual(matlib.nthRoot(8,-3),0.5)
        self.assertAlmostEqual(matlib.nthRoot(2097152,-7),0.125)

        #Abnormalities => odd roots of negative numbers have one real root
        self.assertAlmostEqual(matlib.nthRoot(-8,3),-2)
        self.assertAlmostEqual(matlib.nthRoot(-243,5),-3)

        #Float roots
        self.assertAlmostEqual(matlib.nthRoot(2.5,4),1.25743342)
        self.assertAlmostEqual(matlib.nthRoot(5.2,10),1.17923492)
        self.assertAlmostEqual(matlib.nthRoot(10.857,3),2.21430077)

        self.assertAlmostEqual(matlib.nthRoot(12.345,2.8),2.45368183)
        self.assertAlmostEqual(matlib.nthRoot(90.822,10.785),1.5190293)
        self.assertAlmostEqual(matlib.nthRoot(102.777,1.765),13.8001683)

        self.assertAlmostEqual(matlib.nthRoot(9.58,-3.78),0.550021283)
        self.assertAlmostEqual(matlib.nthRoot(55.987,-0.78),0.0057393)
        self.assertAlmostEqual(matlib.nthRoot(68,-5.92),0.490292,6)

        #Expecting value error
        self.assertRaises(ValueError,matlib.nthRoot,10,0)
        self.assertRaises(ValueError,matlib.nthRoot,-10.85,0)
        self.assertRaises(ValueError,matlib.nthRoot,-5,2)
        self.assertRaises(ValueError,matlib.nthRoot,-50,-10)

        #Wrong data types
        self.assertRaises(TypeError,matlib.nthRoot,'textext',5)
        self.assertRaises(TypeError,matlib.nthRoot,[123,5],5)
        self.assertRaises(TypeError,matlib.nthRoot,100,{'foo','bar'})

    def test_sin(self):
        pi = 3.1415926535897932384626433

        #Testing commonly known values in interval <0,PI>
        self.assertAlmostEqual(matlib.sin(0),0)
        self.assertAlmostEqual(matlib.sin(pi/6),0.5)
        self.assertAlmostEqual(matlib.sin(pi/4),0.70710678)
        self.assertAlmostEqual(matlib.sin(pi/3),0.866025403)
        self.assertAlmostEqual(matlib.sin(pi/2),1)
        self.assertAlmostEqual(matlib.sin(pi),0)

        #Testing over interval <0,2PI>
        self.assertAlmostEqual(matlib.sin(5*pi),0) #same as sin(pi)
        self.assertAlmostEqual(matlib.sin(10*pi),0) #same as sin(0)
        self.assertAlmostEqual(matlib.sin(15*(pi/2)),-1)

        #Testing below interval <0,2PI>
        self.assertAlmostEqual(matlib.sin(-5*pi),0) #same as sin(pi)
        self.assertAlmostEqual(matlib.sin(-20*pi),0) #same as sin(0)
        self.assertAlmostEqual(matlib.sin(-35*(pi/2)),1)

        #Other uncommon values
        self.assertAlmostEqual(matlib.sin(1),0.841470984)
        self.assertAlmostEqual(matlib.sin(4),-0.756802495)
        self.assertAlmostEqual(matlib.sin(42),-0.9165215)

        self.assertAlmostEqual(matlib.sin(-1),-0.841470984)
        self.assertAlmostEqual(matlib.sin(-4),0.756802495)
        self.assertAlmostEqual(matlib.sin(-48),0.768254661)
        
        #Few float values
        self.assertAlmostEqual(matlib.sin(4.5987),-0.993544365)
        self.assertAlmostEqual(matlib.sin(-6.9578),-0.624596458)

        #Wrong data types
        self.assertRaises(TypeError,matlib.sin,'textext')
        self.assertRaises(TypeError,matlib.sin,{3.1415926535})
        self.assertRaises(TypeError,matlib.sin,['foo','ff'])

if __name__ == '__main__':
    unittest.main()
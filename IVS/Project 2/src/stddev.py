## Profiling
#
# @brief profiling of math library
#
# @author David Gajdos (xgajdo22@stud.fit.vutbr.cz)
#
# @version 1.0

import time, sys, matlib, cProfile, pstats
from pstats import SortKey

## Calculates standard deviation from list of numbers
#
#  @param nums - set of numbers (each number is expected to be a string)
#  
#  @return Calculated deviation
def calcDeviation(nums: list):
    xSum = 0
    xSquaredSum = 0
    n = len(nums)

    for i in range(n):
        #conversion of number in string format to int/float
        if '.' in nums[i]:
            currNum = float(nums[i])
        else:
            currNum = int(nums[i])
        xSum = matlib.add(xSum,currNum)
        xSquared = matlib.pow(currNum,2)
        xSquaredSum = matlib.add(xSquaredSum,xSquared)

    average = matlib.divide(xSum,n)
    tmp = matlib.pow(average,2) # average^2
    tmp = matlib.multiply(n,tmp) # average^2*N
    tmp = matlib.subtract(xSquaredSum,tmp) # sum(xi^2) - average^2*N
    tmp = matlib.divide(tmp,(n-1))

    result = matlib.nthRoot(tmp,2)

    return result


numbers = []
#reading from stdin
for line in sys.stdin:
    try:
        explodedNums = line.split()
        numbers.extend(explodedNums)
    except:
        raise TypeError('Wrong input! Only numbers accepted.')

deviation = calcDeviation(numbers)

print(deviation)

#profiler stores output to /profiling/stats
cProfile.run('calcDeviation(numbers)','../profiling/stats')

#p = pstats.Stats('../profiling/stats')
#p.strip_dirs()
#p.sort_stats(SortKey.CUMULATIVE)
#p.print_stats()
#p.dump_stats('foo.profile')
#p.print_callees()
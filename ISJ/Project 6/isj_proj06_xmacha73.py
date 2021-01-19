#!/usr/bin/env python3

import collections
import itertools

def first_nonrepeating(string):
    '''Vraci prvni neopakujici se znak'''
    if type (string) != str:
      return None
    c = collections.Counter(string)
    for char in string:
      if c[char] == 1:
        return char

def combine4(numbers, result):
    '''Vsechny mozne rovnice'''
    list_p=list(itertools.permutations(numbers))
    ress=[]
    for y in list_p:
      for expr in expressions(list(y)):
        expr = ''.join(str(x) for x in expr)
        format_exp=('{}'.format(expr))
        new_list=[]
        new_list.append(format_exp)
        for x in new_list:
            new_var=(p_size(x))
            for y in list(new_var):
                try:
                    another=eval(y)
                    if another == result:
                        ress.append(y)
                except ZeroDivisionError:
                    pass

    unique_list = []
    for x in ress:
        if x not in unique_list:
            unique_list.append(x)
    return unique_list


def p_size(string):
    '''pomocna funkce pro combine4'''
    operators = ['+','-','*','/']
    depth = len([s for s in string if s in operators])
    if depth == 0:
        return [string]
    if depth== 1:
        return ['('+ string + ')']
    answer = []
    for index, symbol in enumerate(string):
        if symbol in operators:
            left = string[:index]
            right = string[(index+1):]
            strings = [lt  + symbol + rt for lt in p_size(left) for rt in p_size(right) ]
            answer.extend(strings)
    return list(answer)

operators = ['+', '*', '-', '/']
def expressions(values):
    '''pomocna funkce pro combine4'''
    if len(values) == 1:
        yield values
    for i in range(len(values)):
        forward = values[:]
        val = forward.pop(i)
        for op in operators:
            for rest in expressions(forward):
                yield [val, op] + rest

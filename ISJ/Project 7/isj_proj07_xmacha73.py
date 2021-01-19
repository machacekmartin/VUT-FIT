#!/usr/bin/env python3

#xmacha73 proj7
import math

class TooManyCallsError(Exception):
	'''TOO MANY CALLS CLASS'''

	def __init__(self, msg):
		'''INI'''
		super(TooManyCallsError, self).__init__(msg)

def limit_calls(max_calls=2, error_message_tail='called too often'):
	'''DECORATOR'''

	def dekorator(func):
		func.promenna=0
		def wrapper(*args, **kwargs):
			func.promenna=func.promenna+1
			if max_calls < func.promenna:
				raise TooManyCallsError("function " + '"' + func.__name__ + '"' +" - "+ error_message_tail)
			return func(*args, **kwargs)
		return wrapper
	return dekorator

def ordered_merge (*args, **kwargs):
	'''ORDERED MERGE FUNC'''

	select=kwargs.get('selector', [])
	nArgs=len(args)
	args=list(args)
	output=[]
	for i in range(nArgs):
		args[i]=iter(args[i])
	for i in select:
		output.append(next(args[i]))
	return output

class Log():
	'''LOG CLASS'''
	def __init__(self, file):
		'''INI'''

		self.fl=open(file, "w")
	def logging (self, msg):
		'''MESSAGE PRINT'''

		self.fl.write(msg+"\n")
	def __enter__(self):
		'''ENTER BEGIN'''

		self.fl.write("Begin\n")
		return self
	def __exit__(self, exc_type, exc_value, traceback):
		'''PRINT END AT THE END'''
		self.fl.write("End\n")
		self.fl.close()

import time

class FooTime:
	def __init__(self, foo):
		self.__now = time.time()
		self.__foo = foo

	def print_foo(self):
		self.__foo.print_foo()
		print 'Time is', self.__now
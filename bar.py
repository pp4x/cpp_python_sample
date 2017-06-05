import time

class Bar:
	def __init__(self, foo):
		self.__now = time.time()
		self.__foo = foo

	def print_bar(self):
		self.__foo.print_foo()
		print 'Time is', time.ctime(self.__now)

	def foo(self):
		return self.__foo
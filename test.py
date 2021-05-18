import pytest
from pytest import *
print("pytest.__file__:", pytest.__file__)
print(dir(pytest))

class FunctionImpl(Function):
	def __str__(self):
		return "FunctionImpl"
	def __call__(self, c):
		return c.data
f = FunctionImpl()
useFunction(f)

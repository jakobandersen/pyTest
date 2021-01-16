import pytest
from pytest import *
print("pytest.__file__:", pytest.__file__)
for k in dir(pytest):
	if k[0] == '_': continue
	print("   {}: {}".format(k, globals()[k]))

try:
	print("Before f()")
	f()
except LogicError as e:
	print("except: e={}".format(e))
	print("except: type(e)={}".format(type(e)))


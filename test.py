import pytest
from pytest import *
print("pytest.__file__:", pytest.__file__)
for k in dir(pytest):
	if k[0] == '_': continue
	print("   {}: {}".format(k, globals()[k]))

print("dir(LogicError):")
for k in dir(LogicError):
	if k[0] == '_': continue
	print("  ", k)
#print("dir(LogicError_):")
#for k in dir(LogicError_):
#	if k[0] == '_': continue
#	print("  ", k)

try:
	print("Before f()")
	f()
except LogicError as e:
	print("except: e={}".format(e))
	print("except: type(e)={}".format(type(e)))
	#print("except: type(e.cause)={}".format(type(e.cause)))

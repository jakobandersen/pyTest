print("pytest.__init__.py: begin")
import ctypes
import sys

_oldFlags = sys.getdlopenflags()
sys.setdlopenflags(_oldFlags | ctypes.RTLD_GLOBAL)
from . import libpytest
from .libpytest import *
sys.setdlopenflags(_oldFlags)

# from http://mail.python.org/pipermail/tutor/2003-November/026645.html
class _Unbuffered:
	def __init__(self, stream):
		self.stream = stream
	def write(self, data):
		self.stream.write(data)
		self.stream.flush()
	def __getattr__(self, attr):
		return getattr(self.stream, attr)
sys.stdout = _Unbuffered(sys.stdout)  # type: ignore

print("pytest.__init__.py: end")

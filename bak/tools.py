import ctypes
import platform

mode = dict(winmode=0) if platform.python_version() >= '3.8' else dict()  

lib = ctypes.cdll.LoadLibrary('./tools.so')

class Test(object):
    def __init__(self):
        lib.getPixel.argtypes = [ctypes.c_int]
        lib.getPixel.restype = [ctypes.c_int]

        self.obj = lib.init()

    def getPixel(self, x, y):
        res = lib.getPixel(self.obj, x, y)
        print("DEBUG: res =", res)
        return res


T1 = Test()
print(T1.getPixel(170, 9))

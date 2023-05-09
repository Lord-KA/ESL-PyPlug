from pyplug import *
from PIL import Image

class mytool:
    def __init__(self):
        print("__init__() called!")

    def apply(self, image, event):
        print("apply() called!")
        print("fgColor", APPCONTEXT.fgColor)

    def getTexture(self):
        return "some_texture.png"

    def buildSetupWidget(self):
        button = createButton(1, 2, 3, 4, "Button 1")
        print("button", button, "has been created")
        APPCONTEXT.fgColor = Color(179)

class mytool_2:
    def __init__(self):
        print("__init__() for second tool called!")

    def apply(self, image, event):
        print("second tool apply called")
        if (event.type == EventType.NoEvent):
            print("event.type = NoEvent")
        else:
            print("event.type = other")
        APPCONTEXT.fgColor = Color(APPCONTEXT.fgColor.toInteger() + 1)

    def getTexture(self):
        print("getTexture() called!")
        return "some_texture_2.png"
    def buildSetupWidget(self):
        button = createButton(1, 2, 3, 4, "Button 2")
        print("button", button, "has been created")
        APPCONTEXT.fgColor = Color(179)

class mytool_3:
    def __init__(self):
        print("__init__() for third tool called!")

    def apply(self, image, event):
        print("third tool apply called")
        arr = image.getHiddenLayer().getPicture(0, 0, 3, 6)
        arr[5][2][0] = 179
        arr[5][2][1] = 0
        arr[5][2][2] = 0
        arr[5][2][3] = 0
        image.setPicture(0, 0, arr)

        print(event.type)

        #print("NoEvent =", EventType.NoEvent)
        #print("MouseMoved =", EventType.MouseMoved)
        #print("MousePressed =", EventType.MousePressed)
        #print("MouseReleased =", EventType.MouseReleased)
        #print("ButtonClicked =", EventType.ButtonClicked)


    def getTexture(self):
        return "some_texture_2.png"
    def buildSetupWidget(self):
        print("buildSetupWidget called!")

def init_module():
    setToolbarSize(300, 300)
    addTool(mytool())
    addTool(mytool_2())
    addTool(mytool_3())

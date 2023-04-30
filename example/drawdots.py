from pyplug import *

run_once = True

class mytool:
    def __init__(self):
        print("__init__() called!")
    def apply(self, image, event):
        print("apply() called!")
        if (event.type != EventType.MousePressed):
            return;
        p_x = event.x
        p_y = event.y
        r = 5
        print("p_x =", p_x, "p_y =", p_y)
        for x in range(p_x - 20, p_x + 20):
            for y in range(p_y - 20, p_y + 20):
                if ((x - p_x)**2 + (y - p_y)**2 < r**2):
                    image.setPixel(x, y, 0x00ff00ff)


    def getTexture(self):
        print("getTexture() called!")
        return "some_texture.png"
    def buildSetupWidget(self):
        print("build_setup_widget called!")

def init_module():
    setToolbarSize(300, 300)
    addTool(mytool())

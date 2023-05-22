from pyplug import *
import time 
import numpy as np
from PIL import Image
from style_transfer.learn import StyleTransfer
import Filter
import torch 
import gc
import threading
import convolution

run_once = True

class convolution_plugin:
    def init(self):
        self.kernel = np.array([[0, 0, 0, 0, 0, 0, 0],
                                [0, 0, 0, 0, 0, 0, 0],
                                [0, 0, 0, 0, 0, 0, 0],
                                [0, 0, 0, 1, 0, 0, 0],
                                [0, 0, 0, 0, 0, 0, 0],
                                [0, 0, 0, 0, 0, 0, 0],
                                [0, 0, 0, 0, 0, 0, 0]])
        self.state = False
        self.pic = 0

    def apply_conv(self, image, pic):
        print("starting...")
        pic = Filter.apply_convolution(pic, self.kernel)
        image.setPicture(0, 0, pic)
        self.state = False

    def apply(self, image, event):
        global running
        position = [int(event.x), int(event.y)]            

        if event.type == EventType.MouseReleased:
            self.clicked = False
            self.previous_points = []
        
        if event.type == EventType.ButtonClicked:
            for y in range(7):
                for x in range(7):
                    try:
                        self.kernel[y][x] = int(getTextEditor(self.editor_array[x][y]))
                    except:
                        setTextEditor(self.editor_array[y][x], "0")

            self.state = True
            label_state = self.label_state
            pic = image.getPicture(0, 0, image.getW(), image.getH())
            e1 = threading.Event()

            t1 = threading.Thread(target=self.apply_conv, args=([image, pic]))
            
            t1.start()
            e1.set() 

            start_time = time.time()
            while self.state:
                setTextLabel(label_state, str(int(time.time() - start_time) + 1))
                time.sleep(1)
            

    def getTexture(self):
        return "../PluginIcons/Conv.png"

    def buildSetupWidget(self):
        self.editor_array = [[createEditor(y * 30 + 20, x * 30 + 20, 30, 30) for x in range(7)] for y in range(7)]
        for y in range(7):
            for x in range(7):
                setTextEditor(self.editor_array[y][x], "0")

        setTextEditor(self.editor_array[3][3], "1")
        self.label_state = createLabel(100, 400, 100, 60, "Not")
        self.button = createButton(100, 300, 50, 50, "Do")

def init_module():
    setToolbarSize(300, 300)
    addTool(convolution_plugin())

import numpy
import tensorflow_hub as hub

# lib with adapter helpers
from pyplug import *

def load_img(path_to_img):
  max_dim = 256
  img = tf.io.read_file(path_to_img)
  img = tf.image.decode_image(img, channels=3)
  img = tf.image.convert_image_dtype(img, tf.float32)

  shape = tf.cast(tf.shape(img)[:-1], tf.float32)
  long_dim = max(shape)
  scale = max_dim / long_dim

  new_shape = tf.cast(shape * scale, tf.int32)

  img = tf.image.resize(img, new_shape)
  img = img[tf.newaxis, :]
  return img


class style_tool:
    def __init__(self, model_url ='https://tfhub.dev/google/magenta/arbitrary-image-stylization-v1-256/2'):
        self.first  = (-1, -1)
        self.second = (-1, -1)
        
        self.style_filename = False
        sel.model = hub_model = hub.load(model_url)
        
    def get_texture():
        return "style_tool_texture.png"

    def build_setup_widget(self):
        set_toolbar_size(300, 600)
        self.text_enter_area = create_text_enter_area(10, 10, 290, 70)
        self.apply_button = create_button(100, 10, 60, 30, "Apply")
        self.status_label = create_label(140, 10, 60, 30, "Awaiting command, my lord")
        #XXX

    def apply(self, image, event):
        #XXX maybe select area with a brush or broken line?
        if event.type == mouse_button_press:
            if self.first.x == -1 or self.first.y == -1:
                self.first = event.pos
                #TODO mark currently selected area in some way:
                # e.g. by *red* dotted line (don't forget to restore 
                # original image)
            elif self.second.x == -1 or self.second.y == -1:
                self.second = event.pos
            else:
                assert(False)

        if event.type == text_enter_area_updated and event.widget_id == self.text_enter_area:
            self.style_filename = event.text_enter_area
            self.style_picture = load_img(self.style_filename)
            
        if event.type == button_pressed and event.widget_id == self.apply_button:
            if not self.style_filename:
                #XXX maybe throw messages as pop-up windows?
                pytool.modify_label(self.status_label, "Select style file first!")
                return

            # Apply style if area is selected.
            if (self.first.x == -1 or self.second.x == -1):
                pytool.modify_label(self.status_label, "Select area first!")
                return

            left_up    = (min(self.first.x, self.second.x), max(self.first.y, self.second.y))
            right_down = (max(self.first.x, self.second.x), min(self.first.y, self.second.y))
            
            # `picture` should be numpy image-holding object.    
            picture = image.getPicture(left_up, right_down)
            picture = picture[:, :, :2]
            
            pytool.modify_label(self.status_label, "Please wait, generation takes about 10 sec.")
            picture = hub_model(tf.constant(picture), tf.constant(self.style_picture))[0]
            image.setPicture(left_up, picture)
            
            self.first  = (-1, -1)
            self.second = (-1, -1)

class conv_matrix:
    def __init__(self):
        self.matrixes = {
            "contrast": np.array([[ 0,  0,  0,  0,  0],
                                  [ 0,  0, -1,  0,  0],
                                  [ 0, -1,  5, -1,  0],
                                  [ 0,  0, -1,  0,  0],
                                  [ 0,  0,  0,  0,  0]]),
                         
            "blur":np.array([[ 0,  0,  0,  0,  0],
                             [ 0,  1,  1,  1,  0],
                             [ 0,  1,  1,  1,  0],
                             [ 0,  1,  1,  1,  0],
                             [ 0,  0,  0,  0,  0]]),           
        }
        self.current_matrix = ""
        self.repeat = 1
        
    def apply(self, image, event):
        if event.type == mouse_button_press:
            if self.first.x == -1 or self.first.y == -1:
                self.first = event.pos
                #TODO mark currently selected area in some way:
                # e.g. by *red* dotted line (don't forget to restore 
                # original image)
            elif self.second.x == -1 or self.second.y == -1:
                self.second = event.pos
            else:
                assert(False)
        
        if event.type == slider_updated and event.widget_id == self.slider_repeat:
            self.repeat = event.value
        
        if event.type == button_pressed:
            if event.widget_id in self.ply_button.keys():
                self.current_matrix = self.style_buttons[event.widget_id]
        
            if event.widget_id == self.apply_button:
                if self.current_matrix == "":
                    #XXX maybe throw messages as pop-up windows?
                    pytool.modify_label(self.status_label, "Select matrix first!")
                    return
    
                # Apply style if area is selected.
                if (self.first.x == -1 or self.second.x == -1):
                    pytool.modify_label(self.status_label, "Select area first!")
                    return
    
                left_up    = (min(self.first.x, self.second.x), max(self.first.y, self.second.y))
                right_down = (max(self.first.x, self.second.x), min(self.first.y, self.second.y))

                # `picture` should be numpy image-holding object.    
                picture = image.getPicture(left_up, right_down)
                picture = picture[:, :, :2]
                chosen_matrix = self.matrixes[self.current_matrix]
                
                for _ in range(self.repeat): 
                    picture = apply_matrix(picture, chosen_matrix, picture.shape[0], picture.shape[1], 
                                                                   chosen_matrix.shape[0], chosen_matrix.shape[1])
                image.setPicture(left_up, picture)
                
                self.first  = (-1, -1)
                self.second = (-1, -1)
            

    
    def build_setup_widget(self):
        self.style_buttons = {}
        h = 100
        for name, _ in self.matrixes:
            id = self.conv_buttons.append(create_button(10, h, 30, 60, name))
            h += 40
            self.style_buttons[id] = name
        
        self.slider_repeat = create_slider(200, 100, 40, 10, 1, 10, 1)
        self.apply_button  = create_button(220, 130, 50, 50, "Apply")
        self.status_label  = create_label (140, 10,  60, 30, "Awaiting command, my lord")
        
        
def init_module():
    stool = style_tool()
    add_tool(stool)
    cmtool = conv_matrix()
    add_tool(cmtool)


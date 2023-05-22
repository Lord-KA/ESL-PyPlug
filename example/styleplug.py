self.previous_points = self.previous_points[1:]
            self.previous_points.append(position)
            
            for t in range(0, 11):
                t = float(t) / 10.0
                new_point = self.interpolator.Interpolate_lin(t, self.previous_points[2], self.previous_points[3]);
                hidden_layer.setPixel(new_point[0], new_point[1], color)
                if new_point[0] > 0:
                    hidden_layer.setPixel(new_point[0] - 1, new_point[1], color)
                if new_point[0] < hidden_layer.getW() - 1:
                    hidden_layer.setPixel(new_point[0] + 1, new_point[1], color)
                if new_point[1] > 0:
                    hidden_layer.setPixel(new_point[0], new_point[1] - 1, color)
                if new_point[1] < hidden_layer.getH() - 1:
                    hidden_layer.setPixel(new_point[0], new_point[1] + 1, color)
        if event.type == EventType.ButtonClicked:
            color = Color(0)
            color.r = 0
            color.g = 0
            color.b = 0
            color.a = 0
            hiddent_layer = image.getHiddenLayer()
            hiddent_layer.clean(color)
            
            if event.id == self.start_button:
                try:
                    style_img = Image.open(self.style_image_path)
                except:
                    setTextLabel(self.state_label, "Invalid path")
                    return 

                if event.id == self.start_button:
                    setTextLabel(self.state_label, "Processing...")
                    time.sleep(0.5)
                    for left_up_corner, right_down_corner in self.fields:
                        shape = [right_down_corner[0] - left_up_corner[0], right_down_corner[1] - left_up_corner[1]]
                        picture = image.getPicture(left_up_corner[0], left_up_corner[1], shape[0], shape[1])
                        alpha   = picture[:, :, 3]
                        picture = picture[:, :, :3]

                        img = Image.fromarray(picture.astype(np.uint8))
                        artwork = self.style_transfer(img, style_img, 
                                        area=256, init_random=False, init_img=None, iter=200)
                        if shape[0] > 256 or shape[1] > 256:
                            artwork = self.style_transfer(img, style_img, 
                                            area=512, init_random=False, init_img=artwork, iter=100)

                        artwork = artwork.resize((shape[0], shape[1]), Image.Resampling.NEAREST)
                        picture = np.array(artwork)
                        picture = np.concatenate([picture, alpha.T.reshape(shape[1], shape[0], 1)], axis=2)
                        
                        picture[:, :, 3] = picture[:, :, 3] * (self.hidden_array[left_up_corner[1]:right_down_corner[1], left_up_corner[0]:right_down_corner[0], 3] / 100) 
                        image.setPicture(left_up_corner[0], left_up_corner[1], picture)
                    
                    self.fields = []
                    setTextLabel(self.state_label, "Done!   ")


        if event.type == EventType.TextEvent:
            self.style_image_path = event.text

    def getTexture(self):
        return "../PluginIcons/Style.png"

    def buildSetupWidget(self):
        self.label_path    = createLabel (10, 30, 150, 30, "Style path") 
        self.editor_path   = createEditor(50, 70, 200, 30)
        self.start_button  = createButton(10, 120, 70, 30, "Start")
        self.reset_button = createButton (100, 120, 180, 30, "Reset selection") 
        self.state_label   = createLabel (50, 200, 200, 30, "")
        print("build_setup_widget called!")


def init_module():
    setToolbarSize(300, 300)
    addTool(style_plugin())

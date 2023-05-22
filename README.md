# ESL-PyPlug
ESL standard python adapter

This lib can be added to Plugins directory of your [ESL c++ standard Photoshop](https://github.com/mishaglik/Elpidifor-s-legacy) application
to provide a way to use python-written plugins.

## Building
```sh
mkdir build
cd build 
cmake ..
make -j
```

## User usage
Copy `libpyplug.so` to `Plugins` dir of your Photoshop.
Add python plugins to `PythonPlugins` dir.
Enjoy.

## Dev usage
Basically, `libpyplug` expects your python plugin to have the same interface as ESL c++ plugin, meaning the following:
1. implement plugin class with `getTexture()`, `apply()`, `buildSetupWidgets()` functions
2. add global `init_module()` function with `setToolBarSize()` and `addTool()` calls

You can use the same API as [ESL c++ standard](https://github.com/mishaglik/Elpidifor-s-legacy), with minor differences:
1. `getPicture()`/`setPicture()` handles 3d numpy arrays instead of `class Picture`

For more info see [examples](https://github.com/Lord-KA/ESL-PyPlug/tree/master/example).


## MIPT Task (compare performance of optimised and non-optimised version)

### booba::Picture

The main idea of `get/setPicture()` functions is to lower the number of `get/setPixel()` calls 
when the whole image or some substantial part of it is modified, thus it must be fast. This means
we need to lower the number of copy by using c++ move semantics. This is done in [main version](https://github.com/lord-ka/Elpidifor-s-legacy) 
of ESL standard, while the [special faulty version](https://github.com/lord-ka/Elpidifor-s-legacy) 
avoids use of move semantics.
For the test we use a plugin that intentionally uses `get/setPicture()` in a wrong way, by getting 
the whole image and modifying a few pixels in it. The image that was modified is 1200x750 pixels.

It took around 13 seconds for optimised version to do 10 `get/setPicure()` calls and 19 seconds 
for non-optimised one in 3 runs. Thus, the difference is 30% in this demo test case.

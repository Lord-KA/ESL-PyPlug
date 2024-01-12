# ESL-PyPlug
ESL standard python adapter

This lib can be added to Plugins directory of your [ESL c++ standard Photoshop](https://github.com/mishaglik/Elpidifor-s-legacy) application
to provide a way to use python-written plugins.


![This is a plug-in with ML-based algorithm that modifies picture with an according style](https://github.com/Lord-KA/ESL-PyPlug/assets/43700614/ffa735d3-faf2-4384-98b5-8c93abf79cc6)

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

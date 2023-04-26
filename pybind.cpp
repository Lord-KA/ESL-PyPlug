#include <cstddef>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

#include <iostream>
#include <string>
#include <stdio.h>

#include "tools.hpp"

#include "dummyImage.hpp"

namespace py = pybind11;

class PyProxyImage : public booba::Image {
public:
    PyProxyImage(booba::Image *image) : image(image) {}
    PyProxyImage(std::initializer_list<booba::Image*> list) : image(*list.begin()) {}

    virtual size_t getH() override
    {
        return image->getH();
    }

    virtual size_t getW() override
    {
        return image->getW();
    }

    virtual uint32_t getPixel(size_t x, size_t y) override
    {
        return image->getPixel(x, y);
    }

    virtual void setPixel(size_t x, size_t y, uint32_t color) override
    {
        return image->setPixel(x, y, color);
    }

private:
    booba::Image *image;
};

class PyProxyTool : public booba::Tool {
public:
    PyProxyTool(py::object tool) : pyTool(tool) {}

    virtual void apply(booba::Image* image, const booba::Event* event)
    {
        PyProxyImage proxy = new PyProxyImage(image);
        fprintf(stderr, "apply was called with img: %p\n", image);
        if (image && event)
            pyTool.attr("apply")(proxy); //TODO events
    }

    virtual const char* getTexture()
    {
        py::object res = pyTool.attr("get_texture")();
        py::print(res);
        return "some_texture.png";
    }

    virtual void buildSetupWidget()
    {
        pyTool.attr("build_setup_widget")();
    }

private:

    py::object pyTool;
};

PYBIND11_EMBEDDED_MODULE(pyplug, m)
{       // `m` is a `py::module_` which is used to bind functions and classes
    m.def("set_toolbar_size", &booba::setToolBarSize);

    m.def("add_tool", [](py::object &pyTool) {
        PyProxyTool *proxy = new PyProxyTool(pyTool);
        booba::addTool(proxy);
        std::cout << "add_tool() called!\n"; //XXX
    });

    py::class_<PyProxyImage>(m, "PyProxyImage")
        .def(py::init<>())
        .def("getH", &PyProxyImage::getH)
        .def("getW", &PyProxyImage::getW)
        .def("get_pixel", &PyProxyImage::getPixel)
        .def("set_pixel", &PyProxyImage::setPixel);
}

void booba::init_module()
{
    py::initialize_interpreter();

    py::module_ plug = py::module_::import("my_plugin");
    py::object res = plug.attr("init_modules")();
}

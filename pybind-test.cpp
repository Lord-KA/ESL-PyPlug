#include <cinttypes>
#include <cstddef>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

#include <iostream>
#include <string>

#include "tools.hpp"

#include "dummyImage.hpp"

namespace py = pybind11;

class PyProxyImage {
public:
    PyProxyImage(booba::Image *image) : image(image) {}
    PyProxyImage(std::initializer_list<booba::Image*> list) : image(*list.begin()) {}

    size_t getH()
    {
        return image->getH();
    }

    size_t getW()
    {
        return image->getW();
    }

    uint32_t getPixel(size_t x, size_t y)
    {
        return image->getPixel(x, y);
    }

    void setPixel(size_t x, size_t y, uint32_t color)
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
        std::cerr << "apply called!\n";
        PyProxyImage proxy(image);
        pyTool.attr("apply")(proxy);
    }

    virtual const char* getTexture()
    {
        py::object res = pyTool.attr("get_texture")();
        py::print(res);
        //XXX return res.cast<char*>();
    }

    virtual void buildSetupWidget()
    {
        pyTool.attr("build_setup_widget")();
    }

private:

    py::object pyTool;
};

PyProxyTool *proxy_tool = NULL;

void booba::addTool(Tool *tool)
{
    std::cerr << "addTool() called!\n";
}

PYBIND11_EMBEDDED_MODULE(pyplug, m)
{       // `m` is a `py::module_` which is used to bind functions and classes
    m.def("set_toolbar_size", [](size_t w, size_t h) {
        std::cout << "set_toolbar_size() called with w = " << w << "; h = " << h << std::endl; //XXX
    });

    m.def("add_tool", [](py::object &pyTool) {
        PyProxyTool *proxy = new PyProxyTool(pyTool);
        booba::addTool(proxy);
        proxy_tool = proxy;
    });

    py::class_<PyProxyImage>(m, "PyProxyImage")
        .def(py::init<>())
        .def("getH", &PyProxyImage::getH)
        .def("getW", &PyProxyImage::getW)
        .def("getPixel", &PyProxyImage::getPixel)
        .def("setPixel", &PyProxyImage::setPixel);
}

int main()
{
    py::scoped_interpreter guard{};

    py::print("Hello world!");

    py::module_ tools = py::module_::import("tools");
    py::object res = tools.attr("init_modules")();

    DummyImage dummyImage;
    assert(proxy_tool);
    proxy_tool->apply(&dummyImage, NULL);
    py::print("finished!");
}

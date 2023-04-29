#include <cstddef>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

#include <iostream>
#include <string>

#include "tools.hpp"

namespace py = pybind11;

class DummyImage : public booba::Image {
public:
    virtual size_t getH() override
    {
        return 179;
    }

    virtual size_t getW() override
    {
        return 234;
    }

    virtual uint32_t getPixel(size_t x, size_t y) override
    {
        return x + y;
    }

    virtual void setPixel(size_t x, size_t y, uint32_t color) override
    {
        assert(!"Not implemented yet!");
    }

    DummyImage() {}
    DummyImage(const DummyImage &other) {}
    ~DummyImage() = default;
};

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
        if (image && event)
            pyTool.attr("apply")(proxy, event); //TODO translate events
    }

    virtual const char* getTexture()
    {
        py::object res = pyTool.attr("getTexture")();
        py::print(res);
        return "some_texture.png";          //XXX fix string cast
    }

    virtual void buildSetupWidget()
    {
        pyTool.attr("buildSetupWidget")();
    }

private:

    py::object pyTool;
};


/*
 * `m` is a `py::module_` which is used to bind functions and classes.
 */
PYBIND11_EMBEDDED_MODULE(pyplug, m)
{
    m.def("setToolbarSize", &booba::setToolBarSize);

    m.def("createButton", &booba::createButton);

    m.def("addTool", [](py::object &pyTool) {
        PyProxyTool *proxy = new PyProxyTool(pyTool);
        booba::addTool(reinterpret_cast<booba::Tool*>(proxy));
    });

    py::class_<PyProxyImage>(m, "PyProxyImage")
        .def(py::init<>())
        .def("getH", &PyProxyImage::getH)
        .def("getW", &PyProxyImage::getW)
        .def("getPixel", &PyProxyImage::getPixel)
        .def("setPixel", &PyProxyImage::setPixel);
}

void booba::init_module()
{
#ifndef STANDALONE_MODE
    py::initialize_interpreter();
#endif

    py::module_ plug = py::module_::import("my_plugin");
    py::object res = plug.attr("init_module")();
}

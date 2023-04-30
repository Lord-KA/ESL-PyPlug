#include <cstddef>
#include <cstdint>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>

#include <iostream>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

#include "tools.hpp"

namespace py = pybind11;

namespace detail {

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

    virtual uint32_t getPixel(size_t x, size_t y)
    {
        return image->getPixel(x, y);
    }

    void setPixel(size_t x, size_t y, uint32_t color)
    {
        return image->setPixel(x, y, color);
    }

    py::array_t<uint32_t> getPicture(size_t x, size_t y, size_t w, size_t h)
    {
        auto pic = image->getPicture(x, y, w, h);
        std::vector<long> shape;
        shape.push_back(w);
        shape.push_back(h);
        py::array_t<uint32_t> arr(py::detail::any_container<long>(std::move(shape)), pic.takeData());
        return arr;
    }

    void setPicture(size_t x, size_t y, py::array_t<uint32_t> &&arr)
    {
        assert(arr.ndim() == 2);
        size_t w = arr.shape(0);
        size_t h = arr.shape(1);
        image->setPicture(booba::Picture(arr.mutable_data(), x, y, w, h));
    }

private:
    booba::Image *image;
};


class PyProxyEvent {
public:
    PyProxyEvent()
        : type(booba::EventType::NoEvent), button(booba::MouseButton::Left),
          x(-1), y(-1), rel_x(-1), rel_y(-1), shift(0), alt(0), ctrl(0),
          id(-1), value(INT64_MIN), time(-1) {}

    PyProxyEvent(booba::Event event)
        : type(event.type)
    {
        using ev = booba::EventType;
        switch (type) {
            case ev::NoEvent:
                break;

            case ev::MouseMoved:
                x = event.Oleg.motion.x;
                y = event.Oleg.motion.y;
                rel_x = event.Oleg.motion.rel_x;
                rel_y = event.Oleg.motion.rel_y;
                break;

            case ev::MousePressed:
            case ev::MouseReleased:
                x = event.Oleg.mbedata.x;
                y = event.Oleg.mbedata.y;
                button = event.Oleg.mbedata.button;

                shift = event.Oleg.mbedata.shift;
                alt   = event.Oleg.mbedata.alt;
                ctrl  = event.Oleg.mbedata.ctrl;
                break;

            case ev::ButtonClicked:
                id = event.Oleg.bcedata.id;
                break;

            case ev::SliderMoved:
                id = event.Oleg.smedata.id;
                value = event.Oleg.smedata.value;
                break;

            case ev::CanvasMPressed:
            case ev::CanvasMReleased:
            case ev::CanvasMMoved:
                id = event.Oleg.cedata.id;
                x = event.Oleg.cedata.x;
                y = event.Oleg.cedata.y;
                break;

            case ev::CanvasMLeft:
                break;

            case ev::TimerEvent:
                time = event.Oleg.tedata.time;
                break;
        }
    }

    booba::EventType type;
    booba::MouseButton button;

    size_t x, y;
    size_t rel_x, rel_y;

    bool shift, alt, ctrl;

    uint64_t id;
    int64_t value;

    uint64_t time;
};


class PyProxyTool : public booba::Tool {
public:
    PyProxyTool(py::object tool) : pyTool(tool) {}

    virtual void apply(booba::Image* image, const booba::Event* event)
    {
        PyProxyImage *proxyImage = new PyProxyImage(image);
        PyProxyEvent *proxyEvent = new PyProxyEvent(*event);
        if (image and event)
            pyTool.attr("apply")(proxyImage, proxyEvent);
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
    m.def("createLabel",  &booba::createLabel);
    m.def("createSlider", &booba::createSlider);
    m.def("createCanvas", &booba::createCanvas);

    m.def("putPixel",    &booba::putPixel);
    m.def("putSprite",   &booba::putSprite);
    m.def("cleanCanvas", &booba::cleanCanvas);

    m.def("addTool", [](py::object &pyTool) {
        PyProxyTool *proxy = new PyProxyTool(pyTool);
        booba::addTool(reinterpret_cast<booba::Tool*>(proxy));
    });

    m.def("addFilter", [](py::object &pyTool) {
        PyProxyTool *proxy = new PyProxyTool(pyTool);
        booba::addFilter(reinterpret_cast<booba::Tool*>(proxy));
    });


    py::class_<PyProxyImage>(m, "PyProxyImage")
        .def(py::init<>())
        .def("getH", &PyProxyImage::getH)
        .def("getW", &PyProxyImage::getW)
        .def("getPixel", &PyProxyImage::getPixel)
        .def("setPixel", &PyProxyImage::setPixel)
        .def("getPicture", &PyProxyImage::getPicture)
        .def("setPicture", &PyProxyImage::setPicture);


    py::class_<booba::ApplicationContext>(m, "ApplicationContext")
        .def_readwrite("fgColor", &booba::ApplicationContext::fgColor)
        .def_readwrite("bgColor", &booba::ApplicationContext::bgColor);
    m.attr("APPCONTEXT") = booba::APPCONTEXT;


    py::enum_<booba::EventType>(m, "EventType")
        .value("NoEvent",         booba::EventType::NoEvent)

        .value("MouseMoved",      booba::EventType::MouseMoved)
        .value("MousePressed",    booba::EventType::MousePressed)
        .value("MouseReleased",   booba::EventType::MouseReleased)

        .value("ButtonClicked",   booba::EventType::ButtonClicked)
        .value("SliderMoved",     booba::EventType::SliderMoved)

        .value("CanvasMPressed",  booba::EventType::CanvasMPressed)
        .value("CanvasMReleased", booba::EventType::CanvasMReleased)
        .value("CanvasMMoved",    booba::EventType::CanvasMMoved)
        .value("CanvasMLeft",     booba::EventType::CanvasMLeft)

        .value("TimerEvent",      booba::EventType::TimerEvent);


    py::class_<booba::MotionEventData>(m, "MotionEventData")
        .def_readwrite("x", &booba::MotionEventData::x)
        .def_readwrite("y", &booba::MotionEventData::y)
        .def_readwrite("rel_x", &booba::MotionEventData::rel_x)
        .def_readwrite("rel_y", &booba::MotionEventData::rel_y);


    py::enum_<booba::MouseButton>(m, "MouseButton")
        .value("Left",  booba::MouseButton::Left)
        .value("Right", booba::MouseButton::Right);


    py::class_<PyProxyEvent>(m, "Event")
        .def_readwrite("type",   &PyProxyEvent::type)

        .def_readwrite("button", &PyProxyEvent::button)

        .def_readwrite("x", &PyProxyEvent::x)
        .def_readwrite("y", &PyProxyEvent::y)
        .def_readwrite("rel_x", &PyProxyEvent::rel_x)
        .def_readwrite("rel_y", &PyProxyEvent::rel_y)

        .def_readwrite("shift", &PyProxyEvent::shift)
        .def_readwrite("alt",   &PyProxyEvent::alt)
        .def_readwrite("ctrl",  &PyProxyEvent::ctrl)

        .def_readwrite("id", &PyProxyEvent::id)
        .def_readwrite("value", &PyProxyEvent::value)
        .def_readwrite("time", &PyProxyEvent::time);
}

} /* namespace detail */

void booba::init_module()
{
#ifndef STANDALONE_MODE
    py::initialize_interpreter();

    py::module_ plug = py::module_::import("my_plugin");
    py::object res = plug.attr("init_module")();
#else
    std::cerr << "DEBUG: init_module is skipped in STANDALONE_MODE.\n";
#endif
}

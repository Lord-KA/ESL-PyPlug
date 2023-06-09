#include <exception>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>

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

    virtual booba::Color getPixel(size_t x, size_t y)
    {
        return image->getPixel(x, y);
    }

    void setPixel(size_t x, size_t y, booba::Color c)
    {
        return image->setPixel(x, y, c);
    }

    py::array_t<uint32_t> getPicture(size_t x, size_t y, size_t w, size_t h)
    {
        auto pic = image->getPicture(x, y, w, h);
        std::vector<long> shape;
        shape.push_back(h);
        shape.push_back(w);
        shape.push_back(4);     // For RGBA format.
        py::array_t<uint8_t> arr(py::detail::any_container<long>(std::move(shape)), (uint8_t *)pic.takeData());
        return arr;
    }

    void setPicture(size_t x, size_t y, py::array_t<uint8_t> &&arr)
    {
        assert(arr.ndim() == 3);
        assert(arr.shape(2) == 4);
        size_t w = arr.shape(1);
        size_t h = arr.shape(0);
        image->setPicture(booba::Picture((booba::Color*)(arr.mutable_data()), x, y, w, h, false));
    }

    PyProxyImage* getHiddenLayer()
    {
        auto layer = image->getHiddenLayer();
        assert(layer != nullptr && "According to ESL, hidden layer is always non-NULL");
        if (hiddenLayer == nullptr)
            hiddenLayer = new PyProxyImage(layer);
        else
            hiddenLayer->image = layer;
        return hiddenLayer;
    }

    void clean(const booba::Color c = booba::Color::WHITE)
    {
        image->clean(c);
    }

    ~PyProxyImage()
    {
        if (hiddenLayer)
            delete hiddenLayer;
    }

private:
    booba::Image *image = nullptr;
    PyProxyImage *hiddenLayer = nullptr;
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

            case ev::MouseLeft:
                break;

            case ev::TimerEvent:
                time = event.Oleg.tedata.time;
                break;

            case ev::TextEvent:
                id = event.Oleg.textdata.id;
                text = event.Oleg.textdata.text;
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
    const char *text;

    uint64_t time;
};

class PyProxyTool : public booba::Tool {
public:
    PyProxyTool(py::object tool) : pyTool(tool) {}

    virtual void apply(booba::Image* image, const booba::Event* event)
    {
        assert(image != nullptr && "PyPlug doesn't support nullptr being passed as apply argument");
        assert(event != nullptr && "PyPlug doesn't support nullptr being passed as apply argument");

        PyProxyImage *proxyImage = new PyProxyImage(image);
        PyProxyEvent *proxyEvent = new PyProxyEvent(*event);
        call("apply", proxyImage, proxyEvent);
    }

    virtual const char* getTexture()
    {
        py::str res = call("getTexture");
        if (res == py::str(py::none()))
            texture = "\0";
        else
            texture = res;
        return texture.c_str();
    }

    virtual void buildSetupWidget()
    {
        call("buildSetupWidget");
    }

private:
    template<typename ...Args>
    py::object call(const char *func, Args... args)
    {
        try {
            return pyTool.attr(func)(args...);
        } catch(const std::exception &exc) {
            std::cerr << "ERROR: PyPlug tool function " << func << " failed: " << exc.what() << std::endl;
            return py::str(py::none());
        }
    }

    py::object pyTool;
    std::string texture;
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
    m.def("createEditor", &booba::createEditor);

    m.def("setValueSlider", &booba::setValueSlider);

    m.def("setTextLabel", &booba::setTextLabel);
    m.def("getTextLabel", &booba::getTextLabel);

    m.def("setTextEditor", &booba::setTextEditor);
    m.def("getTextEditor", &booba::getTextEditor);

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
        .def("setPicture", &PyProxyImage::setPicture)
        .def("getHiddenLayer", &PyProxyImage::getHiddenLayer)
        .def("clean", &PyProxyImage::clean);


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
        .value("MouseLeft",       booba::EventType::MouseLeft)

        .value("TimerEvent",      booba::EventType::TimerEvent)
        .value("TextEvent",       booba::EventType::TextEvent);


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
        .def_readwrite("time", &PyProxyEvent::time)
        .def_readwrite("text", &PyProxyEvent::text);


    py::class_<booba::Color>(m, "Color")
        .def(py::init<uint32_t>())
        .def("toInteger", &booba::Color::toInteger)
        .def("_compare", &booba::Color::operator==)
        .def_readwrite("r", &booba::Color::r)
        .def_readwrite("g", &booba::Color::g)
        .def_readwrite("b", &booba::Color::b)
        .def_readwrite("a", &booba::Color::a);

}

} /* namespace detail */


void booba::init_module()
{
#ifndef STANDALONE_MODE
    py::initialize_interpreter();

    std::string dir = "../PythonPlugins/";
    std::cerr << "Searching for py-plugins in: " << dir << std::endl;

    if (not std::filesystem::directory_entry(dir).exists()) {
        std::cerr << "WARNING: no py-plugin folder (should be \"" << dir << "\", relative to executable)\n";
        return;
    }

    py::module_ sys = py::module_::import("sys");
    sys.attr("path").attr("append")(dir);

    for (auto file : std::filesystem::directory_iterator(dir)) {
        if (file.is_directory() or not file.path().string().ends_with(".py"))
            continue;

        std::string name = file.path();
        std::cerr << "Found py-plugin: " << name << std::endl;
        name = name.substr(name.find_last_of("/") + 1);
        name = name.substr(0, name.find(".py"));
        try {
            py::module_ plug = py::module_::import(name.c_str());
            py::object res = plug.attr("init_module")();
        } catch(const std::exception &exc) {
            std::cerr << "WARINIG: Unable to open plugin: " << name << " (error: " << exc.what() << ")\n";
        }
    }
#else
    std::cerr << "DEBUG: init_module is skipped in STANDALONE_MODE.\n";
#endif
}

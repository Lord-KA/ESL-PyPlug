#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

#include <iostream>
#include <string>

#include "tools.hpp"

namespace py = pybind11;

class PyProxyTool : public booba::Tool {
public:
    PyProxyTool(py::object &tool) : pyTool(tool)
    {

    }

    virtual void apply(booba::Image* image, const booba::Event* event)
    {
        pyTool.attr("apply")();
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

    py::object &pyTool;
};

PYBIND11_EMBEDDED_MODULE(pyplug, m)
{    // `m` is a `py::module_` which is used to bind functions and classes
    m.def("set_toolbar_size", [](size_t w, size_t h) {
        std::cout << "set_toolbar_size() called with w = " << w << "; h = " << h << std::endl;
    });

    m.def("add_tool", [](py::object &pyTool) {
        PyProxyTool *proxy = new PyProxyTool(pyTool);
        //XXX add_tool(proxy);
        std::cout << "add_tool() called!\n";
    });
}

int main()
{
    py::scoped_interpreter guard{};

    py::print("Hello world!");

    py::module_ tools = py::module_::import("tools");
    py::object res = tools.attr("init_modules")();

    py::print("finished!");
}

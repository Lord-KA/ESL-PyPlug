#include <vector>

#define STANDALONE_MODE
/*
 * Just for testing purpouses.
 */
#include "pyplug.cpp"

static std::vector<booba::Tool*> tools;

namespace booba {
    void addTool(Tool *t)
    {
        std::cerr << "Tool " << (size_t)(t) << " was added\n";
        tools.push_back(t);
    }

    bool setToolBarSize(size_t w, size_t h)
    {
        std::cerr << "ToolBar was set to (w = " << w << ", h = " << h << ")\n";
        return true;
    }

    size_t createButton(size_t x, size_t y, size_t w, size_t h, const char *text)
    {
        return x + y + w + h;
    }
}

int main()
{
    py::scoped_interpreter guard{};

    std::cerr << "Started testing!\n";
    py::module_ m = py::module_::import("test-plug");
    py::object res = m.attr("init_module")();

    for (auto tool : tools) {
        tool->buildSetupWidget();
        DummyImage dummyImage;
        tool->apply(&dummyImage, NULL);
    }
    std::cerr << "Finished testing!\n";
}

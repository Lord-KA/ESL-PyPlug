#include "tools.hpp"
#include <vector>

#define STANDALONE_MODE
/*
 * Just for testing purpouses.
 */
#include "pyplug.cpp"

static std::vector<booba::Tool*> tools;

// ESL {{{

namespace booba {
    void addTool(Tool *t)
    {
        std::cerr << "Tool " << (size_t)(t) << " was added\n";
        tools.push_back(t);
    }

    void addFilter(Tool *t)
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

    size_t createLabel(size_t x, size_t y, size_t w, size_t h, const char *text)
    {
        return x + y + w + h;
    }

    size_t createSlider(size_t x, size_t y, size_t w, size_t h, long min, long max, long start)
    {
        return min + max + start;
    }

    size_t createCanvas(size_t x, size_t y, size_t w, size_t h)
    {
        return x + y + w + h;
    }

    void putPixel(uint64_t canvas, size_t x, size_t y, uint32_t color)
    {
        //TODO
    }

    void putSprite(uint64_t canvas, size_t x, size_t y, size_t w, size_t h, const char *texture)
    {
        //TODO
    }

    void cleanCanvas(uint64_t canvasId, uint32_t color)
    {
        //TODO
    }

    ApplicationContext *APPCONTEXT;
}
// ESL }}}

class TestingImage : public booba::Image {
public:
    virtual size_t getH() override
    {
        return 10;
    }

    virtual size_t getW() override
    {
        return 20;
    }

    virtual uint32_t getPixel(size_t x, size_t y) override
    {
        return x + y;
    }

    virtual void setPixel(size_t x, size_t y, uint32_t color) override
    {
        assert(!"Not implemented yet!");
    }

    virtual booba::Picture getPicture(size_t x, size_t y, size_t w, size_t h) override
    {
        return booba::Picture(x, y, w, h, this);
    }

    virtual void setPicture(booba::Picture &&pic) override
    {
        std::cerr << "pic(2, 5) = " << pic(2, 5) << "\n";
        assert(pic(2, 5) == 179);
    }
};


int main()
{
    py::scoped_interpreter guard{};

    booba::APPCONTEXT = new booba::ApplicationContext;

    std::cerr << "Started testing!\n";
    py::module_ m = py::module_::import("test-plug");
    py::object res = m.attr("init_module")();

    for (auto tool : tools) {
        std::cerr << "Testing tool " << (size_t)tool << "\n";
        tool->buildSetupWidget();
        TestingImage image;
        booba::Event event;
        event.type = booba::EventType::NoEvent;
        tool->apply(&image, &event);
    }

    size_t ans = 0;
    for (size_t i = 0; i < 10e6; ++i)
        ans += i;

    TestingImage image;
    booba::Event event;
    event.type = booba::EventType::MouseMoved;

    tools[0]->apply(&image, &event);

    for (size_t i = 0; i < 10e6; ++i)
        ans += i;

    tools[0]->apply(&image, &event);

    for (size_t i = 0; i < 10e6; ++i)
        ans += i;

    tools[1]->apply(&image, &event);

    for (size_t i = 0; i < 10e6; ++i)
        ans += i;

    tools[0]->apply(&image, &event);

    tools[2]->apply(&image, &event);

    std::cerr << "Finished testing!\n";
}

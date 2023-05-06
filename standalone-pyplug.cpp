#include "tools.hpp"
#include <cstdint>
#include <sys/types.h>
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

    void setValueSlider(uint64_t sliderId, int64_t val)
    {
        //TODO
    }

    uint64_t createEditor(size_t x, size_t y, size_t w, size_t h)
    {
        return x * y + w * h;
    }

    void setTextEditor(uint64_t editorId, const char *text)
    {
        //TODO
    }

    char* getTextEditor(uint64_t editorId)
    {
        return (char*)editorId;
    }

    void putPixel(uint64_t canvas, size_t x, size_t y, booba::Color color)
    {
        //TODO
    }

    void putSprite(uint64_t canvas, size_t x, size_t y, size_t w, size_t h, const char *texture)
    {
        //TODO
    }

    void cleanCanvas(uint64_t canvasId, booba::Color color)
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

    virtual booba::Color getPixel(size_t x, size_t y) override
    {
        return booba::Color(static_cast<uint32_t>(x + y));
    }

    virtual void setPixel(size_t x, size_t y, booba::Color color) override
    {
        assert(!"Not implemented yet!");
    }

    virtual booba::Picture getPicture(size_t x, size_t y, size_t w, size_t h) override
    {
        return booba::Picture(x, y, w, h, this);
    }

    virtual void setPicture(booba::Picture &&pic) override
    {
        std::cerr << "pic(2, 5) = " << pic(2, 5).toInteger() << "\n";
        assert(pic(2, 5).toInteger() == 179);
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

    std::cerr << "Texture: " << tools[0]->getTexture() << "\n";

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

    event.type = booba::EventType::ButtonClicked;
    tools[2]->apply(&image, &event);

    std::cerr << "Finished testing!\n";
}

#pragma once

#include "tools.hpp"
#include <cstdint>
#include <cassert>

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

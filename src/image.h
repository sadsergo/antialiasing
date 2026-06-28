#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <limits>
#include <cstdint>

struct Image {
    int width, height;
    std::vector<uint8_t> pixels;  // RGBA, row-major
    std::vector<float>   depth;   // per-pixel NDC-Z depth

    Image(int w, int h)
        : width(w), height(h)
        , pixels(w * h * 4)
        , depth(w * h, std::numeric_limits<float>::infinity())
    {
        for (int i = 0; i < w * h; i++) {
            pixels[i*4+0] = 28;
            pixels[i*4+1] = 28;
            pixels[i*4+2] = 40;
            pixels[i*4+3] = 255;
        }
    }

    void setPixel(int x, int y, glm::vec3 color) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        int i = (y * width + x) * 4;
        pixels[i+0] = uint8_t(glm::clamp(color.r, 0.f, 1.f) * 255.f);
        pixels[i+1] = uint8_t(glm::clamp(color.g, 0.f, 1.f) * 255.f);
        pixels[i+2] = uint8_t(glm::clamp(color.b, 0.f, 1.f) * 255.f);
        pixels[i+3] = 255;
    }

    // Returns true and updates depth if z is closer
    bool depthTest(int x, int y, float z) {
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        float& d = depth[y * width + x];
        if (z < d) { d = z; return true; }
        return false;
    }

    bool save(const std::string& path) const;
};

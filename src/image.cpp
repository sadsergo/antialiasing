#include "image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third_party/stb_image_write.h"

bool Image::save(const std::string& path) const {
    return stbi_write_png(path.c_str(), width, height, 4, pixels.data(), width * 4) != 0;
}

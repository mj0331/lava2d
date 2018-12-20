#include "texture.h"
#include "app.h"

//Weird STB thing required in an implementation source file for STB_Image to link properly
//https://github.com/nothings/stb/issues/3
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace l2d{
    texture::texture(std::string& filepath) {
        file_path = filepath;
        app::app_log.info("TEXTURE LOAD {}", filepath);
        data = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb);
        if(data == nullptr) {
            app::app_log.critical("Failed to load texture {}!", filepath);
            ABORT(20);
        }
    }

    void texture::free_pixel_data() {
        if(data == nullptr) {
            app::app_log.error("Trying to free already freed image {}", file_path);
            return;
        }
        app::app_log.info("Texture image data free for {}", file_path);
        stbi_image_free(data);
    }

    texture::~texture() {
        free_pixel_data();
    }
}
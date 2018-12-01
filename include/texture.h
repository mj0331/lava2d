#if !defined(L2D_TEXTURE_H)
#define L2D_TEXTURE_H

#include <string>

typedef unsigned char pixel_data;

namespace l2d {
    class texture {
        protected:
            std::string file_path;
            pixel_data* data;
            int width;
            int height;
            int channels;

        public:
            texture() = default;
            texture(std::string filepath);
            texture(const l2d::texture& copy);
            ~texture();

            pixel_data* get_pixels() const { return data; }
            int get_width() const { return width; }
            int get_height() const { return height; }
            int get_channel_count() const { return channels; }
            void free_pixel_data();
    };
}

#endif // L2D_TEXTURE_H

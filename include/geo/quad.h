#if !defined(L2D_GEO_QUAD_H)
#define L2D_GEO_QUAD_H

#include "base_shape.h"
#include "texture.h"

#include <string>

namespace l2d{
    namespace geo{
        class quad : public base_shape {
            private:
                void create_vertex_buffer() override;
                void create_texture() override;

                l2d::texture quad_texture;
            public:
                quad() = default;
                quad(std::string texture_path);
                ~quad() = default;
        }; 
    }
}

#endif // L2D_GEO_QUAD_H

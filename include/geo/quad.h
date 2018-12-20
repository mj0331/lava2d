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
                void create_sampler();
                void create_uniform_buffer();
                void create_pipeline();
                void create_command_buffer();

                l2d::texture quad_texture;
            public:
                quad() = default;
                quad(std::string texture_path);
                ~quad();
        }; 
    }
}

#endif // L2D_GEO_QUAD_H

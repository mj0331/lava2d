#if !defined(L2D_GEO_BASE_SHAPE_H)
#define L2D_GEO_BASE_SHAPE_H

#include <glm/glm.hpp>
#include <VEZ.h>

#include <memory>
#include <vector>

namespace l2d{

    typedef struct PipelineDesc
    {
        VezPipeline pipeline = VK_NULL_HANDLE;
        std::vector<VkShaderModule> shaderModules;
    } PipelineDesc;

    namespace geo{
        class base_shape{
            public:
                struct Vertex
                {
                    float x, y, z;
                    float nx, ny, nz;
                    float u, v;
                };

                struct UniformBuffer
                {
                    glm::mat4 model;
                    glm::mat4 view;
                    glm::mat4 projection;
                };
            protected:
                VkBuffer vk_vertex_buffer;
                VkBuffer vk_index_buffer;
                VkQueue vk_graphics_queue = VK_NULL_HANDLE;
                VkImage vk_image = VK_NULL_HANDLE;
                VkImageView vk_image_view = VK_NULL_HANDLE;
                VkSampler vk_sampler = VK_NULL_HANDLE;
                VkBuffer vk_uniform_buffer = VK_NULL_HANDLE;
                PipelineDesc vk_basic_pipeline;
                VkCommandBuffer vk_command_buffer = VK_NULL_HANDLE;

                std::vector<Vertex> vertex_data;
            public:
                base_shape() = default;
                ~base_shape() = default;

                virtual Vertex* get_vertices() { return vertex_data.data(); }
                virtual void create_vertex_buffer() = 0;
                virtual void create_texture() = 0;
        };
    }
}

#endif // L2D_GEO_BASE_SHAPE_H

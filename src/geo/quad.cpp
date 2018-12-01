#include "../../include/geo/quad.h"
#include "../../include/app.h"

#include <VEZ.h>
#include <stb_image.h>

namespace l2d{
    //class app{};

    namespace geo{
        quad::quad(std::string texture_path)
        {
            quad_texture = l2d::texture(texture_path);
            create_vertex_buffer();
            create_texture();
        }

        void quad::create_vertex_buffer() {
            // A single quad with positions, normals and uvs.
            vertex_data = {
                { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
                {  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
                {  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
                { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f }
            };

            // Create the device side vertex buffer.
            VezBufferCreateInfo bufferCreateInfo = {};
            bufferCreateInfo.size = sizeof(vertex_data);
            bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;    
            auto result = vezCreateBuffer(l2d::app::get_instance()->get_device(), VEZ_MEMORY_GPU_ONLY, &bufferCreateInfo, &vk_vertex_buffer);
            if (result != VK_SUCCESS)
                app::app_log.critical("vezCreateBuffer failed for vertex buffer");

            // Upload the host side data.
            result = vezBufferSubData(app::get_instance()->get_device(), vk_vertex_buffer, 0, sizeof(vertex_data), static_cast<void*>(vertex_data.data()));
            if (result != VK_SUCCESS)
                app::app_log.critical("vezBufferSubData failed for vertex buffer");    

            // A single quad with positions, normals and uvs.
            uint32_t indices[] = {
                0, 1, 2,
                0, 2, 3,
            };

            // Create the device side index buffer.
            bufferCreateInfo.size = sizeof(indices);
            bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            result = vezCreateBuffer(app::get_instance()->get_device(), VEZ_MEMORY_GPU_ONLY, &bufferCreateInfo, &vk_index_buffer);
            if (result != VK_SUCCESS)
                app::app_log.critical("vezCreateBuffer failed to create device side index buffer");

            // Upload the host side data.
            result = vezBufferSubData(app::get_instance()->get_device(), vk_index_buffer, 0, sizeof(indices), static_cast<void*>(indices));
            if (result != VK_SUCCESS)
                app::app_log.critical("vezBufferSubData failed to upload index buffer data from host");    
        }

        void quad::create_texture() {
            // // Load image from disk.
            // int width, height, channels;
            // auto pixelData = stbi_load("../../Samples/Data/Textures/texture.jpg", &width, &height, &channels, 4);

            // Create the AppBase::GetDevice() side image.
            VezImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
            imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
            imageCreateInfo.extent = { static_cast<uint32_t>(quad_texture.get_width()), static_cast<uint32_t>(quad_texture.get_height()), 1 };
            imageCreateInfo.mipLevels = 1;
            imageCreateInfo.arrayLayers = 1;
            imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            auto result = vezCreateImage(app::get_instance()->get_device(), VEZ_MEMORY_GPU_ONLY, &imageCreateInfo, &vk_image);
            if (result != VK_SUCCESS)
                app::app_log.critical("vezCreateImage failed");

            // Upload the host side data.
            VezImageSubDataInfo subDataInfo = {};
            subDataInfo.imageSubresource.mipLevel = 0;
            subDataInfo.imageSubresource.baseArrayLayer = 0;
            subDataInfo.imageSubresource.layerCount = 1;
            subDataInfo.imageOffset = { 0, 0, 0 };
            subDataInfo.imageExtent = { imageCreateInfo.extent.width, imageCreateInfo.extent.height, 1 };
            result = vezImageSubData(app::get_instance()->get_device(), vk_image, &subDataInfo, reinterpret_cast<const void*>(quad_texture.get_pixels()));
            if (result != VK_SUCCESS)
                app::app_log.critical("vezImageSubData failed");

            // Destroy the pixel data.
            //quad_texture.free_pixel_data();

            // Create the image view for binding the texture as a resource.
            VezImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.image = vk_image;
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = imageCreateInfo.format;
            imageViewCreateInfo.subresourceRange.layerCount = 1;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            result = vezCreateImageView(app::get_instance()->get_device(), &imageViewCreateInfo, &vk_image_view);
            if (result != VK_SUCCESS)
                app::app_log.critical("vezCreateImageView failed");
        }
    }
}
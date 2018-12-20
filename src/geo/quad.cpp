#include "../../include/geo/quad.h"
#include "../../include/app.h"

#include <VEZ.h>
#include <stb_image.h>

namespace l2d{
    namespace geo{
        quad::quad(std::string texture_path)
            : quad_texture(texture_path)
        {
            create_vertex_buffer();
            create_texture();
            create_sampler();
            create_uniform_buffer();
            create_pipeline();
            create_command_buffer();
        }

        quad::~quad() {
            auto device = app::get_instance()->get_device();

            vezDestroyBuffer(device, vk_vertex_buffer);
            vezDestroyBuffer(device, vk_index_buffer);
            vezDestroyImageView(device, vk_image_view);
            vezDestroyImage(device, vk_image);
            vezDestroySampler(device, vk_sampler);
            vezDestroyBuffer(device, vk_uniform_buffer);

            vezDestroyPipeline(device, vk_basic_pipeline.pipeline);
            for (auto shaderModule : vk_basic_pipeline.shaderModules)
                vezDestroyShaderModule(device, shaderModule);

            vezFreeCommandBuffers(device, 1, &vk_command_buffer);
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
             int width, height, channels;
             auto pixelData = stbi_load("E:\\Projects\\lava2d_cmake\\build\\bin\\Debug\\Debug\\Data\\Textures\\texture.jpg", &width, &height, &channels, 4);

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
            //result = vezImageSubData(app::get_instance()->get_device(), vk_image, &subDataInfo, reinterpret_cast<const void*>(quad_texture.get_pixels()));
            result = vezImageSubData(app::get_instance()->get_device(), vk_image, &subDataInfo, reinterpret_cast<const void*>(pixelData));
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

        void quad::create_sampler() {
            VezSamplerCreateInfo createInfo = {};
            createInfo.magFilter = VK_FILTER_LINEAR;
            createInfo.minFilter = VK_FILTER_LINEAR;
            createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            auto result = vezCreateSampler(app::get_instance()->get_device(), &createInfo, &vk_sampler);
            if (result != VK_SUCCESS)
                app::app_log.critical("vezCreateSampler failed");
        }

        void quad::create_uniform_buffer()
        {
            // Create a buffer for storing per frame matrices.
            VezBufferCreateInfo createInfo = {};
            createInfo.size = sizeof(UniformBuffer);
            createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            if (vezCreateBuffer(app::get_instance()->get_device(), VEZ_MEMORY_CPU_TO_GPU, &createInfo, &vk_uniform_buffer) != VK_SUCCESS)
                app::app_log.critical("vezCreateBuffer failed for uniform buffer");
        }

        void quad::create_pipeline()
        {
            // Create the graphics pipeline.
            if (!app::get_instance()->create_pipeline(
                { { "E:\\Projects\\lava2d_cmake\\build\\bin\\Debug\\Debug\\Data\\Shaders\\SimpleQuad\\SimpleQuad.vert", VK_SHADER_STAGE_VERTEX_BIT },
                { "E:\\Projects\\lava2d_cmake\\build\\bin\\Debug\\Debug\\Data\\Shaders\\SimpleQuad\\SimpleQuad.frag", VK_SHADER_STAGE_FRAGMENT_BIT } },
                &vk_basic_pipeline.pipeline, &vk_basic_pipeline.shaderModules))
            {
                app::app_log.critical("Failed to create graphics pipeline!");
            }
        }

        void quad::create_command_buffer()
        {
            // Get the graphics queue handle.
            vezGetDeviceGraphicsQueue(app::get_instance()->get_device(), 0, &vk_graphics_queue);

            // Create a command buffer handle.
            VezCommandBufferAllocateInfo allocInfo = {};
            allocInfo.queue = vk_graphics_queue;
            allocInfo.commandBufferCount = 1;
            if (vezAllocateCommandBuffers(app::get_instance()->get_device(), &allocInfo, &vk_command_buffer) != VK_SUCCESS)
                app::app_log.critical("vezAllocateCommandBuffers failed");

            // Begin command buffer recording.
            if (vezBeginCommandBuffer(vk_command_buffer, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT) != VK_SUCCESS)
                app::app_log.critical("vezBeginCommandBuffer failed");

            // Set the viewport state and dimensions.
            int width, height;
            app::get_instance()->get_window_size(width, height);
            VkViewport viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
            VkRect2D scissor = { { 0, 0 },{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) } };
            vezCmdSetViewport(0, 1, &viewport);
            vezCmdSetScissor(0, 1, &scissor);
            vezCmdSetViewportState(1);

            // Define clear values for the swapchain's color and depth attachments.
            std::array<VezAttachmentReference, 2> attachmentReferences = {};
            attachmentReferences[0].clearValue.color = { 0.3f, 0.3f, 0.3f, 0.0f };  
            attachmentReferences[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentReferences[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentReferences[1].clearValue.depthStencil.depth = 1.0f;
            attachmentReferences[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentReferences[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            // Begin a render pass.
            VezRenderPassBeginInfo beginInfo = {};
            //beginInfo.framebuffer = AppBase::GetFramebuffer();
            beginInfo.framebuffer = app::get_instance()->get_framebuffer();
            beginInfo.attachmentCount = static_cast<uint32_t>(attachmentReferences.size());
            beginInfo.pAttachments = attachmentReferences.data();
            vezCmdBeginRenderPass(&beginInfo);
        
            // Bind the pipeline and associated resources.
            vezCmdBindPipeline(vk_basic_pipeline.pipeline);
            vezCmdBindBuffer(vk_uniform_buffer, 0, VK_WHOLE_SIZE, 0, 0, 0);
            vezCmdBindImageView(vk_image_view, vk_sampler, 0, 1, 0);

            // Set depth stencil state.
            VezPipelineDepthStencilState depthStencilState = {};
            depthStencilState.depthTestEnable = VK_TRUE;
            depthStencilState.depthWriteEnable = VK_TRUE;
            depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            vezCmdSetDepthStencilState(&depthStencilState);

            // Bind the vertex buffer and index buffers.
            VkDeviceSize offset = 0;
            vezCmdBindVertexBuffers(0, 1, &vk_vertex_buffer, &offset);
            vezCmdBindIndexBuffer(vk_index_buffer, 0, VK_INDEX_TYPE_UINT32);

            // Draw the quad.
            vezCmdDrawIndexed(6, 1, 0, 0, 0);

            // End the render pass.
            vezCmdEndRenderPass();

            // End command buffer recording.
            if (vezEndCommandBuffer() != VK_SUCCESS)
                app::app_log.critical("vezEndCommandBuffer failed");
        }
    }
}
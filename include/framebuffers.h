#if !defined(L2D_FRAMEBUFFERS_H)
#define L2D_FRAMEBUFFERS_H

#include <VEZ.h>

namespace l2d{
    typedef struct {
        VkImage colorImage = VK_NULL_HANDLE;
        VkImageView colorImageView = VK_NULL_HANDLE;
        VkImage depthStencilImage = VK_NULL_HANDLE;
        VkImageView depthStencilImageView = VK_NULL_HANDLE;
        VezFramebuffer handle = VK_NULL_HANDLE;
    } color_depth_framebuffer;
}

#endif // L2D_FRAMEBUFFERS_H

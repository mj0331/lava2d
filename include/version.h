#if !defined(L2D_VERSION_H)
#define L2D_VERSION_H

namespace l2d{
    struct version{
        uint32_t major:10;
        uint32_t minor:10;
        uint32_t patch:12;
    };
}

#endif // L2D_VERSION_H

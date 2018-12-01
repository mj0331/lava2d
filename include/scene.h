#if !defined(L2D_SCENE_H)
#define L2D_SCENE_H

#include <vector>
#include <string>

#include "geo/quad.h"

namespace l2d{
    class scene{
        protected:
            std::string id;
            std::vector<scene*> child_scenes;
            std::vector<geo::quad> sprites;

        public:
            scene();
            scene(const std::string& id);
            ~scene();

            void update(float dt);
            const std::string& get_id() const { return id; }
    };
}

#endif // L2D_SCENE_H

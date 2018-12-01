#include "scene.h"

namespace l2d{
    scene::scene() {
        
    }

    scene::scene(const std::string& id) {
        this->id = id;
    }

    scene::~scene() {

    }

    void scene::update(float dt) {
        //Scene code here

        //Updating child scenes
        for(auto& scene : child_scenes) {
            scene->update(dt);
        }
    }
}
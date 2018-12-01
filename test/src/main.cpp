#include "app.h"
#include "geo/quad.h"

class my_app : public l2d::app {
private:
    l2d::geo::quad my_quad;
public:
    my_app(int w, int h, std::string title, l2d::version v);

    void update(float dt) override;
    void on_key_down(int key) override;
};

my_app::my_app(int w, int h, std::string title, l2d::version v)
    : l2d::app(w, h, title, v), my_quad("./Data/Textures/texture.jpg")
{
    
}

void my_app::on_key_down(int key) {
    if(key == GLFW_KEY_ESCAPE) {
        shutdown();
    }
}

void my_app::update(float dt) {
    root_scene->update(dt);
}

int main()
{
    my_app test_app(960, 540, "Test App", l2d::version{0, 0, 1});
    test_app.set_root_scene(new l2d::scene("blank_scene"));
    test_app.run();
    
    return 0;
}
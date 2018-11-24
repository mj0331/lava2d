#include "app.h"

class my_app : public l2d::app {
public:
    my_app(int w, int h, std::string title, l2d::version v);

    void update(float dt) override;
    void on_key_down(int key) override;
};

my_app::my_app(int w, int h, std::string title, l2d::version v)
    : l2d::app(w, h, title, v)
{
    
}

void my_app::on_key_down(int key) {
    if(key == GLFW_KEY_ESCAPE) {
        shutdown();
    }
}

void my_app::update(float dt) {

}

int main()
{
    my_app test_app(960, 540, "Test App", l2d::version{0, 0, 1});
    test_app.run();
    
    return 0;
}
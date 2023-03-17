#include "Camera.h"
#include "Scene.h"
#include "Vector.h"
#include "Renderer.h"


int main() {
    std::string scene_name = R"(cornell-box)";
    // build Scene
    Scene s(scene_name);

	// build render
    MonteCarloPathTracer renderer(0.6, 4);

    //
    renderer.render(s);
    renderer.write_render_result(scene_name, s.p_camera_->width_, s.p_camera_->height_);
}

#include "Scene.h"
#include "Renderer.h"

#include "Triangle.h"

int main() {
    //std::string scene_name = R"(stairscase)";
    //std::string scene_name = R"(cornell-box-test)";
    std::string veach_mis_name = R"(veach-mis)";
    

    Scene veach_mis(veach_mis_name);
    MonteCarloPathTracer mis_path_tracer(0.8, 5120);
    mis_path_tracer.render(veach_mis);
    mis_path_tracer.write_render_result(veach_mis_name + "5120", veach_mis.p_camera_->width_, veach_mis.p_camera_->height_);

    std::string cornell_box_name = R"(cornell-box)";
    Scene cornell_box(cornell_box_name);
    MonteCarloPathTracer box_path_tracer(0.8, 512);
    box_path_tracer.render(cornell_box);
    box_path_tracer.write_render_result(cornell_box_name + "512", cornell_box.p_camera_->width_, cornell_box.p_camera_->height_);

}

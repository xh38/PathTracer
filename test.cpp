#include "Scene.h"
#include "Renderer.h"

#include "Triangle.h"

int main() {
    
    //std::string scene_name = R"(cornell-box-test)";
    // 3 hours
    //std::string veach_mis_name = R"(veach-mis)";
    //Scene veach_mis(veach_mis_name);
    //MonteCarloPathTracer mis_path_tracer(0.8, 5120);
    //mis_path_tracer.render(veach_mis);
    //mis_path_tracer.write_render_result(veach_mis_name + "_5120_", veach_mis.p_camera_->width_, veach_mis.p_camera_->height_);

    std::string cornell_box_name = R"(cornell-box)";
    Scene cornell_box(cornell_box_name);
    MonteCarloPathTracer box_path_tracer(0.8, 1);
    box_path_tracer.render(cornell_box);
    //box_path_tracer.write_render_result(cornell_box_name + "1", cornell_box.p_camera_->width_, cornell_box.p_camera_->height_);

	/*std::string stairscase_name = R"(stairscase)";
    Scene stairscase(stairscase_name);
    MonteCarloPathTracer stair_tracer(0.5, 128);
    stair_tracer.render(stairscase);
    stair_tracer.write_render_result(stairscase_name + "_128", stairscase.p_camera_->width_, stairscase.p_camera_->height_);*/
}

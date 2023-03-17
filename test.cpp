#include "Scene.h"
#include "Renderer.h"

#include "Triangle.h"

int main() {
    //std::string scene_name = R"(stairscase)";
    //std::string scene_name = R"(cornell-box-test)";
    std::string scene_name = R"(veach-mis)";
    // build Scene
    Scene s(scene_name);
    std::cout << "Scene " << scene_name << " build done!" << std::endl;

    MonteCarloPathTracer path_tracer(0.8, 512);
    //Ray test_ray = s.p_camera_->sample_ray(4, 60);
    //Intersection inter = s.get_closest_intersection_bvh(test_ray);
    //Vec3 color = path_tracer.trace_ray(test_ray, s, inter);
    //std::cout << color;
    //if (inter.happened && inter.p_m->is_light())
    //{
    //    std::cout << "hit light" << std::endl;
    //}
    path_tracer.render(s);
    path_tracer.write_render_result(scene_name, s.p_camera_->width_, s.p_camera_->height_);
}

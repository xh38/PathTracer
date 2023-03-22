#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Scene.h"
#include "tinyxml2.h"
#include <sstream>
#include <array>


bool Scene::get_closest_intersection(Ray& ray, std::shared_ptr<Intersection>& inter) {
	return mesh_->intersect(ray, inter);
}

bool Scene::get_closest_intersection_bvh(Ray& ray, std::shared_ptr<Intersection>& inter) {
	return this->BVH->intersect(ray, inter);
}


void Scene::sample_light(Intersection& light_sample, float& pdf) {
	float p = get_rand_float() * light_area_sum;
	//std::cout << "light area in total:" << area_sum << std::endl;
	float sample_area_sum = 0;
	for (const auto& light : lights_)
	{
		sample_area_sum += light->area;
		if (p <= sample_area_sum)
		{
			float no_use;
			light->sample_point(light_sample, no_use);
			pdf = 1 / light_area_sum;
			break;
		}
	}
	
}


void load_materials(std::vector<std::shared_ptr<Material>>& scene_m, const std::vector<tinyobj::material_t>& loaded_m, const std::map<std::string, Vec3>& light_map, std::string prefix) {
	for (tinyobj::material_t m : loaded_m)
	{
		Material temp_m{};
		temp_m.name = m.name;
		temp_m.Kd = { m.diffuse[0], m.diffuse[1], m.diffuse[2] };
		temp_m.Ks = { m.specular[0], m.specular[1], m.specular[2] };
		temp_m.Tr = { m.transmittance[0], m.transmittance[1], m.transmittance[2] };
		temp_m.Ni = m.ior;
		temp_m.Ns = m.shininess;
		if (m.diffuse_texname != "")
		{
			std::string path = prefix + m.diffuse_texname;
			temp_m.texture = Texture(path);
			temp_m.hasTexture = true;
		}
		if (light_map.count(m.name) != 0)
		{
			temp_m.hasEmit = true;
			temp_m.radiance = light_map.at(m.name);
		}
		scene_m.push_back(std::make_shared<Material>(temp_m));
	}
}

Scene::Scene(const std::string& scene_name) {
	std::cout << "start building Scene" << std::endl;
	//load Camera and light setup from .xml
	std::cout << "start loading Camera and lights" << std::endl;
	tinyxml2::XMLDocument doc;

	std::string prefix = R"(C:\Users\AAA\Desktop\PathTracer\PathTracer\PathTracer\example-scenes-cg22\)";
	std::string xml_filename = prefix + scene_name + "\\" + scene_name + ".xml";
	if (doc.LoadFile(xml_filename.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		throw std::runtime_error("xml file not find");
	}

	//Camera base
	tinyxml2::XMLElement* camera_element = doc.FirstChildElement("camera");
	std::string type(camera_element->Attribute("type"));
	int width = atoi(camera_element->Attribute("width"));
	int height = atoi(camera_element->Attribute("height"));
	float fovy = atof(camera_element->Attribute("fovy"));

	//eye
	tinyxml2::XMLElement* eye_element = camera_element->FirstChildElement("eye");
	float eye_x = atof(eye_element->Attribute("x"));
	float eye_y = atof(eye_element->Attribute("y"));
	float eye_z = atof(eye_element->Attribute("z"));
	Vec3 eye(eye_x, eye_y, eye_z);

	//lookat
	tinyxml2::XMLElement* lookat_element = camera_element->FirstChildElement("lookat");
	float lookat_x = atof(lookat_element->Attribute("x"));
	float lookat_y = atof(lookat_element->Attribute("y"));
	float lookat_z = atof(lookat_element->Attribute("z"));
	Vec3 lookat(lookat_x, lookat_y, lookat_z);

	//up
	tinyxml2::XMLElement* up_element = camera_element->FirstChildElement("up");
	float up_x = atof(up_element->Attribute("x"));
	float up_y = atof(up_element->Attribute("y"));
	float up_z = atof(up_element->Attribute("z"));
	Vec3 up(up_x, up_y, up_z);

	Camera cam(eye, lookat, up, fovy, width, height);
	p_camera_ = std::make_unique<Camera>(cam);
	std::cout << "camera loaded" << std::endl;

	//lights
	std::map<std::string, Vec3> light_map;
	tinyxml2::XMLElement* light_element = doc.FirstChildElement("light");
	while(light_element!=nullptr)
	{
		std::string mtl_name(light_element->Attribute("mtlname"));

		std::istringstream radiance_string(light_element->Attribute("radiance"));
		Vec3 radiance;
		for (float& i : radiance.e)
		{
			std::string temp;
			std::getline(radiance_string, temp, ',');
			i = atof(temp.c_str());
		}
		light_map.insert(std::make_pair(mtl_name, radiance));
		light_element = light_element->NextSiblingElement("light");
	}
	std::cout << light_map.size() << " lights radiance loaded" << std::endl;

	//load triangle mesh form .obj and load the material from .mtl
	std::string obj_filename = prefix + scene_name + "\\" + scene_name + ".obj";
	std::string mtl_folder = prefix + scene_name;

	std::cout << "loading obj file " << obj_filename << std::endl;
	std::cout << "loading mtl in " << mtl_folder << std::endl;

	tinyobj::ObjReaderConfig reader_config;
	tinyobj::ObjReaderConfig config;
	reader_config.mtl_search_path = mtl_folder; // Path to material files

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(obj_filename, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}
		exit(1);
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();


	std::string texture_prefix =  prefix + scene_name + "\\";
	load_materials(materials_, materials, light_map, texture_prefix);
	std::cout << materials_.size() << " materials loaded" << std::endl;
	std::vector<std::shared_ptr<Object>> triangles;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			const int material_index = shapes[s].mesh.material_ids[f];

			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
			std::array<Vec3, 3> face_vertices;
			std::array<float, 3> tex_coord_x;
			std::array<float, 3> tex_coord_y;
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				

				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

				face_vertices[v] = Vec3(vx, vy, vz);


				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
				}

			}
			auto p_triangle = std::make_shared<Triangle>(Triangle(face_vertices[0], face_vertices[1], face_vertices[2], materials_[material_index]));
			triangles.push_back(p_triangle);
			if (materials_[material_index]->is_light())
			{
				lights_.push_back(p_triangle);
			}
			//else
			//{
			//	std::cout << "tri" << std::endl;
			//	std::cout << "v0: " << p_triangle->v0 << std::endl;
			//	std::cout << "v1: " << p_triangle->v1 << std::endl;
			//	std::cout << "v2: " << p_triangle->v2 << std::endl;
			//}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
	}
	std::cout << triangles.size() << " triangles loaded" << std::endl;

	light_area_sum = 0.0;
	for (auto light : lights_)
	{
		light_area_sum += light->area;
	}

	TriangleMesh mesh(triangles);
	mesh_ = std::make_unique<TriangleMesh>(mesh);
	BVH = new BVHAccel(triangles);
	std::cout << "BVH has depth of: " << BVH->depth() << std::endl;
	std::cout << BVH->count_leaves() << " triangles in BVH" << std::endl;
	std::cout << lights_.size() << " emitting triangles in total" << std::endl;
}



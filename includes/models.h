#include <plane.h>

// settings
#define SCR_WIDTH 1920
#define SCR_HEIGHT 640

Plane *plane;
Shader *shader;

void loadModels() {
	// build and compile shaders
    // -------------------------
    plane = new Plane();
    shader = new Shader(FileSystem::getPath("resources/blinn-phong.vs").c_str(), FileSystem::getPath("resources/blinn-phong.fs").c_str());

}

void render(const Shader &shader, Model obj, glm::mat4 model) {
    shader.setMat4("model", model);
    obj.Draw(shader);
}

// renders the 3D scene
// --------------------
void renderScene(const Shader &shader, glm::vec3 lightColor, glm::vec3 lightPos)
{
    plane->Draw(shader);
}
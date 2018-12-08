#include <plane.h>
#include <obstacles.h>

// settings
#define SCR_WIDTH 1920
#define SCR_HEIGHT 600

Plane *plane;
Shader *shader;
Block *block;
Fence *fence;
Model *cube, *openCube;

void loadModels() {
	// build and compile shaders
    // -------------------------
    plane = new Plane(SCR_WIDTH, SCR_HEIGHT);
    shader = new Shader(FileSystem::getPath("resources/blinn-phong.vs").c_str(), FileSystem::getPath("resources/blinn-phong.fs").c_str());
    cube = new Model(FileSystem::getPath("resources/objects/cube/cube.obj").c_str());
    openCube = new Model(FileSystem::getPath("resources/objects/cube/openCube.obj").c_str());
    block = new Block(cube, "resources/textures/bricks2.jpg");
    fence = new Fence(openCube, "resources/textures/toy_box_normal.png");
}

void render(const Shader &shader, Model obj, glm::mat4 model) {
    shader.setMat4("model", model);
    obj.Draw(shader);
}

// renders the 3D scene
// --------------------
void renderScene(const Shader &shader, glm::vec3 lightColor, glm::vec3 lightPos)
{
    // fence->Draw(shader);
    plane->Draw(shader);
    block->queueWave(shader);
    fence->queueWave(shader);
}
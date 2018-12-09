#include <plane.h>
#include <obstacles.h>

// settings
#define SCR_WIDTH 1500
#define SCR_HEIGHT 700

Plane *plane;
Shader *shader;
Block *block;
Fence *fence;
Shrinker *shrinker;
Model *cube, *openCube, *wrench, *sky;
glm::mat4 skyMat;

void loadModels() {
	plane = new Plane();
    shader = new Shader(FileSystem::getPath("resources/blinn-phong.vs").c_str(), FileSystem::getPath("resources/blinn-phong.fs").c_str());

    cube = new Model(FileSystem::getPath("resources/objects/cube/cube.obj").c_str());
    block = new Block(cube, "resources/textures/bricks2.jpg");

    openCube = new Model(FileSystem::getPath("resources/objects/cube/openCube.obj").c_str());
    fence = new Fence(openCube, "resources/textures/toy_box_normal.png");

    wrench = new Model(FileSystem::getPath("resources/objects/wrench/wrench.obj").c_str());
    shrinker = new Shrinker(wrench, "resources/textures/metal.png");

    sky = new Model(FileSystem::getPath("resources/objects/planet/planet.obj").c_str());
    skyMat = glm::rotate(skyMat, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    skyMat = glm::rotate(skyMat, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    skyMat = glm::scale(skyMat, glm::vec3(2.0));
}

void render(const Shader &shader, Model obj, glm::mat4 model) {
    shader.setMat4("model", model);
    obj.Draw(shader);
}

// renders the 3D scene
// --------------------
int renderScene(Shader &shader, glm::vec3 lightColor, glm::vec3 lightPos, float deltaTime)
{
    int score;
    score = block->renderWave(shader, deltaTime, plane);
    // fence->renderWave(shadcder, deltaTime, plane);
    // shrinker->renderWave(shader, deltaTime, plane);
    plane->Draw(shader);
    skyMat = glm::rotate(skyMat, glm::radians(deltaTime * 5.0f), glm::vec3(0.0, 0.0, 1.0));
    shader.setVec3("lightPos", glm::vec3(0.0, 0.0, -500.0));
    render(shader, *sky, skyMat);
    shader.setVec3("lightPos", glm::vec3(1.0, 1.0, 2.0));
    return score;
}
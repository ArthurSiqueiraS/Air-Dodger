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
bool shrink;
float shrinkTimer;
int shrinkBounces;

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
    skyMat = glm::translate(skyMat, glm::vec3(0.0, -1.0, 0.0));
    skyMat = glm::scale(skyMat, glm::vec3(10.0));

    shrink = false;
    shrinkTimer = 5.0;
    shrinkBounces = 0;
}

void render(const Shader &shader, Model obj, glm::mat4 model) {
    shader.setMat4("model", model);
    obj.Draw(shader);
}

// renders the 3D scene
// --------------------
int renderScene(Shader &shader, glm::vec3 lightColor, glm::vec3 lightPos, float deltaTime)
{
    int score = 150, ev;
    bool endGame = false, availablePositions[8] = { true, true, true, true, true, true, true, true };
    ev = fence->renderWave(shader, deltaTime, plane, availablePositions);
    endGame = ev == -1;
    ev = block->renderWave(shader, deltaTime, plane, availablePositions);
    endGame = endGame || ev == -1;
    if(endGame)
        score *= -1;
    else
        score *= ev;

    plane->Draw(shader);

    if(!shrink) {
        if(shrinkBounces > 0 && shrinkTimer > 0.1)
            shrink = true;
        else if (shrinkBounces == 0) {
            ev = shrinker->renderWave(shader, deltaTime, plane, availablePositions);
            if(ev == 2) {
                shrink = true;
                shrinkTimer = 5.0;
                score += 100;
            }
        }
        if(shrinkTimer < 5.0) {
            plane->deShrink(deltaTime * 0.01);
            shrinkTimer = min(shrinkTimer + deltaTime, 5.0f);
        }
    }
    else {
        plane->shrink(deltaTime * 0.01);
        shrinkTimer = max(shrinkTimer - deltaTime, 0.0f);
        if(shrinkTimer <= 0.0) {
            shrink = false;
            if(shrinkBounces == 0)
                shrinkBounces = 5;
            else
                --shrinkBounces;
        }
    }
    skyMat = glm::rotate(skyMat, glm::radians(deltaTime * 1.5f), glm::vec3(0.0, 0.0, 1.0));
    shader.setVec3("lightPos", glm::vec3(0.0, 0.0, -500.0));
    render(shader, *sky, skyMat);
    shader.setVec3("lightPos", glm::vec3(1.0, 1.0, 2.0));
    return score;
}
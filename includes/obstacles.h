#include <learnopengl/model.h>
#include <helpers.h>
#include <queue>
#include <boundedTransform.h>

class Obstacle {
protected:
    Model *obstacle;
    glm::mat4 modelMat;
    unsigned int texture;
    int maxPositions;
    float *validY;
    queue<glm::mat4 *> waves;

public:
    Obstacle(Model *model, const char *pathToTexture, int positions, float minY, float maxY) {
        obstacle = model;
        texture = loadTexture(FileSystem::getPath(pathToTexture).c_str());
        srand(time(NULL));
        float rangeY = maxY - minY;
        maxPositions = positions;
        validY = (float *) malloc(sizeof(float) * maxPositions);
        for(int i = 0; i < maxPositions; ++i)
            validY[i] = minY + i * rangeY/(maxPositions - 1);
    }

    virtual void Draw(Shader shader) {
        shader.setMat4("model", modelMat);
        obstacle->Draw(shader);
    }

    void transformBoundingBox(glm::vec3 transform) {
        
    }

    void queueWave(Shader shader) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glm::mat4 renderMat = modelMat;
        for(int i = 0; i < maxPositions; ++i) {
            modelMat = glm::translate(renderMat, glm::vec3(0.0, validY[i], 0.0));
            Draw(shader);
        }
        modelMat = renderMat;
    }
};

class Block : public Obstacle {
public:
    Block(Model *block, const char *pathToTexture) : Obstacle(block, pathToTexture, 8, -4, 4) {
        modelMat = glm::translate(modelMat, glm::vec3(-1.0, 0.0, 0.1f));
        modelMat = glm::scale(modelMat, glm::vec3(0.25f));
    }
};

class Fence : public Obstacle {
public:
    Fence(Model *fence, const char *pathToTexture) : Obstacle(fence, pathToTexture, 8, -4.5, 3.5) {
        modelMat = glm::translate(modelMat, glm::vec3(1.0, 0.0, 0.0));
        modelMat = glm::rotate(modelMat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(0.04f, 0.25f, 0.25f));
        // printf("%f %f\n", obstacle->boundingBox[0], obstacle->boundingBox[1]);
    }

    virtual void Draw(Shader shader) {
        Obstacle::Draw(shader);
        glm::mat4 backMat = modelMat;
        modelMat = glm::translate(modelMat, glm::vec3(-3.5, 0.0, 0.0));
        Obstacle::Draw(shader);
        modelMat = backMat;
        // printf("%f %f\n", obstacle->boundingBox[0], obstacle->boundingBox[1]);
    }
};
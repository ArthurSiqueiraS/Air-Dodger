#include <boundedModel.h>
#include <helpers.h>
#include <queue>

class Obstacle {
protected:
    Model *obstacle;
    glm::mat4 modelMat;
    unsigned int texture;
    int maxPositions;
    float *validY;
    queue<glm::mat4 *> waves;
    float frequency, timer;
    glm::vec3 direction;

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
        timer = 1.5;
        frequency = 3.0;
    }

    void renderWave(Shader shader, float deltaTime) {
        timer += deltaTime;
        if(timer >= frequency) {
            queueWave(shader);
            timer = 0.0;
        }

        glBindTexture(GL_TEXTURE_2D, texture);
        queue<glm::mat4 *> waveCycle;
        while(!waves.empty()) {
            waveCycle.push(waves.front());
            waves.pop();
            glm::mat4 *waveMats = (glm::mat4 *) waveCycle.back();
            for(int i = 0; i < maxPositions; ++i) {
                glm::vec3 backUpBoundingBox[8];
                copyBoundingBox(backUpBoundingBox, obstacle->boundingBox);
                waveMats[i] = translate(obstacle, waveMats[i], direction * deltaTime);
                float offsetY = waveMats[i][3][1] - modelMat[3][1],
                      currYmin = obstacle->boundingBox[0].y, currYmax = obstacle->boundingBox[7].y;
                obstacle->updateBoundingBoxY(currYmin + offsetY, currYmax + offsetY);
                Draw(shader, waveMats[i]);
                if(frequency > 2.9) {
                    printf("[%d]\n", i);
                    print();
                }
                copyBoundingBox(obstacle->boundingBox, backUpBoundingBox);
            }
        }

        while(!waveCycle.empty()) {
            glm::mat4 * wave = waveCycle.front();
            if(wave[0][3][0] > -3)
                waves.push(wave);
            else
                free(wave);
            waveCycle.pop();
        }
    }

    virtual void Draw(Shader shader, glm::mat4 model) = 0;

    void queueWave(Shader shader) {
        if(direction.x > -19.0 && direction.z > -19.0)
            direction *= 1.1;
        if(frequency > 0.51)
            frequency -= 0.1;

        glm::mat4 *waveMats = (glm::mat4 *) malloc(sizeof(glm::mat4) * maxPositions);

        for(int i = 0; i < maxPositions; ++i)
            waveMats[i] = glm::translate(modelMat, glm::vec3(0.0, validY[i], 0.0));

        waves.push(waveMats);
    }

    void copyBoundingBox(glm::vec3 *copy, glm::vec3 *original) {
        for(int i = 0; i < 8; ++i)
            copy[i] = original[i];
    }

    void print() {
        printf("X: %f %f\n", obstacle->boundingBox[0].x, obstacle->boundingBox[7].x);
        printf("Y: %f %f\n", obstacle->boundingBox[0].y, obstacle->boundingBox[7].y);
        printf("Z: %f %f\n", obstacle->boundingBox[0].z, obstacle->boundingBox[7].z);
    }
};

class Block : public Obstacle {
public:
    Block(Model *block, const char *pathToTexture) : Obstacle(block, pathToTexture, 8, -4, 4) {
        direction = glm::vec3(-2.0, 0.0, 0.0);
        modelMat = translate(obstacle, modelMat, glm::vec3(3.0, 0.0, 0.0f));
        modelMat = scale(obstacle, modelMat, glm::vec3(0.25f));
    }

    void Draw(Shader shader, glm::mat4 model) {
        shader.setMat4("model", model);
        obstacle->Draw(shader);
    }
};

class Fence : public Obstacle {
public:
    Fence(Model *fence, const char *pathToTexture) : Obstacle(fence, pathToTexture, 8, -4.5, 3.5) {
        direction = glm::vec3(0.0, 0.0, -2.0);
        modelMat = translate(obstacle, modelMat, glm::vec3(3.0, 0.0, 0.0));
        modelMat = rotate(obstacle, modelMat, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        modelMat = scale(obstacle, modelMat, glm::vec3(0.04f, 0.25f, 0.25f));
    }

    void Draw(Shader shader, glm::mat4 model) {
        shader.setMat4("model", model);
        obstacle->Draw(shader);
        glm::mat4 frontMat = glm::translate(model, glm::vec3(-3.0, 0.0, 0.0));
        shader.setMat4("model", frontMat);
        obstacle->Draw(shader);
    }
};
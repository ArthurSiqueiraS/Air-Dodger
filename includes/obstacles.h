#include <boundedModel.h>
#include <helpers.h>
#include <queue>

typedef struct wave {
    glm::mat4 * mats;
    glm::vec3 *boundingBox;
    int *chance;
} Wave;

class Obstacle {
protected:
    Model *obstacle;
    glm::mat4 modelMat;
    unsigned int texture;
    int maxPositions;
    int maxSelf;
    int P;
    float *validY;
    queue<Wave> waves;
    float frequency, timer, collisionBiasX, collisionBiasY;
    glm::vec3 aspect;
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
        P = 4;
    }

    int renderWave(Shader shader, float deltaTime, Plane *plane, bool *availablePositions) {
        int score = 0;
        timer += deltaTime;
        if(timer >= frequency) {
            enqueueWave(availablePositions);
            score = 1;
            timer = 0.0;
        }

        glBindTexture(GL_TEXTURE_2D, texture);
        queue<Wave> waveCycle;
        while(!waves.empty()) {
            waveCycle.push(waves.front());
            waves.pop();
            Wave wave = waveCycle.back();
            for(int i = 0; i < maxPositions; ++i) {
                if(wave.chance[i] < P) {
                    wave.mats[i] = glm::translate(wave.mats[i], direction * deltaTime);
                    Draw(shader, wave.mats[i]);
                    if(collision(wave.boundingBox, plane, wave.mats[i][3][1] - modelMat[3][1])) {
                        score = handleCollision();
                    }
                }
            }
            translate(wave.boundingBox, wave.mats[0], direction * deltaTime, aspect);
        }

        while(!waveCycle.empty()) {
            Wave wave = waveCycle.front();
            if(wave.mats[0][3][0] > -3)
                waves.push(wave);
            else
                free(wave.mats);
            waveCycle.pop();
        }

        return score;
    }

protected:
    void copyBoundingBox(glm::vec3 *copy, glm::vec3 *original) {
        for(int i = 0; i < 8; ++i)
            copy[i] = original[i];
    }

private:
    virtual void Draw(Shader shader, glm::mat4 model) = 0;

    virtual void enqueueWave(bool *availablePositions) {
        if(direction.x > -15.0 && direction.z > -15.0)
            direction *= 1.1;
        if(frequency > 0.51)
            frequency -= 0.1;

        Wave wave;
        wave.boundingBox = (glm::vec3 *) malloc(sizeof(glm::vec3) * 8);
        copyBoundingBox(wave.boundingBox, obstacle->boundingBox);
        wave.mats = (glm::mat4 *) malloc(sizeof(glm::mat4) * maxPositions);
        wave.chance = (int *) malloc(sizeof(int) * maxPositions);

        int qt = 0;
        for(int i = 0; i < maxPositions; ++i) {
            wave.mats[i] = translate(wave.boundingBox, modelMat, glm::vec3(0.0, validY[i], 0.0));
            if(availablePositions[i] && qt < maxSelf) {
                wave.chance[i] = rand() % 10;
                if(wave.chance[i] < P) {
                    availablePositions[i] = false;
                    ++qt;
                }
            }
            else
                wave.chance[i] = 8;
        }

        waves.push(wave);
    }

    virtual bool collision(glm::vec3 *boundingBox, Plane *plane, float offsetY) {
        glm::vec3 planeBB[8];
        copyBoundingBox(planeBB, plane->getBoundingBox());

        float biasX = collisionBiasX, biasY = collisionBiasY, waveBias = 0.075;

        if(plane->isShrunk()) {
            biasX /= 2;
            biasY /= 2;
            waveBias = 0.04;
        }
        updateBoundingBoxX(planeBB, planeBB[0].x - biasX, planeBB[7].x + biasX);
        updateBoundingBoxY(planeBB, planeBB[0].y - biasY, planeBB[7].y + biasY);

        // if(obstacle->boundingBox[0].x > planeBB[7].x || obstacle->boundingBox[7].x < planeBB[0].x)
        //     return false;
        float obstXmin = boundingBox[0].x, obstXmax = boundingBox[7].x,
              obstYmin = boundingBox[0].y + offsetY - waveBias, obstYmax = boundingBox[7].y + offsetY - waveBias,
              planeXmin = planeBB[0].x, planeXmax = planeBB[7].x,
              planeYmin = planeBB[0].y, planeYmax = planeBB[7].y;

            if(((planeXmin >= obstXmin && planeXmin <= obstXmax) || (planeXmax >= obstXmin && planeXmax <= obstXmax))
            || ((obstXmin >= planeXmin && obstXmin <= planeXmax) || (obstXmax >= planeXmin && obstXmax <= planeXmax))) {
                if(((planeYmin >= obstYmin && planeYmin <= obstYmax) || (planeYmax >= obstYmin && planeYmax <= obstYmax))
                || ((obstYmin >= planeYmin && obstYmin <= planeYmax) || (obstYmax >= planeYmin && obstYmax <= planeYmax))) {
                    return true;
                }
            }

        return false;
    }

    virtual int handleCollision() {
        return -1;
    }
};

class Block : public Obstacle {
public:
    Block(Model *block, const char *pathToTexture) : Obstacle(block, pathToTexture, 8, -4, 4) {
        aspect = glm::vec3(0.25f);
        direction = glm::vec3(-2.0, 0.0, 0.0);
        collisionBiasX = 0.2;
        collisionBiasY = 0.0;
        modelMat = scale(obstacle->boundingBox, modelMat, aspect);
        modelMat = translate(obstacle->boundingBox, modelMat, glm::vec3(12.0, 0.0, 0.0f), aspect);
        maxSelf = 7;
        P = 6;
    }

private:
    void Draw(Shader shader, glm::mat4 model) {
        shader.setMat4("model", model);
        obstacle->Draw(shader);
    }
};

class Fence : public Obstacle {
public:
    Fence(Model *fence, const char *pathToTexture) : Obstacle(fence, pathToTexture, 8, -4.5, 3.5) {
        aspect = glm::vec3(0.04f, 0.25f, 0.25f);
        direction = glm::vec3(0.0, 0.0, -2.0);
        collisionBiasX = 0.05;
        collisionBiasY = 0.05;
        modelMat = rotate(modelMat, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        modelMat = scale(obstacle->boundingBox, modelMat, aspect);
        modelMat = translate(obstacle->boundingBox, modelMat, glm::vec3(0.0, 0.0, 12.0), aspect);
        maxSelf = 8;
        P = 2;
    }

private:
    void Draw(Shader shader, glm::mat4 model) {
        shader.setMat4("model", model);
        obstacle->Draw(shader);
        glm::mat4 frontMat = glm::translate(model, glm::vec3(-4.0, 0.0, 0.0));
        shader.setMat4("model", frontMat);
        obstacle->Draw(shader);
    }

    virtual void enqueueWave(bool *availablePositions) {
        if(direction.x > -15.0 && direction.z > -15.0)
            direction *= 1.1;
        if(frequency > 0.51)
            frequency -= 0.1;

        Wave wave;
        wave.boundingBox = (glm::vec3 *) malloc(sizeof(glm::vec3) * 8);
        copyBoundingBox(wave.boundingBox, obstacle->boundingBox);
        wave.mats = (glm::mat4 *) malloc(sizeof(glm::mat4) * maxPositions);
        wave.chance = (int *) malloc(sizeof(int) * maxPositions);

        int qt = 0;
        for(int i = 0; i < maxPositions; i += 2) {
            wave.mats[i] = translate(wave.boundingBox, modelMat, glm::vec3(0.0, validY[i], 0.0));
            wave.mats[i + 1] = translate(wave.boundingBox, modelMat, glm::vec3(0.0, validY[i + 1], 0.0));
            if(qt < maxSelf) {
                wave.chance[i] = rand() % 10;
                if(wave.chance[i] < P) {
                    availablePositions[i] = false;
                    availablePositions[i + 1] = false;
                    ++qt;
                    wave.chance[i + 1] = rand() % 14;
                }
                else {
                    wave.chance[i + 1] = 8;
                }
                if(wave.chance[i + 1] < P * 3) {
                    ++qt;
                }
            }
            else {
                wave.chance[i] = 8;
                wave.chance[i + 1] = 8;
            }
        }

        waves.push(wave);
    }

    bool collision(glm::vec3 *boundingBox, Plane *plane, float offsetY) {
        if(plane->isShrunk() || plane->isTurned()) {
            return false;
        }

        glm::vec3 planeBB[8];
        copyBoundingBox(planeBB, plane->getBoundingBox());

        float biasX = collisionBiasX, biasY = collisionBiasY;

        if(plane->isShrunk()) {
            biasX /= 2;
            biasY /= 2;
        }
        updateBoundingBoxX(planeBB, planeBB[0].x - biasX, planeBB[7].x + biasX);
        updateBoundingBoxY(planeBB, planeBB[0].y - biasY, planeBB[7].y + biasY);

        // if(obstacle->boundingBox[0].x > planeBB[7].x || obstacle->boundingBox[7].x < planeBB[0].x)
        //     return false;
        float obstXmin = boundingBox[0].z, obstXmax = boundingBox[7].z,
              obstYmin = boundingBox[0].x + offsetY, obstYmax = boundingBox[7].x + offsetY + 0.15,
              planeXmin = planeBB[0].x, planeXmax = planeBB[7].x,
              planeYmin = planeBB[0].y, planeYmax = planeBB[7].y;

            if(((planeXmin >= obstXmin && planeXmin <= obstXmax) || (planeXmax >= obstXmin && planeXmax <= obstXmax))
            || ((obstXmin >= planeXmin && obstXmin <= planeXmax) || (obstXmax >= planeXmin && obstXmax <= planeXmax))) {
                if(((planeYmin >= obstYmin && planeYmin <= obstYmax) || (planeYmax >= obstYmin && planeYmax <= obstYmax))
                || ((obstYmin >= planeYmin && obstYmin <= planeYmax) || (obstYmax >= planeYmin && obstYmax <= planeYmax))) {
                    return true;
                }
            }

        return false;
    }
};

class Shrinker : public Obstacle {
public:
    Shrinker(Model *wrench, const char *pathToTexture) : Obstacle(wrench, pathToTexture, 8, -13.5, 13.5) {
        aspect = glm::vec3(0.075f);
        direction = glm::vec3(-6.5, 0.0, 0.0);
        collisionBiasX = 0.35;
        collisionBiasY = 0.15;
        modelMat = scale(obstacle->boundingBox, modelMat, aspect);
        modelMat = translate(obstacle->boundingBox, modelMat, glm::vec3(40.0, 0.0, 0.0), aspect);
        P = 1;
        maxSelf = 1;
    }

    void Draw(Shader shader, glm::mat4 model) {
        model = rotate(model, 90.0f, glm::vec3(1.0, 0.0, 0.0));
        model = rotate(model, 10.0f, glm::vec3(0.0, 1.0, 0.0));
        shader.setMat4("model", model);
        obstacle->Draw(shader);
    }

    int handleCollision() {
        return 2;
    }
};
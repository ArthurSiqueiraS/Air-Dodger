#include <boundedModel.h>
#include <helpers.h>
#include <queue>

typedef struct wave {
    glm::mat4 * mats;
    glm::vec3 *boundingBox;
} Wave;

class Obstacle {
protected:
    Model *obstacle;
    glm::mat4 modelMat;
    unsigned int texture;
    int maxPositions;
    float *validY;
    queue<Wave> waves;
    float frequency, timer, collisionBias;
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
    }

    int renderWave(Shader shader, float deltaTime, Plane *plane) {
        int score = 0;
        timer += deltaTime;
        if(timer >= frequency) {
            if(waves.empty())
                queueWave();
            score = 150;
            timer = 0.0;
        }

        glBindTexture(GL_TEXTURE_2D, texture);
        queue<Wave> waveCycle;
        while(!waves.empty()) {
            waveCycle.push(waves.front());
            waves.pop();
            Wave wave = waveCycle.back();
            for(int i = 0; i < maxPositions; ++i) {
                glm::vec3 backUpBoundingBox[8];
                wave.mats[i] = glm::translate(wave.mats[i], direction * deltaTime);
                Draw(shader, wave.mats[i]);
                collision(wave.boundingBox, plane);
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

    virtual void Draw(Shader shader, glm::mat4 model) = 0;

    void queueWave() {
        if(direction.x > -19.0 && direction.z > -19.0)
            direction *= 1.1;
        if(frequency > 0.51)
            frequency -= 0.1;

        Wave wave;
        wave.mats = (glm::mat4 *) malloc(sizeof(glm::mat4) * maxPositions);

        for(int i = 0; i < maxPositions; ++i)
            wave.mats[i] = glm::translate(modelMat, glm::vec3(0.0, validY[i], 0.0));

        wave.boundingBox = (glm::vec3 *) malloc(sizeof(glm::vec3) * 8);
        copyBoundingBox(wave.boundingBox, obstacle->boundingBox);

        waves.push(wave);
    }

    void copyBoundingBox(glm::vec3 *copy, glm::vec3 *original) {
        for(int i = 0; i < 8; ++i)
            copy[i] = original[i];
    }

    bool collision(glm::vec3 *boundingBox, Plane *plane) {
        glm::vec3 planeBB[8];
        copyBoundingBox(planeBB, plane->getBoundingBox());
        updateBoundingBoxX(planeBB, planeBB[0].x - collisionBias, planeBB[7].x + collisionBias);

        // if(obstacle->boundingBox[0].x > planeBB[7].x || obstacle->boundingBox[7].x < planeBB[0].x)
        //     return false;
        float obstXmin = boundingBox[0].x, obstXmax = boundingBox[7].x,
              planeXmin = planeBB[0].x, planeXmax = planeBB[7].x;

        for(int i = 0; i < 8; ++i) {
            float planeX = planeBB[i].x,
                  obstX = boundingBox[i].x;
            if(
                (planeX >= obstXmin && planeX <= obstXmax) ||
                (obstX >= planeXmin && obstX <= planeXmax)
            ) {
                printf("hit [%d]\n", i);
                return true;
            }
        }

        return false;
    }

    // void print() {
    //     printf("X: %f %f\n", boundingBox[0].x, boundingBox[7].x);
    //     printf("Y: %f %f\n", boundingBox[0].y, boundingBox[7].y);
    //     printf("Z: %f %f\n", boundingBox[0].z, boundingBox[7].z);
    // }
};

class Block : public Obstacle {
public:
    Block(Model *block, const char *pathToTexture) : Obstacle(block, pathToTexture, 8, -4, 4) {
        aspect = glm::vec3(0.25f);
        direction = glm::vec3(-3.0, 0.0, 0.0);
        collisionBias = 0.2;
        modelMat = scale(obstacle->boundingBox, modelMat, aspect);
        modelMat = translate(obstacle->boundingBox, modelMat, glm::vec3(12.0, 0.0, 0.0f), aspect);
    }

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
        modelMat = rotate(modelMat, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        modelMat = scale(obstacle->boundingBox, modelMat, aspect);
        modelMat = translate(obstacle->boundingBox, modelMat, glm::vec3(0.0, 0.0, 12.0), aspect);
    }

    void Draw(Shader shader, glm::mat4 model) {
        shader.setMat4("model", model);
        obstacle->Draw(shader);
        glm::mat4 frontMat = glm::translate(model, glm::vec3(-3.0, 0.0, 0.0));
        shader.setMat4("model", frontMat);
        obstacle->Draw(shader);
    }
};

class Shrinker : public Obstacle {
public:
    Shrinker(Model *wrench, const char *pathToTexture) : Obstacle(wrench, pathToTexture, 8, -13.5, 13.5) {
        aspect = glm::vec3(0.075f);
        direction = glm::vec3(-6.5, 0.0, 0.0);
        collisionBias = 0.35;
        modelMat = scale(obstacle->boundingBox, modelMat, aspect);
        modelMat = translate(obstacle->boundingBox, modelMat, glm::vec3(40.0, 0.0, 0.0), aspect);
    }

    void Draw(Shader shader, glm::mat4 model) {
        model = rotate(model, 90.0f, glm::vec3(1.0, 0.0, 0.0));
        model = rotate(model, 10.0f, glm::vec3(0.0, 1.0, 0.0));
        shader.setMat4("model", model);
        obstacle->Draw(shader);
    }
};
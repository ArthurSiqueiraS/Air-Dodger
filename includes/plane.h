#ifndef PLANE_H
#define PLANE_H

#include <learnopengl/model.h>

class Plane {
public:;
    Plane(int SCR_WIDTH, int SCR_HEIGHT ) {
        plane = new Model(FileSystem::getPath("resources/objects/airplane/airplane.obj"));
        vLimit = 0.90;
        hLimit = SCR_WIDTH/SCR_HEIGHT - 0.1;
        moveSpeed = 2.0;
        turnSpeed = 8.0;
        resizeSpeed = 4.0;
        angle = 0.0;
        size = 0.015;
        shrinkSize = 0.007;
        aspect = size;
    }

    void Draw(Shader shader) {
        glm::mat4 renderMat = glm::scale(planeMat, glm::vec3(aspect));
        // Normalize plane angle
        renderMat = glm::rotate(renderMat, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        renderMat = glm::rotate(renderMat, glm::radians(15.0f), glm::vec3(0.0, 0.0, 1.0));
        renderMat = glm::rotate(renderMat, glm::radians(90.0f), glm::vec3(-1.0, 0.0, 0.0));
        // Turn movement animations
        renderMat = glm::translate(renderMat, glm::vec3(angle/90, 0.0, angle/22.5));
        renderMat = glm::rotate(renderMat, glm::radians(angle), glm::vec3(-1.0, 0.0, 0.0));
        renderMat = glm::rotate(renderMat, glm::radians(angle/6), glm::vec3(0.0, 0.0, 1.0));

        shader.setMat4("model", renderMat);
        plane->Draw(shader);
    }

    void move(glm::vec3 movement) {
        planeMat = glm::translate(planeMat, moveSpeed * movement);
        if(planeMat[3][0] < -hLimit)
            planeMat[3][0] = -hLimit;
        if(planeMat[3][0] > hLimit)
            planeMat[3][0] = hLimit;
        if(planeMat[3][1] < -vLimit)
            planeMat[3][1] = -vLimit;
        if(planeMat[3][1] > vLimit)
            planeMat[3][1] = vLimit;
    }

    void turn(float degrees) {
        if(angle < 90.0f)
            angle = glm::min(angle + degrees * turnSpeed, 90.0f);
    }

    void unturn(float degrees) {
        if(angle > 0.0f)
            angle = glm::max(angle - degrees * turnSpeed, 0.0f);
    }

    void shrink(float dec) {
        if(aspect > shrinkSize)
            aspect = glm::max(aspect - dec * resizeSpeed, shrinkSize);
    }

    void deShrink(float dec) {
        if(aspect < size)
            aspect = glm::min(aspect + dec * resizeSpeed, size);
    }

private:
    Model *plane;
    glm::mat4 planeMat;
    float vLimit;
    float hLimit;
    float aspect;
    float moveSpeed;
    float turnSpeed;
    float resizeSpeed;
    float angle;
    float size;
    float shrinkSize;
};
#endif
#include <boundedModel.h>

class Plane {
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

public:
    Plane() {
        plane = new Model(FileSystem::getPath("resources/objects/airplane/airplane.obj"));
        // Ortho limits
        // vLimit = 0.90;
        // hLimit = SCR_WIDTH/SCR_HEIGHT - 0.1;
        vLimit = 1.1;
        hLimit = 2.4;
        moveSpeed = 2.0;
        turnSpeed = 8.0;
        resizeSpeed = 4.0;
        angle = 0.0;
        size = 0.025;
        shrinkSize = size/2;
        aspect = size;
    }

    void print() {
        printf("Plane:\n");
        printf("X: %f %f\n", plane->boundingBox[0].x, plane->boundingBox[7].x);
        printf("Y: %f %f\n", plane->boundingBox[0].y, plane->boundingBox[7].y);
        printf("Z: %f %f\n", plane->boundingBox[0].z, plane->boundingBox[7].z);
    }

    void Draw(Shader shader) {
        glm::vec3 backUpBoundingBox[8];
        copyBoundingBox(backUpBoundingBox, plane->boundingBox);
        shader.setMat4("model", normalizePlaneMat());
        plane->Draw(shader);
        copyBoundingBox(plane->boundingBox, backUpBoundingBox);
    }

    void move(glm::vec3 movement) {
        glm::vec3 transform = moveSpeed * movement;
        float x = transform.x, y = transform.y, z = transform.z;
        if(planeMat[3][0] + x < -hLimit)
            x = -hLimit - planeMat[3][0];
        if(planeMat[3][0] + x > hLimit)
            x = hLimit - planeMat[3][0];
        if(planeMat[3][1] + y < -vLimit)
            y = -vLimit - planeMat[3][1];
        if(planeMat[3][1] + y > vLimit)
            y = vLimit - planeMat[3][1];

        transform = glm::vec3(x, y, z);
        planeMat = translate(plane, planeMat, transform);
    }

    void turn(float degrees) {
        if(angle < 90.0f) {
            angle = glm::min(angle + degrees * turnSpeed, 90.0f);
            float currentYmin = plane->boundingBox[0].y, currentYmax = plane->boundingBox[7].y,
                currentZmin = plane->boundingBox[0].z, currentZmax = plane->boundingBox[7].z,
                offset = degrees;
            plane->updateBoundingBoxY(currentYmin - offset, currentYmax + offset);
            plane->updateBoundingBoxZ(currentZmin + offset, currentZmax - offset);
        }
    }

    void unturn(float degrees) {
        if(angle > 0.0f) {
            angle = glm::max(angle - degrees * turnSpeed, 0.0f);
            float currentYmin = plane->boundingBox[0].y, currentYmax = plane->boundingBox[7].y,
                currentZmin = plane->boundingBox[0].z, currentZmax = plane->boundingBox[7].z,
                offset = degrees;

            plane->updateBoundingBoxY(currentYmin + offset, currentYmax - offset);
            plane->updateBoundingBoxZ(currentZmin - offset, currentZmax + offset);
        }
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
    glm::mat4 normalizePlaneMat() {
        glm::mat4 renderMat = scale(plane, planeMat, glm::vec3(aspect * 1.75, aspect, aspect));
        // Normalize plane angle
        renderMat = rotate(plane, renderMat, 180.0f, glm::vec3(0.0, 1.0, 0.0));
        renderMat = rotate(plane, renderMat, 15.0f, glm::vec3(0.0, 0.0, 1.0));
        renderMat = rotate(plane, renderMat, 90.0f, glm::vec3(-1.0, 0.0, 0.0));
        // Turn movement animations
        renderMat = glm::translate(renderMat, glm::vec3(angle/90, 0.0, angle/22.5));
        renderMat = rotate(plane, renderMat, angle, glm::vec3(-1.0, 0.0, 0.0));
        renderMat = rotate(plane, renderMat, angle/6, glm::vec3(0.0, 0.0, 1.0));
        return renderMat;
    }

    void copyBoundingBox(glm::vec3 *copy, glm::vec3 *original) {
        for(int i = 0; i < 8; ++i)
            copy[i] = original[i];
    }
};
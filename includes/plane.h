#include <boundedModel.h>

class Plane {
private:
    Model *plane;
    glm::mat4 planeMat;
    glm::vec3 *boundingBox, *normalized;
    float vLimit;
    float hLimit;
    float aspect;
    float moveSpeed;
    float turnSpeed;
    float resizeSpeed;
    float angle;
    float size;
    float shrinkSize;
    bool shrunk, turned;
    glm::vec3 scaleVec;

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
        shrunk = false;
        turned = false;
        aspect = size;
        scaleVec = glm::vec3(aspect * 1.75, aspect, aspect);

        boundingBox = (glm::vec3 *) malloc(sizeof(glm::vec3) * 8);
        copyBoundingBox(boundingBox, plane->boundingBox);
        normalized = (glm::vec3 *) malloc(sizeof(glm::vec3) * 8);
    }

    void Draw(Shader shader) {
        shader.setMat4("model", normalize());
        plane->Draw(shader);
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
        planeMat = glm::translate(planeMat, transform);
        translate(boundingBox, planeMat, transform/scaleVec);
    }

    bool isTurned() {
        return turned;
    }

    void turn(float degrees) {
        if(angle < 90.0f) {
            angle = glm::min(angle + degrees * turnSpeed, 90.0f);
            float currentYmin = boundingBox[0].y, currentYmax = boundingBox[7].y,
                currentZmin = boundingBox[0].z, currentZmax = boundingBox[7].z,
                bias = degrees/2;
            updateBoundingBoxY(boundingBox, currentYmin - bias, currentYmax + bias);
            updateBoundingBoxZ(boundingBox, currentZmin + bias, currentZmax - bias);
        }
        else
            turned = true;
    }

    void unturn(float degrees) {
        if(angle > 0.0f) {
            angle = glm::max(angle - degrees * turnSpeed, 0.0f);
            float currentYmin = boundingBox[0].y, currentYmax = boundingBox[7].y,
                currentZmin = boundingBox[0].z, currentZmax = boundingBox[7].z,
                bias = degrees/2;

            updateBoundingBoxY(boundingBox, currentYmin + bias, currentYmax - bias);
            updateBoundingBoxZ(boundingBox, currentZmin - bias, currentZmax + bias);
        }
        else
            turned = false;
    }

    bool isShrunk() {
        return shrunk;
    }

    void shrink(float dec) {
        if(aspect > shrinkSize)
            aspect = glm::max(aspect - dec * resizeSpeed, shrinkSize);
        else
            shrunk = true;
    }

    void deShrink(float dec) {
        if(aspect < size)
            aspect = glm::min(aspect + dec * resizeSpeed, size);
        else
            shrunk = false;
    }

    glm::vec3 *getBoundingBox() {
        normalize();
        return normalized;
    }

private:
    glm::mat4 normalize() {
        copyBoundingBox(normalized, boundingBox);
        return normalizePlaneMat(normalized);
    }

    glm::mat4 normalizePlaneMat(glm::vec3 *boundingBox) {
        glm::mat4 renderMat = rotate(planeMat, 180.0f, glm::vec3(0.0, 1.0, 0.0));
        // Normalize plane angle
        renderMat = rotate(renderMat, 15.0f, glm::vec3(0.0, 0.0, 1.0));
        renderMat = rotate(renderMat, 90.0f, glm::vec3(-1.0, 0.0, 0.0));
        // Turn movement animations
        // renderMat = translate(boundingBox, renderMat, glm::vec3(angle/90, 0.0, angle/22.5), scaleVec);

        renderMat = rotate(renderMat, angle, glm::vec3(-1.0, 0.0, 0.0));
        renderMat = rotate(renderMat, angle/6, glm::vec3(0.0, 0.0, 1.0));
        scale(boundingBox, renderMat, scaleVec);
        renderMat = scale(renderMat, glm::vec3(aspect * 1.75, aspect, aspect));

        return renderMat;
    }

    void copyBoundingBox(glm::vec3 *copy, glm::vec3 *original) {
        for(int i = 0; i < 8; ++i)
            copy[i] = original[i];
    }
};
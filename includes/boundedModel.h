#ifndef BOUNDEDMODEL_H
#define BOUNDEDMODEL_H

glm::mat4 scale(glm::vec3 *boundingBox, glm::mat4 mat, glm::vec3 vec) {
    for(int i = 0; i < 8; ++i)
        boundingBox[i] *= vec;
    return glm::scale(mat, vec);
}

glm::mat4 translate(glm::vec3 *boundingBox, glm::mat4 mat, glm::vec3 vec, glm::vec3 aspect = glm::vec3(1.0)) {
    for(int i = 0; i < 8; ++i)
        boundingBox[i] += vec * aspect;
    return glm::translate(mat, vec);
}

glm::mat4 rotate(glm::mat4 mat, float degrees, glm::vec3 vec) {
    return glm::rotate(mat, glm::radians(degrees), vec);
}

void updateBoundingBoxX(glm::vec3 *boundingBox, float Xmin, float Xmax) {
    for(int i = 0; i < 4;++i)
        boundingBox[i].x = Xmin;
    for(int i = 4; i < 8;++i)
        boundingBox[i].x = Xmax;
}

void updateBoundingBoxY(glm::vec3 *boundingBox, float Ymin, float Ymax) {
    float y = Ymin;
    for(int i = 0; i < 8; i+= 2) {
        for(int j = 0; j < 2;++j)
            boundingBox[i+ j].y = y;
        y = y == Ymin ? Ymax : Ymin;
    }
}

void updateBoundingBoxZ(glm::vec3 *boundingBox, float Zmin, float Zmax) {
    for(int i = 0; i < 8;++i)
        boundingBox[i].z = i % 2 == 0 ? Zmin : Zmax;
}

void updateBoundingBox(glm::vec3 *boundingBox, glm::vec3 coordMin, glm::vec3 coordMax) {
    updateBoundingBoxX(boundingBox, coordMin.x, coordMax.x);
    updateBoundingBoxY(boundingBox, coordMin.y, coordMax.y);
    updateBoundingBoxZ(boundingBox, coordMin.z, coordMax.z);
}
#endif
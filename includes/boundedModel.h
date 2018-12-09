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
#endif
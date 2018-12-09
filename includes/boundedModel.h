#ifndef BOUNDEDMODEL_H
#define BOUNDEDMODEL_H

glm::mat4 scale(Model *model, glm::mat4 mat, glm::vec3 vec) {
    for(int i = 0; i < 8; ++i)
        model->boundingBox[i] = model->boundingBox[i] * vec;
    return glm::scale(mat, vec);
}

glm::mat4 translate(Model *model, glm::mat4 mat, glm::vec3 vec) {
    for(int i = 0; i < 8; ++i)
        model->boundingBox[i] = model->boundingBox[i] + vec;
    return glm::translate(mat, vec);
}

glm::mat4 rotate(Model *model, glm::mat4 mat, float degrees, glm::vec3 vec) {
    return glm::rotate(mat, glm::radians(degrees), vec);
}
#endif
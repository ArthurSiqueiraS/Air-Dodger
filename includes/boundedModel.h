#ifndef BOUNDEDMODEL_H
#define BOUNDEDMODEL_H

void transformBoudingBox(Model *model, glm::vec3 transform) {
    for(int i = 0; i < 8; ++i)
        model->boundingBox[i] = model->boundingBox[i] * transform;
}

glm::mat4 scale(Model *model, glm::mat4 mat, glm::vec3 vec) {
    transformBoudingBox(model, vec);
    return glm::scale(mat, vec);
}
#endif
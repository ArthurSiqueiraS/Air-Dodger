#include <models.h>
#include <irrklang/irrKlang.h>

irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();

GLFWwindow* window;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool UPpress = false;
bool DOWNpress = false;
bool LEFTpress = false;
bool RIGHTpress = false;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Plane controls
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        plane->move(glm::vec3(0.0, deltaTime, 0.0));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        plane->move(glm::vec3(0.0, -deltaTime, 0.0));
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        plane->move(glm::vec3(-deltaTime, 0.0, 0.0));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        plane->move(glm::vec3(deltaTime, 0.0, 0.0));

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        plane->turn(deltaTime * 90.0f);
    else
        plane->unturn(deltaTime * 90.0f);

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        plane->shrink(deltaTime * 0.01);
    else
        plane->deShrink(deltaTime * 0.01);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

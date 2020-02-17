#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <string>

#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>

#ifdef _WIN32
    #include <windows.h>
#endif

#include <GL/gl.h>

// forward declaration
class Detector;
class Skybox;

/**
 * @brief The Drawable class manages drawable objects
 *
 * Hint: This is an abstract class
 */
class Drawable{

public:

    Drawable(std::string name = "UNNAMED");

    /**
     * @brief init initializes the object
     *
     * The shaders are loaded and the object gets created
     */
    virtual void init();

    /**
     * @brief recreate Recreates the object
     *
     * This should be called, whenever the structure of an object changes
     *
     * Hint: Do not call this, when the position is updated)
     */
    virtual void recreate();

    /**
     * @brief draw Draws the objects
     * @param projection_matrix The current projection matrix
     *
     * The method draws the objects on the current gl context.
     *
     * Hint: This method is currently pure virtual. You can
     * implement it in the drawable.cpp to avoid redundant
     * code.
     */
    virtual void draw(glm::mat4 projection_matrix) const = 0;

    /**
     * @brief update Updates the object's position, rotation etc.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     * @param modelViewMatrix the mode view matrix of the parent object
     *
     * Hint: use this method to update the _modelViewMatrix
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) = 0;

protected:

    std::string _name;  /**< The name of the object */

    GLuint _program;            /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;  /**< The vertex array object containing the vertices */
    glm::mat4 _modelViewMatrix; /**< The model view matrix to get the object into model view space */

    GLuint _frameBufferObject;

    /**
     * @brief initShader Initializes the shader.
     *
     * The function loads a vertex and a fragment shader into
     * the _program.
     */
    virtual void initShader();

    /**
     * @brief loadShaderFile Loads a shader from a given path
     * @param path The path to the shader
     * @return The content of the shader file
     *
     * Hint: You can use the Qt Resource System for the path
     * (e.g. ":/shader/shader.fs.glsl")
     */
    virtual std::string loadShaderFile(std::string path) const;

    /**
     * @brief loadTexture loads a texture from a given path
     * @param path the path to the texture
     * @return the GLuint pointing to the texture
     *
     * Hint: You can use the Qt Resource System for the path
     * (e.g. ":/res/images/earth.bmp")
     */
    virtual GLuint loadTexture(std::string path);

    /**
     * @brief getVertexShader Getter for the vertex shader
     * @return the vertex shader as string
     *
     * This method must return the shader as string,
     * it must not return the path to the shader.
     *
     * Hint: use loadShaderFile(std::string path)
     */
    virtual std::string getVertexShader() const = 0;

    /**
     * @brief getFragmentShader Getter for the fragment shader
     * @return the fragment shader as string
     *
     * This method must return the shader as string,
     * it must not return the path to the shader.
     *
     * Hint: use loadShaderFile(std::string path)
     */
    virtual std::string getFragmentShader() const = 0;

    /**
     * @brief createObject Creates the object.
     *
     * Hint: This method should fill the _vertexArrayObject
     * with data.
     */
    virtual void createObject() = 0;

};


#endif // DRAWABLE_H


#ifndef GROUND_H
#define GROUND_H

#include "objects/drawable.h"
#include "image/image.h"
#include <memory>
#include <vector>
#include <stack>
#include <glm/vec3.hpp>

#include "sfcenter.h"

class Ground : public Drawable
{
public:
    Ground(std::string name = "GROUND",
           std::string textureLocation = ":/res/images/stars.bmp"
           );

    /**
     * @see Drawable::init()
     */
    virtual void init() override;

    /**
     * @see Drawable::draw(glm::mat4)
     */
    virtual void draw(glm::mat4 projection_matrix) const override;

    /**
     * @see Drawable::update(float, glm::mat4)
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;



    std::vector<unsigned int> indices;
    std::vector<glm::vec3> positions;

    virtual void setLights(std::shared_ptr<ShowerFrontCenter> front);

protected:


    /**
     * @see Drawable::getVertexShader()
     */
    virtual std::string getVertexShader() const override;

    /**
     * @see Drawable::getFragmentShader()
     */
    virtual std::string getFragmentShader() const override;

    /**
     * @see Drawable::createObject()
     */
    virtual void createObject() override;

    /**
     * @brief loadTexture loads the textures for the skybox
     */
    virtual void loadTexture();

    std::string _textureLocation;
    GLuint textureID;

    std::shared_ptr<ShowerFrontCenter> _showerFront;

    //everything needed for shadows
    GLuint depthMapFBO;
    GLuint depthMap;

    void loadFBO();
};

#endif // GROUND_H

#ifndef SHOWERFRONTCENTER_H
#define SHOWERFRONTCENTER_H

#include "objects/drawable.h"

#include <memory>
#include <vector>
#include <glm/vec3.hpp>


class ShowerFrontCenter : public Drawable
{
public:
    ShowerFrontCenter(std::string name="UNNAMED SHOWERFRONTCENTER",
                      glm::vec3 axis = glm::vec3(0.0f,-1.0f,0.0f),
                      glm::vec3 start = glm::vec3(0.0f,10.0f,0.0f));

    /**
     * @see Drawable::init()
     */
    virtual void init() override;

    /**
     * @see Drawable::recreate()
     */
    virtual void recreate() override;

    /**
     * @see Drawable::draw(glm::mat4)
     */
    virtual void draw(glm::mat4 projection_matrix) const override;

    /**
     * @see Drawable::update(float, glm::mat4)
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * @brief getPosition for lighting
     */
    virtual glm::vec3 getPosition();
    /**
     * @brief getPosition for camera focus point
     */
    virtual glm::vec3 getPositionCam();



protected:

    glm::vec3 _color;

    glm::vec3 _pos;
    std::vector<glm::vec3> _position;

    glm::vec3 _showerAxis;
    glm::vec3 _startPos;

    /**
     * @see Airshower::colorToShader()
     */
    virtual void colorToShader();

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


};

#endif // SHOWERFRONTCENTER_H

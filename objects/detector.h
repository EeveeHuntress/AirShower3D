#ifndef DETECTOR_H
#define DETECTOR_H

#include "objects/drawable.h"

#include <memory>
#include <vector>

#include <stack>
#include <glm/vec3.hpp>

#include "sfcenter.h"

#include "solarpanel.h"

class Detector : public Drawable
{
public:
    Detector(std::string name="UNNAMED DETECTOR"
            );

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
     * @brief change light source
     */
    virtual void setLights(std::shared_ptr<ShowerFrontCenter> front);



    ~Detector();


protected:

    float _radius;              /**< radius of the detector tank*/

    int _triangles;             /**< number of triangles to be drawn*/

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<glm::vec2> texcoords;


    GLuint textureID;

    std::vector<SolarPanel> _panel;

    std::shared_ptr<ShowerFrontCenter> _showerFront;

    /**
     * @see Drawable::createObject()
     */
    virtual void createObject() override;

    /**
     * @see Drawable::getVertexShader()
     */
    virtual std::string getVertexShader() const override;

    /**
     * @see Drawable::getFragmentShader()
     */
    virtual std::string getFragmentShader() const override;

    /**
     * @see Drawable::loadTexture()
     */
    virtual void loadTexture();

};



#endif // DETECTOR_H

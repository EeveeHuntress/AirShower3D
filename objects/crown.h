#ifndef CROWN_H
#define CROWN_H

#include "objects/drawable.h"
#include "image/image.h"

#include <glm/vec3.hpp>

#include "sfcenter.h"

class Detector;


class Crown
{
public:
    Crown(std::string name = "Crown");

    // initialize object
    virtual void init();

    // draw object
    virtual void draw(glm::mat4 projection_matrix);

    // update with modelviewmatrix and transformations
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix);

    // recreate object when parameters changed
    virtual void recreate();


    // set lighting source
    virtual void setLights(glm::vec3 front);


protected:

    int _levels;
    int _maxLevels;
    float _distance;
    int _number;
    float _radius;

    std::vector<Detector> _detectors;       // detectors in the crowns

    std::vector<glm::vec3> _crownPositions; // positions of all detectors in crowns

    glm::vec3 _showerFront;    // shower front for lighting calculation

    glm::mat4 _modelViewMatrix; /**< The model view matrix to get the object into model view space */

    /**
     * @see Drawable::createObject()
     */
    virtual void createObject();

};

#endif // CROWN_H

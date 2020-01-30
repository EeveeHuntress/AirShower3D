#ifndef AIRSHOWER_H
#define AIRSHOWER_H

#include "objects/drawable.h"

#include <memory>
#include <vector>
#include <glm/vec3.hpp>


class Airshower : public Drawable
{
public:
    Airshower(std::string name="UNNAMED AIRSHOWER",
              std::string data_path="",
              std::string type="",
              glm::vec3 axis= glm::vec3(0.0f,0.0f,-1.0f)
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
     * @brief getMaxY
     * @return maximal y-coord of airshower
     */
    virtual float getMaxY();

    /**
     * @brief getMinY
     * @return minimal y-coord of airshower
     */
    virtual float getMinY();

    /**
     * change value of _offsetY
     */
    virtual void setOffsetY(float newY);

    /**
     * @brief getMaxTime
     * @return maximal timestamp value of shower file
     */
    virtual float getMaxTime();

    /**
     * @brief setMaxTime
     */
    virtual void setMaxTime(float newTime);

    /**
     * change value of _maxTime
     */
//    virtual float setMaxTime();

    /**
     * @brief getShowerFront
     * @return position of shower front
     */
    virtual glm::vec3 getShowerFront();


//    ~Airshower();


protected:

    std::string _dataPath;
    std::string _type;              /** hadrons(hd), electromagnatic(em) and muons(mu) */
    glm::vec3 _axis;                /** shower axis is input when simulating the data*/

    glm::vec3 _showerFront;

    glm::vec3 _max;
    glm::vec3 _min;
    float _maxTime;
    float _offsetY;

    float _sizeFactor;

    glm::vec3 _color;

    int _lines;             /**< number of lines to be drawn*/

    float _time;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> positionsToShader;

    std::vector<float> timestamps;


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
     * creates complete shower
     */
    virtual void createTestShower();

    /**
     * reads shower data from file
     */
    virtual void readFromFile();

    /**
     * writes color of tracks to shader
     */
    virtual void colorToShader();


};



#endif // AIRSHOWER_H

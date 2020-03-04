#include <GL/glew.h>
#include "airshower.h"


#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <stack>

#include <QFile>
#include <QTextStream>

#include <fstream>
#include <string>
#include <sstream>
#include <iterator>

#include <iomanip>

#include "glbase/gltool.hpp"

#include "gui/config.h"
#include "gui/mainwindow.hpp"

Airshower::Airshower(std::string name, std::string data_path, std::string type, glm::vec3 axis):
    Drawable(name)
{
    _dataPath = data_path;
    _type = type;
    _axis = axis;

    _max = glm::vec3(0.0f);
    _maxTime = 0.0f;
    _min = glm::vec3(0.0f);
    _offsetY = 0.0f;

    //data is measured in centimeter
    //1.0f here -> 1km
    _sizeFactor=1.0f/100000.0f;
}


void Airshower::init()
{
    Airshower::colorToShader();
    Airshower::readFromFile();
    if(positions.size()==0)
       Airshower::createTestShower();
    _showerFront = glm::vec3(0.0f,Airshower::getMaxY(),0.0f);
//    _min = _showerFront;

//    Drawable::init();
    Drawable::initShader();
//    createObject(); //do this later
}

//get color of shower
void Airshower::colorToShader()
{
    _color=glm::vec3(0.0f,0.0f,0.0f);
    if(_type.compare("hd")==0)
    {
        _color=glm::vec3(0.0f,0.0f,1.0f);
    }
    else if(_type.compare("mu")==0)
    {
        _color = (glm::vec3(0.0f,1.0f,0.0f));
    }
    else
    {
        _color = (glm::vec3(1.0f,0.0f,0.0f));
    }
}

void Airshower::recreate()
{
    Drawable::recreate();
    glFlush();
    createObject();
}



void Airshower::draw(glm::mat4 projection_matrix) const
{
    if(_program == 0){
        std::cerr << "Airshower" << _name << "not initialized. Call init() first." << std::endl;
        return;
    }

    // Load program
    glUseProgram(_program);

    // bin vertex array object
    glBindVertexArray(_vertexArrayObject);

    // set parameter
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    // set color in shader
    int colLocation = glGetUniformLocation(_program, "col");
    glUniform3fv(colLocation, 1 ,glm::value_ptr(_color));

    // call draw
    if(!Config::onlyShowerFront)
     glDrawArrays(GL_LINES,0,positionsToShader.size());
    else
     glDrawArrays(GL_POINTS,0,positionsToShader.size());


    // unbin vertex array object
    glBindVertexArray(0);

    // check for errors
    VERIFY(CG::checkError());
}

void Airshower::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    bool changed = false;
    if(Config::isPlaying)
    {
        Config::time += (/*elapsedTimeMs**/Config::animationSpeed)*Config::maxTime;
        Config::pauseTime=Config::time;
        changed = true;
    }
    else
    {
        Config::time=Config::pauseTime;
        changed = false;
    }

    if(Config::loop)
    {
        while(Config::time>1.0f)
        {
            Config::time -= 1.0f;
            changed = true;
        }
    }
    else
    {
        if(Config::time>1.0f)
        {
            Config::time = 1.0f;
            changed=false;
        }
    }

    if(changed)
        Airshower::recreate();

    _modelViewMatrix = modelViewMatrix;

//    std::cout << "Time: " << Config::time << std::endl;
//    std::cout << "Max Time: " << Config::maxTime << std::endl;
//    std::cout << "Pause Time: " << Config::pauseTime << std::endl;
}


void Airshower::createObject()
{
    positionsToShader.clear();
    timestampsToShader.clear();

    //get current time from Config
    float testTime = Config::time;
    glm::vec3 offset = glm::vec3(0.0f, _offsetY*_sizeFactor, 0.0f);

    for(int i=0; i<timestamps.size(); i++)
    {
        timestampsToShader.push_back(timestamps.at(i)/Config::maxTime);
    }


//    std::cout << "Time: " << timestamps.at(3) << std::endl;
//    std::cout << "TimeNorm: " << timestampsToShader.at(3) << std::endl;

    for(int i=0; i<positions.size()-1; i+=2)
    {
        //tracks that are fully in the timeframe
        if(timestampsToShader.at(i+1)<=testTime && !Config::onlyShowerFront)
        {
            glm::vec3 start = positions.at(i)-offset;
            glm::vec3 end = positions.at(i+1)-offset;
            positionsToShader.push_back(start);
            positionsToShader.push_back(end);
        }
        //linear interpolation of track that is partially in timeframe
        else if(timestampsToShader.at(i)<testTime && timestampsToShader.at(i+1)>testTime)
        {

            glm::vec3 newTrackStart;
            float lambda = (testTime-timestampsToShader.at(i))/(timestampsToShader.at(i+1)-timestampsToShader.at(i));

            newTrackStart = positions.at(i);

            glm::vec3 newTrackEnd = positions.at(i) + lambda*(positions.at(i+1)-positions.at(i));

            if(!Config::onlyShowerFront)
               positionsToShader.push_back(newTrackStart-offset);
            positionsToShader.push_back(newTrackEnd-offset);
        }
    }



    // Set up a vertex array object for the geometry
    if(_vertexArrayObject == 0)
        glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    // fill vertex array object with data

    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positionsToShader.size() * 3 * sizeof(float), positionsToShader.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // unbind vertex array object
    glBindVertexArray(0);
    // delete buffers (the data is stored in the vertex array object)
    glDeleteBuffers(1, &position_buffer);

    // check for errors
    VERIFY(CG::checkError());
}


void Airshower::createTestShower()
{
    positions.clear();
    timestampsToShader.clear();

    //Test Shower
    positions.push_back(glm::vec3(0.0f,15.0f,3.0f));
    timestampsToShader.push_back(0.0f);
    positions.push_back(glm::vec3(0.0f,5.0f,-3.0f));
    timestampsToShader.push_back(0.19f);

    positions.push_back(glm::vec3(0.0f,5.0f,-3.0f));
    timestampsToShader.push_back(0.19f);
    positions.push_back(glm::vec3(0.0f,4.0f,-2.9f));
    timestampsToShader.push_back(0.2f);

    positions.push_back(glm::vec3(0.0f,4.0f,-2.9f));
    timestampsToShader.push_back(0.2f);
    positions.push_back(glm::vec3(0.0f,2.0f,-2.0f));
    timestampsToShader.push_back(0.6f);

    positions.push_back(glm::vec3(0.0f,4.0f,-2.9f));
    timestampsToShader.push_back(0.2f);
    positions.push_back(glm::vec3(1.0f,3.0f,-2.5f));
    timestampsToShader.push_back(0.4f);

    positions.push_back(glm::vec3(0.0f,2.0f,-2.0f));
    timestampsToShader.push_back(0.6f);
    positions.push_back(glm::vec3(0.0f,0.0f,-1.0f));
    timestampsToShader.push_back(1.0f);

    positions.push_back(glm::vec3(0.0f,2.0f,-2.0f));
    timestampsToShader.push_back(0.6f);
    positions.push_back(glm::vec3(0.0f,0.0f,-2.0f));
    timestampsToShader.push_back(1.0f);

    positions.push_back(glm::vec3(1.0f,3.0f,-2.5f));
    timestampsToShader.push_back(0.4f);
    positions.push_back(glm::vec3(2.0f,0.0f,0.0f));
    timestampsToShader.push_back(1.0f);

}

void Airshower::readFromFile()
{
    positions.clear();
    timestamps.clear();

    bool first = true;

    //read file
    QFile myfile(_dataPath.c_str());
    QTextStream in(&myfile);

    if (myfile.open(QFile::ReadOnly | QFile::Text))
    {
        //converted data
        float xstart,ystart,zstart,tstart, xend,yend,zend,tend, trash;

        //write data to attributes
        while(!myfile.atEnd()/* && testCounter<880210*/)
        {
            in >> trash >> trash >> xstart >> ystart >> zstart >> tstart >> xend >> yend >> zend >> tend /*>> trash*/;

            if(!(first && _type.compare("hd") == 0))
            {

            if(_maxTime<tend)
                _maxTime = tend;

            //find min and max points
            if(_min.y>zend)
              _min = glm::vec3(xend,zend,yend);
            if(_max.y<zstart)
              _max = glm::vec3(xstart,zstart,ystart);

            //create new track
            positions.push_back(glm::vec3(xstart,zstart,ystart)*_sizeFactor);
            timestamps.push_back(tstart);
            positions.push_back(glm::vec3(xend,zend,yend)*_sizeFactor);
            timestamps.push_back(tend);
            }
            else
            {
                std::cout << "Test" << std::endl;
                first = false;
            }
        }

      myfile.close();
    }
    else
    {
        std::cout << "Could not open file " << _dataPath << std::endl;
        std::cout << "Creating Testshower..." << std::endl;
    }
}


std::string Airshower::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/airshower.vs.glsl");
}

std::string Airshower::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/airshower.fs.glsl");
}

glm::vec3 Airshower::getShowerAxis()
{
    return this->_axis;
}


glm::vec3 Airshower::getShowerFront()
{
    return this->_showerFront;
}

float Airshower::getMaxY()
{
    return this->_max.y;
}

float Airshower::getMinY()
{
    return this->_min.y;
}

float Airshower::getMaxTime()
{
    return this->_maxTime;
}

void Airshower::setMaxTime(float newTime)
{
    _maxTime=newTime;
}

void Airshower::setOffsetY(float newY)
{
    _offsetY = newY;
}

#include <GL/glew.h>
#include "sfcenter.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <stack>

#include <iomanip>

#include "glbase/gltool.hpp"

#include "gui/config.h"
#include "gui/mainwindow.hpp"


ShowerFrontCenter::ShowerFrontCenter(std::string name, glm::vec3 axis, glm::vec3 start):
    Drawable(name)
{
    _startPos = start;
    _showerAxis = axis;
}

void ShowerFrontCenter::init()
{
    ShowerFrontCenter::colorToShader();
    _pos = _startPos;
    Drawable::init();
}


void ShowerFrontCenter::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    Config::time += (elapsedTimeMs*Config::animationSpeed)/100000.0f;


    if(Config::loop)
    {
        while(Config::time>1.0)
        {
            Config::time -= 1.0f;
        }
    }
    else
    {
        if(Config::time>1.0)
        {
            Config::time = 1.0f;
        }
    }

    glm::vec3 end = (_startPos.y/_showerAxis.y)*glm::vec3(_showerAxis.x, 0, _showerAxis.z);
    _pos = _startPos + Config::time * (end - _startPos);

    _modelViewMatrix = modelViewMatrix;
    ShowerFrontCenter::recreate();
}

void ShowerFrontCenter::recreate()
{
    Drawable::recreate();
    glFlush();
    createObject();
}

void ShowerFrontCenter::colorToShader()
{
    _color = glm::vec3(1.0f);
}

void ShowerFrontCenter::draw(glm::mat4 projection_matrix) const
{
    if(_program == 0){
        std::cerr << "Showerfrontcenter" << _name << "not initialized. Call init() first." << std::endl;
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
    glDrawArrays(GL_POINTS,0,_position.size());

    // unbin vertex array object
    glBindVertexArray(0);

    // check for errors
    VERIFY(CG::checkError());
}


void ShowerFrontCenter::createObject()
{
    _position.clear();

    _position.push_back(_pos);

    // Set up a vertex array object for the geometry
    if(_vertexArrayObject == 0)
        glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    // fill vertex array object with data

    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, _position.size() * 3 * sizeof(float), _position.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // unbind vertex array object
    glBindVertexArray(0);
    // delete buffers (the data is stored in the vertex array object)
    glDeleteBuffers(1, &position_buffer);

    // check for errors
    VERIFY(CG::checkError());
}

std::string ShowerFrontCenter::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/airshower.vs.glsl");
}

std::string ShowerFrontCenter::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/airshower.fs.glsl");
}

glm::vec3 ShowerFrontCenter::getPosition()
{
//    return glm::vec3(_modelViewMatrix* glm::vec4(_pos,1));
    return _pos;
}

glm::vec3 ShowerFrontCenter::getPositionCam()
{
    return _pos;
}

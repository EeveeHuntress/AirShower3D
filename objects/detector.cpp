#include <GL/glew.h>
#include "detector.h"


#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <stack>

#include "glbase/gltool.hpp"

#include "gui/config.h"

#include "solarpanel.h"

Detector::Detector(std::string name):
    Drawable(name),
    _radius(Config::radius)
{
    Config::subdivCount=48;
}


void Detector::init()
{
    Drawable::init();


    _panel.push_back(SolarPanel(_name));
    _panel.back().init();
}

void Detector::recreate()
{
    //Drawable::recreate();
    glFlush();
    createObject();

    //_panel.back().recreate();
}



void Detector::draw(glm::mat4 projection_matrix) const
{
    if(_program == 0){
        std::cerr << "Detector" << _name << "not initialized. Call init() first." << std::endl;
        return;
    }

    // Load program
    glUseProgram(_program);

    // bin vertex array object
    glBindVertexArray(_vertexArrayObject);

    // set parameter
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    //lighting values

    //shower front

    glUniform3fv(glGetUniformLocation(_program, "showerPosition"),1, glm::value_ptr(_showerFront->getPosition()));

    VERIFY(CG::checkError());

    glm::vec3 La(0.6f);
    glUniform3fv(glGetUniformLocation(_program, "La"), 1, glm::value_ptr(La));
    glm::vec3 Ls(1.0, 1.0, 1.0);
    glUniform3fv(glGetUniformLocation(_program, "Ls"), 1, glm::value_ptr(Ls));
    glm::vec3 Ld(0.6f);
    glUniform3fv(glGetUniformLocation(_program, "Ld"), 1, glm::value_ptr(Ld));
    float shininess = 3.f;
    glUniform1f(glGetUniformLocation(_program, "shininess"), shininess);
    glm::vec3 kd(0.7f);
    glUniform3fv(glGetUniformLocation(_program, "kd"), 1, glm::value_ptr(kd));
    glm::vec3 ks(.3f);
    glUniform3fv(glGetUniformLocation(_program, "ks"), 1, glm::value_ptr(ks));
    glm::vec3 ka(0.3f);
    glUniform3fv(glGetUniformLocation(_program, "ka"), 1, glm::value_ptr(ka));

    // call draw
    glDrawElements(GL_TRIANGLES, 3*_triangles, GL_UNSIGNED_INT, 0);

    // unbin vertex array object
    glBindVertexArray(0);


    // check for errors
    VERIFY(CG::checkError());


    _panel.back().draw(projection_matrix);
}

void Detector::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    std::stack<glm::mat4> modelview_stack;

    modelview_stack.push(modelViewMatrix);

        // rotate around y-axis
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(0.0f), glm::vec3(0,1,0));
        _modelViewMatrix = glm::mat4(modelview_stack.top());

    modelview_stack.pop();

    _panel.back().update(elapsedTimeMs, _modelViewMatrix);
}

void Detector::setLights(std::shared_ptr<ShowerFrontCenter> front)
{
    _showerFront=front;
    _panel.back().setLights(front);
}

void Detector::createObject(){


    positions.clear();
    indices.clear();
    normals.clear();

    int subdivs = Config::subdivCount;
    float radius = Config::radius;

    _triangles = 4*subdivs ; //number of triangles to be drawn

    float hight = 1.2/1.8 * radius;
    const int Nphis = subdivs;

    unsigned int indexCounter=0;

    glm::mat3 rotx = glm::mat3(glm::vec3(1,0,0),glm::vec3(0,0,1),glm::vec3(0,-1,0));


    for(int iphi = 0; iphi < Nphis; iphi++)
    {
        float phi1 = iphi * (2* M_PI)/Nphis ; //left
        float phi2 = (iphi+1.) * (2* M_PI)/Nphis ; //right

        float x1 = radius * cos(phi1);
        float y1 = radius * sin(phi1);
        float z1 = 0.0f;

        float x2 = radius * cos(phi2);
        float y2 = radius * sin(phi2);
        float z2 = 0.0f;

        float x3 = radius * cos(phi1);
        float y3 = radius * sin(phi1);
        float z3 = hight;

        float x4 = radius * cos(phi2);
        float y4 = radius * sin(phi2);
        float z4 = hight;

        //middle of top
        positions.push_back(glm::vec3(0.0f,0.0f,0.0f)*rotx); //0+indexCounter
        normals.push_back(glm::normalize(glm::vec3(0,-1,0)));
        //middle of bottom
        positions.push_back(glm::vec3(0.0f,0.0f,hight)*rotx);//1+indexCounter
        normals.push_back(glm::normalize(glm::vec3(0,1,0)));

        //bottom left for floor
        positions.push_back(glm::vec3(x1,y1,z1)*rotx);//2+indexCounter
        normals.push_back(glm::normalize(glm::vec3(0,-1,0)));
        //bottom right for floor
        positions.push_back(glm::vec3(x2,y2,z2)*rotx);//3+indexCounter
        normals.push_back(glm::normalize(glm::vec3(0,-1,0)));
        //top left for ceiling
        positions.push_back(glm::vec3(x3,y3,z3)*rotx);//4+indexCounter
        normals.push_back(glm::normalize(glm::vec3(0,1,0)));
        //top right for ceiling
        positions.push_back(glm::vec3(x4,y4,z4)*rotx);//5+indexCounter
        normals.push_back(glm::normalize(glm::vec3(0,1,0)));

        //bottom left for mantle
        positions.push_back(glm::vec3(x1,y1,z1)*rotx);//6+indexCounter
        normals.push_back(glm::normalize(positions.at(indexCounter+6)));
        //bottom right for mantle
        positions.push_back(glm::vec3(x2,y2,z2)*rotx);//7+indexCounter
        normals.push_back(glm::normalize(positions.at(indexCounter+7)));
        //top left for mantle
        positions.push_back(glm::vec3(x3,y3,z3)*rotx);//8+indexCounter
        normals.push_back(glm::normalize(positions.at(indexCounter+8)));
        //top right for mantle
        positions.push_back(glm::vec3(x4,y4,z4)*rotx);//9+indexCounter
        normals.push_back(glm::normalize(positions.at(indexCounter+9)));


        //bottom
        indices.push_back(indexCounter);
        indices.push_back(indexCounter+2);
        indices.push_back(indexCounter+3);

        //face of mantle
        indices.push_back(indexCounter+7);
        indices.push_back(indexCounter+6);
        indices.push_back(indexCounter+8);

        indices.push_back(indexCounter+7);
        indices.push_back(indexCounter+8);
        indices.push_back(indexCounter+9);

        //top
        indices.push_back(indexCounter+1);
        indices.push_back(indexCounter+5);
        indices.push_back(indexCounter+4);

        indexCounter+=10;

    }

    // Set up a vertex array object for the geometry
    if(_vertexArrayObject == 0)
        glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    // fill vertex array object with data
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Hint: the texture coordinates buffer is missing

    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    GLuint normal_buffer;
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(1);

    // unbind vertex array object
    glBindVertexArray(0);
    // delete buffers (the data is stored in the vertex array object)
    glDeleteBuffers(1, &position_buffer);
    glDeleteBuffers(1, &index_buffer);


    // check for errors
    VERIFY(CG::checkError());
}


std::string Detector::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/detector.vs.glsl");
}

std::string Detector::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/detector.fs.glsl");
}

Detector::~Detector(){
}

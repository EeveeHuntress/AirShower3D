#include <GL/glew.h>

#include "solarpanel.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <stack>

#include <QFile>
#include <QTextStream>

#include <QKeyEvent>

#include "glbase/gltool.hpp"
#include "image/image.h"
#include "glbase/texload.hpp"

#include "gui/config.h"

#include "sfcenter.h"


SolarPanel::SolarPanel(std::string name): Drawable(name)
{
    _scale=1.0f;

    _longside = Config::radius * 0.75f ;
    _shortside = _longside * 0.5f;
}

void SolarPanel::init()
{

    Drawable::init();

    loadTexture();
}

void SolarPanel::draw(glm::mat4 projection_matrix) const
{

    // Load program
    glUseProgram(_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textureID);

    // bin vertex array object
    glBindVertexArray(_vertexArrayObject);

    // set parameter
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));
    glUniform1i(glGetUniformLocation(_program, "textur"), 0);


    //lighting values

    //shower front

    glUniform3fv(glGetUniformLocation(_program, "showerPosition"),1, glm::value_ptr(_showerFront));

    VERIFY(CG::checkError());

    glm::vec3 La(0.6f);
    glUniform3fv(glGetUniformLocation(_program, "La"), 1, glm::value_ptr(La));
    glm::vec3 Ls(1.0, 1.0, 1.0);
    glUniform3fv(glGetUniformLocation(_program, "Ls"), 1, glm::value_ptr(Ls));
    glm::vec3 Ld(1.0f);
    glUniform3fv(glGetUniformLocation(_program, "Ld"), 1, glm::value_ptr(Ld));
    float shininess = 10.f;
    glUniform1f(glGetUniformLocation(_program, "shininess"), shininess);
    glm::vec3 kd(1.0f);
    glUniform3fv(glGetUniformLocation(_program, "kd"), 1, glm::value_ptr(kd));
    glm::vec3 ks(0.5f);
    glUniform3fv(glGetUniformLocation(_program, "ks"), 1, glm::value_ptr(ks));
    glm::vec3 ka(0.3f);
    glUniform3fv(glGetUniformLocation(_program, "ka"), 1, glm::value_ptr(ka));


    // call draw
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // unbin vertex array object
    glBindVertexArray(0);

    // check for errors
    VERIFY(CG::checkError());

}

void SolarPanel::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    _modelViewMatrix = modelViewMatrix;
}

void SolarPanel::recreate()
{
    //_scale=1.0f*Config::radius;
    Drawable::recreate();
    glFlush();
    createObject();
}

void SolarPanel::setLights(glm::vec3 front)
{
    _showerFront=front;
}

std::string SolarPanel::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/solarpanel.vs.glsl");
}

std::string SolarPanel::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/solarpanel.fs.glsl");
}


void SolarPanel::loadTexture()
{
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    Image image(":/res/images/solar.png");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(),image.getHeight(),0,GL_RGBA, GL_UNSIGNED_BYTE, image.getData());


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_2D);

    VERIFY(CG::checkError());
}

void SolarPanel::createObject()
{
    std::vector<glm::vec2> texCoordOptions;
    std::vector<glm::vec2> texCoords;


    _longside = Config::radius * 0.75f ;
    _shortside = _longside * 0.5f;

    glm::vec3 translate = glm::vec3(0,0,Config::radius/1.5f);

    positions.clear();
    indices.clear();

    float radius = Config::radius;
    float hight = 1.2/1.8 * radius;

    float hChange = sin(glm::radians(55.0f))* _shortside;
    float zChange = cos(glm::radians(55.0f))* _shortside;

    float newHight = hight + hChange;

    positions.push_back(glm::vec3(_longside/2.0f,hight,0)+translate);    //0
    positions.push_back(glm::vec3(-_longside/2.0f,hight,0)+translate);   //1
    positions.push_back(glm::vec3(_longside/2.0f,newHight,-zChange)+translate);   //2
    positions.push_back(glm::vec3(-_longside/2.0f,newHight,-zChange)+translate);  //3

    glm::vec3 normal = glm::normalize(glm::cross(positions.at(2)-positions.at(0),positions.at(1)-positions.at(0)));

    normals.push_back(normal);
    normals.push_back(normal);
    normals.push_back(normal);
    normals.push_back(normal);

//    std::cout << "Test Position: x=" << positions.at(0).x << ", y=" << positions.at(0).y << ", z=" << positions.at(0).z << std::endl;

    texCoords.push_back(glm::vec2(1,0.5f));
    texCoords.push_back(glm::vec2(0,0.5f));
    texCoords.push_back(glm::vec2(1,0));
    texCoords.push_back(glm::vec2(0,0));

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(3);

    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(2);

    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(1);

    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);



        // Set up a vertex array object for the geometry
        if(_vertexArrayObject == 0)
            glGenVertexArrays(1, &_vertexArrayObject);
        glBindVertexArray(_vertexArrayObject);


        GLint posAttrib = glGetAttribLocation(_program, "vpos");
        GLint texAttrib = glGetAttribLocation(_program, "texCoords");
        GLint normAttrib = glGetAttribLocation(_program, "vnorm");


        // fill vertex array object with data
        GLuint position_buffer;
        glGenBuffers(1, &position_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(posAttrib);

        GLuint index_buffer;
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        GLuint tex_buffer;
        glGenBuffers(1,&tex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, tex_buffer);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size()*sizeof (glm::vec2),texCoords.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(texAttrib,2,GL_FLOAT,GL_TRUE,0,0);
        glEnableVertexAttribArray(texAttrib);

        GLuint normal_buffer;
        glGenBuffers(1, &normal_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(float), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_TRUE, 0, 0);
        glEnableVertexAttribArray(normAttrib);

        // unbind vertex array object
        glBindVertexArray(0);
        // delete buffers (the data is stored in the vertex array object)
        glDeleteBuffers(1, &position_buffer);
        glDeleteBuffers(1, &index_buffer);
        glDeleteBuffers(1, &tex_buffer);

        // check for errors
        VERIFY(CG::checkError());



}

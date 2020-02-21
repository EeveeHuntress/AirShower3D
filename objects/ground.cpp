#include <GL/glew.h>

#include "ground.h"

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

Ground::Ground(std::string name, std::string textureLocation): Drawable(name)
{
    _textureLocation=textureLocation;
}

void Ground::init()
{

    Drawable::init();

    loadTexture();
}

void Ground::draw(glm::mat4 projection_matrix) const
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
    glUniform1i(glGetUniformLocation(_program, "texture"), 0);


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
    float shininess = 2.f;
    glUniform1f(glGetUniformLocation(_program, "shininess"), shininess);
    glm::vec3 kd(0.9f);
    glUniform3fv(glGetUniformLocation(_program, "kd"), 1, glm::value_ptr(kd));
    glm::vec3 ks(.0f);
    glUniform3fv(glGetUniformLocation(_program, "ks"), 1, glm::value_ptr(ks));
    glm::vec3 ka(0.5f);
    glUniform3fv(glGetUniformLocation(_program, "ka"), 1, glm::value_ptr(ka));


    // call draw
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // unbin vertex array object
    glBindVertexArray(0);

    // check for errors
    VERIFY(CG::checkError());

}

void Ground::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{

    //take out any unwanted translation from modelview matrix
    //glm::mat4 view = glm::mat4(glm::mat3(modelViewMatrix));

//    std::stack<glm::mat4> modelview_stack;

//    modelview_stack.push(modelViewMatrix);

//    modelview_stack.top() = glm::translate(modelview_stack.top(), glm::vec3(0,10,0));
//    modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(90.0f), glm::vec3(1,0,0));

//    modelview_stack.top()=glm::scale(modelview_stack.top(), glm::vec3(10.0f));
//    _modelViewMatrix = glm::mat4(modelview_stack.top());


    _modelViewMatrix = modelViewMatrix;




}

void Ground::setLights(std::shared_ptr<ShowerFrontCenter> front)
{
    _showerFront=front;
}

std::string Ground::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/ground.vs.glsl");
}

std::string Ground::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/ground.fs.glsl");
}


void Ground::loadTexture()
{
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    Image image(_textureLocation);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(),image.getHeight(),0,GL_RGBA, GL_UNSIGNED_BYTE, image.getData());


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_2D);

//    std::cout<<image.getHeight()<<std::endl;

    VERIFY(CG::checkError());
}

void Ground::createObject()
{
    ///TODO: your code here

//    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoordOptions;
    std::vector<glm::vec2> texCoords;
    //std::vector<unsigned int> indices;
    positions.clear();
    indices.clear();


    float factor = 100;

    positions.push_back(glm::vec3(1,0,1)*factor);    //0
    positions.push_back(glm::vec3(-1,0,1)*factor);   //1
    positions.push_back(glm::vec3(1,0,-1)*factor);   //2
    positions.push_back(glm::vec3(-1,0,-1)*factor);  //3

    texCoords.push_back(glm::vec2(1,1));
    texCoords.push_back(glm::vec2(0,1));
    texCoords.push_back(glm::vec2(1,0));
    texCoords.push_back(glm::vec2(0,0));

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


        // fill vertex array object with data
        GLuint position_buffer;
        glGenBuffers(1, &position_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);


        GLuint index_buffer;
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


        GLuint tex_buffer;
        glGenBuffers(1,&tex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, tex_buffer);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size()*sizeof (glm::vec2),texCoords.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_TRUE,0,0);
        glEnableVertexAttribArray(1);

        // unbind vertex array object
        glBindVertexArray(0);
        // delete buffers (the data is stored in the vertex array object)
        glDeleteBuffers(1, &position_buffer);
        glDeleteBuffers(1, &index_buffer);
        glDeleteBuffers(1,&tex_buffer);

        // check for errors
        VERIFY(CG::checkError());



}


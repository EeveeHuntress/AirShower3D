#include <GL/glew.h>

#include "skybox.h"

#include <glm/gtc/type_ptr.hpp>

#include "glbase/gltool.hpp"

#include "image/image.h"
#include "glbase/gltool.hpp"

#include "glbase/texload.hpp"

#include <iostream>
#include <stack>


Skybox::Skybox(std::string name, std::string textureLocation): Drawable(name)
{
    _textureLocation=textureLocation;
}

void Skybox::init()
{

    Drawable::init();

    loadTexture();
}

void Skybox::draw(glm::mat4 projection_matrix) const
{

    glDepthMask(GL_FALSE);

    // Load program
    glUseProgram(_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,textureID);

    // bin vertex array object
    glBindVertexArray(_vertexArrayObject);

    // set parameter
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));
    glUniform1i(glGetUniformLocation(_program, "skybox"), 0);

    // call draw
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);

    // unbin vertex array object
    glBindVertexArray(0);

    // check for errors
    VERIFY(CG::checkError());

}

void Skybox::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{

    //take out any unwanted translation from modelview matrix
    glm::mat4 view = glm::mat4(glm::mat3(modelViewMatrix));

    _modelViewMatrix = view;

}

std::string Skybox::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/skybox.vs.glsl");
}

std::string Skybox::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/skybox.fs.glsl");
}


void Skybox::loadTexture()
{
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    Image image(_textureLocation);

    //specify texture for all sides of cubemap
    for(int i=0;i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, image.getWidth(),image.getHeight(),0,GL_RGBA, GL_UNSIGNED_BYTE, image.getData());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    VERIFY(CG::checkError());
}

void Skybox::createObject()
{
    std::vector<glm::vec2> texCoordOptions;
    std::vector<glm::vec2> texCoords;
    positions.clear();
    indices.clear();
    //Cube coordinates
    positions.push_back(glm::vec3(-1,-1,-1) *1.f); // 0
    positions.push_back(glm::vec3(-1,-1, 1) *1.f); // 1
    positions.push_back(glm::vec3(-1, 1,-1) *1.f); // 2
    positions.push_back(glm::vec3(-1, 1, 1) *1.f); // 3
    positions.push_back(glm::vec3( 1,-1,-1) *1.f); // 4
    positions.push_back(glm::vec3( 1,-1, 1) *1.f); // 5
    positions.push_back(glm::vec3( 1, 1,-1) *1.f); // 6
    positions.push_back(glm::vec3( 1, 1, 1) *1.f); // 7

    texCoordOptions.push_back(glm::vec2(0,0));
    texCoordOptions.push_back(glm::vec2(0,1));
    texCoordOptions.push_back(glm::vec2(1,0));
    texCoordOptions.push_back(glm::vec2(1,1));

    // indices (two triangles for each side of the cube)
    // left (x = -1)
    indices.push_back(0);
    texCoords.push_back(texCoordOptions.at(0));
    indices.push_back(2);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(1);
    texCoords.push_back(texCoordOptions.at(2));

    indices.push_back(1);
    texCoords.push_back(texCoordOptions.at(2));
    indices.push_back(2);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(3);
    texCoords.push_back(texCoordOptions.at(3));

    // right (x = 1)
    indices.push_back(4);
    texCoords.push_back(texCoordOptions.at(0));
    indices.push_back(5);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(6);
    texCoords.push_back(texCoordOptions.at(2));

    indices.push_back(5);
    texCoords.push_back(texCoordOptions.at(2));
    indices.push_back(7);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(6);
    texCoords.push_back(texCoordOptions.at(3));

    // bottom (y = -1)
    indices.push_back(0);
    texCoords.push_back(texCoordOptions.at(0));
    indices.push_back(1);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(4);
    texCoords.push_back(texCoordOptions.at(2));

    indices.push_back(4);
    texCoords.push_back(texCoordOptions.at(2));
    indices.push_back(1);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(5);
    texCoords.push_back(texCoordOptions.at(3));

    // top (y = 1)
    indices.push_back(2);
    texCoords.push_back(texCoordOptions.at(0));
    indices.push_back(6);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(3);
    texCoords.push_back(texCoordOptions.at(2));

    indices.push_back(3);
    texCoords.push_back(texCoordOptions.at(2));
    indices.push_back(6);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(7);
    texCoords.push_back(texCoordOptions.at(3));

    // back (z = -1)
    indices.push_back(0);
    texCoords.push_back(texCoordOptions.at(0));
    indices.push_back(4);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(2);
    texCoords.push_back(texCoordOptions.at(2));

    indices.push_back(4);
    texCoords.push_back(texCoordOptions.at(2));
    indices.push_back(6);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(2);
    texCoords.push_back(texCoordOptions.at(3));

    // front (z = 1)
    indices.push_back(1);
    texCoords.push_back(texCoordOptions.at(0));
    indices.push_back(3);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(5);
    texCoords.push_back(texCoordOptions.at(2));

    indices.push_back(3);
    texCoords.push_back(texCoordOptions.at(2));
    indices.push_back(7);
    texCoords.push_back(texCoordOptions.at(1));
    indices.push_back(5);
    texCoords.push_back(texCoordOptions.at(3));



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


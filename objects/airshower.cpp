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

    _timeSteps = 0.00001;
    steps = 2000;
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
    glUniform3fv(colLocation, 1,glm::value_ptr(_color));
    // set maxtime in shader
    int maxtimeLocation = glGetUniformLocation(_program, "maxtime");
    glUniform1f(maxtimeLocation, Config::maxTime);
    // set stepLength in shader
    int stepLocation = glGetUniformLocation(_program, "stepLength");
    glUniform1f(stepLocation, 1.0f/float(steps));

    // call draw
//    if(!Config::onlyShowerFront)
     glDrawArrays(GL_LINES,0,positionsToShader.size());
//    else
//     glDrawArrays(GL_POINTS,0,positionsToShader.size());


    // unbin vertex array object
    glBindVertexArray(0);

    // check for errors
    VERIFY(CG::checkError());
}


void Airshower::updateCam(glm::vec3 cameraPos, glm::vec3 showerFrontCenter)
{
    _cameraPos = cameraPos;
    _showerFrontCenter = showerFrontCenter;
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

//    if(changed)
//        Airshower::recreate();

    //write time to shader
    glUseProgram(_program);
    glBindVertexArray(_vertexArrayObject);

    // Enable blending
//    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

//    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
//    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // set time in shader
    int timeLocation = glGetUniformLocation(_program, "time");
    glUniform1f(timeLocation, Config::time);


    // toogle showerFront in shader
    int boolLocation = glGetUniformLocation(_program, "onlyFront");
    float showerFront=0.0f;
    if(Config::onlyShowerFront)
        showerFront = 1.0f;
    glUniform1f(boolLocation, showerFront );

    // set camera position in shader (for Pennrose-Terrell effect)
    int cameraposLocation = glGetUniformLocation(_program, "camerapos");
    glUniform3fv(cameraposLocation, 1, glm::value_ptr(_cameraPos));

    // set shower front center in shader (for Pennrose-Terrell effect)
    int sfcenterLocation = glGetUniformLocation(_program, "sfcenter");
    glUniform3fv(sfcenterLocation, 1, glm::value_ptr(_showerFrontCenter));

    // set "bool" in shader (for Pennrose-Terrell effect)
    // pteffect=0.0f means the effect will be neglected
    int pteffect = glGetUniformLocation(_program, "pteffect");
    if(Config::pteffect)
        glUniform1f(pteffect, 1.0f);
    else
        glUniform1f(pteffect, 0.0f);


    glBindVertexArray(0);

    _modelViewMatrix = modelViewMatrix;
}



void Airshower::createObject()
{
    positionsToShader.clear();
    timestampsToShader.clear();

    //get current time from Config
    glm::vec3 offset = glm::vec3(0.0f, _offsetY*_sizeFactor, 0.0f);
    _timeSteps = _maxTime/steps;

    //iterate over the tracks
    for(int ipos=0; ipos<positions.size()-1; ipos+=2)
    {
        //calculate the index of the section before the start and end position of the track
        int startCount = floor(timestamps.at(ipos)/_timeSteps);
        int endCount = floor(timestamps.at(ipos+1)/_timeSteps);

        //track is not devided
        if(startCount==endCount)
        {
            positionsToShader.push_back(positions.at(ipos)-offset);
            timestampsToShader.push_back(timestamps.at(ipos)/_maxTime);
            positionsToShader.push_back(positions.at(ipos+1)-offset);
            timestampsToShader.push_back(timestamps.at(ipos+1)/_maxTime);
            continue;
        }

        //calculate first partitition and push to vector positionsToShader
        float TestTime=_timeSteps*(startCount+1);
        float lambda = (TestTime-timestamps.at(ipos))/(timestamps.at(ipos+1)-timestamps.at(ipos));
        glm::vec3 newTrack = positions.at(ipos) + lambda*(positions.at(ipos+1)-positions.at(ipos));

        positionsToShader.push_back(positions.at(ipos)-offset);
        timestampsToShader.push_back(timestamps.at(ipos)/_maxTime);

        positionsToShader.push_back(newTrack-offset);
        timestampsToShader.push_back(TestTime/_maxTime);

        //calculate intermediate sections
        for(int iter=startCount+1; iter<endCount; iter++)
        {
            float StartTestTime=_timeSteps*(iter);
            float StartLambda = (StartTestTime-timestamps.at(ipos))/(timestamps.at(ipos+1)-timestamps.at(ipos));
            glm::vec3 newTrackStart = positions.at(ipos) + StartLambda*(positions.at(ipos+1)-positions.at(ipos));

            positionsToShader.push_back(newTrackStart-offset);
            timestampsToShader.push_back(StartTestTime/_maxTime);

            float EndTestTime=_timeSteps*(iter+1);
            float EndLambda = (EndTestTime-timestamps.at(ipos))/(timestamps.at(ipos+1)-timestamps.at(ipos));
            glm::vec3 newTrackEnd = positions.at(ipos) + EndLambda*(positions.at(ipos+1)-positions.at(ipos));

            positionsToShader.push_back(newTrackEnd-offset);
            timestampsToShader.push_back(EndTestTime/_maxTime);

        }

        //calculate last section if it doesn't end at the end position of the track
        TestTime = _timeSteps*endCount;
        if(TestTime < timestamps.at(ipos+1) )
        {
            lambda = (TestTime-timestamps.at(ipos))/(timestamps.at(ipos+1)-timestamps.at(ipos));
            newTrack = positions.at(ipos) + lambda*(positions.at(ipos+1)-positions.at(ipos));

            positionsToShader.push_back(newTrack-offset);
            timestampsToShader.push_back(TestTime/_maxTime);

            positionsToShader.push_back(positions.at(ipos+1)-offset);
            timestampsToShader.push_back(timestamps.at(ipos+1)/_maxTime);
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

    GLuint timestamp_buffer;
    glGenBuffers(1, &timestamp_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, timestamp_buffer);
    glBufferData(GL_ARRAY_BUFFER, timestampsToShader.size() * sizeof(float), timestampsToShader.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // unbind vertex array object
    glBindVertexArray(0);
    // delete buffers (the data is stored in the vertex array object)
    glDeleteBuffers(1, &position_buffer);
    glDeleteBuffers(1, &timestamp_buffer);

    // check for errors
    VERIFY(CG::checkError());
}


void Airshower::createTestShower()
{
    _maxTime=1.0f;
    _max = glm::vec3(0.0f,15.0f,3.0f);
    _min = glm::vec3(2.0f,0.0f,0.0f);

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

    float eCut;
    if(_type == "em") eCut = Config::eCut_em;
    if(_type == "hd") eCut = Config::eCut_hd;
    if(_type == "mu") eCut = Config::eCut_mu;

    bool first = true;

    // read file
    if(Config::pathToAirshowerFiles != "DEFAULT") // if defined read custom (binary-)file from config
    {
        std::ifstream myfile;
        myfile.open(_dataPath.c_str(), std::ios_base::in | std::ios_base::binary);

        if(myfile.is_open())
        {
            std::cout << "reading " << _type << "-shower into memory...";
            float particle, energy, xstart, ystart, zstart, tstart, xend, yend, zend, tend, trash;
            myfile.read((char*)&trash,4); // first 4 bytes in each file are bullshit

            while(!myfile.eof())
            {
                myfile.read((char*)&particle,4);
                myfile.read((char*)&energy,4);
                myfile.read((char*)&xstart,4);
                myfile.read((char*)&ystart,4);
                myfile.read((char*)&zstart,4);
                myfile.read((char*)&tstart,4);
                myfile.read((char*)&xend,4);
                myfile.read((char*)&yend,4);
                myfile.read((char*)&zend,4);
                myfile.read((char*)&tend,4);
                myfile.read((char*)&trash,4); //
                myfile.read((char*)&trash,4); // last 16 bytes in each line
                myfile.read((char*)&trash,4); // are also bullshit
                myfile.read((char*)&trash,4); //   ...dunno why

                if(energy < eCut) continue;

                if(!(first && _type.compare("hd") == 0)) // skip first particle of hd shower (has to be more flexible if photon primaries are considered)
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
                    first = false;
                }

                if(myfile.eof())
                    break;
            }

            myfile.close();
            std::cout << " done." << std::endl;
        }
        else
        {
            std::cout << "Could not open file " << _dataPath << std::endl;
            std::cout << "Creating Testshower..." << std::endl;
        }
    }
    else // use sampleshower
    {
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
                    //                std::cout << "Test" << std::endl;
                    first = false;
                }
            }

            myfile.close();
        }
        else
        {
            std::cout << "Could not open sample shower " << _dataPath << std::endl;
            std::cout << "Creating Testshower..." << std::endl;
        }
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
    recreate();
}

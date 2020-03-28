#include <GL/glew.h>

#include "crown.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "glbase/gltool.hpp"

#include "image/image.h"
#include "glbase/gltool.hpp"

#include "glbase/texload.hpp"

#include <iostream>
#include <stack>

#include "objects/detector.h"
#include "gui/config.h"

Crown::Crown(std::string name)
{
    _levels=Config::crownLevels;
    _maxLevels = 23;
    _distance= Config::detectorDistance;
    _number = 1 + 6*(_levels*(_levels+1))/2;
    _radius = Config::radius;
}


void Crown::init()
{
    createObject();
}

void Crown::draw(glm::mat4 projection_matrix)
{
    // call draw for all detectors
    for(unsigned int i=0;i< _number;i++)
    {
        _detectors.at(i).draw(projection_matrix);
    }

}


void Crown::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{

    // update all detectors
    _modelViewMatrix = modelViewMatrix;

        std::stack<glm::mat4> modelview_stack;

        for(unsigned int i=0;i< _detectors.size();i++)
        {
            modelview_stack.push(modelViewMatrix);
            glm::vec3 trans= _crownPositions.at(i);

            modelview_stack.top()=glm::translate(modelview_stack.top(),trans/*+glm::vec3(0,0.01,0)*/);
            modelview_stack.top()=glm::scale(modelview_stack.top(), glm::vec3(Config::radius/_radius));

//            _radius = Config::radius;

            _detectors.at(i).update(elapsedTimeMs, modelview_stack.top());

            modelview_stack.pop();
        }

}


void Crown::setLights(std::shared_ptr<ShowerFrontCenter> front)
{
    _showerFront=front;

    for(unsigned int i=0;i< _detectors.size();i++)
    {
        _detectors.at(i).setLights(front);
    }
}



void Crown::recreate()
{
    if(Config::distanceChanged)
    {
        for(int i=0;i<_crownPositions.size();i++)
        {
            _crownPositions.at(i)=_crownPositions.at(i)/_distance*Config::detectorDistance;
        }
        _distance=Config::detectorDistance;
    }

    if(Config::crownLevelsChanged && Config::crownLevels<=_maxLevels)
    {
        _levels=Config::crownLevels;
        _number = 1 + 6*(_levels*(_levels+1))/2;
    }
}

void Crown::createObject()
{
    _crownPositions.clear();
    _detectors.clear();

    _crownPositions.push_back(glm::vec3(0.0f));
    _detectors.push_back(Detector("Detector 0"));
    _detectors.back().init();

    int maxLevels = _maxLevels;

    for (int level=0;level<maxLevels;level++)
    {
        int index=0;

        // calculate detector positions on hexagon edges
        for(float i=0.0f; i< M_PI*2.0f-0.0001; i+=M_PI/3.0f) // need to subtract small amount, because of floating point precision
        {
            glm::vec3 position=glm::vec3(sinf(i)*_distance*(level+1),0,cosf(i)*_distance*(level+1));
            _crownPositions.push_back(position);
            _detectors.push_back(Detector("Detector "+std::to_string(level*6+index)));
            _detectors.back().init();

            // calculate detector positions between hexagon edges
            if(level!=0)
            {
                glm::vec3 next_position=glm::vec3(sinf(i+M_PI/3.0f)*_distance*(level+1),0,cosf(i+M_PI/3.0f)*_distance*(level+1));
                glm::vec3 gapVec= (next_position- position);
                gapVec=gapVec/float(level+1);

                for(int gap=1; gap<=level; gap++)
                {
                    _crownPositions.push_back(gapVec*float(gap)+position);
                    _detectors.push_back(Detector("Detector "+std::to_string(level*6+index)));
                    _detectors.back().init();
                }
            }
            index++;
        }
    }

}



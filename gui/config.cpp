#include "config.h"

/*
 *  Default values. Change them as needed.
 */

float Config::animationSpeed = 1.0f;

bool Config::localRotation = false;

bool Config::changeRadius=false;        /** toggle change of resolution of detectors */
float Config::radius = 1.0f;            /** chanfe resolution factor of detectors */

bool Config::subdivide=false;           /** toggle change of resolution of detectors */
unsigned int Config::subdivCount=48;    /** chanfe resolution factor of detectors */

float Config::time=0.0;                 /** time elapsed for shower animation */
float Config::maxTime =0.0;

bool Config::loop=false;                /** toggle looping of animation */

bool Config::onlyShowerFront=false;     /** toggle to only show the current shower front */
bool Config::changedAirshower=false;    /** signal for the airshowers */

bool Config::lookatShowerFront=false;   /** toggle for camera to look at shower front */

bool Config::isPlaying=true;
float Config::pauseTime=0.0f;
int Config::timeChanged=0;

int Config::crownLevels=3;
bool Config::crownLevelsChanged=false;

float Config::detectorDistance=3.0f;
bool Config::distanceChanged=false;

std::string Config::configFileName="NONE";

std::string Config::pathToAirshowerFiles="DEFAULT";

float Config::showerAxisZenith=30.f;

float Config::showerAxisAzimuth=0.f;

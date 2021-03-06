#include "config.h"

/*
 *  Default values. Change them as needed.
 */

float Config::animationSpeed = 1.0f;

bool Config::localRotation = false;

float Config::radius = 0.3f;            /** chanfe resolution factor of detectors */

unsigned int Config::subdivCount = 18;    /** chanfe resolution factor of detectors */

float Config::time = 0.0;                 /** time elapsed for shower animation */
float Config::maxTime = 0.0;

bool Config::loop = true;                /** toggle looping of animation */

bool Config::isPlaying = true;
float Config::pauseTime = 0.0f;

bool Config::emComponent = true;
bool Config::muComponent = true;
bool Config::hdComponent = true;

bool Config::onlyShowerFront = false;     /** toggle to only show the current shower front */
bool Config::changedAirshower = false;    /** signal for the airshowers */

bool Config::lookatShowerFront = false;   /** toggle for camera to look at shower front */

bool Config::enableDetectors = true;
bool Config::whiteBackground = false;

bool Config::pteffect = false;

int Config::crownLevels = 23;
bool Config::crownLevelsChanged = false;

float Config::detectorDistance = 1.5f;
bool Config::distanceChanged = false;

std::string Config::configFileName = "NONE";

std::string Config::pathToAirshowerFiles = "DEFAULT";

float Config::showerAxisZenith = 30.f;

float Config::showerAxisAzimuth = 0.f;

float Config::eCut_em = 1E50f;
float Config::eCut_hd = 1E50f;
float Config::eCut_mu = 1E50f;

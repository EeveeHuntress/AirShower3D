#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>
#include <vector>

/**
 * @brief The global configuration
 *
 * This class contains global configuration parameters
 * that are used all over the program.
 *
 * Hint: Add as many parameters as you need.
 */
class Config{
public:

    static float animationSpeed;     /**< the number of steps per ms */

    static bool localRotation;       /**< rotate the planets locally */

    static float radius;            /// radius of detector

    static bool subdivide;          /// checker if subdivision value has changed and detectors need to recreate
    static unsigned int subdivCount;/// number of subdivisions

    static float time;              /// current time of animation
    static float maxTime;           /// amount of the shower needs to hit the ground
    static bool loop;               /// loop the animation

    static bool emComponent;
    static bool muComponent;
    static bool hdComponent;

    static bool onlyShowerFront;    /// only draw shower front
    static bool changedAirshower;   /// checker to toggle between drawing full shower or only shower front

    static bool lookatShowerFront;  /// checker for focus point of camera to look at the shower front center

    static bool isPlaying;          /// is the airshower animation playing
    static float pauseTime;         /// time saved when paused

    static bool enableDetectors;
    static bool whiteBackground;

    static int crownLevels;         /// levels of detector crowns to draw
    static bool crownLevelsChanged; /// checker for updating crown levels

    static float detectorDistance;  /// distance between detectors
    static bool distanceChanged;    /// checker for changing distance

    static bool pteffect;

    static std::string configFileName; // name of the user-degfined config file
    static std::string pathToAirshowerFiles; // path to the airshower data files

    static float showerAxisZenith;           // zenith angle of the simulated shower axis
    static float showerAxisAzimuth;          // azimuth angle of the simulated shower axis

    static float eCut_em; // energy thresholds for particle tracks
    static float eCut_hd;
    static float eCut_mu;
};

#endif // CONFIG_H


#include <iostream>
#include <cmath>
#include <GL/glew.h>

#include "glwidget.hpp"

#include <QMouseEvent>

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtx/transform.hpp>

#include "gui/config.h"
#include "gui/ConfigFile.h"

#include "objects/detector.h"
#include "objects/skybox.h"
#include "objects/airshower.h"
#include "objects/sfcenter.h"
#include "objects/ground.h"
#include "objects/crown.h"
#include "objects/solarpanel.h"

#include "gui/mainwindow.hpp"

#ifndef M_PI_2
#define M_PI_2 (3.14159265359f * 0.5f)
#endif

using namespace glm;

GLWidget::GLWidget(QWidget *&parent) : QOpenGLWidget(parent),//static_cast<QWidget*>(0)),
    _updateTimer(this), _stopWatch()
{
    // update the scene periodically
    QObject::connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(animateGL()));
    _updateTimer.start(18);
    _stopWatch.start();

    cameraBelow=false;

    ConfigFile configfile;

    if(Config::configFileName != "NONE")
    {
      configfile = ConfigFile(Config::configFileName.c_str());
      configfile.readInto<std::string>(Config::pathToAirshowerFiles, "PathToAirshowerFiles");
      configfile.readInto<float>(Config::showerAxisZenith, "ShowerAxisZenith");
      configfile.readInto<float>(Config::showerAxisAzimuth, "ShowerAxisAzimuth");
    }

    // create all drawable elements
    _skybox = std::make_shared<Skybox>("Skybox", ":/res/images/stars.bmp");

    //convert shower axis to carthesian coordinates 
    Config::showerAxisZenith *= M_PI_2/90.f;
    Config::showerAxisAzimuth *= M_PI_2/90.f;
    float axis_x = std::sin(Config::showerAxisZenith)*std::cos(Config::showerAxisAzimuth);
    float axis_y = std::cos(Config::showerAxisZenith);
    float axis_z = std::sin(Config::showerAxisZenith)*std::sin(Config::showerAxisAzimuth);
    glm::vec3 showerAxis = -1.f*glm::vec3(axis_x,axis_y,axis_z);

    _ground=std::make_shared<Ground>("Ground", ":/res/images/grass.bmp");
    _crown= std::make_shared<Crown>("Crown");

    //shower data is divided into three types
    if(Config::pathToAirshowerFiles == "DEFAULT")
    {
      _airshower_em = std::make_shared<Airshower>("Test_Shower_em", ":/dat/data/track000001.em.txt", "em", showerAxis);
      _airshower_hd = std::make_shared<Airshower>("Test_Shower_hd", ":/dat/data/track000001.hd.txt", "hd", showerAxis);
      _airshower_mu = std::make_shared<Airshower>("Test_Shower_mu", ":/dat/data/track000001.mu.txt", "mu", showerAxis);
    }
    else
    {
      _airshower_em = std::make_shared<Airshower>("Test_Shower_em", (Config::pathToAirshowerFiles + "track000001.em.txt").c_str(), "em", showerAxis);
      _airshower_hd = std::make_shared<Airshower>("Test_Shower_hd", (Config::pathToAirshowerFiles + "track000001.hd.txt").c_str(), "hd", showerAxis);
      _airshower_mu = std::make_shared<Airshower>("Test_Shower_mu", (Config::pathToAirshowerFiles + "track000001.mu.txt").c_str(), "mu", showerAxis);
    }
}

void GLWidget::show()
{
    QOpenGLWidget::show();

    // check for a valid context
    if (!isValid() || !context()->isValid() || context()->format().majorVersion() != 4) {
        QMessageBox::critical(this, "Error", "Cannot get a valid OpenGL 4 context.");
        exit(1);
    }
}

void GLWidget::initializeGL()
{
    /* Initialize OpenGL extensions */
    glewExperimental = GL_TRUE; // otherwise some function pointers are NULL...
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    glGetError(); // clear a gl error produced by glewInit

    // make sure the context is current
    makeCurrent();

    /// TODO: Init all drawables here
    _crown->init();

    //init mu airshower first
    _airshower_mu->init();
    _airshower_hd->init();
    _airshower_em->init();

    float maxTime = _airshower_em->getMaxTime();
    if( maxTime < _airshower_mu->getMaxTime() )
        maxTime = _airshower_mu->getMaxTime();
    if( maxTime < _airshower_hd->getMaxTime() )
        maxTime = _airshower_hd->getMaxTime();

    Config::maxTime = maxTime;
    _airshower_hd->setMaxTime(Config::maxTime);
    _airshower_mu->setMaxTime(Config::maxTime);
    _airshower_em->setMaxTime(Config::maxTime);


    _skybox->init();
    _ground->init();

    float offset = _airshower_em->getMinY();
    if( offset > _airshower_mu->getMinY() )
        offset = _airshower_mu->getMinY();
    if( offset > _airshower_hd->getMinY() )
        offset = _airshower_hd->getMinY();

    float hight = _airshower_em->getMaxY();
    if( hight < _airshower_mu->getMaxY() )
        hight = _airshower_mu->getMaxY();
    if( hight < _airshower_hd->getMaxY() )
        hight = _airshower_hd->getMaxY();

    _airshower_em->setOffsetY(offset);
    _airshower_mu->setOffsetY(offset);
    _airshower_hd->setOffsetY(offset);

    _sfcenter = std::make_shared<ShowerFrontCenter>("ShowerFrontCenter", _airshower_em->getShowerAxis(), glm::vec3(0.0f,(hight-offset)/100000,0.0f));
    _sfcenter->init();

    _ground->setLights(_sfcenter);
    _crown->setLights(_sfcenter);   //also sets light for detectors

    _airshower_em->recreate();
    _airshower_mu->recreate();
    _airshower_hd->recreate();
}

void GLWidget::resizeGL(int width, int height)
{
    // update the viewport
    glViewport(0, 0, width, height);
}

void GLWidget::paintGL()
{
    // Render: set up view
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    GLint m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport );

    // calculate projection matrix from resolution
    glm::mat4 projection_matrix = glm::perspective(glm::radians(50.0f),
                float(m_viewport[2])/m_viewport[3],
                0.1f, 100.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if(Config::crownLevelsChanged|| Config::distanceChanged)
    {
        _crown->recreate();
        Config::crownLevelsChanged=false;
        Config::distanceChanged=false;
    }

    if(Config::enableDetectors)
    {
        _skybox->draw(projection_matrix);

        _ground->draw(projection_matrix);

        if(!cameraBelow)
        {

            _crown->draw(projection_matrix);
        }
    }

    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    if(Config::hdComponent)
        _airshower_hd->draw(projection_matrix);
    if(Config::muComponent)
        _airshower_mu->draw(projection_matrix);
    if(Config::emComponent)
        _airshower_em->draw(projection_matrix);
}

//default values for camera position
ivec2 mouse_pos = ivec2(-1.0,-1.0);
double theta= M_PI_2;
double phi = 0.0;
double radius= -15.0;


void GLWidget::mousePressEvent(QMouseEvent *event)
{
    //save mouse position when pressing
    ivec2 pos = ivec2(event->pos().x(), event->pos().y());
    mouse_pos = pos;
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
     mouse_pos = ivec2(-1.0, -1.0); //reset default
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    ivec2 pos = ivec2(event->pos().x(), event->pos().y());
    ivec2 diff = pos - mouse_pos; //move direction
    if (mouse_pos.x > 0 && mouse_pos.y > 0 && (theta+radians(float(diff.y)))>0 && (theta+radians(float(diff.y)))<M_PI )
    {
        theta += radians(2.0*float(diff.y)/(-radius));
        phi += radians(2.0*float(diff.x)/(-radius));
    }
    //update mouse position
    mouse_pos = ivec2(event->pos().x(), event->pos().y());
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    //handle zoom
    if((radius + radians((float)event->angleDelta().ry())/8.0f)<0) //won't zoom past origin/lookat point
    {
        radius += radians((float)event->angleDelta().ry())/8.0f;
    }
}

void GLWidget::animateGL()
{
    // make the context current in case there are glFunctions called
    makeCurrent();

    // get the time delta
    float timeElapsedMs = _stopWatch.nsecsElapsed() / 1000000.0f;
    // restart stopwatch for next update
    _stopWatch.restart();

    // calculate current modelViewMatrix for the default camera
    glm::vec3 camera = glm::vec3( -radius * sin(theta) * sin(phi), radius * cos(theta), radius * sin(theta)* cos(phi));
    glm::vec3 focus = glm::vec3(0.0f, 5.0f, 0.0f);
    if(Config::lookatShowerFront)
    {
        focus = _sfcenter->getPositionCam();
    }

    glm::mat4 modelViewMatrix = glm::lookAt(camera+glm::vec3(0.0f,5.0f,0.0f), focus, glm::vec3(0.0, 1.0, 0.0));

    if((camera.y+5)>=0)
        cameraBelow=false;
    else
        cameraBelow=true;

    // update drawables
    _airshower_hd->updateCam(camera, _sfcenter->getPositionCam());
    _airshower_em->updateCam(camera, _sfcenter->getPositionCam());
    _airshower_mu->updateCam(camera, _sfcenter->getPositionCam());
    _airshower_hd->update(timeElapsedMs,modelViewMatrix);
    _airshower_em->update(timeElapsedMs,modelViewMatrix);
    _airshower_mu->update(timeElapsedMs,modelViewMatrix);
    _skybox->update(timeElapsedMs, modelViewMatrix);
    _ground->update(timeElapsedMs, modelViewMatrix);
    _crown->update(timeElapsedMs,modelViewMatrix);
    _sfcenter->update(timeElapsedMs,modelViewMatrix);

    // update the widget (do not remove this!)
    update();
}




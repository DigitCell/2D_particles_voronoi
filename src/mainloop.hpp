#ifndef MAINLOOP_HPP
#define MAINLOOP_HPP

#pragma once

#include <chrono>
#include "glm/mat4x4.hpp"

#include "support/arcball_camera.hpp"

#include "Constants.hpp"
#include "graphmodule.hpp"
#include "solver.hpp"
#include "support/ConfigParser.h"
#include <opencv2/img_hash.hpp>

#include "support/meshCreator.h"

class MainLoop
{
public:
    MainLoop();

    GraphModule graphModule;
    Solver solver;

    Sim_parameters sim_parameters;
    bool RunLoop();

    struct {
      glm::mat4x4 view;
      glm::mat4x4 proj;
      glm::mat4x4 viewProj;
    } matrix_;

    std::chrono::steady_clock::time_point time_;
    float deltatime_;
    ArcBallCamera camera_;
    void update_camera();
    void update_time();
    TEventData camera_event;
    ConfigParser config;

    int tick=0;

    void setup();
    bool Step(float deltaTime, int interTick);
    bool updateEvents(bool &done);
    void configureModules();

    MeshData originalMesh;
    void CreateNewMesh();

};

#endif // MAINLOOP_HPP

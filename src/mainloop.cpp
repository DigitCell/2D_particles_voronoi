#include "mainloop.hpp"

MainLoop::MainLoop():deltatime_(0.0f), config(path_string+"config.txt")
{

    std::setbuf(stderr, nullptr);
    std::srand(static_cast<uint32_t>(std::time(nullptr)));

    camera_event.wheelDelta = 0.0f;
    camera_event.mouseX = 0.0f;
    camera_event.mouseY = 0.0f;
    camera_event.bMouseMove = false;
    camera_event.bRotatePressed = false;
    camera_event.bTranslatePressed = false;

    /* Camera setup. */
    camera_.dolly(295.0f);

    int const w  = static_cast<int>(graphModule.mainScreenWidth);
    int const h = static_cast<int>(graphModule.mainScreenHeight);

    /* Setup the projection matrix */
    float const aspectRatio = w / static_cast<float>(h);
    matrix_.proj = glm::perspective(
      glm::radians(60.0f), aspectRatio, 0.01f, 2000.0f
    );

    /* Start the chrono. */
    time_ = std::chrono::steady_clock::now();

    setup();
}

bool MainLoop::Step(float deltaTime, int interTick)
{
    solver.Tick=interTick;
    solver.update(sim_parameters);

    //graphModule.pixelsNeighbor_draw(solver);
    //graphModule.pixelsSort_draw(solver);
    //graphModule.pixels_draw(solver);

}


bool MainLoop::RunLoop()
{
    std::cout << "Running main loop" << std::endl;
    bool running = true;
    static Uint32 lastFrameEventTime = 0;
    const Uint32 debounceTime = 100; // 100ms

    int interTick=0;
    sim_parameters.radius=particleRadius_init;

    bool done = false;
    while (!done){
        updateEvents(done);

        /* Clean Screen */
        graphModule.ClearRenderScreen();

        //graphModule.Render(matrix_.view, matrix_.viewProj);

        if(sim_parameters.runSimulation)
        {
            Step(deltatime_, interTick);
            interTick++;
        }

        if(sim_parameters.nextStep)
        {
             Step(deltatime_, interTick);
             sim_parameters.nextStep=false;
             interTick++;
        }

        if(graphModule.drawMeshMain)
        {
            graphModule.stipples_draw(solver);
            graphModule.boxes_draw(solver);
           // graphModule.voronoi_draw(solver);


           // glm::vec3 translateMesh=graphModule.meshOffset;
           // graphModule.RenderMeshCut(graphModule.graphProcessor.buffers.meshbase, translateMesh, matrix_.view, matrix_.viewProj);
        }

        graphModule.GuiRender(sim_parameters, solver);
        SDL_GL_SwapWindow(graphModule.screen); //update window
        tick++;
    }

    graphModule.CloseRender();
    return true;
}


void MainLoop::configureModules()
{
    solver.importDataToBUffers();
    graphModule.stipples_new(solver);
    graphModule.boxes_new(solver);

    graphModule.voronoi_new(solver);

    //graphModule.pixelsNeighbor_new(solver);
    //graphModule.PixelsSort_new(solver);
    //graphModule.Pixels_new(solver);

    graphModule.pixelTexture_new(solver);

}

void MainLoop::CreateNewMesh()
{
    //originalMesh=MeshCreator::createSphere(1.0);
    //std::string objFile=path_string+"textures/"+ "torus.obj";
    //originalMesh = WavefrontObjSerializer::deserialize(objFile.c_str());
    // graphModule.setup_mesh_geometry(graphModule.graphProcessor.buffers.meshbase, originalMesh);
}


void MainLoop::setup()
{
    configureModules();
    CreateNewMesh();
   // glfwSetTime(time_);
}


bool MainLoop::updateEvents(bool& done)
{
    camera_event.bMouseMove = false;
    camera_event.wheelDelta = 0.0f;
    Uint32 curTime = SDL_GetTicks();
    while (SDL_PollEvent(&graphModule.event))
    {
        ImGui_ImplSDL2_ProcessEvent(&graphModule.event);
        ImGuiIO& io = ImGui::GetIO();
        if (graphModule.event.type == SDL_QUIT)
        {
            done = true;
            ImGui::SaveIniSettingsToDisk("tempImgui.ini");
        }
        if (graphModule.event.type == SDL_WINDOWEVENT && graphModule.event.window.event == SDL_WINDOWEVENT_CLOSE && graphModule.event.window.windowID == SDL_GetWindowID(graphModule.screen))
            done = true;
        else
        {
            graphModule.HandleEvents(graphModule.event, camera_event);
        }
    }

    /* Update chrono and calculate deltatim */
    update_time();

    /* Camera event handling and matrices update */
    update_camera();
}



void MainLoop::update_camera() {

  camera_.event(
    camera_event.bMouseMove, camera_event.bTranslatePressed, camera_event.bRotatePressed,
    camera_event.mouseX, camera_event.mouseY, camera_event.wheelDelta
  );

  /* Compute the view matrix */
  matrix_.view = glm::mat4(1.0f);
  matrix_.view = glm::lookAt(glm::vec3(0.0f, 0.65f*camera_.dolly(), camera_.dolly()),
                             glm::vec3(0.0f, 0.0f, 0.0f),
                             glm::vec3(0.0f, 1.0f, 0.0f));
  matrix_.view =glm::translate(matrix_.view, glm::vec3(camera_.translate_x(), camera_.translate_y(), 0.0f));
  matrix_.view = glm::rotate(
    glm::rotate(matrix_.view, camera_.yaw(), glm::vec3(1.0f, 0.0f, 0.0f)),
    camera_.pitch(), glm::vec3(0.0f, 1.0f, 0.0f)
  );

  /* Update the viewproj matrix */
  matrix_.viewProj = matrix_.proj * matrix_.view;
}

void MainLoop::update_time() {
  std::chrono::steady_clock::time_point tick = std::chrono::steady_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(tick - time_);
  time_ = tick;

  deltatime_ = static_cast<float>(time_span.count());
}

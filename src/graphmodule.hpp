#ifndef GRAPHMODULE_HPP
#define GRAPHMODULE_HPP

#pragma once

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "Constants.hpp"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <SDL2/SDL.h>
#include "iostream"
#include "support/shaderutils.hpp"

#include "vector"
#include "array"
#include "stdio.h"

#include "support/Icosphere.h"
#include "solver.hpp"

#include "support/meshCreator.h"


class GraphModule
{
public:
    GraphModule();

    ShaderUtils shaderutils;

    struct DebugParameters_t {
      bool show_grid = true;
      bool coordinates = true;
      bool show_simulation_volume = true;
      bool freeze = false;
    };

    DebugParameters_t debug_parameters_;

    bool InitSDL();
    bool InitImGui();
    bool InitShaders();

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    glm::mat4x4 initView;


    bool drawWireFrame=false;
    bool drawMesh=true;
    bool drawMeshMain=true;
    bool drawParticlesonMap=false;

    float scaleWindow01=0.577f;
    float scaleWindow02=1.0f;
    glm::vec3 meshOffset=glm::vec3(0.0,0,0);



    int mainScreenWidth=2300;
    int mainScreenHeight=1500;

    uint width;
    uint height;

    float f = 0.0f;
    int counter = 0;
    const uint resolutionCircle=resolutionCircle_init;
    float sx=1.0f;
    float sy=1.0f;

    int cone_resolution=64;


    uint samples= particles_init;

    SDL_Window* screen = NULL;
    SDL_GLContext gContext;
    SDL_Event event;

    void GuiRender(Sim_parameters& sim_parameters, Solver& solver);
    void CloseRender();
    void ClearRenderScreen();
    bool clearScreen=true;

    struct graphProcessor_
    {
        struct buffers_{
            struct {
                GLuint vao;
                GLuint vbo;
                GLsizei nvertices;
                unsigned int resolution;
            } grid;

            struct MeshBase_{
                GLuint vao;
                GLuint vbo;
                GLuint vbo_n;
                GLuint ibo;
                GLsizei nvertices;
                GLenum indices_type;
                GLsizei nindices;
                unsigned int resolution;
            } meshbase;

        } buffers;

        struct shaders_{
            GLuint grid;
            GLuint basic;
            GLuint basic_color;
        } shaders;

        struct uniforms_{
            struct {
                GLint mvp;
                GLint scaleFactor;
            } grid;

            struct {
                GLint mvp;
                GLint color;
                GLint scaleFactor;
            } basic;
            struct {
                GLint mvp;
                GLint scaleFactor;
            } basic_color;

        } uniforms;

        struct Stipples_
        {
            GLuint vao;
            GLuint prog;
        } stipples;

        Stipples_ boxes;
        Stipples_ pixels;
        Stipples_ pixelsSort;
        Stipples_ pixelsNeighbor;



        struct Voronoi_ {
             GLuint vao;     /*  VAO with bound cone and offsets */
             GLuint pts;     /*  VBO containing point locations  */
             GLuint prog;    /*  Shader program (compiled)       */
             GLuint img;     /*  Target image texture            */

             GLuint tex;     /*  RGB texture (bound to fbo)          */
             GLuint depth;   /*  Depth texture (bound to fbo)        */
             GLuint fbo;     /*  Framebuffer for render-to-texture   */

             GLuint renderTexture;

         } voronoi;

        struct RenderScreen_
        {
            GLuint framebuffer;
            GLuint textureColorbuffer;
            GLuint rbo;
        } renderscreenStipples;

        RenderScreen_ renderscreenQuad;
        RenderScreen_ renderscreenPixel;
        RenderScreen_ renderscreenPixelSort;
        RenderScreen_ renderscreenPixelNeighbor;

        struct{
            struct{
                GLuint id;
                int width;
                int height;
            } source_texture;

            struct{
                GLuint id;
                int width;
                int height;
            } img;

            struct{
                GLuint id;
                int width;
                int height;
            } depth;

            struct{
                GLuint id;
                int width;
                int height;
            } tex;
        } textures;

    } graphProcessor;

    void setup_grid_geometry();
    void draw_grid(const glm::mat4x4 &mvp);
    void HandleEvents(SDL_Event e, TEventData &camera_event);
    void Render(const glm::mat4x4 &view, const glm::mat4x4 &viewProj);

    void setup_texture();
    GLuint texture_new();
    GLuint setup_texture_emptyWH(int widths_, int heights_);

    bool Setup();



    void draw_mesh(graphProcessor_::buffers_::MeshBase_ &meshIn, glm::vec3 translate, const glm::mat4x4 &mvp, const glm::vec4 &color, bool bFill);
    void update_mesh_geometry(graphProcessor_::buffers_::MeshBase_ &meshIn, MeshData &mesh);
    void update_indices_for_Line_geometry(graphProcessor_::buffers_::MeshBase_ &meshIn, MeshData &mesh);
    void update_indices_for_Triangle_geometry(graphProcessor_::buffers_::MeshBase_ &meshIn, MeshData &mesh);
    void setup_mesh_geometry(graphProcessor_::buffers_::MeshBase_ &meshIn, MeshData &mesh);
    void RenderMeshCut(graphProcessor_::buffers_::MeshBase_ &meshIn, glm::vec3 translate, const glm::mat4x4 &view, const glm::mat4x4 &viewProj);
    bool InitframeBuffers();
    void stipples_new(Solver &solver);
    void stipples_draw(Solver &solver);
    void boxes_new(Solver &solver);
    void boxes_draw(Solver &solver);

    void voronoi_cone_bind(uint16_t n);
    GLuint voronoi_instances(int samples);
    void voronoi_new(Solver& solver);
    void voronoi_draw(Solver& solver);
    void Pixels_new(Solver &solver);
    void pixels_draw(Solver &solver);

    void pixelTexture_new(Solver &solver);
    GLuint setup_texture_emptyWHRGBA32F(int widths_, int heights_);
    void PixelsSort_new(Solver &solver);
    void pixelsSort_draw(Solver &solver);
    void pixelsNeighbor_new(Solver &solver);
    void pixelsNeighbor_draw(Solver &solver);
    GLuint texture32F_new();
};

#endif // GRAPHMODULE_HPP

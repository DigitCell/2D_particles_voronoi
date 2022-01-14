#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#pragma once


#include <GL/glew.h>
#include "structs.hpp"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "glm/ext.hpp"
#include "glm/glm.hpp"

#include <SDL2/SDL.h>
#include "string"

#include "../external/CLI11/CLI11.hpp"
#include "../external/png++/png.hpp"
#include <opencv2/opencv.hpp>


/* OpenGL debug macro */
#ifdef NDEBUG
# define CHECKGLERROR()
#else
# define CHECKGLERROR()    CheckGLError(__FILE__, __LINE__, "", true)
#endif

#define PointData_BP     0
#define PointPos_BP      1
#define ColorList_BP     2
#define MapData_BP       3
#define MapSize_BP       4
#define MapSizeFloat_BP  5
#define Neightb_BP       6
#define NeightbSize_BP   7
#define NeightbSort_BP   8
#define NeightbInfo_BP   9
#define NeightSpec_BP   10
#define DistMapData_BP  11

#define PI  3.14159215
#define PHI 1.61803398


#define particles_init    600//30*50
#define cellMaxParticles 100
#define neightbMaxParticles 800
#define neightbCalcMaxParticles 800

#define resolutionCircle_init 18.0f

#define worldX 1200.0f
#define worldY 1200.0f
#define mapDivider 40
#define particleRadius_init 11.0f

#define maxVelocity_init 1.5f

#define screen_coeff 10.0f


/* Maximum size per shader file (with include). 64 Ko */
#define MAX_SHADER_BUFFERSIZE  (128u*1024u)

static const std::string  path_string="../SDL_voronoi_03/";
static const std::string  path_shader=path_string+"shaders/compute/";

//static const std::string  source_image_file_name="../images/hitl.jpg";

static
const char* GetErrorString(GLenum err) {
#define STRINGIFY(x) #x
  switch (err)
  {
    // [GetError]
    case GL_NO_ERROR:
      return STRINGIFY(GL_NO_ERROR);

    case GL_INVALID_ENUM:
      return STRINGIFY(GL_INVALID_ENUM);

    case GL_INVALID_VALUE:
      return STRINGIFY(GL_INVALID_VALUE);

    case GL_INVALID_OPERATION:
      return STRINGIFY(GL_INVALID_OPERATION);

    case GL_STACK_OVERFLOW:
      return STRINGIFY(GL_STACK_OVERFLOW);

    case GL_STACK_UNDERFLOW:
      return STRINGIFY(GL_STACK_UNDERFLOW);

    case GL_OUT_OF_MEMORY:
      return STRINGIFY(GL_OUT_OF_MEMORY);

    default:
      return "GetErrorString : Unknown constant";
  }
#undef STRINGIFY
}

inline void CheckGLError(const char* file, const int line, const char* errMsg, bool bExitOnFail) {
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    fprintf(stderr,
            "OpenGL error @ \"%s\" [%d] : %s [%s].\n",
            file, line, errMsg, GetErrorString(err));

    if (bExitOnFail) {
      exit(EXIT_FAILURE);
    }
  }
}

#endif // CONSTANTS_HPP

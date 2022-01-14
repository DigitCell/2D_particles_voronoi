#include "graphmodule.hpp"
#include "graphmodule.hpp"
#include "graphmodule.hpp"


GraphModule::GraphModule()
{
    if(!InitSDL())
        printf("Some problems in inint SDL");

    if(!InitImGui())
        printf("Some problems in inint ImGui");

    shaderutils.InitGL();

    InitShaders();
    InitframeBuffers();
    Setup();

}




void GraphModule::voronoi_cone_bind(uint16_t n)
{
    GLuint vbo;
    size_t bytes = (n + 2) * 3 * sizeof(float);
    float* buf = (float*)malloc(bytes);

    /* This is the tip of the cone */
    buf[0] = 0.0f;
    buf[1] = 0.0f;
    buf[2] = -1.0f;

    for (uint16_t i=0; i <= n; ++i)
    {
        float angle = 2.0f * M_PI * i / n;
        buf[i*3 + 3] = cos(angle);
        buf[i*3 + 4] = sin(angle);
        buf[i*3 + 5] = 1.0f;
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, bytes, buf, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    CHECKGLERROR();

    free(buf);
}


GLuint GraphModule::voronoi_instances(int samples)
{
    GLuint vbo;
    size_t bytes = samples * 3 * sizeof(float);
    float* buf = (float*)malloc(bytes);

    /*  Fill the buffer with values between 0 and 1, using        *
     *  rejection sampling to create a good initial distribution  */
    uint16_t i=0;

    float wf=static_cast<float>(width);
    float hf=static_cast<float>(height);

    while (i <samples)
    {
        int x = rand() % width;
        int y = rand() % height;

       // int x = i*(int)(width/2.0f)+(int)(width/4.0f);
       // int y = (int)(height/4.0f);
       uint16_t p =0;// graphModule.img[y*width + x];
    //  uint16_t p = graphModule.img[(3*y)*width + 3*x];

        if ( p>0)
        {
            buf[3*i]     = (x + 0.5f) /wf;
            buf[3*i + 1] = (y + 0.5f) /hf;
            buf[3*i + 2] = 0.0f;
            i++;
        }
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, bytes, buf, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    glVertexAttribDivisor(1, 1);

    free(buf);
    CHECKGLERROR();

    return vbo;
}


void GraphModule::pixelTexture_new(Solver& solver)
{
    solver.dataProcessor.buffers.writeToTexture.id=setup_texture_emptyWHRGBA32F(solver.worldSize.x,solver.worldSize.y);
    solver.dataProcessor.buffers.distMapTexture.id=setup_texture_emptyWHRGBA32F(solver.worldSize.x,solver.worldSize.y);
}

void GraphModule::voronoi_new(Solver& solver)
{

    width=solver.worldSize.x;
    height=solver.worldSize.y;

    //set_aspect_ratio();


    glGenVertexArrays(1, &graphProcessor.voronoi.vao);

    glBindVertexArray(graphProcessor.voronoi.vao);
        //voronoi_cone_bind(cone_resolution);         /* Uses bound VAO   */
        //graphProcessor.voronoi.pts = voronoi_instances(samples);            /* (same) */

    {
        GLuint vbo;
        size_t bytes = (cone_resolution + 2) * 3 * sizeof(float);
        float* buf = (float*)malloc(bytes);

        /* This is the tip of the cone */
        buf[0] = 0.0f;
        buf[1] = 0.0f;
        buf[2] = -1.0f;

        for (uint16_t i=0; i <= cone_resolution; ++i)
        {
            float angle = 2.0f * M_PI * i / cone_resolution;
            buf[i*3 + 3] = cos(angle);
            buf[i*3 + 4] = sin(angle);
            buf[i*3 + 5] = 1.0f;
        }
    /*
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, buf, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    */

        glGenBuffers(1u, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, buf, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        //glEnableVertexAttribArray(0);
       // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);

        size_t const attrib_size = 3 * sizeof(float);
        {
             unsigned int const attrib_pos = 0u;
             glBindVertexBuffer(attrib_pos, vbo, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 3, GL_FLOAT, GL_FALSE, 0);
             glVertexAttribBinding(attrib_pos,0);

             glEnableVertexAttribArray(attrib_pos);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        free(buf);
        CHECKGLERROR();

      }
    {
        size_t const attrib_size = 4 * sizeof(float);
        {
             unsigned int const attrib_pos = 1u;
             glBindVertexBuffer(attrib_pos, solver.dataProcessor.buffers.pointPos.id, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 4, GL_FLOAT, GL_FALSE,0);
             glVertexAttribBinding(attrib_pos,1);
             glEnableVertexAttribArray(attrib_pos);
             glVertexBindingDivisor(1,1);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        CHECKGLERROR();
    }

    glBindVertexArray(0);
    CHECKGLERROR();

    char *src_buffer = new char[MAX_SHADER_BUFFERSIZE]();
    graphProcessor.voronoi.prog = shaderutils.CreateRenderProgram((path_string+"shaders/"+ "voronoi/vs_voronoi.glsl").data(),
                                     (path_string+"shaders/"+ "voronoi/fs_voronoi.glsl").data(),
                                     src_buffer);

    delete [] src_buffer;
    graphProcessor.voronoi.tex   = texture_new();
    graphProcessor.voronoi.depth = texture_new();
    graphProcessor.voronoi.img   = texture_new();

    graphProcessor.textures.source_texture.id=setup_texture_emptyWH(solver.worldSize.x,solver.worldSize.y);

    graphProcessor.voronoi.img =graphProcessor.textures.source_texture.id;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glBindTexture(GL_TEXTURE_2D, graphProcessor.voronoi.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, graphProcessor.voronoi.depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, graphProcessor.voronoi.img);


    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height,
    //             0, GL_RGB, GL_UNSIGNED_BYTE, img);

    GLuint fboIds[2];
    glGenFramebuffers(2, fboIds);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboIds[0]);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, graphProcessor.textures.source_texture.id, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboIds[1]);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, graphProcessor.voronoi.img, 0);

    glBlitFramebuffer(0, 0, width, height,
                      0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glGenFramebuffers(1, &graphProcessor.voronoi.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.voronoi.fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, graphProcessor.voronoi.tex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, graphProcessor.voronoi.depth, 0);
    shaderutils.fbo_check("voronoi");

    CHECKGLERROR();

    shaderutils.teardown(NULL);
}


void GraphModule::voronoi_draw(Solver& solver)
{
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.voronoi.fbo);

    GLint viewport[4];
    //save view port
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, .0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float scaleCone=0.025f*solver.voronoi_scaleradius;
    glUseProgram(graphProcessor.voronoi.prog);
    glBindVertexArray(graphProcessor.voronoi.vao);
    glUniform2f(glGetUniformLocation(graphProcessor.voronoi.prog, "scale"), scaleCone*sx, scaleCone*sy);
    glUniform2f(glGetUniformLocation(graphProcessor.voronoi.prog, "worldSize"), solver.worldSize.x, solver.worldSize.y);
    glUniform2f(glGetUniformLocation(graphProcessor.voronoi.prog, "iResolution"), solver.worldSize.x, solver.worldSize.y);
    glUniform1f(glGetUniformLocation(graphProcessor.voronoi.prog, "radius"),solver.radius);

    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, cone_resolution+2, samples);

    //restore viewport
    shaderutils.teardown(viewport);
    CHECKGLERROR();
}




void GraphModule::stipples_new(Solver& solver)
{

    glGenVertexArrays(1u, &graphProcessor.stipples.vao);
    glBindVertexArray(graphProcessor.stipples.vao);

    int circleResolution=resolutionCircle;
    {   // Make and bind a VBO that draws a simple circle
        GLuint vbo;
        size_t bytes = (2 + circleResolution) * 2 * sizeof(float);
        float* buf = (float*)malloc(bytes);

        buf[0] = 0;
        buf[1] = 0;
        for (size_t i=0; i <=circleResolution; ++i)
        {
            float angle = 2 * M_PI * i / circleResolution;
            buf[i*2 + 0] = cos(angle);
            buf[i*2 + 1] = sin(angle);
        }

        glGenBuffers(1u, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, buf, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        //glEnableVertexAttribArray(0);
       // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);

        size_t const attrib_size = 2 * sizeof(float);
        {
             unsigned int const attrib_pos = 0u;
             glBindVertexBuffer(attrib_pos, vbo, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 2, GL_FLOAT, GL_FALSE, 0);
             glVertexAttribBinding(attrib_pos,0);

             glEnableVertexAttribArray(attrib_pos);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

         CHECKGLERROR();
        free(buf);
    }

    // Bind the Voronoi points array to location 1 in the VAO
    /*
    glBindBuffer(GL_ARRAY_BUFFER, solver.dataProcessor.buffers.pointPos.id);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
    glVertexAttribDivisor(1, 1);
    */
    size_t const attrib_size = 4 * sizeof(float);
    {
         unsigned int const attrib_pos = 1u;
         glBindVertexBuffer(attrib_pos, solver.dataProcessor.buffers.pointPos.id, 0, attrib_size);
         glVertexAttribFormat(attrib_pos, 4, GL_FLOAT, GL_FALSE,0);
         glVertexAttribBinding(attrib_pos,1);
         glEnableVertexAttribArray(attrib_pos);
         glVertexBindingDivisor(1,1);
         glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    char *src_buffer = new char[MAX_SHADER_BUFFERSIZE]();
    graphProcessor.stipples.prog = shaderutils.CreateRenderProgram((path_string+"shaders/"+ "basic/vs_stipples.glsl").data(),
                                     (path_string+"shaders/"+ "basic/fs_stipples.glsl").data(),
                                     src_buffer);
    delete [] src_buffer;

    CHECKGLERROR();

    shaderutils.teardown(NULL);

}

void GraphModule::stipples_draw(Solver& solver)
{

    // bind to framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenStipples.framebuffer);
    glEnable(GL_DEPTH_TEST);
    // make sure we clear the framebuffer's content
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, width, height);
    glClearColor(0.15f, 0.15f, .15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(graphProcessor.stipples.prog);
    glUniform2f(glGetUniformLocation(graphProcessor.stipples.prog, "radius"), solver.radius * sx, solver.radius * sy);
    glUniform2f(glGetUniformLocation(graphProcessor.stipples.prog, "worldSize"), solver.worldSize.x, solver.worldSize.y);
    glBindVertexArray(graphProcessor.stipples.vao);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, resolutionCircle+2, samples);

    CHECKGLERROR();

    // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
     //clear all relevant buffers
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
     glClear(GL_COLOR_BUFFER_BIT);

     shaderutils.teardown(viewport);

     CHECKGLERROR();
}



void GraphModule::boxes_new(Solver& solver)
{

    glGenVertexArrays(1u, &graphProcessor.boxes.vao);
    glBindVertexArray(graphProcessor.boxes.vao);

    int circleResolution=resolutionCircle;
    {   // Make and bind a VBO that draws a simple circle

        GLfloat verts[] = {-1.0f, -1.0f,     1.0f, -1.0f,
                            1.0f,  1.0f,    -1.0f,  1.0f};

        GLuint vbo;
        size_t bytes =sizeof(verts);

        glGenBuffers(1u, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, verts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        //glEnableVertexAttribArray(0);
       // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);

        size_t const attrib_size = 2 * sizeof(float);
        {
             unsigned int const attrib_pos = 0u;
             glBindVertexBuffer(attrib_pos, vbo, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 2, GL_FLOAT, GL_FALSE, 0);
             glVertexAttribBinding(attrib_pos,0);

             glEnableVertexAttribArray(attrib_pos);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        CHECKGLERROR();
    }

    size_t const attrib_size = 1 * sizeof(float);
    {
         unsigned int const attrib_pos = 1u;
         glBindVertexBuffer(attrib_pos, solver.dataProcessor.buffers.mapSizeFloat.id, 0, attrib_size);
         glVertexAttribFormat(attrib_pos, 1, GL_FLOAT, GL_FALSE,0);
         glVertexAttribBinding(attrib_pos,1);
         glEnableVertexAttribArray(attrib_pos);
         glVertexBindingDivisor(1,1);
         glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    char *src_buffer = new char[MAX_SHADER_BUFFERSIZE]();
    graphProcessor.boxes.prog = shaderutils.CreateRenderProgram((path_string+"shaders/"+ "basic/vs_boxes.glsl").data(),
                                     (path_string+"shaders/"+ "basic/fs_boxes.glsl").data(),
                                     src_buffer);
    delete [] src_buffer;

    CHECKGLERROR();

    shaderutils.teardown(NULL);

}

void GraphModule::boxes_draw(Solver& solver)
{

    // bind to framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenQuad.framebuffer);
    glEnable(GL_DEPTH_TEST);
    // make sure we clear the framebuffer's content
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, width, height);
    glClearColor(0.15f, 0.15f, .15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(drawParticlesonMap)
    {
        glUseProgram(graphProcessor.stipples.prog);
        glUniform2f(glGetUniformLocation(graphProcessor.stipples.prog, "radius"), solver.radius * sx, solver.radius * sy);
        glUniform2f(glGetUniformLocation(graphProcessor.stipples.prog, "worldSize"), solver.worldSize.x, solver.worldSize.y);
        glBindVertexArray(graphProcessor.stipples.vao);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, resolutionCircle+2, samples);

        CHECKGLERROR();
    }


    glUseProgram(graphProcessor.boxes.prog);
    glUniform2f(glGetUniformLocation(graphProcessor.boxes.prog, "worldSize"), solver.worldSize.x, solver.worldSize.y);
    glUniform1i(glGetUniformLocation(graphProcessor.boxes.prog, "divider"), mapDivider);
    glBindVertexArray(graphProcessor.boxes.vao);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, solver.mapElementsAmount);

    CHECKGLERROR();



    // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
     //clear all relevant buffers
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
     glClear(GL_COLOR_BUFFER_BIT);

     shaderutils.teardown(viewport);

     CHECKGLERROR();
}


void GraphModule::Pixels_new(Solver& solver)
{

    glGenVertexArrays(1u, &graphProcessor.pixels.vao);
    glBindVertexArray(graphProcessor.pixels.vao);

    {   // Make and bind a VBO that draws a simple circle

        GLfloat verts[] = {-1.0f, -1.0f,
                           -1.0f,  1.0f,
                            1.0f, -1.0f,

                            1.0f, -1.0f,
                           -1.0,   1.0f,
                            1.0,   1.0f

                          };

        GLuint vbo;
        size_t bytes =sizeof(verts);

        glGenBuffers(1u, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, verts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        //glEnableVertexAttribArray(0);
       // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);

        size_t const attrib_size = 2 * sizeof(float);
        {
             unsigned int const attrib_pos = 0u;
             glBindVertexBuffer(attrib_pos, vbo, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 2, GL_FLOAT, GL_FALSE, 0);
             glVertexAttribBinding(attrib_pos,0);

             glEnableVertexAttribArray(attrib_pos);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        CHECKGLERROR();
    }

    {   // Make and bind a VBO that draws a simple circle

        GLfloat norms[] = { 0.0, 0.0,
                            0.0, 1.0,
                            1.0, 0.0,

                            1.0, 0.0,
                            0.0, 1.0,
                            1.0, 1.0
                          };


        GLuint vbo;
        size_t bytes =sizeof(norms);

        glGenBuffers(1u, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, norms, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        size_t const attrib_size = 2 * sizeof(float);
        {
             unsigned int const attrib_pos = 1u;
             glBindVertexBuffer(attrib_pos, vbo, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 2, GL_FLOAT, GL_FALSE, 0);
             glVertexAttribBinding(attrib_pos,1);

             glEnableVertexAttribArray(attrib_pos);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        CHECKGLERROR();
    }



    glBindVertexArray(0);

    char *src_buffer = new char[MAX_SHADER_BUFFERSIZE]();
    graphProcessor.pixels.prog = shaderutils.CreateRenderProgram((path_string+"shaders/"+ "voronoi/vs_pixels.glsl").data(),
                                     (path_string+"shaders/"+ "voronoi/fs_pixels0.glsl").data(),
                                     src_buffer);
    delete [] src_buffer;

    CHECKGLERROR();

    shaderutils.teardown(NULL);

}

void GraphModule::pixels_draw(Solver& solver)
{

    // bind to framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenPixel.framebuffer);
    glEnable(GL_DEPTH_TEST);
    // make sure we clear the framebuffer's content
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, .0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
   // glBindTexture(GL_TEXTURE_2D, solver.dataProcessor.buffers.imageTexture.img);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.voronoi.depth);

    glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, solver.dataProcessor.buffers.imageTexture.img);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.voronoi.tex);

    glUseProgram(graphProcessor.pixels.prog);

    glUniform2f(glGetUniformLocation(graphProcessor.pixels.prog, "iResolution"), solver.worldSize.x, solver.worldSize.y);
   // glUniform2f(glGetUniformLocation(graphProcessor.pixels.prog, "worldSize"), solver.worldSize.x, solver.worldSize.y);
    //glUniform1i(glGetUniformLocation(graphProcessor.pixels.prog, "divider"), mapDivider);
    glUniform1i(glGetUniformLocation(graphProcessor.pixels.prog, "compTexture"),1);
    glUniform1i(glGetUniformLocation(graphProcessor.pixels.prog, "compTexture0"),0);


    glBindVertexArray(graphProcessor.pixels.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    CHECKGLERROR();


    // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
     //clear all relevant buffers
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
     glClear(GL_COLOR_BUFFER_BIT);

     shaderutils.teardown(viewport);

     glBindTexture(GL_TEXTURE_2D, 0);
     // glDisable(GL_TEXTURE_2D);

     CHECKGLERROR();
}

void GraphModule::PixelsSort_new(Solver& solver)
{

    glGenVertexArrays(1u, &graphProcessor.pixelsSort.vao);
    glBindVertexArray(graphProcessor.pixelsSort.vao);

    {   // Make and bind a VBO that draws a simple circle

        GLfloat verts[] = {-1.0f, -1.0f,
                           -1.0f,  1.0f,
                            1.0f, -1.0f,

                            1.0f, -1.0f,
                           -1.0,   1.0f,
                            1.0,   1.0f

                          };

        GLuint vbo;
        size_t bytes =sizeof(verts);

        glGenBuffers(1u, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, verts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        //glEnableVertexAttribArray(0);
       // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);

        size_t const attrib_size = 2 * sizeof(float);
        {
             unsigned int const attrib_pos = 0u;
             glBindVertexBuffer(attrib_pos, vbo, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 2, GL_FLOAT, GL_FALSE, 0);
             glVertexAttribBinding(attrib_pos,0);

             glEnableVertexAttribArray(attrib_pos);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        CHECKGLERROR();
    }

    {   // Make and bind a VBO that draws a simple circle

        GLfloat norms[] = { 0.0, 0.0,
                            0.0, 1.0,
                            1.0, 0.0,

                            1.0, 0.0,
                            0.0, 1.0,
                            1.0, 1.0
                          };


        GLuint vbo;
        size_t bytes =sizeof(norms);

        glGenBuffers(1u, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, norms, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        size_t const attrib_size = 2 * sizeof(float);
        {
             unsigned int const attrib_pos = 1u;
             glBindVertexBuffer(attrib_pos, vbo, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 2, GL_FLOAT, GL_FALSE, 0);
             glVertexAttribBinding(attrib_pos,1);

             glEnableVertexAttribArray(attrib_pos);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        CHECKGLERROR();
    }



    glBindVertexArray(0);

    char *src_buffer = new char[MAX_SHADER_BUFFERSIZE]();
    graphProcessor.pixelsSort.prog = shaderutils.CreateRenderProgram((path_string+"shaders/"+ "voronoi/vs_pixelsSort.glsl").data(),
                                     (path_string+"shaders/"+ "voronoi/fs_pixelsSort.glsl").data(),
                                     src_buffer);
    delete [] src_buffer;

    CHECKGLERROR();

    shaderutils.teardown(NULL);

}

void GraphModule::pixelsSort_draw(Solver& solver)
{

    // bind to framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenPixelSort.framebuffer);
   // glEnable(GL_DEPTH_TEST);
    // make sure we clear the framebuffer's content
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, width, height);
    //glClearColor(0.0f, 0.0f, .0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(solver.Tick==0)
    {
      glClearColor(0.0f, 0.0f, .0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, solver.dataProcessor.buffers.writeToTexture.id);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.renderscreenPixelNeighbor.textureColorbuffer);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.renderscreenPixelSort.textureColorbuffer);

    glUseProgram(graphProcessor.pixelsSort.prog);

    glUniform2f(glGetUniformLocation(graphProcessor.pixelsSort.prog, "iResolution"), solver.worldSize.x, solver.worldSize.y);
    glUniform1i(glGetUniformLocation(graphProcessor.pixelsSort.prog, "iFrame"), solver.Tick);
    glUniform1i(glGetUniformLocation(graphProcessor.pixelsSort.prog, "compTexture0"),0);
    glUniform1i(glGetUniformLocation(graphProcessor.pixelsSort.prog, "compTexture1"),1);
    glUniform1i(glGetUniformLocation(graphProcessor.pixelsSort.prog, "compTexture2"),2);


    glBindVertexArray(graphProcessor.pixelsSort.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    CHECKGLERROR();


    // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     //glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
     //clear all relevant buffers
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
     //glClear(GL_COLOR_BUFFER_BIT);

     shaderutils.teardown(viewport);

     glBindTexture(GL_TEXTURE_2D, 0);
     // glDisable(GL_TEXTURE_2D);

     CHECKGLERROR();
}


void GraphModule::pixelsNeighbor_new(Solver& solver)
{

    glGenVertexArrays(1u, &graphProcessor.pixelsNeighbor.vao);
    glBindVertexArray(graphProcessor.pixelsNeighbor.vao);

    {   // Make and bind a VBO that draws a simple circle

        GLfloat verts[] = {-1.0f, -1.0f,
                           -1.0f,  1.0f,
                            1.0f, -1.0f,

                            1.0f, -1.0f,
                           -1.0,   1.0f,
                            1.0,   1.0f

                          };

        GLuint vbo;
        size_t bytes =sizeof(verts);

        glGenBuffers(1u, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, verts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        //glEnableVertexAttribArray(0);
       // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);

        size_t const attrib_size = 2 * sizeof(float);
        {
             unsigned int const attrib_pos = 0u;
             glBindVertexBuffer(attrib_pos, vbo, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 2, GL_FLOAT, GL_FALSE, 0);
             glVertexAttribBinding(attrib_pos,0);

             glEnableVertexAttribArray(attrib_pos);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        CHECKGLERROR();
    }

    {   // Make and bind a VBO that draws a simple circle

        GLfloat norms[] = { 0.0, 0.0,
                            0.0, 1.0,
                            1.0, 0.0,

                            1.0, 0.0,
                            0.0, 1.0,
                            1.0, 1.0
                          };


        GLuint vbo;
        size_t bytes =sizeof(norms);

        glGenBuffers(1u, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bytes, norms, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        size_t const attrib_size = 2 * sizeof(float);
        {
             unsigned int const attrib_pos = 1u;
             glBindVertexBuffer(attrib_pos, vbo, 0, attrib_size);
             glVertexAttribFormat(attrib_pos, 2, GL_FLOAT, GL_FALSE, 0);
             glVertexAttribBinding(attrib_pos,1);

             glEnableVertexAttribArray(attrib_pos);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        CHECKGLERROR();
    }



    glBindVertexArray(0);

    char *src_buffer = new char[MAX_SHADER_BUFFERSIZE]();
    graphProcessor.pixelsNeighbor.prog = shaderutils.CreateRenderProgram((path_string+"shaders/"+ "voronoi/vs_pixelsNeighbor.glsl").data(),
                                     (path_string+"shaders/"+ "voronoi/fs_pixelsNeighbor.glsl").data(),
                                     src_buffer);
    delete [] src_buffer;

    CHECKGLERROR();

    shaderutils.teardown(NULL);

}

void GraphModule::pixelsNeighbor_draw(Solver& solver)
{


    // bind to framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenPixelNeighbor.framebuffer);
    //glEnable(GL_DEPTH_TEST);
    // make sure we clear the framebuffer's content
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, width, height);
    if(solver.Tick==0)
    {
      glClearColor(0.0f, 0.0f, .0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, solver.dataProcessor.buffers.writeToTexture.id);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.renderscreenPixelNeighbor.textureColorbuffer);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.renderscreenPixelSort.textureColorbuffer);

    glUseProgram(graphProcessor.pixelsNeighbor.prog);

    glUniform2f(glGetUniformLocation(graphProcessor.pixelsNeighbor.prog, "iResolution"), solver.worldSize.x, solver.worldSize.y);
    glUniform1i(glGetUniformLocation(graphProcessor.pixelsNeighbor.prog, "iFrame"), solver.Tick);
    glUniform1i(glGetUniformLocation(graphProcessor.pixelsNeighbor.prog, "compTexture0"),0);
    glUniform1i(glGetUniformLocation(graphProcessor.pixelsNeighbor.prog, "compTexture1"),1);
    glUniform1i(glGetUniformLocation(graphProcessor.pixelsNeighbor.prog, "compTexture2"),2);

    glBindVertexArray(graphProcessor.pixelsNeighbor.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    CHECKGLERROR();

    // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     //glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
     //clear all relevant buffers
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    // glClear(GL_COLOR_BUFFER_BIT);

     shaderutils.teardown(viewport);

     glBindTexture(GL_TEXTURE_2D, 0);
     // glDisable(GL_TEXTURE_2D);

     CHECKGLERROR();
}


bool GraphModule::InitframeBuffers()
{

    width=worldX;
    height=worldY;
    // framebuffer configuration Stipples

    glGenFramebuffers(1, &graphProcessor.renderscreenStipples.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenStipples.framebuffer);
    // create a color attachment texture

    glGenTextures(1, &graphProcessor.renderscreenStipples.textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.renderscreenStipples.textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, graphProcessor.renderscreenStipples.textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &graphProcessor.renderscreenStipples.rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, graphProcessor.renderscreenStipples.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, graphProcessor.renderscreenStipples.rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CHECKGLERROR();

    // framebuffer configuration Quard

    glGenFramebuffers(1, &graphProcessor.renderscreenQuad.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenQuad.framebuffer);
    // create a color attachment texture

    glGenTextures(1, &graphProcessor.renderscreenQuad.textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.renderscreenQuad.textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, graphProcessor.renderscreenQuad.textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

    glGenRenderbuffers(1, &graphProcessor.renderscreenQuad.rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, graphProcessor.renderscreenQuad.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, graphProcessor.renderscreenQuad.rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CHECKGLERROR();


    // framebuffer configuration Pixel

    glGenFramebuffers(1, &graphProcessor.renderscreenPixel.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenPixel.framebuffer);
    // create a color attachment texture

    glGenTextures(1, &graphProcessor.renderscreenPixel.textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.renderscreenPixel.textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, graphProcessor.renderscreenPixel.textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

    glGenRenderbuffers(1, &graphProcessor.renderscreenPixel.rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, graphProcessor.renderscreenPixel.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, graphProcessor.renderscreenPixel.rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CHECKGLERROR();

    // framebuffer configuration Pixel

    glGenFramebuffers(1, &graphProcessor.renderscreenPixelSort.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenPixelSort.framebuffer);
    // create a color attachment texture

    glGenTextures(1, &graphProcessor.renderscreenPixelSort.textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.renderscreenPixelSort.textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, graphProcessor.renderscreenPixelSort.textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

    glGenRenderbuffers(1, &graphProcessor.renderscreenPixelSort.rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, graphProcessor.renderscreenPixelSort.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, graphProcessor.renderscreenPixelSort.rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CHECKGLERROR();

    // framebuffer configuration Pixel

    glGenFramebuffers(1, &graphProcessor.renderscreenPixelNeighbor.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, graphProcessor.renderscreenPixelNeighbor.framebuffer);
    // create a color attachment texture

    glGenTextures(1, &graphProcessor.renderscreenPixelNeighbor.textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, graphProcessor.renderscreenPixelNeighbor.textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, graphProcessor.renderscreenPixelNeighbor.textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

    glGenRenderbuffers(1, &graphProcessor.renderscreenPixelNeighbor.rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, graphProcessor.renderscreenPixelNeighbor.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, graphProcessor.renderscreenPixelNeighbor.rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CHECKGLERROR();


}




bool GraphModule::InitShaders()
{

    /* Setup programs */
    char *src_buffer = new char[MAX_SHADER_BUFFERSIZE]();
    graphProcessor.shaders.grid = shaderutils.CreateRenderProgram((path_string+"shaders/"+ "grid/vs_grid.glsl").data(),
                                    (path_string+"shaders/"+ "grid/fs_grid.glsl").data(),
                                    src_buffer);

    graphProcessor.shaders.basic = shaderutils.CreateRenderProgram((path_string+"shaders/"+ "basic/vs_basic.glsl").data(),
                                     (path_string+"shaders/"+ "basic/fs_basic.glsl").data(),
                                     src_buffer);

    graphProcessor.shaders.basic_color = shaderutils.CreateRenderProgram((path_string+"shaders/"+ "basic/vs_basic_color.glsl").data(),
                                     (path_string+"shaders/"+ "basic/fs_basic_color.glsl").data(),
                                     src_buffer);
    delete [] src_buffer;

    /* Shaders uniform location */


    graphProcessor.uniforms.grid.mvp           = shaderutils.GetUniformLocation(graphProcessor.shaders.grid, "uMVP");
    graphProcessor.uniforms.grid.scaleFactor   = shaderutils.GetUniformLocation(graphProcessor.shaders.grid, "uScaleFactor");


    graphProcessor.uniforms.basic.color        = shaderutils.GetUniformLocation(graphProcessor.shaders.basic, "uColor");
    graphProcessor.uniforms.basic.mvp          = shaderutils.GetUniformLocation(graphProcessor.shaders.basic, "uMVP");
    graphProcessor.uniforms.basic.scaleFactor   = shaderutils.GetUniformLocation(graphProcessor.shaders.basic, "uScaleFactor");

    graphProcessor.uniforms.basic_color.mvp           = shaderutils.GetUniformLocation(graphProcessor.shaders.basic_color, "uMVP");
    graphProcessor.uniforms.basic_color.scaleFactor   = shaderutils.GetUniformLocation(graphProcessor.shaders.basic_color, "uScaleFactor");


    return true;
}

bool GraphModule::Setup()
{
    setup_grid_geometry();
    return true;
}


bool GraphModule::InitSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }

    const char* glsl_version = "#version 150";
    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);


    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    screen = SDL_CreateWindow("SPH", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mainScreenWidth, mainScreenHeight, window_flags);
    gContext = SDL_GL_CreateContext(screen);

    SDL_GL_MakeCurrent(screen, gContext);
    SDL_GL_SetSwapInterval(1);

    GLenum err;
    glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to init GLEW" << std::endl;
        SDL_GL_DeleteContext(gContext);
        SDL_DestroyWindow(screen);
        SDL_Quit();
        return true;
    }


    //Main loop flag
    bool quit = false;
    SDL_Event e;
    SDL_StartTextInput();

    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    std::cout << "OpenGL version       | " << major << "." << minor << std::endl;
    std::cout << "GLEW version         | " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "---------------------+-------" << std::endl;

    return true;
}

bool GraphModule::InitImGui()
{
    // Setup Dear ImGui context
       IMGUI_CHECKVERSION();
       ImGui::CreateContext();
       ImGui::LoadIniSettingsFromDisk("tempImgui.ini");
       ImGuiIO& io = ImGui::GetIO(); (void)io;
       static ImGuiStyle* style = &ImGui::GetStyle();
       style->Alpha = 1.00f; //0.75f

       io.WantCaptureMouse=true;
       //io.WantCaptureKeyboard=false;
       //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
       //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

       // Setup Dear ImGui style
       ImGui::StyleColorsDark();
       //ImGui::StyleColorsClassic();

       // Setup Platform/Renderer backends
       ImGui_ImplSDL2_InitForOpenGL(screen, gContext);
       ImGui_ImplOpenGL3_Init();

       return true;
}


void GraphModule::ClearRenderScreen(){
    if(clearScreen)
    {
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void GraphModule::Render(glm::mat4x4 const &view, glm::mat4x4 const& viewProj){

    glm::mat4x4 mvp;
    glm::mat4x4 model;
    glm::vec4 color;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // -- SOLID OBJECTS

    //Grid
    if (debug_parameters_.show_grid)
      draw_grid(viewProj);

      //model=glm::translate(viewProj, glm::vec3(10.0,0,0));
     // model=glm::scale(model, glm::vec3(10.0,1.0,1.0));
     // draw_wirecube(viewProj, glm::vec4(1.0,1.0,1.0,1.0));

    // -- TRANSPARENT OBJECTS

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

}


void GraphModule::GuiRender(Sim_parameters& sim_parameters, Solver& solver)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(screen);
    ImGui::NewFrame();

    {
        ImGui::Begin("Mesh parameters");

            ImGui::Text("Framerate  : %.1f ms or %.1f Hz", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Separator();

            ImGui::Checkbox("Run Simualtion", &sim_parameters.runSimulation);
            ImGui::Checkbox("Draw particles on map", &drawParticlesonMap);

            if(ImGui::Button("Next Step"))
                    sim_parameters.nextStep=true;


            ImGui::SliderFloat("Max velocity length",&sim_parameters.maxVelocity, 0.0f, 47.5f);
            ImGui::SliderFloat("Particle radius",&sim_parameters.radius, 0.5f, 35.0f);
            ImGui::SliderFloat("Voronoi radius",&solver.voronoi_scaleradius, 0.15f, 5.0f);
            ImGui::SliderFloat("Stiffness Coeff",&sim_parameters.stiffnessCoeff, 0.00f, 0.5f);
            ImGui::SliderFloat("Damping Coeff",&sim_parameters.dampingCoeff, 0.0f, 1.5f);

            solver.radius=sim_parameters.radius;
            ImGui::SliderFloat("Noise Coeff",&sim_parameters.noiseCoeff, 0.0f, 2.5f);
            ImGui::SliderFloat("delta time",&sim_parameters.dt, 0.0f, 2.5f);
            ImGui::SliderFloat("Scale Window 01",&scaleWindow01, 0.1f, 1.5f);
            ImGui::SliderFloat("Scale Window 02",&scaleWindow02, 0.1f, 1.5f);


        ImGui::End();

        ImVec2 uv0 = ImVec2(10.0f/256.0f, 10.0f/256.0f);

        // Normalized coordinates of pixel (110,210) in a 256x256 texture.
        ImVec2 uv1 = ImVec2((10.0f+100.0f)/256.0f, (10.0f+200.0f)/256.0f);
/*
        ImGui::Begin("Texture 2 viewer");
            ImGui::Text("pointer = %p", solver.function_image.id);
            ImGui::Text("size = %d x %d", solver.function_image.width,  solver.function_image.height);
            ImGui::Image((void*)solver.function_image.id, ImVec2(solver.function_image.width, solver.function_image.height));
        ImGui::End();
*/

        ImGui::Begin("Particle viewer");
           ImGui::Text("pointer = %p",   graphProcessor.renderscreenStipples.textureColorbuffer);
           ImGui::Text("size = %d x %d", height, width);
           ImGui::Image((void*)(intptr_t)graphProcessor.renderscreenStipples.textureColorbuffer, ImVec2(scaleWindow01*height, scaleWindow01*width));
        ImGui::End();

        ImGui::Begin("Map viewer");
           ImGui::Text("pointer = %p",   graphProcessor.renderscreenQuad.textureColorbuffer);
           ImGui::Text("size = %d x %d", height, width);
           ImGui::Image((void*)(intptr_t)graphProcessor.renderscreenQuad.textureColorbuffer, ImVec2(scaleWindow01*height, scaleWindow01*width));
        ImGui::End();

        ImGui::Begin("Voronoi viewer");
           ImGui::Text("pointer = %p",   graphProcessor.voronoi.tex);
           ImGui::Text("size = %d x %d", height, width);
           ImGui::Image((void*)(intptr_t)graphProcessor.voronoi.tex, ImVec2(scaleWindow01*height, scaleWindow01*width));
        ImGui::End();
/*
        ImGui::Begin("Pixels viewer");
           ImGui::Text("pointer = %p",   graphProcessor.renderscreenPixel.textureColorbuffer);
           ImGui::Text("size = %d x %d", height, width);
           ImGui::Image((void*)(intptr_t) graphProcessor.renderscreenPixel.textureColorbuffer, ImVec2(scaleWindow01*height, scaleWindow01*width));
        ImGui::End();

        ImGui::Begin("PixelsSort viewer");
           ImGui::Text("pointer = %p",  graphProcessor.voronoi.depth);
           ImGui::Text("size = %d x %d", height, width);
           ImGui::Image((void*)(intptr_t) graphProcessor.voronoi.depth, ImVec2(scaleWindow01*height, scaleWindow01*width));
        ImGui::End();
*/

        ImGui::Begin("Pixels distMapToTexture");
           ImGui::Text("pointer = %p",   solver.dataProcessor.buffers.distMapTexture.id);
           ImGui::Text("size = %d x %d", height, width);
           ImGui::Image((void*)(intptr_t) solver.dataProcessor.buffers.distMapTexture.id, ImVec2(scaleWindow02*height, scaleWindow02*width));
        ImGui::End();

        ImGui::Begin("Pixels writeToTexture");
           ImGui::Text("pointer = %p",   solver.dataProcessor.buffers.writeToTexture.id);
           ImGui::Text("size = %d x %d", height, width);
           ImGui::Image((void*)(intptr_t) solver.dataProcessor.buffers.writeToTexture.id, ImVec2(scaleWindow01*height, scaleWindow01*width));
        ImGui::End();



    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



void GraphModule::CloseRender()
{
    //close program, return true

    glDeleteVertexArrays(1u, &graphProcessor.buffers.grid.vao);
    glDeleteBuffers(1u, &graphProcessor.buffers.grid.vbo);


    SDL_StopTextInput();
    SDL_DestroyWindow(screen);
    screen = NULL;
    SDL_Quit();

}

void  GraphModule::HandleEvents(SDL_Event e, TEventData& camera_event){

   bool &bRotatePressed = camera_event.bRotatePressed;
   if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT)
       bRotatePressed=true;
   if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT)
       bRotatePressed=false;
   //bRotatePressed = bRotatePressed || (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT);
  // bRotatePressed = bRotatePressed && (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT);

   bool &bTranslatePressed = camera_event.bTranslatePressed;
   if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_MIDDLE)
       bTranslatePressed=true;
   if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_MIDDLE)
       bTranslatePressed=false;
   //bTranslatePressed = bTranslatePressed || (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_MIDDLE);
   //bTranslatePressed = bTranslatePressed && (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_MIDDLE);

   if(e.type==SDL_MOUSEMOTION){
       camera_event.mouseX = static_cast<float>(e.motion.x);
       camera_event.mouseY = static_cast<float>(e.motion.y);
       camera_event.bMouseMove = true;
   }

   if(e.type==SDL_MOUSEWHEEL)
   {
       camera_event.wheelDelta=0.5f*e.wheel.y;
   }

   if (e.type == SDL_KEYDOWN){
       if (e.key.keysym.sym == SDLK_LCTRL){
           bTranslatePressed=true;
       }
   }

   if (e.type == SDL_KEYUP){
       if (e.key.keysym.sym == SDLK_LCTRL){
           bTranslatePressed=false;
       }
   }

}



void GraphModule::setup_grid_geometry() {
  // size taken in world space
  float const world_size = 1.0f;

  unsigned int const &res          = 32u; //static_cast<unsigned int>(gpu_particle_->simulation_box_size()) / 2u;
  unsigned int const &num_vertices = 4u * (res + 1u);
  unsigned int const num_component = 2u;
  unsigned int const buffersize    = num_vertices * num_component;
  std::vector<float> vertices(buffersize);

  graphProcessor.buffers.grid.resolution = res;
  graphProcessor.buffers.grid.nvertices  = static_cast<GLsizei>(num_vertices); //

  float const cell_padding = world_size / res;
  float const offset = cell_padding * (res/2.0f);

  for (unsigned int i=0u; i<=res; ++i) {
    unsigned int const index = 4u * num_component * i;
    float const cursor = cell_padding * i - offset;

    // horizontal lines
    vertices[index + 0u] = - offset;
    vertices[index + 1u] = cursor;
    vertices[index + 2u] = + offset;
    vertices[index + 3u] = cursor;
    // vertical lines
    vertices[index + 4u] = cursor;
    vertices[index + 5u] = - offset;
    vertices[index + 6u] = cursor;
    vertices[index + 7u] = + offset;
  }

  // Allocate Storage.
  glGenBuffers(1u, &graphProcessor.buffers.grid.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, graphProcessor.buffers.grid.vbo);
    GLsizeiptr const bytesize = static_cast<GLsizeiptr>(vertices.size() * sizeof(vertices[0u]));
    glBufferStorage(GL_ARRAY_BUFFER, bytesize, vertices.data(), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0u);

  // Set attribute locations.
  glGenVertexArrays(1u, &graphProcessor.buffers.grid.vao);
  glBindVertexArray(graphProcessor.buffers.grid.vao);
  {
    size_t const attrib_size = num_component * sizeof(vertices[0u]);
    glBindVertexBuffer(0, graphProcessor.buffers.grid.vbo, 0u, attrib_size);
    {
      size_t const attrib_pos = 0u;
      glVertexAttribFormat(attrib_pos, num_component, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(attrib_pos, 0);
      glEnableVertexAttribArray(attrib_pos);
    }
  }
  glBindVertexArray(0u);
}



GLuint GraphModule::texture_new()
{
    GLuint tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return tex;
}

GLuint GraphModule::texture32F_new()
{
    GLuint tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return tex;
}

GLuint  GraphModule::setup_texture_emptyWH(int widths_, int heights_) {

    int widths, heights, nrChannels;
    width=widths_;
    height=heights_;

    GLuint id=texture_new();

    GLsizei const w = static_cast<GLsizei>(width);
    GLsizei const h = static_cast<GLsizei>(height);

    glBindTexture(GL_TEXTURE_2D,   id);
    glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, w, h, 0, GL_RGB,  GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0u);
    CHECKGLERROR();

    return id;
}


GLuint  GraphModule::setup_texture_emptyWHRGBA32F(int widths_, int heights_) {

    int widths, heights, nrChannels;
    width=widths_;
    height=heights_;

    GLuint id=texture_new();

    GLsizei const w = static_cast<GLsizei>(width);
    GLsizei const h = static_cast<GLsizei>(height);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0,   GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    //glTexStorage2D(GL_TEXTURE_2D, 2, GL_RGBA32F, w, h);
    glBindTexture(GL_TEXTURE_2D, 0u);
    CHECKGLERROR();

    return id;
}

void GraphModule::setup_texture() {
#if 0
  unsigned int const res = sprite_width*sprite_height;
  char *pixels = new char[3u*res];
  char *texdata = new char[res];

  for (unsigned int i=0u; i < res; ++i) {
    char *px = pixels + 3*i;
    HEADER_PIXEL(sprite_data, px);
    texdata[i] = *px;
  }
  delete [] pixels;

  GLsizei const w = static_cast<GLsizei>(sprite_width);
  GLsizei const h = static_cast<GLsizei>(sprite_height);
  glGenTextures(1u, &gl_sprite_tex_);
  glBindTexture(GL_TEXTURE_2D, gl_sprite_tex_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexStorage2D(GL_TEXTURE_2D, 4u, GL_R8, w, h);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED, GL_UNSIGNED_BYTE, texdata);
    glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0u);

  delete [] texdata;

  // bind here just for testing.
  glBindTexture(GL_TEXTURE_2D, gl_sprite_tex_);

  CHECKGLERROR();
#endif
}

void GraphModule::draw_grid(glm::mat4x4 const &mvp) {
  float constexpr kDefaultSimulationVolumeSize = 128.0f;
  glUseProgram(graphProcessor.shaders.grid);
  {
    glUniformMatrix4fv(graphProcessor.uniforms.grid.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1f(graphProcessor.uniforms.grid.scaleFactor, kDefaultSimulationVolumeSize);

    glBindVertexArray(graphProcessor.buffers.grid.vao);
      glDrawArrays(GL_LINES, 0, graphProcessor.buffers.grid.nvertices);
    glBindVertexArray(0u);
  }
  glUseProgram(0u);

  CHECKGLERROR();
}


void GraphModule::RenderMeshCut(graphProcessor_::buffers_::MeshBase_& meshIn,glm::vec3 translate, glm::mat4x4 const &view, glm::mat4x4 const& viewProj){
    glm::mat4x4 mvp;
    glm::vec4 color=glm::vec4(0.3,1.0,0.5,1.0);
    glm::vec4 color2=glm::vec4(0.3,1.0,1.0,1.0);

    const float radius_scale=0.95f;

    glm::mat4x4 model;
    model=glm::translate(viewProj, translate);
    model=glm::scale(model, glm::vec3(radius_scale,radius_scale,radius_scale));

    draw_mesh(meshIn, translate, model, glm::vec4(0.3,1.0,0.5,1.0), true);

    CHECKGLERROR();

}


void GraphModule::draw_mesh( graphProcessor_::buffers_::MeshBase_& meshIn, glm::vec3 translate, glm::mat4x4 const &mvp, glm::vec4 const &color, bool bFill) {
  GLenum mode = (bFill) ? GL_TRIANGLE_STRIP : GL_LINES;

  glm::mat4 model(1.0);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  const glm::vec3 plane=glm::vec3(1,0,0);

  model=glm::translate(initView, glm::vec3(0.0, 0.0,-200.0));
  model=glm::rotate(model, glm::radians(30.0f), plane);
  model=glm::scale(model, glm::vec3(0.3,0.3,0.3));
/*
  glUseProgram(pgm_.terrain);
  {
      glUniformMatrix4fv(ulocation_.terrain.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
      glUniformMatrix4fv(ulocation_.terrain.modelView, 1, GL_FALSE, glm::value_ptr(model));
      glUniform4fv(ulocation_.terrain.color, 1u, glm::value_ptr(color));
      glUniform1f(ulocation_.terrain.scaleFactor, kDefaultSimulationVolumeSize);

     //glLineWidth(12.0f);
     glBindVertexArray(meshIn.vao);
         //  glDrawElements(GL_POINTS, meshIn.nindices-1, GL_UNSIGNED_INT, 0 );
               glDrawElements(GL_LINES, meshIn.nindices, GL_UNSIGNED_INT, 0 );
         //    glDrawElements(GL_TRIANGLES, meshIn.nindices , GL_UNSIGNED_INT, 0);
     glBindVertexArray(0u);
  }
  glUseProgram(0u);
  */

  glUseProgram(graphProcessor.shaders.basic_color);
  {
      glUniformMatrix4fv(graphProcessor.uniforms.basic_color.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
      glUniform1f(graphProcessor.uniforms.basic_color.scaleFactor, 25.0f);

     //glLineWidth(12.0f);
     glBindVertexArray(meshIn.vao);

          // glDrawElements(GL_POINTS, meshIn.nindices, GL_UNSIGNED_INT, 0 );
         //     glDrawElements(GL_LINES, meshIn.nindices, GL_UNSIGNED_INT, 0 );

         if(drawWireFrame)
            glDrawElements(GL_LINES, meshIn.nindices, GL_UNSIGNED_INT, 0 );
         else
            glDrawElements(GL_TRIANGLES, meshIn.nindices , GL_UNSIGNED_INT, 0);
     glBindVertexArray(0u);
  }
  glUseProgram(0u);

  CHECKGLERROR();
}



void GraphModule::update_mesh_geometry(graphProcessor_::buffers_::MeshBase_& meshIn,  MeshData& mesh) {

    meshIn.nvertices=mesh.numberVertices();
    meshIn.nindices=mesh.numberIndices();

    // Vertices buffer.
    {
        glBindBuffer(GL_ARRAY_BUFFER, meshIn.vbo);
          size_t  bytesize = meshIn.nvertices * 3*sizeof(float);
          glBufferData(GL_ARRAY_BUFFER, bytesize, mesh.vertices.data(),  GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);
    }
    // Color buffer
    {
        glBindBuffer(GL_ARRAY_BUFFER, meshIn.vbo_n);
          size_t  bytesize = meshIn.nvertices * 4*sizeof(float);
          glBufferData(GL_ARRAY_BUFFER, bytesize, mesh.colors.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0u);
    }

    // Indices buffer.
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIn.ibo);
          size_t bytesize = meshIn.nindices* sizeof(uint);
          glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesize, mesh.triangelVertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
    }

}



void GraphModule::update_indices_for_Line_geometry(graphProcessor_::buffers_::MeshBase_& meshIn,  MeshData& mesh) {

    meshIn.nindices=mesh.lineIndices.size();
    // Indices buffer.
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIn.ibo);
          size_t bytesize = meshIn.nindices* sizeof(uint);
          glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesize, mesh.lineIndices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
    }
}


void GraphModule::update_indices_for_Triangle_geometry(graphProcessor_::buffers_::MeshBase_& meshIn,  MeshData& mesh) {
    meshIn.nindices=mesh.numberIndices();
    // Indices buffer.
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIn.ibo);
          size_t bytesize = meshIn.nindices* sizeof(uint);
          glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesize, mesh.triangelVertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
    }
}

void GraphModule::setup_mesh_geometry(graphProcessor_::buffers_::MeshBase_& meshIn,  MeshData& mesh) {

   unsigned int const num_component = 3u;

   meshIn.nvertices=mesh.numberVertices();
   meshIn.nindices=mesh.numberIndices();


  // Vertices buffer.
  {
      glGenBuffers(1u, &meshIn.vbo);
      glBindBuffer(GL_ARRAY_BUFFER, meshIn.vbo);
        size_t  bytesize = meshIn.nvertices * 3*sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, bytesize, mesh.vertices.data(),  GL_DYNAMIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0u);
  }
  // Color buffer
  {
      glGenBuffers(1u, &meshIn.vbo_n);
      glBindBuffer(GL_ARRAY_BUFFER, meshIn.vbo_n);
        size_t  bytesize = meshIn.nvertices * 4*sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, bytesize, mesh.colors.data(), GL_DYNAMIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0u);
  }

  // Indices buffer.
  {
      glGenBuffers(1u, &meshIn.ibo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIn.ibo);
        size_t bytesize = meshIn.nindices* sizeof(uint);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesize, mesh.triangelVertices.data(), GL_DYNAMIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
  }

  // Set attribute locations.
  glGenVertexArrays(1u, &meshIn.vao);
  glBindVertexArray(meshIn.vao);
  {

      {
           size_t const attrib_size = 3 * sizeof(float);
           unsigned int const attrib_pos = 0u;
           glBindVertexBuffer(attrib_pos, meshIn.vbo, 0, attrib_size);
           glVertexAttribFormat(attrib_pos, 3, GL_FLOAT, GL_FALSE, 0);
           glVertexAttribBinding(attrib_pos,0);
           glEnableVertexAttribArray(attrib_pos);
      }

      {
           size_t const attrib_size = 4 * sizeof(float);
           unsigned int const attrib_pos = 1u;
           glBindVertexBuffer(attrib_pos,meshIn.vbo_n, 0, attrib_size);
           glVertexAttribFormat(attrib_pos, 4, GL_FLOAT, GL_FALSE, 0);
           glVertexAttribBinding(attrib_pos,1);
           glEnableVertexAttribArray(attrib_pos);
      }

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIn.ibo);

  }
  glBindVertexArray(0u);


   CHECKGLERROR();

   //delete [] pmv;
}

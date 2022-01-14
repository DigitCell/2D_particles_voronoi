#ifndef SHADERUTILS_HPP
#define SHADERUTILS_HPP

#include "../Constants.hpp"

/* Maximum size per shader file (with include). 64 Ko */
#define MAX_SHADER_BUFFERSIZE  (128u*1024u)

class ShaderUtils
{
public:
    ShaderUtils();

    void InitGL();
    GLuint CreateRenderProgram(char const* vsfile, const char *gsfile, char const* fsfile, char *src_buffer);
    GLuint CreateRenderProgram(char const* vsfile, char const* fsfile, char *src_buffer);
    GLuint CreateComputeProgram(char const* program_name, char *src_buffer);
    void CheckShaderStatus(GLuint shader, char const* name);
    bool CheckProgramStatus(GLuint program, char const* name);
    void CheckGLError(char const* file, int const line, char const* errMsg, bool bExitOnFail);
    bool IsBufferBound(GLenum pname, GLuint buffer);
    GLuint GetAttribLocation(GLuint const pgm, char const *name);
    GLuint GetUniformLocation(GLuint const pgm, char const *name);
    GLuint GetUInteger(GLenum const pname);


    void fbo_check(const char* description)
    {   /*  Check to see if the framebuffer is complete  */
        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            fprintf(stderr, "Error: %s framebuffer is incomplete (%i)\n",
                    description, status);
            exit(-1);
        }
    }

    void teardown(GLint* viewport)
    {
        glBindVertexArray(0);
        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (viewport)
        {
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }
    }

    /*
     *  Builds a quad covering the viewport, returning the relevant VAO
     */
    GLuint quad_new()
    {
        GLfloat verts[] = {-1.0f, -1.0f,     1.0f, -1.0f,
                            1.0f,  1.0f,    -1.0f,  1.0f};
        GLuint vbo;
        GLuint vao;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindVertexArray(0);

        CHECKGLERROR();

        return vao;
    }

};

#endif // SHADERUTILS_HPP

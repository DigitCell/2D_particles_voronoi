#include "texture.hpp"



#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

Texture::Texture()
{

}

Texture::~Texture()
{
   // stbi_image_free(img);
}

GLuint Texture::texture_new()
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

void Texture::texture_create_empty()
{
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

void Texture::setup_texture_empty() {

    int widths, heights, nrChannels;

    id=texture_new();

    GLsizei const w = static_cast<GLsizei>(width);
    GLsizei const h = static_cast<GLsizei>(height);

    glBindTexture(GL_TEXTURE_2D,   id);
    glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, w, h, 0, GL_RGB,  GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0u);
    CHECKGLERROR();
}

GLuint  Texture::setup_texture_emptyWH(int widths_, int heights_) {

    int widths, heights, nrChannels;
    width=widths_;
    height=heights_;

    id=texture_new();

    GLsizei const w = static_cast<GLsizei>(width);
    GLsizei const h = static_cast<GLsizei>(height);

    glBindTexture(GL_TEXTURE_2D,   id);
    glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, w, h, 0, GL_RGB,  GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0u);
    CHECKGLERROR();

    return id;
}


void Texture::setup_texture_Source(std::string filename) {

   // stbi_set_flip_vertically_on_load(true);
    int nrChannels;
    img =(char*) stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if (img == NULL)
    {
        fprintf(stderr, "Error loading image: %s\n", stbi_failure_reason());
        exit(-1);
    }
    else if ((unsigned)width> UINT16_MAX || (unsigned)height > UINT16_MAX)
    {
        fprintf(stderr, "Error: image is too large (%i x %i)\n", width, height);
        exit(-1);
    }

    id=texture_new();

    GLsizei const w = static_cast<GLsizei>(width);
    GLsizei const h = static_cast<GLsizei>(height);

    glBindTexture(GL_TEXTURE_2D,   id);
    glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, w, h, 0, GL_RGB,  GL_UNSIGNED_BYTE, img);
    glBindTexture(GL_TEXTURE_2D, 0u);
    CHECKGLERROR();

    //stbi_image_free(img);

}

bool Texture::setup_texture_file_CV(std::string filename, cv::Mat& imageIN) {

    imageIN= cv::imread(filename.c_str(), cv::IMREAD_COLOR );
    if( imageIN.empty() ){
       return false;
    }

    cv::cvtColor( imageIN, imageIN, cv::COLOR_BGR2RGB );
    //cv::cvtColor( imageIN, imageIN, cv::IMREAD_GRAYSCALE);

    width=imageIN.cols;
    height=imageIN.rows;
    glBindTexture(GL_TEXTURE_2D,   id);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imageIN.cols, imageIN.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, imageIN.data );
    glBindTexture(GL_TEXTURE_2D,   id);

    return true;

}


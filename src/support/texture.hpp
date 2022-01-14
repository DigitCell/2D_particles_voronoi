#ifndef TEXURE_HPP
#define TEXURE_HPP
#include "../Constants.hpp"
#include <stdlib.h>

#include <opencv2/opencv.hpp>


class Texture
{
public:
    Texture();
    ~Texture();

    GLuint id;
    size_t size;

    int width;
    int height;

    char* img;

    GLuint texture_new();
    void setup_texture_Source(std::string filename);
    void setup_texture_empty();
    void texture_create_empty();
    bool setup_texture_file_CV(std::string filename, cv::Mat &imageIN);

    GLuint setup_texture_emptyWH(int widths_, int heights_);
};

#endif // TEXURE_HPP

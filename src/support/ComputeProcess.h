#ifndef COMPUTEPROCESS_H
#define COMPUTEPROCESS_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "../Constants.hpp"

#include "Buffer.h"
#include <string>


class ComputeProcess
{
    public:
        static int workGroupsCapabilities[7];

        static void getWorkGroupsCapabilities();
        static void printWorkGroupsCapabilities();

        ComputeProcess();



protected:
        struct Volume
        {
            int x = 0, y = 0, z = 0, count = 0;
            Volume();
            Volume(int _x, int _y, int _z);
        };

        struct DispatchParams
        {
            Volume numWorkgroups;
            Volume workgroupSize;

            DispatchParams();
            DispatchParams(Volume _numWorkgroups, Volume _workgroupSize);
        };

        struct ComputeProgram
        {
            GLuint id;
            DispatchParams dispatchParams;

            ComputeProgram();
            ComputeProgram(std::string sourcefile, DispatchParams params, bool Bin);

        } currentProgram;

        static GLuint createComputeProgram(std::string sourcefile);
        static GLuint createComputeProgramBin(std::string sourcefile);

        void useProgram(ComputeProgram& cprogram);

        void runComputeShader(int num_workgroup_x, int num_workgroup_y, int num_workgroup_z);
        void runComputeShader(int num_workgroup_x, int num_workgroup_y, int num_workgroup_z, int workgroup_size_x, int workgroup_size_y, int workgroup_size_z);
        void runComputeShader(ComputeProgram& cprogram);
        void runComputeShader();

        void startDurationRecording();
        float endDurationRecording();

        static DispatchParams calculateOptimalDisptachSpace(int numInstancesX, int numInstancesY, int numInstancesZ);

    private:
        GLuint durationQuery;

        static char* loadShaderSource(std::string filename);

        static bool gotCapabilities;


};

#endif // COMPUTEPROCESS_H

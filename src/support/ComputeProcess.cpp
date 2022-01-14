#include "ComputeProcess.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

#include <cassert>
#include <cstdio>
#include <cstdlib>


ComputeProcess::ComputeProcess()
{
    if(!gotCapabilities){
        getWorkGroupsCapabilities();
        printWorkGroupsCapabilities();
    }
}


/* Read the shader and process the #include preprocessors. */

// ----------------------------------------------------------------------------

static
int ReadFile(const char* filename, const unsigned int maxsize, char out[]) {
  FILE* fd = nullptr;
  size_t nelems = 0;
  size_t nreads = 0;

  if (!(fd = fopen(filename, "r"))) {
    fprintf(stderr, "warning: \"%s\" not found.\n", filename);
    return 0;
  }
  memset(out, 0, maxsize);
  fseek(fd, 0, SEEK_END);
  nelems = static_cast<size_t>(ftell(fd));
  nelems = (nelems > maxsize) ? maxsize : nelems;
  fseek(fd, 0, SEEK_SET);

  nreads = fread(out, sizeof(char), nelems, fd);
  fclose(fd);

  return nreads == nelems;
}

static
void ReadShaderFile(char const* filename, unsigned int const maxsize, char out[], int *level);

static
void ReadShaderFile(char const* filename, unsigned int const maxsize, char out[]) {
  /// Simple way to deal with include recursivity, without reading guards.
  /// Known limitations : do not handle loop well.

  int max_level = 8;
  ReadShaderFile(filename, maxsize, out, &max_level);
  if (max_level < 0) {
    fprintf(stderr, "Error : too many includes found.\n");
  }
}

static
unsigned int CountChar(char const* str, unsigned int n, char c) {
  unsigned int count = 0u;
  for (unsigned int i=0u; i<n && str[i] != '\0'; ++i) {
    count += (str[i] == c) ? 1u : 0u;
  }
  return count;
}

/* Return true if the given filename is in the list of special extensions. */
static
bool IsSpecialFile(const char *fn) {
  const char* exts[] = { ".hpp" };

  const size_t length_fn = strlen(fn);
  for (auto ext : exts) {
    const size_t length_ext = strlen(ext);
    if (0 == strncmp(fn + length_fn-length_ext, ext, length_ext)) {
      return true;
    }
  }

  return false;
}

static
void ReadShaderFile(char const* filename, unsigned int const maxsize, char out[], int *level) {
  char const * substr = "#include \"";
  size_t const len = strlen(substr);
  char *first = nullptr;
  char *last = nullptr;
  char include_fn[64u] = {0};
  char include_path[256u] = {0};
  size_t include_len = 0u;

  /* Prevent long recursive includes */
  if (*level <= 0) {
    return;
  }
  --(*level);

  /* Read the shaders */
  ReadFile(filename, maxsize, out);

  /* Check for include file an retrieve its name */
  last = out;
  //unsigned int newline_count = 0u;
  while (nullptr != (first = strstr(last, substr))) {
    //newline_count = CountChar(last, first-last, '\n');

    /* pass commented include directives */
    if ((first != out) && (*(first-1) != '\n')) {
      last = first + 1;
      continue;
    }

    first += len;

    last = strchr(first, '"');
    if (!last) return;

    /* Copy the include file name */
    include_len = static_cast<size_t>(last-first);
    strncpy(include_fn, first, include_len);
    include_fn[include_len] = '\0';

    /* Count number of line before the include line */
    unsigned int newline_count = 0u;
    for (char *c = out; c != first; ++c) {
      if (*c == '\n') ++newline_count;
    }

    newline_count += CountChar(last, static_cast<unsigned int>(last-first), '\n');

    /* Set include global path */
    sprintf(include_path, "%s/%s", path_shader.c_str(), include_fn);

    /* Create memory to hold the include file */
    char *include_file = reinterpret_cast<char*>(calloc(maxsize, sizeof(char)));

    /* Retrieve the include file */
    if (!IsSpecialFile(include_path)) {
      ReadShaderFile(include_path, maxsize, include_file, level);
    }

    /* Add the line directive to the included file */
    sprintf(include_file, "%s\n#line %u", include_file, newline_count); // [incorrect]

    /* Add the second part of the shader */
    last = strchr(last, '\n');
    sprintf(include_file, "%s\n%s", include_file, last);

    /* Copy it back to the shader buffer */
    sprintf(first-len, "%s", include_file);

    /* Free include file data */
    free(include_file);
  }
}


GLuint ComputeProcess::createComputeProgram(std::string sourcefile)
{
    GLuint csProgramID;
    GLuint shaderID;


    char *source=new char[MAX_SHADER_BUFFERSIZE];// = loadShaderSource( path_shader+sourcefile);
    ReadShaderFile((path_shader+sourcefile).c_str(), MAX_SHADER_BUFFERSIZE, source);

    shaderID = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shaderID, 1, &source, NULL);
    glCompileShader(shaderID);

    delete[] source;

    GLint result = GL_FALSE;
    int InfoLogLength = 1024;
    char shaderErrorMessage[1024] = {0};

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

    glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage);
    if (result == GL_FALSE)
    {
      std::cout << "\nSHADER ERROR:\n" << shaderErrorMessage << "\n";
      exit(0);
    }

    csProgramID = glCreateProgram();
    glAttachShader(csProgramID, shaderID);
    glLinkProgram(csProgramID);
    glDeleteShader(shaderID);

    return csProgramID;
}

GLuint ComputeProcess::createComputeProgramBin(std::string sourcefile)
{
    GLuint csProgramID;
    GLuint shaderID;

    shaderID = glCreateShader(GL_COMPUTE_SHADER);

    std::ifstream inStream(path_string+"shaders/compute/"+sourcefile, std::ios::binary);
    std::istreambuf_iterator<char> startIt(inStream), endIt;
    std::vector<char> buffer(startIt, endIt);
    inStream.close();

    // Load using glShaderBinary
    glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, buffer.data(), buffer.size());

    // Specialize the shader (specify the entry point)
    glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);

    // Check for success/failure
    GLint status=0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

    if( GL_FALSE == status ) {
        std::cout << "\nSHADER ERROR:\n"  << "\n";

        GLint maxLength = 0;
        glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog( shaderID, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader( shaderID);
        // Either of them. Don't leak shaders.

        exit(0);
    }

    csProgramID = glCreateProgram();
    glAttachShader(csProgramID, shaderID);
    glLinkProgram(csProgramID);


    GLint isLinked = 0;
    glGetProgramiv(csProgramID, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        std::cout << "\nLINK SHADER TO PROGRAM ERROR:\n"  << "\n";

        GLint maxLength = 0;
        glGetProgramiv(csProgramID, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(csProgramID, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(csProgramID);
        // Don't leak shaders either.
        glDeleteShader(shaderID);

        exit(0);
    }

    glDeleteShader(shaderID);

    return csProgramID;
}

char* ComputeProcess::loadShaderSource(std::string filename)
{
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    char* source = new char[content.length()+1];
    strcpy(source, content.c_str());

    return source;
}

void ComputeProcess::useProgram(ComputeProgram& cprogram)
{
    currentProgram = cprogram;
    glUseProgram(currentProgram.id);
}

void ComputeProcess::runComputeShader(int num_workgroup_x, int num_workgroup_y, int num_workgroup_z)
{
    glDispatchCompute(num_workgroup_x, num_workgroup_y, num_workgroup_z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ComputeProcess::runComputeShader(int num_workgroup_x, int num_workgroup_y, int num_workgroup_z, int workgroup_size_x, int workgroup_size_y, int workgroup_size_z)
{
    glDispatchComputeGroupSizeARB(num_workgroup_x, num_workgroup_y, num_workgroup_z, workgroup_size_x, workgroup_size_y, workgroup_size_z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ComputeProcess::runComputeShader(ComputeProgram& cprogram)
{
    Volume& workgroups = cprogram.dispatchParams.numWorkgroups;
    Volume& localSize = cprogram.dispatchParams.workgroupSize;

    glDispatchComputeGroupSizeARB(workgroups.x, workgroups.y, workgroups.z, localSize.x, localSize.y, localSize.z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    //glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ComputeProcess::runComputeShader()
{
    runComputeShader(currentProgram);
}

void ComputeProcess::startDurationRecording()
{
    glGenQueries(1, &durationQuery);
    glBeginQuery(GL_TIME_ELAPSED, durationQuery);
}

float ComputeProcess::endDurationRecording()
{
    unsigned int durationNano;

    glEndQuery(GL_TIME_ELAPSED);
    glGetQueryObjectuiv(durationQuery, GL_QUERY_RESULT, &durationNano);
    glDeleteQueries(1, &durationQuery);

    return (float)durationNano/(float)1e6; // convert from ns to ms;
}

ComputeProcess::DispatchParams ComputeProcess::calculateOptimalDisptachSpace(int numInstancesX, int numInstancesY, int numInstancesZ)
{
    // Calculate the number of workgroups and local invocations for optimized performances :
    // maximize the local size while minimizing the number of workgroups

    int rangeX = std::min(numInstancesX, workGroupsCapabilities[3]);
    int rangeY = std::min(numInstancesY, workGroupsCapabilities[4]);
    int rangeZ = std::min(numInstancesZ, workGroupsCapabilities[5]);

    int record = 0;

    std::vector<Volume> localSizeOptions;
    unsigned int startIndex = 0;

    for(int x = 1; x <= rangeX; x++){
        if(numInstancesX % x != 0)
            continue;
        for(int y = 1; y <= rangeY; y++){
            if(numInstancesY % y != 0)
                continue;
            for(int z = 1; z <= rangeZ; z++){
                if(numInstancesZ % z != 0)
                    continue;
                Volume v(x, y, z);
                if(v.count < workGroupsCapabilities[6]){
                    if(v.count >= record){
                        if(v.count > record)
                            startIndex = localSizeOptions.size();
                        record = v.count;
                        localSizeOptions.push_back(v);
                    }
                }
            }
        }
    }

    // Arbitrarily we choose the repartition of workgroups that corresponds to
    // the limits of number workgroups along each axes, i.e x >= y >= z

    DispatchParams bestParams;

    // Use unsigned ints for a bigger value range
    // Prefer using longs with a 64bit compiler

    unsigned int min = 0;

    unsigned int maxWorkgroupsX = (unsigned int)workGroupsCapabilities[0];
    unsigned int maxWorkgroupsY = (unsigned int)workGroupsCapabilities[1];
    unsigned int maxWorkgroupsZ = (unsigned int)workGroupsCapabilities[2];

    for(unsigned int i = startIndex; i < localSizeOptions.size(); i++){
        Volume localSize = localSizeOptions[i];
        Volume workgroup(numInstancesX / localSize.x, numInstancesY / localSize.y, numInstancesZ / localSize.z);

        unsigned int numWorkgroupsX = (unsigned int)workgroup.x;
        unsigned int numWorkgroupsY = (unsigned int)workgroup.y;
        unsigned int numWorkgroupsZ = (unsigned int)workgroup.z;

        unsigned int d = maxWorkgroupsX + maxWorkgroupsY + maxWorkgroupsZ - numWorkgroupsX - numWorkgroupsY - numWorkgroupsZ;

        if(d < min || i == startIndex){
            min = d;
            bestParams = DispatchParams(workgroup, localSize);
        }
    }

    printf("%d %d %d workgroups of size %d %d %d\n",
           bestParams.numWorkgroups.x, bestParams.numWorkgroups.y, bestParams.numWorkgroups.z,
           bestParams.workgroupSize.x, bestParams.workgroupSize.y, bestParams.workgroupSize.z);

    return bestParams;
}

int ComputeProcess::workGroupsCapabilities[7];
bool ComputeProcess::gotCapabilities = false;

void ComputeProcess::getWorkGroupsCapabilities()
{
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupsCapabilities[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupsCapabilities[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupsCapabilities[2]);

  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupsCapabilities[3]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupsCapabilities[4]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupsCapabilities[5]);

  glGetIntegerv (GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupsCapabilities[6]);

  gotCapabilities = true;
}

void ComputeProcess::printWorkGroupsCapabilities()
{
    printf("Max number of workgroups:\nx: %d\ny: %d\nz: %d\n", workGroupsCapabilities[0], workGroupsCapabilities[1], workGroupsCapabilities[2]);
    printf("Max number of local invocations:\nx: %d\ny: %d\nz: %d\n", workGroupsCapabilities[3], workGroupsCapabilities[4], workGroupsCapabilities[5]);
    printf("Max total number of local invocations: %d\n\n", workGroupsCapabilities[6]);
}

ComputeProcess::Volume::Volume()
{

}

ComputeProcess::Volume::Volume(int _x, int _y, int _z) : x(_x), y(_y), z(_z), count(_x * _y * _z)
{

}

ComputeProcess::DispatchParams::DispatchParams()
{

}

ComputeProcess::DispatchParams::DispatchParams(Volume _numWorkgroups, Volume _workgroupSize) : numWorkgroups(_numWorkgroups), workgroupSize(_workgroupSize)
{

}

ComputeProcess::ComputeProgram::ComputeProgram()
{

}

ComputeProcess::ComputeProgram::ComputeProgram(std::string sourcefile, ComputeProcess::DispatchParams params, bool bin) : dispatchParams(params)
{
    if(bin)
     id = createComputeProgramBin(sourcefile);
    else
     id = createComputeProgram(sourcefile);
}


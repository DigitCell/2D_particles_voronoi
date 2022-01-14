#version 430

#extension GL_ARB_compute_variable_group_size : enable

#include "Common.glsl"

uniform int numPoints;
uniform float radius;
uniform vec2 worldSize;
uniform ivec2 mapSize;
uniform int mapDivider;

uniform vec2 mapCellSize;
uniform int maxNeightbSize;
uniform int maxCellSize;

//layout(rgba32f) uniform writeonly image2D writeToTexture;
layout(rgba32f) uniform writeonly image2D distinationTexture;

int GetIndex(int x, int y)
{
    return y*mapSize.x+x;
}

int GetIndexMap(int i, int x, int y)
{
    int sindex=y*mapSize.x+x;
    return sindex*maxCellSize+i;
}


ivec2 iW2xy(int id)
{
    return ivec2(id%int(mapSize.x ), id/int(mapSize.x ));
}
int GetNeightbIndex(int index, int neightb_index)
{
    return index*maxNeightbSize+neightb_index;
}

#define N ivec2(int(sqrt(numPoints)),int(sqrt(numPoints)))

ivec2 i2xy(int id)
{
    return ivec2(id%int( N ), id/int( N ));
}

int xy2i(ivec2 p)
{
    return p.x + p.y*int( N );
}

int GetTextureIndex(int x, int y)
{
    return y*ivec2(worldSize).x+x;
}

int GetWorldVec2Index(vec2 coord)
{
    return int(coord.y*worldSize.x+coord.x);
}

int GetWorldiVec2Index(ivec2 coord)
{
    return coord.y*int(worldSize.x)+coord.x;
}

ivec2 GetWorldIndexToiVec2(int id)
{
    return ivec2(id%int(worldSize.x), id/int(worldSize.x));
}


vec2 hMap(float h){
    float c =  smoothstep(0., (h)*2., h -.1 )*h;
    c += (1.-smoothstep(0., (h)*3., h - .1))*c*.5;
    return vec2(clamp(c,0.,1.), h);
}

void main() {

  //ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
  int coordIndex = int(gl_GlobalInvocationID.x);
  ivec2 coord=GetWorldIndexToiVec2(coordIndex);

  //int coordIndex=GetTextureIndex(int(coord.x),int(coord.y));
  int selfIdx=coordIndex;

  int centerIndex=int(
                        distMapData[
                            GetTextureIndex(int(distMapData[selfIdx].coord.x),int(distMapData[selfIdx].coord.y))
                                   ].index.x
                     );

  vec3  col = pal(centerIndex/170.0f,
                    vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(2.0,1.0,0.0),vec3(0.5,0.20,0.25) );

  //vec2 centerCoord=DistMapData[centerIndex].xy;
  vec2 centerCoord=pointData[centerIndex].coord;
  float dist=length(centerCoord-vec2(coord));

  if(dist>22.5)
  {
    col=vec3(0.3,0.3,0.5);
  }

  if(distMapData[selfIdx].coord.x>0 && distMapData[selfIdx].coord.y>0)
    //imageStore(destination,coord, vec4( DistMapData[selfIdx].x/worldSize.x, DistMapData[selfIdx].y/worldSize.y, 0.0f, 1.0f));
    imageStore(distinationTexture,coord, vec4( col, 1.0f));
  else
    imageStore(distinationTexture,coord, vec4( vec3(0.1,0.1,0.1), 1.0f));

/*

  vec2 centerCoord=pointData[centerIndex].coord;
  float dist=length(centerCoord-vec2(coord));
  //if(DistMapData[centerIndex].z<15.0)
  if(dist<7.5)
  {
    imageStore(distinationTexture,coord, vec4( vec3(0.0,0.3,0.5), 1.0f));
  }
  */

}

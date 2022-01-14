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

//Iq palettes https://www.shadertoy.com/view/ll2GD3
vec3 pallite( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    if(t==-1.0)
      return  vec3(0.1,0.1,0.1);
    return a + b*cos( 6.28318*(c*t+d) );
}

float Voronoi(in vec2 pos, int indx){

    int neightbSteps =min(neightbSize[indx],3);

    float dc = distance(pos,pointData[indx].coord);

    float d0 = 0.0;

    if(neightbSteps==1)
    {
      int nindex0=neightbSort[GetNeightbIndex(indx,0)];
      d0 = distance(pos,pointData[nindex0].coord)/25.0f;
    }

    float df=1.0-clamp(d0,0.,1.);
    return df;
}

void main() {

  //ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
  int coordIndex = int(gl_GlobalInvocationID.x);
  ivec2 coord=GetWorldIndexToiVec2(coordIndex);

  //int coordIndex=GetTextureIndex(int(coord.x),int(coord.y));
  int selfIdx=coordIndex;
/*
  int centerIndex=int(
                        distMapData[
                            GetTextureIndex(int(distMapData[selfIdx].coord.x),int(distMapData[selfIdx].coord.y))
                                   ].index.x
                     );
*/
  int centerIndex=distMapData[selfIdx].nb.x;
  vec3  col = pallite((pointData[centerIndex].force.x)/170.0f,
                    vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(2.0,1.0,0.0),vec3(0.5,0.20,0.25) );

  //if(distMapData[selfIdx].coord.x<1.0 && distMapData[selfIdx].coord.y<1.0)
  //    col=vec3(0.1,0.1,0.1);

  //vec2 centerCoord=pointData[centerIndex].coord;
  //float dist=length(centerCoord-vec2(coord));

//get the position of the closest particle
  vec2  p1 = pointData[distMapData[selfIdx].nb.x].coord;
//get the position of the second closest particle
	vec2  p2 = pointData[distMapData[selfIdx].nb.y].coord;
//get the distance to voronoi edge
  float r = dot(coord-(p1+p2)/2.,normalize(p2-p1));
  col += smoothstep(2.,0.,abs(r))*.3;


/*
  if(dist<radius)
  {
     col += (0.5 + 0.5*sin(0.7*distMapData[selfIdx].dist.y))*vec3(1.0);
     // borders
     col = mix( vec3(1.0,0.6,0.0), col, smoothstep( 0.04, 0.17, distMapData[selfIdx].dist.y ) );
   }
   */
/*
   if(distMapData[selfIdx].dist.y<3)
   {
     col=vec3(0.3,0.3,0.5);
   }
*/
  //vec2 centerCoord=DistMapData[centerIndex].xy;
/*
  if(dist>radius-2.0 && dist<radius)
  {
    col=vec3(0.3,0.3,0.5);
  }
  if(dist>radius-4.0 && dist<radius-2.0)
  {
    col=vec3(1.0,1.0,1.0);
  }
*/
    float c = Voronoi(coord, centerIndex);

  //  col*=c;


//  if(DistMapData[selfIdx].x>0 && DistMapData[selfIdx].y>0)
    //imageStore(destination,coord, vec4( DistMapData[selfIdx].x/worldSize.x, DistMapData[selfIdx].y/worldSize.y, 0.0f, 1.0f));
    imageStore(distinationTexture,coord, vec4( col, 1.0f));
  //else
    //imageStore(distinationTexture,coord, vec4( vec3(0.1,0.1,0.1), 1.0f));


}

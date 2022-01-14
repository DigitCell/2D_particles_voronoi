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


float Voronoi(in ivec2 coord,in int selfIdx) {

    bool ghost;
    vec2 pi0 = pointData[distMapData[selfIdx].nb.x].coord;
    vec2 pi1 = pointData[distMapData[selfIdx].nb.y].coord;
    vec2 pi2 = pointData[distMapData[selfIdx].nb.z].coord;
    vec2 pi3 = pointData[distMapData[selfIdx].nb.w].coord;

    float d0 = distance(coord,pi0)/2.0;
    float d1 = distance(coord,pi1)/2.0;
    float d2 = distance(coord,pi2)/2.0;
    float d3 = distance(coord,pi3)/2.0;


    float df =min(2./(1./max(d1 - d0, .001) + 1./max(d2 - d0, .001)), 1.);
    df*= step(d0,20.0);
    //float df =min(0.4/(1./max(d1 - d0, .001) + 0.2/max(d2 - d0, .001)+ 0.05/max(d3 - d0, .001) ), 11.0);
    //df*= smoothstep(radius*2.2,radius*1.8 ,d0);

    return df;
}

vec2 hMap(in ivec2 coord, in int index){
    float h = Voronoi(coord, index);
    //float c =  smoothstep(0.0, (h)*2.0, h -0.1 )*h;
    //c += (1.-smoothstep(0.0, (h)*3.0, h - 0.1))*c*0.5;

    float c = smoothstep(0., (h)*2., h - .09)*h;
    c += (1.-smoothstep(0., (h)*3., h - .22))*c*.5;
    return vec2(clamp(c,0.0,1.0), h);
}


vec3 pal2( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}

void main() {

  //ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
  int coordIndex = int(gl_GlobalInvocationID.x);
  ivec2 coord=GetWorldIndexToiVec2(coordIndex);

  //int coordIndex=GetTextureIndex(int(coord.x),int(coord.y));
  int selfIdx=coordIndex;
  int centerIndex=distMapData[selfIdx].nb.x;

  vec2 c = hMap(coord, selfIdx);
  //int exIndex=int(pointData[distMapData[selfIdx].nb.x].force.x);
  int exIndex=int((pointData[distMapData[selfIdx].nb.z].force.x+pointData[distMapData[selfIdx].nb.y].force.x)/2.1);

  // vec3  col = pal( float(idn)/NP,  vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(2.0,1.0,0.0),vec3(0.5,0.20,0.25) )*c.x;
  //vec3  col = pal2( float(exIndex)/75.0, vec3(0.45,0.5,0.5),vec3(0.5,0.75,0.5),vec3(2.0,1.0,1.0),vec3(0.5,0.2,0.25) )*c.x;
  vec3  col = pal2( float(exIndex)/5.0, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.3,0.20,0.20))*c.x;

  // Apply the bump - or a powered variation of it - to the color for a bit of highlighting.
  vec2 c2 = vec2(0.0);//hMap(ivec2(coord)+ 2, selfIdx);
  float b =  max(1.0*c2.x- c.x,0.)*0.5;
  //col +=1.0* vec3(1.)*(b*2. + b*b*b*12.);
  col += vec3(.5, .7, 1)*(b*b*.5 + b*b*b*b*.5);



  /*
  vec3  col = pallite((pointData[centerIndex].force.x)/170.0f,
                    vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(2.0,1.0,0.0),vec3(0.5,0.20,0.25) );




//get the position of the closest particle
  vec2  p1 = pointData[distMapData[selfIdx].nb.x].coord;
//get the position of the second closest particle
	vec2  p2 = pointData[distMapData[selfIdx].nb.y].coord;
//get the distance to voronoi edge
  float r = dot(coord-(p1+p2)/2.,normalize(p2-p1));
  col += smoothstep(2.,0.,abs(r))*.3;
*/

  //col= sqrt(clamp(col, 0., 1.));

  imageStore(distinationTexture,coord, vec4( col, 1.0f));



}

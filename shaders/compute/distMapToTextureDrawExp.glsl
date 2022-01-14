#version 430

#extension GL_ARB_compute_variable_group_size : enable

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

//precision highp float;
//precision highp int;

layout (local_size_variable) in;


struct PointData
{
    vec2 coord;
    vec2 velocity;
    vec2 force;
    vec2 headTail;
    vec2 prevNext;
    vec2 radius;
};

struct NeightbData
{
   vec2 dist;
};

struct VoronoiData
{
    index;
    coord;
    dist;
};

layout (std430, binding = PointData_BP ) buffer PointDataBuffer
{
    PointData pointData[];
};

layout (std430, binding = MapData_BP  ) buffer MapDataBuffer
{
    int mapData[];
};

layout (std430, binding = MapSize_BP  ) buffer MapSizeBuffer
{
    int mapCellAmount[];
};

layout (std430, binding = MapSizeFloat_BP  ) buffer MapSizeFloatBuffer
{
    float mapSizeFloat[];
};

layout (std430, binding = PointPos_BP  ) buffer PointPosBuffer
{
    float pointPos[];
};


layout (std430, binding = Neightb_BP  ) buffer NeightbBuffer
{
    int neightb[];
};

layout (std430, binding = NeightbSize_BP  ) buffer NeightbSizeBuffer
{
    int neightbSize[];
};

layout (std430, binding = NeightbSort_BP  ) buffer NeightbSortBuffer
{
    int neightbSort[];
};

layout (std430, binding = NeightbInfo_BP  ) buffer NeightbInfoBuffer
{
    NeightbData neightbInfo[];
};

layout (std430, binding = DistMapData_BP ) buffer DistMapDataBuffer
{
    vec4 DistMapData[];
};

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
vec3 pal( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    if(t==0.0)
      return  vec3(0.1,0.1,0.1);

    return a + b*cos( 6.28318*(c*t+d) );
}





float Voronoi(in vec2 pos, int indx){

    int neightbSteps =min(neightbSize[indx],3);

    float dc = distance(pos,pointData[indx].coord);

    float d0 = 0.0;
    float d1 = 0.0;
    float d2 = 0.0;
    float d3 = 0.0;

    if(neightbSteps==1)
    {
      int nindex0=neightbSort[GetNeightbIndex(indx,0)];
      d0 = distance(pos,pointData[nindex0].coord);

    }

    if(neightbSteps==2)
    {
      int nindex0=neightbSort[GetNeightbIndex(indx,0)];
      d0 = distance(pos,pointData[nindex0].coord);
      int nindex1=neightbSort[GetNeightbIndex(indx,1)];
      d1 = distance(pos,pointData[nindex1].coord);
    }

    if(neightbSteps==3)
    {
      int nindex0=neightbSort[GetNeightbIndex(indx,0)];
      d0 = distance(pos,pointData[nindex0].coord);
      int nindex1=neightbSort[GetNeightbIndex(indx,1)];
      d1 = distance(pos,pointData[nindex1].coord);
      int nindex2=neightbSort[GetNeightbIndex(indx,2)];
      d2 = distance(pos,pointData[nindex2].coord);
    }

      if(neightbSteps>3)
    {
      int nindex0=neightbSort[GetNeightbIndex(indx,0)];
      d0 = distance(pos,pointData[nindex0].coord);
      int nindex1=neightbSort[GetNeightbIndex(indx,1)];
      d1 = distance(pos,pointData[nindex1].coord);
      int nindex2=neightbSort[GetNeightbIndex(indx,2)];
      d2 = distance(pos,pointData[nindex2].coord);
      int nindex3=neightbSort[GetNeightbIndex(indx,3)];
      d3 = distance(pos,pointData[nindex3].coord);
    }

    float df =min(0.05/(1./max((d0 - dc), .001)+1./max((d1 - dc), .001) + 1./max((d2 - dc), .001)), 1.);
    df*= smoothstep(25.0*1.3,0. ,dc);
    return df;
}



vec2 hMap(in vec2 pos, int indx)
{
    float h = Voronoi(pos, indx);
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
                        DistMapData[
                            GetTextureIndex(int(DistMapData[selfIdx].x),int(DistMapData[selfIdx].y))
                                   ].w
                     );

  vec3  col = pal((centerIndex)/170.0f,
                    vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(2.0,1.0,0.0),vec3(0.5,0.20,0.25) );

  //vec2 centerCoord=DistMapData[centerIndex].xy;
  vec2 centerCoord=pointData[centerIndex].coord;
  float dist=length(centerCoord-vec2(coord));
/*
  if(dist>22.5)
  {
    col=vec3(0.3,0.3,0.5);
  }
*/
    vec2 c = hMap(coord, centerIndex);
    vec2 c2 = hMap(coord + 2.,centerIndex);
    float b =  (c2.x - c.x)*.5;
    col += vec3(1.)*(b*2. + b*b*b*12.);

//  if(DistMapData[selfIdx].x>0 && DistMapData[selfIdx].y>0)
    //imageStore(destination,coord, vec4( DistMapData[selfIdx].x/worldSize.x, DistMapData[selfIdx].y/worldSize.y, 0.0f, 1.0f));
    imageStore(distinationTexture,coord, vec4( col, 1.0f));
  //else
    //imageStore(distinationTexture,coord, vec4( vec3(0.1,0.1,0.1), 1.0f));



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

#version 430
#extension GL_ARB_compute_variable_group_size : enable

#include "Common.glsl"

uniform int numPoints;
uniform float radius;
uniform vec2 worldSize;
uniform ivec2 mapSize;
uniform int mapDivider;
uniform vec2 mapCellSize;
uniform int maxCellSize;

int GetIndex(int x, int y)
{
    return y*mapSize.x+x;
}

int GetIndexMap(int i, int x, int y)
{
    int sindex=y*mapSize.x+x;
    return sindex*maxCellSize+i;
}

//border conditions
void BorderConditions(inout vec2 pos, inout vec2 velocity, vec2 size, float RADIUS)
{
    if(size.x - pos.x < RADIUS*2.0)
        {velocity.x = -abs(velocity.x); pos.x = size.x - RADIUS*2.0;}
    if(pos.x < RADIUS*2.0)
        {velocity.x = abs(velocity.x);  pos.x = RADIUS*2.0;}
    if(size.y - pos.y < RADIUS*2.0)
        { velocity.y= -abs(velocity.y); pos.y = size.y - RADIUS*2.0;}
    if(pos.y < RADIUS*2.0)
        {velocity.y = abs(velocity.y);  pos.y = RADIUS*2.0;}

    //pos = mod(pos, size);
}

//border conditions
void mapUpdate(int index, vec2 pos, ivec2 size)
{
        int x = int(trunc(pos.x / mapCellSize.x));
        int y = int(trunc(pos.y / mapCellSize.y));

        if (x <0) x = 0;
        else if (x > size.x - 1) x = size.x - 1;

        if (y < 0) y =0;
        else if (y > size.y - 1)
            y = size.y - 1;

        //this handles the linked list between particles on the same square
        if(mapSize[GetIndex(x,y)]<maxCellSize)
        {
            int indexAdd=atomicAdd(mapIndexSize[GetIndex(x,y)],1);
            mapIndexSizeFloat[GetIndex(x,y)]=float(mapIndexSize[GetIndex(x,y)]);
            mapData[GetIndexMap(indexAdd,x,y)] = index;
        }
}


void main() {

  int indx = int(gl_GlobalInvocationID.x);
  BorderConditions(pointData[indx].coord, pointData[indx].velocity, worldSize, radius);
  mapUpdate(indx, pointData[indx].coord, mapSize);

}

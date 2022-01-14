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


int GetIndex(int x, int y)
{
    return y*mapSize.x+x;
}

int GetIndexMap(int i, int x, int y)
{
    int sindex=y*mapSize.x+x;
    return sindex*maxCellSize+i;
}

int GetNeightbIndex(int index, int neightb_index)
{
    return index*maxNeightbSize+neightb_index;
}

void NeightbSort(int index)
{
    int neightbAmount=neightbSize[index];
    for(int ip=0; ip<neightbAmount;ip++)
    {
      neightbSort[GetNeightbIndex(index, ip)]=neightb[GetNeightbIndex(index,ip)];
    }

    for(int ip1=0; ip1<neightbAmount;ip1++)
    {
      for(int ip2=ip1+1; ip2<neightbAmount;ip2++)
      {
          int indexA=neightbSort[ip1];
          int indexB=neightbSort[ip2];

          float distA=distance(pointData[index].coord, pointData[neightbSort[GetNeightbIndex(index, indexA)]].coord);
          float distB=distance(pointData[index].coord, pointData[neightbSort[GetNeightbIndex(index, indexB)]].coord);

        //  if(neightbInfo[indexA].dist.x>neightbInfo[indexB].dist.x)
          if(distA>distB)
          {
             neightbSort[indexA]=indexB;
             neightbSort[indexB]=indexA;
            // float a=neightbInfo[indexA].dist.x;
            // neightbInfo[indexA].dist.x=neightbInfo[indexB].dist.x;
            // neightbInfo[indexB].dist.x=a;

          }

      }
    }

}

void NeightbSort2(int index)
{
    int neightbAmount=neightbSize[index];

    for(int ip1=0; ip1<neightbAmount;ip1++)
    {
      for(int ip2=ip1+1; ip2<neightbAmount;ip2++)
      {
          int indexA=neightbSort[ip1];
          int indexB=neightbSort[ip2];

          float distA=neightbInfo[GetNeightbIndex(index, indexA)].dist.x;
          float distB=neightbInfo[GetNeightbIndex(index, indexB)].dist.x;

          if(distA>distB)
          {
             neightbSort[indexA]=indexB;
             neightbSort[indexB]=indexA;

             float a=neightbInfo[indexA].dist.x;
             neightbInfo[indexA].dist.x=neightbInfo[indexB].dist.x;
             neightbInfo[indexB].dist.x=a;
          }
      }
   }
}


void main() {

  int indx = int(gl_GlobalInvocationID.x);
  NeightbSort2(indx);

}

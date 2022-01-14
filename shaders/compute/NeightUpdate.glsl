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

//border conditions
void NeightbUpdate(int index, vec2 pos, ivec2 size)
{
    const PointData p =  pointData[index];

    int pX =int(trunc(p.coord.x / mapCellSize.x));
    int pY =int(trunc(p.coord.y / mapCellSize.y));

    if (pX < 0) pX =0;
    else if (pX > size.x - 1) pX = size.x - 1;

    if (pY < 0) pY = 0;
    else if (pY > size.y - 1)
        pY = size.y - 1;

    neightbSize[index]=0;
    int neightbSizeIndex=0;

    //iterate over the nine squares on the grid around p
    for (int mapX = pX - 1; mapX <= pX +1; mapX++){
        for (int mapY = pY - 1; mapY <= pY + 1; mapY++){
            //go through the current square's linked list of overlapping values, if there is one
           // for (Particle* nextDoor = &particles[map[GetIndex(mapX,mapY)]]; nextDoor != NULL; nextDoor = nextDoor->next){
            if(mapX<0 || mapY<0 || mapX>mapSize.x-1 || mapY>mapSize.y-1)
                 continue;
            int cellAmount=mapIndexSize[GetIndex(mapX,mapY)];

            if(cellAmount==0)
                continue;
          //  if(cellAmount>57)
          //      continue;

            for(int ip=0; ip<cellAmount;ip++)
            {
                int indexJ=mapData[GetIndexMap(ip,mapX,mapY)];
                if(indexJ!=index)
                {
                  PointData pj =  pointData[indexJ];
                  float diffX = pj.coord.x - p.coord.x;
                  float diffY = pj.coord.y - p.coord.y;
                  float r2 = diffX*diffX + diffY*diffY;
                  float r = sqrt(r2);
                  //float q = r / radius;

                  //save this neighbor
                //  if (q < 1 && q > 0.0000000000001f){
                  if(r>0.0 && r<(2.0*mapSize.x))
                  {
                      int jn=neightbSizeIndex;
                    //  int jn=min(neightbSize[index],16);// (neightb_size[i]==0)?0:neightb_size[i]-1;
                      if (neightbSize[index] <maxNeightbSize)
                      {
                          //  neightb[GetNeightbIndex(index, j)]=mapData[GetIndexMap(ip,mapX,mapY)];
                          int nmpIndex=mapData[GetIndexMap(ip,mapX,mapY)];
                          int getIndex=GetNeightbIndex(index, jn);
                          neightb[getIndex]=nmpIndex;
                          neightbSort[getIndex]=nmpIndex;
                          neightbInfo[getIndex].dist.x=r;
                          //  neightb_r[GetIndexNeightb(i,j)]=r;
                          //  neightb_Lij[GetIndexNeightb(i,j)]=particleHeight;
                          neightbSizeIndex++;
                        //  int indexAdd=atomicAdd(neightbSize[index],1);
                      }

                  }
              }
          }
        }
    }


    neightbSize[index]=neightbSizeIndex;

}


void main() {

  int indx = int(gl_GlobalInvocationID.x);
  NeightbUpdate(indx, pointData[indx].coord, mapSize);
  //barrier             ();
  //NeightbSort(indx, pointData[indx].coord, mapSize);

}

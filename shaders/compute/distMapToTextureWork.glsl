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
uniform int passNumber;

layout(rgba16f) uniform image2D destination;


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
    return y*int(worldSize.x)+x;
}

int GetWorldVec2Index(vec2 coord)
{
    return int(coord.y*worldSize.x+coord.x);
}

int GetWorldiVec2Index(ivec2 coord)
{
    return int(coord.y*worldSize.x+coord.x);
}

void jumpFloodKernel(ivec2 coord, int passIndex)
{

  float DIST_MAX=999999999.;

  int diagramXDim=int(worldSize.x);
  int diagramYDim=int(worldSize.y);

  const int coordX=int(coord.x);
  const int coordY=int(coord.y);

  float maximalDim = max(worldSize.x, worldSize.y);
  int selfIdx = GetTextureIndex(coordX, coordY);

  // Ignore out-of-bounds index
  if (coord.x > diagramXDim && coord.y > diagramYDim) return;

  int log2v=9;//int(log2(maximalDim));

  float closestDistance =  distMapData[selfIdx].dist.x;
  float closestColor =     distMapData[selfIdx].index.x;
  vec2  closestCandidate = distMapData[selfIdx].coord;
  float closestDistanceBorder=distMapData[selfIdx].dist.y;
  vec2  otherCandidateBorder;

  int steps[12]=int[12](1,150,
                        1,75,1,
                        35,1,15,
                        1,5,1,
                        1);

/*
    int steps[9]=int[9]( 29,20,14,
                         11,7,3,
                         1,1,1);
*/

	// JFA pass(es) loop
	//for (int passIndex = 0; passIndex <log2v; passIndex++)
	{
        int step =steps[passIndex];
        closestCandidate = distMapData[selfIdx].coord;
        //closestDistance=DIST_MAX;//DistMapData[selfIdx].z;
        closestDistance=distMapData[selfIdx].dist.x;
        closestColor = distMapData[selfIdx].index.x;

		    // JFA pass computations
        bool findCandidate=false;
        bool findCandidateBorder=false;
        for (int gridY = -1; gridY < 2; gridY++)
        {
            for (int gridX = -1; gridX < 2; gridX++)
            {

                if(gridX==0 && gridY==0) continue;
                int xLookup = coordX + gridX*step;
                int yLookup = coordY + gridY*step;

                // Ignore out-of-bounds
                if (xLookup <0 || xLookup > diagramXDim-1 || yLookup <0 || yLookup >diagramYDim-1) continue;

                int lookupIdx = yLookup* diagramXDim + xLookup;
                vec2 otherCandidate = distMapData[lookupIdx].coord;
                float otherColor=distMapData[lookupIdx].index.x;

                if (otherCandidate!=vec2(0.0))
                {
                  float otherDistance =length(otherCandidate-coord);
                  if (otherDistance < closestDistance)// && otherDistance<radius )
                    {
                      findCandidate=true;
                    	closestCandidate = otherCandidate;
                    	closestDistance = otherDistance;
                      closestColor=otherColor;
                    }
                }
            }
        }

        barrier();

        if(findCandidate)
        {
          distMapData[selfIdx].coord=closestCandidate;
          distMapData[selfIdx].dist.x=closestDistance;
          distMapData[selfIdx].index.x=closestColor;
          distMapData[selfIdx].dist.y=closestDistanceBorder;
        }

	}

}

void main() {

  ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
  jumpFloodKernel(coord, passNumber);
}

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

int GetTextureIndexVec2i(ivec2 index)
{
    return index.y*int(worldSize.x)+index.x;
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

  int steps[12]=int[12](1,250,
                        1,75,1,
                        35,1,15,
                        1,5,1,
                        1);


/*
  int steps[9]=int[9]( 1,29,1,
                       17,1,11,
                       1,5,1);
*/

	// JFA pass(es) loop
	for (int passIndex = 0; passIndex <log2v; passIndex++)
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
                //if (xLookup <0 || xLookup > diagramXDim-1 || yLookup <0 || yLookup >diagramYDim-1) continue;
                if(xLookup <0) xLookup=passIndex+1;
                if(xLookup > diagramXDim-1) xLookup=diagramXDim-passIndex-1;
                if(yLookup <0) yLookup=passIndex+1;
                if(yLookup >diagramYDim-1) yLookup=diagramYDim-passIndex-1;

                int lookupIdx = yLookup* diagramXDim + xLookup;
                vec2 otherCandidate = distMapData[lookupIdx].coord;
                float otherColor=distMapData[lookupIdx].index.x;

                if (otherCandidate!=vec2(-1.0))
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

// --- chose length of closest LIST
//#define LIST xxxx    //  1 (not enough for repulsion & draw voronoi)
//  #define LIST xyyy    //  2
//#define LIST xyzz    //  3
#define LIST xyzw    //  4

//hashing noise by IQ
float hash( int k ) {
    uint n = uint(k);
	n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return uintBitsToFloat( (n>>9U) | 0x3f800000U ) - 1.0;
}

void insertion_sort(inout ivec4 i, inout vec4 d, int i_, float d_){
    if(any(equal(ivec4(i_),i))) return;
    if     (d_ < d[0])
        i = ivec4(i_,i.xyz),    d = vec4(d_,d.xyz);
    else if(d_ < d[1])
        i = ivec4(i.x,i_,i.yz), d = vec4(d.x,d_,d.yz);
    else if(d_ < d[2])
        i = ivec4(i.xy,i_,i.z), d = vec4(d.xy,d_,d.z);
    else if(d_ < d[3])
        i = ivec4(i.xyz,i_),    d = vec4(d.xyz,d_);
    i = i.LIST; d = d.LIST;
}

float distance2Particle(int id, vec2 worldCoord){ // screen coords
    if(id==-1) return 1e20;
    vec2 delta = pointData[id].coord-worldCoord;
    return sqrt(dot(delta, delta));
}

void jumpFloodKernel2(ivec2 coord, int passIndex)
{

  float DIST_MAX=999999999.;

  int diagramXDim=int(worldSize.x);
  int diagramYDim=int(worldSize.y);

  const int coordX=int(coord.x);
  const int coordY=int(coord.y);
    // Ignore out-of-bounds index
  if (coord.x > diagramXDim && coord.y > diagramYDim) return;

  int selfIdx = GetTextureIndex(coordX, coordY);

int steps=5;
for (int passIndex = 0; passIndex <steps; passIndex++)
	{
  //in this vector the four new closest particles' ids will be stored
  ivec4 new = ivec4(-1);
  //in this vector the distance to these particles will be stored
  vec4 dis = vec4(1e20);

  //get all known closest particles from old self and neighbours
  ivec4 old   = distMapData[GetTextureIndexVec2i(coord + ivec2( 0, 0))].nb;
  ivec4 east  = distMapData[GetTextureIndexVec2i(coord + ivec2( 1, 0))].nb;
  ivec4 north = distMapData[GetTextureIndexVec2i(coord + ivec2( 0, 1))].nb;
  ivec4 west  = distMapData[GetTextureIndexVec2i(coord + ivec2(-1, 0))].nb;
  ivec4 south = distMapData[GetTextureIndexVec2i(coord + ivec2( 0,-1))].nb;
  //collect them in a array so we can loop over it
  ivec4[5] candidates = ivec4[5](old, east, north, west, south);

  for(int i=0; i<5; i++){
        for(int j=0; j<4; j++){
            int id = candidates[i][j];
            float dis2 = distance2Particle(id, coord); // to screen
            insertion_sort( new, dis, id, dis2 );

        }
    }

  barrier();

  //if(findCandidate)
  {
    distMapData[selfIdx].nb=new;
  }
}
/*
   for(int k = 0; k < 1; k++){
        //random hash. We should make sure that two pixels in the same frame never make the same hash!
        float h = hash(
            iFragCoord.x +
            iFragCoord.y*int(iResolution2.x) +
            iFrame*int(iResolution2.x*iResolution2.y) +
            k
        );
        //pick random id of particle
        int p = int(h*float(PARTICLES));
        insertion_sort(new, dis, p, distance2Particle(p, worldCoord) );
    }
*/


}



void main() {

  ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
  //jumpFloodKernel(coord, passNumber);
  jumpFloodKernel2(coord, passNumber);
}

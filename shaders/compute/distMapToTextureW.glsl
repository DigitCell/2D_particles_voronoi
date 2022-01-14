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
    vec2 index;
    vec2 coord;
    vec2 dist;
};

layout (std430, binding = PointData_BP ) buffer PointDataBuffer
{
    PointData pointData[];
};

layout (std430, binding = DistMapData_BP ) buffer DistMapDataBuffer
{
    VoronoiData distMapData[];
};

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

//Iq palettes https://www.shadertoy.com/view/ll2GD3
vec3 pal( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}


// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}
// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }
// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}
// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }




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
/*
  int steps[9]=int[9](  1,11,9,
                        7,3,7,
                        1,3,1);
*/
    int steps[9]=int[9]( 29,20,14,
                         11,7,3,
                         1,1,1);

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
          if (otherDistance < closestDistance && otherDistance<radius )
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
    }

	}

}




void main() {

  ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
  jumpFloodKernel(coord, passNumber);


}

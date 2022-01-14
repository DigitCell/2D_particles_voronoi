#version 430
#extension GL_ARB_compute_variable_group_size : enable

#include "noise.glsl"
#include "/noise/perlinNoise.glsl"
#include "Common.glsl"

uniform int numPoints;
uniform float radius;
uniform vec2 worldSize;
uniform ivec2 mapSize;
uniform int mapDivider;
uniform float dt;
uniform int maxNeightbSize;
uniform float maxVelocity;
uniform int maxCellSize;
uniform int tick;
uniform float noiseCoeff;
uniform float stiffnessCoeff;
uniform float dampingCoeff;

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
void BorderConditions(inout vec2 pos, inout vec2 velocity, vec2 size, float RADIUS)
{
    float borderCoeff=0.2;
    if(size.x - pos.x < RADIUS*borderCoeff)
        {velocity.x = -abs(velocity.x); pos.x = size.x - RADIUS*borderCoeff;}
    if(pos.x < RADIUS*borderCoeff)
        {velocity.x = abs(velocity.x);  pos.x = RADIUS*borderCoeff;}
    if(size.y - pos.y < RADIUS*borderCoeff)
        { velocity.y= -abs(velocity.y); pos.y = size.y - RADIUS*borderCoeff;}
    if(pos.y < RADIUS*borderCoeff)
        {velocity.y = abs(velocity.y);  pos.y = RADIUS*borderCoeff;}
    //pos = mod(pos, size);
}


vec2 Fv(inout PointData p0, PointData p1)
{
  float vectLength=length(p1.coord-p0.coord);
  if(vectLength>0){
      bool ghost;
      float d = vectLength;
      float sc=p0.radius.x* radius;
      float fd = 0.8/(1.+d/sc) - 4.0*exp(-d/sc);
      vec2 flocking_force =(p1.velocity - p0.velocity)/(1.+7.0*d/sc);
      vec2 interaction = ((p1.coord-p0.coord)/vectLength)* min(0.01,fd);
      return flocking_force*0.4 + interaction*0.3;
    }
    else{
      return vec2(0.0);
   }
}

vec2 Fv2(PointData p0, PointData p1)
{
    float vectLength=length(p1.coord-p0.coord);
    if(vectLength>0.0 && vectLength<2*radius) {
      //  vec2 interaction = -0.75*(p1.coord-p0.coord)/pow(vectLength,2);
        vec2 interaction = -3.75*(p1.coord-p0.coord)/pow(vectLength,2);
        return interaction;
    }
    else if (vectLength>=2*radius) {
        vec2 interaction = -3.75*(p1.coord-p0.coord)/vectLength;
        return interaction;
    }
    else  {
      return vec2(0.0);
    }
}

vec2 Fv3(PointData p0, PointData p1)
{
    return .12*dt* vec2(perlinNoise(p1.coord/worldSize.x, vec2(23.0,23.0),151),
                        perlinNoise(1.0*p1.coord/worldSize.y, vec2(23.0,23.0),151));
}

vec2 MaxVelocity(vec2 velocity)
{
    float d = length(velocity);
    if(d>maxVelocity)
    {
      return (velocity/d)*maxVelocity;
    }
    return velocity;
}

const float stiffness=stiffnessCoeff;//0.035;
const float damping=dampingCoeff;//0.2;
vec2  spring(vec2 posdif,vec2 veldif, float edgelen)
{
    float d = length(posdif);
    if(d>0.0)
    {
      vec2 vel = normalize(posdif)*(d-edgelen)*stiffness; // spring
      if(dot(veldif,veldif)!=0.)
      {
        vel +=normalize(posdif)*( dot(normalize(posdif),veldif)*damping); // damper
      }
      return vel;
    }
    else
     return vec2(0);
}

void main() {

    int indx = int(gl_GlobalInvocationID.x);
    if(indx>=numPoints || indx<0)
      return;

    //pointData[indx].force=vec2(0);
    vec2 force=vec2(0);
  //  int neightbSteps =neightbSize[indx];
    int neightbSteps =min(neightbSize[indx],32);
    for(int inp=0; inp<neightbSteps;inp++)
    {
        int nindex=neightbSort[GetNeightbIndex(indx,inp)];
        force+=Fv(pointData[indx], pointData[nindex]);
      //  force+=0.07*Fv2(pointData[indx],pointData[nindex]);
        //force=MaxVelocity(force);
    }

    float f, scale = 34.,      // scale of noise
      sampling = 7.;       // density of vectors

   //vec2 uv=vec2(1.0-2.0*pointData[indx].coord.x/worldSize.x, 1.0-2.0*pointData[indx].coord.y/worldSize.y);
   vec2 uv=vec2(pointData[indx].coord.x/worldSize.x,pointData[indx].coord.y/worldSize.y);

    vec2 forceNoise=noiseCoeff*0.05*dt*noise2(floor(scale*uv)/sampling, tick );

  //  vec2 forceNoise=.225*dt* vec2(perlinNoise(pointData[indx].coord/worldSize.x, vec2(15.0,15.0),1111),
  //                                perlinNoise(pointData[indx].coord/worldSize.y, vec2(15.0,15.0),1331));

    float springDist= radius*1.2;

    if(pointData[indx].headTail.x>0 && pointData[indx].headTail.y>0)
    {
       force+=spring(pointData[int(pointData[indx].prevNext.x)].coord-pointData[indx].coord,
                                pointData[int(pointData[indx].prevNext.x)].velocity-pointData[indx].velocity,springDist);
       force+=spring(pointData[int(pointData[indx].prevNext.y)].coord-pointData[indx].coord,
                               pointData[int(pointData[indx].prevNext.y)].velocity-pointData[indx].velocity,springDist);
    }
    if(pointData[indx].headTail.x>0 && pointData[indx].headTail.y<0)
    {
       force+=spring(pointData[int(pointData[indx].prevNext.x)].coord-pointData[indx].coord,
                                pointData[int(pointData[indx].prevNext.x)].velocity-pointData[indx].velocity,springDist);
    }
    if(pointData[indx].headTail.x<0 && pointData[indx].headTail.y>0)
    {
       force+=spring(pointData[int(pointData[indx].prevNext.y)].coord-pointData[indx].coord,
                               pointData[int(pointData[indx].prevNext.y)].velocity-pointData[indx].velocity,springDist);
    }

    barrier             ();
    pointData[indx].velocity+=force*dt+forceNoise;
  //  pointData[indx].velocity+=forceNoise;
    pointData[indx].velocity=MaxVelocity(pointData[indx].velocity);
    pointData[indx].coord+=pointData[indx].velocity*dt;

    BorderConditions(pointData[indx].coord, pointData[indx].velocity, worldSize, radius);

    pointPos[indx*4+0]=pointData[indx].coord.x;
    pointPos[indx*4+1]=pointData[indx].coord.y;
    pointPos[indx*4+2]=pointData[indx].force.x;
    pointPos[indx*4+3]=pointData[indx].radius.x;
}

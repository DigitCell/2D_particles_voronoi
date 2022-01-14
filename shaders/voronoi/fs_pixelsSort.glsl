#version 330 core

in vec2 texCoord;

out vec4 U;
vec2 fragCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);

//uniform vec2 iMouse;
uniform vec2 iResolution;
uniform int iFrame;
uniform sampler2D compTexture0;
uniform sampler2D compTexture1;
uniform sampler2D compTexture2;


#define PI 3.14159265359
#define TWOPI 6.28318530718

//configurable settings:
#define NP 1400. //number of particles,  must be <=  (iResolution.x)^2
#define NGH 200 // number of ghosts scanned each frame
#define dt 1.5 // time interval;  dt<0.5 => 1% ghosts,  dt>2. => 50% ghosts
#define ZOOM 1. // 1.= full resolution 2.=half resolution with faster movement and lower max density
#define DIST 2.5 //average dist
//#define N8  //interact with 8 particles (default is with 4 particle)
//#define VORONOI //voronoi display, better with few particles

//-----------------------
#define ZERO  min(iFrame,0)
#define N ivec2(int(sqrt(NP)),int(sqrt(NP)))
#define size (iResolution.xy/ZOOM)
#define mouse (iMouse/ZOOM)
#define texel(a, p) texelFetch(a, ivec2(p), 0)


int tot_n = N.x*N.y;

ivec2 i2xy(int id){
    return ivec2(id%N.x, id/N.x);
}

int xy2i(ivec2 p){
    return p.x + p.y*N.x;
}

ivec4 getClosest(ivec2 p)
{ return floatBitsToInt(texel(compTexture2, p));}

ivec4 getNeighbor(int id)
{ return floatBitsToInt(texel(compTexture1, i2xy(id)));} \

ivec4[2]
 getNeighbor2(int id){ uvec4 v= floatBitsToUint(texel(compTexture1, i2xy(id))); return ivec4[2](ivec4(v&65535u), ivec4(v>>16)); }\

vec4 getParticle(int id,out bool ghost)
{ vec4 p= texel(compTexture0, i2xy(id));ghost=min(p.x,p.y)<0.;return vec4(abs(p.xy),p.zw); }



ivec4 u; //ids
vec4 d; //distances
vec2 pos; //this particle position
int tid;
bool ghost;

ivec2 cross_distribution(int i)
{
    return (1<<(i/4)) * ivec2( ((i&2)/2)^1, (i&2)/2 ) * ( 2*(i%2) - 1 );
}

float particleDistance(int id, vec2 p)
{
    return distance(getParticle(id,ghost).xy, p);
}

//insertion sort
void sort(int utemp)
{
    if(utemp <0) return;
   	float dtemp = particleDistance(utemp, pos);
    //sorting
    if(d.x > dtemp)
    {
        d = vec4(dtemp, d.xyz);
        u = ivec4(utemp, u.xyz);
    }
    else if(d.y > dtemp && dtemp > d.x)
    {
        d.yzw = vec3(dtemp, d.yz);
        u.yzw = ivec3(utemp, u.yz);
    }
    else if(d.z > dtemp && dtemp > d.y)
    {
        d.zw = vec2(dtemp, d.z);
        u.zw = ivec2(utemp, u.z);
    }
    else if(d.w > dtemp && dtemp > d.z)
    {
        d.w = dtemp;
        u.w = utemp;
    }
}

void sortpos(ivec2 p)
{
    ivec4 nb = getClosest(p);
    for(int j = ZERO; j < 4; j++)
    {
        sort(nb[j]);
    }
}

void sortPart(int id)
{
    ivec4 nb = getNeighbor(id);
    for(int j = ZERO; j < 4; j++)
    {
        sort(nb[j]);
    }
}

void main( )
{

  pos = fragCoord;

  if (any(greaterThan(pos,ceil(size) ))) { discard;} //skip pixels out of screen

  ivec2 p = ivec2(pos);

  u = ivec4(-1); d = vec4(1e10);

  sortpos(p); //resort this position, cause particles are moving

  //jump flood sorting; minimized gere, raising to 12 or 16 has small effect and  high impact on framerate
  for(int i = ZERO; i < 8; i++)
  {
      sortpos(p+cross_distribution(i));
  }

  /*
  // random sort skipped, use particle random sort instead
  for(int i = 0; i < 100; i++)
  {
      //int id=hash(ivec4(p, iFrame, i)).x%tot_n;
      int id=(iFrame*100+i) % tot_n;
      vec4 pp= getParticle(id,ghost);
      if(ghost) sort(id); //random sort
  }
  */

  // sort neighbors of closest particles
  for(int i = ZERO; i<4; i++)
  {
      if(u[i]<0) continue;
      sortPart(u[i]);
  }

  // if( any(lessThan(u, ivec4(-1))) || any(greaterThan(u, ivec4(tot_n))) )  u = ivec4(0);


  U = intBitsToFloat(u);
}

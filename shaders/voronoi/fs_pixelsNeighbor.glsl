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

//insertion sort
void sort(int utemp)
{
    if(utemp == tid || utemp < 0) return;

   	vec4 part = getParticle(utemp,ghost);
    vec2 dx = part.xy - pos;
    float dtemp = length(dx);
    //sorting
    if(dx.x > abs(dx.y))
    {
        if(d.x > dtemp)
        {
            d.x = dtemp;
        	u.x = utemp;
        }
    }
    else if(dx.x < -abs(dx.y))
    {
        if(d.y > dtemp)
        {
            d.y = dtemp;
        	u.y = utemp;
        }
    }
    else if(dx.y > abs(dx.x))
    {
        if(d.z > dtemp)
        {
            d.z = dtemp;
        	u.z = utemp;
        }
    }
    else if(d.w > dtemp)
    {
        d.w = dtemp;
        u.w = utemp;
    }
}

void sortneighbor(int id)
{
    ivec4 nb = getNeighbor(id);
    for(int j = 0; j < 4; j++)
    {
        sort(nb[j]);
    }
}

void main()
{

    ivec2 p = ivec2(fragCoord);

    if(p.x > N.x || p.y > N.y) discard;

    int id = xy2i(p);

    u = ivec4(-1); d = vec4(1e10);

    tid = id;
    pos = getParticle(id,ghost).xy;

    sortneighbor(id);

    for(int i = 0; i < NGH; i++)
    {
        //sort(hash(ivec4(p, iFrame, i)).x%tot_n); //random sort
        int id=(iFrame*NGH+i) % tot_n;
        vec4 pp= getParticle(id,ghost);
        if(ghost) sort(id); //random sort
    }
    ivec4 nc = getClosest(ivec2(pos));
    for(int i = ZERO; i < 4; i++)
    {
        sort(nc[i]);
    }

    ivec4 nb = getNeighbor(id);
    for(int i = 0; i < 4; i++)
    {
        //sort 4x4  particles
        sortneighbor(nb[i]);
         ivec4 nbb = getNeighbor(nb[i]);
        for(int j = 0; j < 4; j++)
    	{
            //sort 4x4x4  particles
            sortneighbor(nbb[j]);
        }
    }

    if( any(lessThan(u, ivec4(-1))) || any(greaterThan(u, ivec4(tot_n))))
    {
        u = ivec4(0);
    }


    U = intBitsToFloat(u);



}

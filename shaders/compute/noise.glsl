#define CS(a) cos(a + vec2(0,33))
vec2 g( vec2 n ) { return CS(n.x*n.y); }  // use better hash than n.x*n.y
vec2 noise2(vec2 p, int tick)
{
    const float kF = 3.1415927;

    vec2 i = floor(p);
	  vec2 f = fract(p);
    f = f*f*(3.-2.*f);
    float t = (mod(i.x+i.y,2.)*2.-1.) * (tick*0.1);       // checkered rotation ( flownoise )
    return mix(mix(CS( t+ kF*dot(p,g(i+vec2(0,0)))), // Gabor kernel (and overlapping neighbors)
               	   CS(-t+ kF*dot(p,g(i+vec2(1,0)))),f.x),
               mix(CS(-t+ kF*dot(p,g(i+vec2(0,1)))),
               	   CS( t+ kF*dot(p,g(i+vec2(1,1)))),f.x),f.y);
}

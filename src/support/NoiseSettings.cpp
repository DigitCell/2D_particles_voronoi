#include "NoiseSettings.h"

#include <stdlib.h>


void NoiseSettings::seed(int newOffsetSeed)
{
    offsetSeed = newOffsetSeed;
    srand(offsetSeed);

   // offset.x = (float)rand() - (float)RAND_MAX/2.f;
   // offset.y = (float)rand() - (float)RAND_MAX/2.f;
   // offset.z = (float)rand() - (float)RAND_MAX/2.f;
}

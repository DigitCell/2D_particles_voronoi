#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include <vector>
#include <cmath> 


#include "meshData.h"
#include "pointTransformer.h"


class LineUtils {
    public:
       glm::vec3 getIntersectionWithXYplane(const glm::vec3 & lineEnd, const glm::vec3 & lineStart) {

        auto delta = lineEnd-lineStart;
        
        double xCross = abs(delta.x) < 1e-12 ? lineEnd.x : (((delta.z / delta.x * lineStart.x) - lineStart.z ) / (delta.z/delta.x));
        double yCross = abs(delta.y) < 1e-12 ? lineEnd.y : (((delta.z / delta.y * lineStart.y) - lineStart.z ) / (delta.z/delta.y));
        return glm::vec3(xCross,yCross,0);


    }


      bool  getIntersectionWithXYplane(const glm::vec3 &b, const glm::vec3 &a, float& t, glm::vec3& pointOfIntersect) {

        auto ab = b-a;
        glm::vec3 planeNormal=glm::vec3(0.0f,0.0f,1.0f);

        float divider=glm::dot(planeNormal,ab);
        //if(divider>0.00000001f or divider<-0.0000001f)
        {
            t=-glm::dot(planeNormal,a)/divider;
        }
        /*
        else {
            return false;
        }
*/
        if (t >= 0.0f && t <= 1.0f) {
            pointOfIntersect = a+ t * ab;
            return true;
        }

        // Else no intersection

        return false;


    }
};


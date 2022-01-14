#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>


class PointTransformer {
    public : 
        PointTransformer (glm::vec3 orginOrientation, glm::vec3 targetOrientation,glm::vec3 offsetToOrigin );

        glm::vec3 transformOriginToTarget(const glm::vec3 & v);
        glm::vec3 transformTargetToOrigin(const glm::vec3 & v);

        glm::vec3 mirrorZ(const glm::vec3 &v);
private :
        // Returns a quaternion such that q*start = dest
        glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

        // use boolean to lazy calculate rotations
        glm::quat _rotateOriginToTarget;
        bool _rotateOriginToTargetIsSet;
        glm::quat _rotateTargetToOrigin;
        bool _rotateTargetToOriginIsSet;

        glm::vec3 _origin; 
        glm::vec3 _target;
        glm::vec3 _offsetToOrigin;

};

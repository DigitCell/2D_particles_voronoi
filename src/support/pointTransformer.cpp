#include "../include/pointTransformer.h"

using namespace glm;


PointTransformer::PointTransformer (glm::vec3 orginOrientation, glm::vec3 targetOrientation,glm::vec3 offsetToOrigin ):
_origin ( orginOrientation ),_target ( targetOrientation ),_offsetToOrigin ( offsetToOrigin) {

}

glm::vec3 PointTransformer::transformOriginToTarget(const glm::vec3 & v) {

    //if ( !_rotateOriginToTargetIsSet)
    {
        _rotateOriginToTarget = rotationBetweenVectors(_origin, _target);
       // _rotateOriginToTarget = glm::rotation(_origin, _target);
        }
   // _rotateOriginToTarget;
    return (_rotateOriginToTarget * (v-_offsetToOrigin));
}
glm::vec3 PointTransformer::transformTargetToOrigin(const glm::vec3 & v){
    //  if ( !_rotateTargetToOriginIsSet)
    {
       _rotateTargetToOrigin = rotationBetweenVectors(_target,_origin);
       //   _rotateTargetToOrigin = rotation(_target,_origin);
    }    
    return ((_rotateTargetToOrigin * v) + _offsetToOrigin);
}


glm::vec3 PointTransformer::mirrorZ(const glm::vec3& v){

    glm::vec3 mirrorV=v;
    mirrorV.z*=-1.0f;
    return mirrorV;
}

    
glm::quat PointTransformer::rotationBetweenVectors(glm::vec3  start, glm::vec3  dest){
    start = normalize(start);
	dest = normalize(dest);

	
	float cosTheta = dot(start, dest);
	vec3 rotationAxis;

    if (cosTheta < -1.0f + 0.0001f){
		// special case when vectors in opposite directions :
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		// This implementation favors a rotation around the Up axis,
		rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
        if (length2(rotationAxis) < 0.01f ) // bad luck, they were parallel, try again!
			rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);
		
		rotationAxis = normalize(rotationAxis);
		return angleAxis(glm::radians(180.0f), rotationAxis);
	}

	// Implementation from Stan Melax's Game Programming Gems 1 article
	rotationAxis = cross(start, dest);

    float s = sqrt( (1.0f+cosTheta)*2.0f );
    float invs = 1.0f / s;

	return quat(
		s * 0.5f, 
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);



}
      

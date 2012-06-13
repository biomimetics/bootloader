/***************************************************************************
* Name: tracking.c
* Desc: Target Tracking
* Date: 2009-06-28
* Author: stanbaek
**************************************************************************/

#include "init.h"

#define MAX_DISTANCE    1634818     // 1023^2 + 767^2
#define MAX_SLOPE       2

// const float p_gain = 0.1;

const Target REF = {512, 384, 0, 0};
const Target DEFAULT_TARGET = {512, 384, 0, 0}; 

Target Current_Target;

Target* Tracking_FindTarget(Blob *blobs) {

    double dist, smallestDist;
    unsigned int i, j;
    float slope;
    static Target prevTarget = {512, 384, 0, 0};
    Target center;

    smallestDist = MAX_DISTANCE;

    Current_Target.x = DEFAULT_TARGET.x;
    Current_Target.y = DEFAULT_TARGET.y;
    Current_Target.slope = 0;
    Current_Target.distance = prevTarget.distance;

    for (i = 0; i < 3; i++) {
        for (j = i+1; j < 4; j++) {
            if ( (blobs[j].size != BLOB_SIZE_NULL) && ( blobs[i].size != BLOB_SIZE_NULL) ) {
                if (blobs[i].x != blobs[j].x) {
                    slope = 1.0*(blobs[i].y - blobs[j].y)/(blobs[i].x - blobs[j].x);
                    if (fabs(slope) < MAX_SLOPE) {
                        center.x = (blobs[i].x + blobs[j].x) >> 1;
                        center.y = (blobs[i].y + blobs[j].y) >> 1;
                        dist = pow(prevTarget.x - center.x, 2) + pow(prevTarget.y - center.y, 2);
                        if (dist < smallestDist) {
                            Current_Target.x = center.x;
                            Current_Target.y = center.y;
                            Current_Target.distance = (int)fabs(blobs[i].x - blobs[j].x);
                            Current_Target.slope = slope;
                            smallestDist = dist;
                        }
                    }
                }
            }
        }
    }

    prevTarget.x = Current_Target.x;
    prevTarget.y = Current_Target.y;
    prevTarget.slope = Current_Target.slope;
    prevTarget.distance = Current_Target.distance;
    
    return &Current_Target;
        
}    

Target* Tracking_GetCurrentTarget(void) {
    return &Current_Target;
}


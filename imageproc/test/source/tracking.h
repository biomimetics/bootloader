/******************************************
* Name: tracking.h                     
* Desc: Header for target tracking subroutines
* Date: 2009-6-29
* Author: stanbaek
******************************************/



typedef struct {
    int x;
    int y;
    int distance;
    float slope;
} Target;


Target* Tracking_FindTarget(Blob *blobs);
Target* Tracking_GetCurrentTarget(void);



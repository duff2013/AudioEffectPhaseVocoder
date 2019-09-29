/*****************************************************************/
//https://gist.github.com/volkansalma/2972237#file-atan2_approximation-c
#include "Arduino.h"
#define PIBY2_FLOAT  PI/2.0f

// |error| < 0.005
float atan2_fast( float y, float x ) {
    if ( x == 0.0f ) {
        if ( y > 0.0f ) return PIBY2_FLOAT;
        if ( y == 0.0f ) return 0.0f;
        return -PIBY2_FLOAT;
    }
    float atan;
    float z = y / x;
    if ( fabs( z ) < 1.0f ) {
        atan = z / (1.0f + 0.28f * z * z);
        if ( x < 0.0f ) {
            if ( y < 0.0f ) return atan - PI;
            return atan + PI;
        }
    }
    else {
        atan = PIBY2_FLOAT - z / (z * z + 0.28f);
        if ( y < 0.0f ) return atan - PI;
    }
    return atan;
}

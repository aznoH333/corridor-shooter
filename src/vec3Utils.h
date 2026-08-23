#ifndef VEC_3_UTILS
#define VEC_3_UTILS

#include "raylib.h"

// mat3
typedef struct {
    float values[3][3];
} Mat3;


Mat3 Mat3Zero();
Mat3 Mat3One();



// vector manipulation
Vector3 Vector3MultiplyByMat(Vector3 vector, Mat3 mat);
Vector3 Vector3Rotate(Vector3 vector, float yaw, float pitch, float roll);

#endif
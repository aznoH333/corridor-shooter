#include "vec3Utils.h"
#include "raymath.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Mat 3 utils#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Mat3 Mat3Zero() {
    Mat3 mat = { .values = {0} };

    return mat;
}

Mat3 Mat3One() {
    Mat3 mat = Mat3Zero();


    mat.values[0][0] = 1;
    mat.values[1][1] = 1;
    mat.values[2][2] = 1;

    return mat;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Vector manipulation#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Vector3MultiplyByMat(Vector3 vector, Mat3 mat) {
    Vector3 output = {.x = 0, .y = 0, .z = 0};
    float convertedInput[3] = { vector.x, vector.y, vector.z };


    for (int i = 0; i < 3; ++i) {
        output.x += mat.values[i][0] * convertedInput[i];
        output.y += mat.values[i][1] * convertedInput[i];
        output.z += mat.values[i][2] * convertedInput[i];

    }


    return output;
}

Vector3 Vector3Rotate(Vector3 vector, float yaw, float pitch, float roll) {
    Vector3 output = vector;


    float cosY = cosf(yaw);
    float sinY = sinf(yaw);

    float cosP = cosf(pitch);
    float sinP = sinf(pitch);

    float cosR = cosf(roll);
    float sinR = sinf(roll);

    Mat3 yawRotation = {
        .values = {
            { cosY, 0, -sinY },
            {    0, 1,     0 },
            { sinY, 0,  cosY }
        }
    };

    Mat3 pitchRotation = {
        .values = {
            { 1,    0,     0 },
            { 0, cosP, -sinP },
            { 0, sinP,  cosP }
        }
    };

    Mat3 rollRotation = {
        .values = {
            { cosR, -sinR, 0 },
            { sinR,  cosR, 0 },
            {  0,       0, 1 }
        }
    };

    output = Vector3MultiplyByMat(output, yawRotation);
    output = Vector3MultiplyByMat(output, pitchRotation);
    output = Vector3MultiplyByMat(output, rollRotation);

    return output;
}


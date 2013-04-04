//-----------------------------------------------------------------------------
//           Name: vector3f.h
//         Author: Kevin Harris
//  Last Modified: 06/04/12
//    Description: OpenGL compatible utility class for a 3D vector of floats
//                 NOTE: This class has been left unoptimized for readability.
//-----------------------------------------------------------------------------

#ifndef _GUILDHALL_VECTOR3F_H_
#define _GUILDHALL_VECTOR3F_H_

namespace guildhall {

class Vector3f
{
public:

    float x;
    float y;
    float z;

    Vector3f(void)
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    Vector3f(float x_, float y_, float z_);
    void set(float x_, float y_, float z_);
    float length(void);
    void normalize(void);

    // Static utility methods
    static float distance(const Vector3f &v1, const Vector3f &v2);
    static float dotProduct(const Vector3f &v1,  const Vector3f &v2 );
    static Vector3f crossProduct(const Vector3f &v1, const Vector3f &v2);

    // Operators...
    Vector3f operator + (const Vector3f &other);
    Vector3f operator - (const Vector3f &other);
    Vector3f operator * (const Vector3f &other);
    Vector3f operator / (const Vector3f &other);

    Vector3f operator * (const float scalar);
    friend Vector3f operator * (const float scalar, const Vector3f &other);
    
    Vector3f& operator = (const Vector3f &other);
    Vector3f& operator += (const Vector3f &other);
    Vector3f& operator -= (const Vector3f &other);

    Vector3f operator + (void) const;
    Vector3f operator - (void) const;
};

}
#endif // _GUILDHALL_VECTOR3F_H_

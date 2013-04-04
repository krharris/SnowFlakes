#include "vector3f.h"
#include <math.h>

namespace guildhall {

Vector3f::Vector3f( float x_, float y_, float z_ )
{
    x = x_;
    y = y_;
    z = z_;
}

void Vector3f::set( float x_, float y_, float z_ )
{
    x = x_;
    y = y_;
    z = z_;
}

float Vector3f::length( void )
{
    return( (float)sqrt( x * x + y * y + z * z ) );
}

void Vector3f::normalize( void )
{
    float fLength = length();

    x = x / fLength;
    y = y / fLength;
    z = z / fLength;
}

// Operators...

Vector3f Vector3f::operator + ( const Vector3f &other )
{
    Vector3f vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x + other.x;
    vResult.y = y + other.y;
    vResult.z = z + other.z;

    return vResult;
}

Vector3f Vector3f::operator + ( void ) const
{
    return *this;
}

Vector3f Vector3f::operator - ( const Vector3f &other )
{
    Vector3f vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x - other.x;
    vResult.y = y - other.y;
    vResult.z = z - other.z;

    return vResult;
}

Vector3f Vector3f::operator - ( void ) const
{
    Vector3f vResult(-x, -y, -z);

    return vResult;
}

Vector3f Vector3f::operator * ( const Vector3f &other )
{
    Vector3f vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x * other.x;
    vResult.y = y * other.y;
    vResult.z = z * other.z;

    return vResult;
}

Vector3f Vector3f::operator * ( const float scalar )
{
    Vector3f vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x * scalar;
    vResult.y = y * scalar;
    vResult.z = z * scalar;

    return vResult;
}

Vector3f operator * ( const float scalar, const Vector3f &other )
{
    Vector3f vResult(0.0f, 0.0f, 0.0f);

    vResult.x = other.x * scalar;
    vResult.y = other.y * scalar;
    vResult.z = other.z * scalar;

    return vResult;
}

Vector3f Vector3f::operator / ( const Vector3f &other )
{
    Vector3f vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x / other.x;
    vResult.y = y / other.y;
    vResult.z = z / other.z;

    return vResult;
}

Vector3f& Vector3f::operator = ( const Vector3f &other )
{
    x = other.x;
    y = other.y;
    z = other.z;

    return *this;
}

Vector3f& Vector3f::operator += ( const Vector3f &other )
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vector3f& Vector3f::operator -= ( const Vector3f &other )
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

// Static utility methods...

float Vector3f:: distance( const Vector3f &v1,  const Vector3f &v2  )
{
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    float dz = v1.z - v2.z;

    return (float)sqrt( dx * dx + dy * dy + dz * dz );
}

float Vector3f::dotProduct( const Vector3f &v1,  const Vector3f &v2 )
{
    return( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z  );
}

Vector3f Vector3f::crossProduct( const Vector3f &v1,  const Vector3f &v2 )
{
    Vector3f vCrossProduct;

    vCrossProduct.x = v1.y * v2.z - v1.z * v2.y;
    vCrossProduct.y = v1.z * v2.x - v1.x * v2.z;
    vCrossProduct.z = v1.x * v2.y - v1.y * v2.x;

    return vCrossProduct;
}

}

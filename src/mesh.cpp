#include <math.h>
#include "mesh.h"
#include <QVector3D>
#include <QColor>

namespace p3d {

    Vector3::Vector3() {}

    Vector3::Vector3(const QVector3D& qtVector)
            : x(qtVector.x()), y(qtVector.y()), z(qtVector.z()) {}

    Vector3::Vector3(float xx, float yy, float zz)
            : x(xx), y(yy), z(zz) {}

    Vector3 Vector3::operator+(const Vector3& vec) const {
        return Vector3(vec.x + x, vec.y + y, vec.z + z);
    }

    Vector3 Vector3::operator-(const Vector3& vec) const {
        return *this + (-vec);
    }

    Vector3 Vector3::operator-() const {
        return Vector3(-x, -y, -z);
    }

    void Vector3::set(float xx, float yy, float zz) {
        x = xx;
        y = yy;
        z = zz;
    }

    Vector3 Vector3::norm() {
        float d = sqrtf(x * x + y * y + z * z);
        if (d == 0) {
            return *this;
        }
        return Vector3(x / d, y / d, z / d);
    }

    Vector3 operator*(float f, const Vector3& vec) {
        return Vector3(f * vec.x, f * vec.y, f * vec.z);
    }

    float dot(const Vector3& vec1, const Vector3& vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    void TexCoords::set(float uu, float vv) {
        u = uu;
        v = vv;
    }

    Color::Color() {}

    Color::Color(const QColor& qtColor)
            : r((float) qtColor.redF())
            , g((float) qtColor.greenF())
            , b((float) qtColor.blueF())
            , a((float) qtColor.alphaF()) {}

    Color::Color(float rr, float gg, float bb, float aa)
            : r(rr), g(gg), b(bb), a(aa) {}

    void Color::set(float rr, float gg, float bb, float aa) {
        r = rr;
        g = gg;
        b = bb;
        a = aa;
    }

}

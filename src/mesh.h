#pragma once

class QVector3D;
class QColor;

namespace p3d {
    struct Vector3 {
        union {
            struct {
                float x, y, z;
            };
            float a[3];
        };

        Vector3();

        Vector3(const QVector3D& qtVector);

        Vector3(float xx, float yy, float zz);

        Vector3 operator+(const Vector3& vec) const;

        Vector3 operator-(const Vector3& vec) const;

        Vector3 operator-() const;

        void set(float xx, float yy, float zz);

        Vector3 norm();
    };

    Vector3 operator*(float f, const Vector3& vec);

    float dot(const Vector3& vec1, const Vector3& vec2);

    struct TexCoords {
        union {
            struct {
                float u, v;
            };
            float a[2];
        };

        void set(float uu, float vv);
    };

    struct Color {
        union {
            struct {
                float r, g, b, a;
            };
            float c[4];
        };

        Color();

        Color(const QColor& qtColor);

        Color(float rr, float gg, float bb, float aa = 1);

        void set(float rr, float gg, float bb, float aa = 1);
    };
}

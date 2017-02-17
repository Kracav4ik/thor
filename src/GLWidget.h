#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QWheelEvent>


class Camera{
private:
    float rot_x;
    float rot_y;
    float pos_x;
    float pos_y;
    float pos_z;
    float scale;
public:
    Camera()
            :rot_x(0)
            ,rot_y(0)
            ,pos_x(0)
            ,pos_y(0)
            ,pos_z(-0.7f)
            ,scale(0.3f)
    {}
    void zoom_in();

    void zoom_out();

    QMatrix4x4 get_matrix() const;

    void rotate(float angle_x, float angle_y);

    void move_x(float dist);

    void move_y(float dist);
};


class GLWidget: public QOpenGLWidget, private QOpenGLFunctions {
    Q_OBJECT
private:
    Camera cam;
    QOpenGLShaderProgram* program;
    GLuint pos_attr;
    QMatrix4x4 mvp;
    GLuint uv_attr;
    GLuint texture;
    float rot_x2;
    float rot_y2;
    GLuint tex_attr;
    GLuint u_mvp;
    QPoint pos;

protected:
    virtual void wheelEvent(QWheelEvent* event) override;

    virtual void mouseMoveEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void refresh_mvp();

    virtual void paintGL() override;

    virtual void initializeGL() override;

public:
    GLWidget(QWidget* parent);
};


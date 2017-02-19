#include "GLWidget.h"
#include "mesh.h"

using namespace p3d;

struct MeshPoint {
    Vector3 pos;
    Color color;
};

QVector<MeshPoint> drawThor(float r, float d, float da1, float da2, const QColor& color) {
    QVector<MeshPoint> q;
    for (int a1 = 0; a1 < 360; a1 += 1) {
        QMatrix4x4 m1;
        m1.rotate(a1 + da1, 1, 0, 0);
        for (int a2 = 0; a2 < 360; a2 += 6) {
            QMatrix4x4 m2;
            m2.translate(QVector3D(0, r + d, 0));
            m2.rotate(a2 + da2, 0, 0, 1);
            q.append({m1 * m2 * QVector3D(d, 0, 0), color});
        }
    }
    return q;
}

QVector<MeshPoint> drawAxis(QVector3D a, QVector3D b, const QColor& color) {
    QVector<MeshPoint> q;
    for (int i = 0; i <= 100; ++i) {
        float k = i / 100.f;
        q.append({a * k + (1 - k) * b, color});
    }
    return q;
}


void Camera::zoom_in() {
    scale *= 1.2f;
}

void Camera::zoom_out() {
    scale /= 1.2f;
}

QMatrix4x4 Camera::get_matrix() const {
    QMatrix4x4 res;
    res.scale(scale);
    res.rotate(rot_y, 1, 0, 0);
    res.rotate(rot_x, 0, 0, -1);
    res.translate(-pos_x, -pos_y, -pos_z);
    return res;
}

void Camera::rotate(float angle_x, float angle_y) {
    rot_x += angle_x;
    rot_y += angle_y;
}

void Camera::move_x(float dist) {
    QMatrix4x4 m;
    m.rotate(rot_x, 0, 0, 1);
    QPointF step = m * QPointF(dist, 0);
    pos_x += step.x();
    pos_y += step.y();
}

void Camera::move_y(float dist) {
    QMatrix4x4 m;
    m.rotate(rot_x, 0, 0, 1);
    QPointF step = m * QPointF(0, dist);
    pos_x += step.x();
    pos_y += step.y();
}

void GLWidget::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        cam.zoom_in();
    } else {
        cam.zoom_out();
    }
    refresh_mvp();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
    QPoint point = event->pos();
    if (pos.isNull()) {
        pos = point;
        return;
    }

    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = pos - point;
        cam.rotate(.5f * delta.x(), .5f * delta.y());
        refresh_mvp();
    } else if (event->buttons() & Qt::RightButton) {
        QPoint delta2 = pos - point;
        rot_x2 += .5 * delta2.x();
        rot_y2 += .5 * delta2.y();
        refresh_mvp();
    }
    pos = point;
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
    printf("%s\n", event->text().toUtf8().data());
    static const float STEP = .25f;
    switch (event->key()) {
        case Qt::Key_W:
            cam.move_y(STEP);
            break;
        case Qt::Key_S:
            cam.move_y(-STEP);
            break;
        case Qt::Key_A:
            cam.move_x(-STEP);
            break;
        case Qt::Key_D:
            cam.move_x(STEP);
            break;
        default:
            break;
    }
    refresh_mvp();
}

void GLWidget::refresh_mvp() {
    mvp = cam.get_matrix();
    mvp.rotate(rot_x2, 0, 1, 0);
    mvp.rotate(rot_y2, 1, 0, 0);

    update();
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    program->bind();

    QVector<MeshPoint> vertices = drawThor(3, 2, .3f * ++da1, .2f * ++da2, QColor(255, 255, 0));
    vertices << drawAxis(QVector3D(0, 0, -5), QVector3D(0, 0, 10), QColor(255, 0, 0));
    vertices << drawAxis(QVector3D(0, -5, 0), QVector3D(0, 10, 0), QColor(0, 255, 0));
    vertices << drawAxis(QVector3D(-5, 0, 0), QVector3D(10, 0, 0), QColor(0, 0, 255));

    program->setUniformValue(u_mvp, mvp);

    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), &vertices.data()->pos);
    glVertexAttribPointer(col_attr, 4, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), &vertices.data()->color);

    glEnableVertexAttribArray(pos_attr);
    glEnableVertexAttribArray(col_attr);

    glDrawArrays(GL_POINTS, 0, vertices.length());

    glDisableVertexAttribArray(col_attr);
    glDisableVertexAttribArray(pos_attr);

    program->release();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(.3, 0, .3, 1);

    glEnable(GL_DEPTH_TEST);

    program = new QOpenGLShaderProgram(this);
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec3 a_position;
attribute vec4 a_color;
uniform mat4 u_mvp;

varying vec4 v_color;

void main(){
    v_color = a_color;
    gl_Position = u_mvp * vec4(a_position, 1);
}
)");
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
varying vec4 v_color;

void main(){
    gl_FragColor = v_color;
}
)");
    program->link();
    pos_attr = (GLuint) program->attributeLocation("a_position");
    col_attr = (GLuint) program->attributeLocation("a_color");
    u_mvp = (GLuint) program->uniformLocation("u_mvp");
}

GLWidget::GLWidget(QWidget* parent)
        : QOpenGLWidget(parent), rot_x2(0), rot_y2(0), da1(0), da2(0) {
    setFocusPolicy(Qt::ClickFocus);

    refresh_mvp();
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(100);
    setMouseTracking(true);
}


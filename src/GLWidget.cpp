#include "GLWidget.h"

QVector<QVector3D> drawThor(float r, float d) {
    QVector<QVector3D> q;
    QMatrix4x4 m;
    for (int _ = 1; _ < 361; ++_) {
        m.rotate(1, 1, 0, 0);
        for (int i = 1; i < 361; ++i) {
            m.translate(QVector3D(1, 0, 0));
            m.rotate(1, 0, 0, 1);
            QVector3D vector3D = QVector3D(1, 1, 0) * r;
            q.append(m * vector3D * d);
        }
    }
    return q;
}

QVector<QVector3D> drawAxis(QVector3D a, QVector3D b) {
    QVector<QVector3D> q;
    for (float _ = 1; _ < 101; ++_) {
        q.append(a * _ / 100 + (1 - _ / 100) * b);
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

    QVector<QVector3D> vertices = drawThor(40, .05);
    QVector<QVector3D> axisz = drawAxis(QVector3D(0, 0, -50), QVector3D(0, 0, 100));
    QVector<QVector3D> axisy = drawAxis(QVector3D(0, -50, 0), QVector3D(0, 100, 0));
    QVector<QVector3D> axisx = drawAxis(QVector3D(-50, 0, 0), QVector3D(100, 0, 0));

    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, vertices.data());

    program->setUniformValue(u_mvp, mvp);

    program->setUniformValue(u_color, QColor(255, 255, 0, 255));

    glEnableVertexAttribArray(pos_attr);

    glDrawArrays(GL_POINTS, 0, vertices.length());

    glDisableVertexAttribArray(pos_attr);

    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, axisx.data());

    program->setUniformValue(u_color, QColor(255, 0, 0, 255));

    glEnableVertexAttribArray(pos_attr);

    glDrawArrays(GL_POINTS, 0, axisx.length());

    glDisableVertexAttribArray(pos_attr);

    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, axisy.data());

    program->setUniformValue(u_color, QColor(0, 255, 0, 255));

    glEnableVertexAttribArray(pos_attr);

    glDrawArrays(GL_POINTS, 0, axisy.length());

    glDisableVertexAttribArray(pos_attr);

    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, axisz.data());

    program->setUniformValue(u_color, QColor(0, 0, 255, 255));

    glEnableVertexAttribArray(pos_attr);

    glDrawArrays(GL_POINTS, 0, axisz.length());

    glDisableVertexAttribArray(pos_attr);

    program->release();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(.3, 0, .3, 1);

    glEnable(GL_DEPTH_TEST);

    program = new QOpenGLShaderProgram(this);
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
// Input vertex data, different for all executions of this shader.
attribute vec3 a_position;
uniform mat4 u_mvp;

void main(){
    gl_Position = u_mvp * vec4(a_position, 1);
}
)");
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
uniform vec4 color;

void main(){
//     Output color = color of the texture at the specified UV
    gl_FragColor = color;
}
)");
    program->link();
    pos_attr = (GLuint) program->attributeLocation("a_position");
    u_mvp = (GLuint) program->uniformLocation("u_mvp");
    u_color = (GLuint) program->uniformLocation("color");
}

GLWidget::GLWidget(QWidget* parent)
        : QOpenGLWidget(parent), rot_x2(0), rot_y2(0) {
    setFocusPolicy(Qt::ClickFocus);

    refresh_mvp();

    setMouseTracking(true);
}


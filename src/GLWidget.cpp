#include "GLWidget.h"

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

    GLfloat vertices[] = {
            -1, -1, 0, 0,
            -1, 1, 0, 1,
            1, -1, 1, 0,
            1, 1, 1, 1,
    };

    GLubyte indices[] = {
            0, 1, 2, 1, 2, 3
    };

    glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices);
    glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices + 2);

    program->setUniformValue(u_mvp, mvp);

    glBindTexture(GL_TEXTURE_2D, texture);

    glEnableVertexAttribArray(pos_attr);
    glEnableVertexAttribArray(uv_attr);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

    glDisableVertexAttribArray(pos_attr);
    glDisableVertexAttribArray(uv_attr);


    program->release();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(.3, 0, .3, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(.75f);

    program = new QOpenGLShaderProgram(this);
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
// Input vertex data, different for all executions of this shader.
attribute vec2 a_position;
attribute vec2 a_vertexUV;
uniform mat4 u_mvp;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;

void main(){
    gl_Position = u_mvp * vec4(a_position, 0, 1);
    // UV of the vertex. No special space for this one.
    UV = a_vertexUV;
}
)");
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

//     Output color = color of the texture at the specified UV
    gl_FragColor = texture2D( myTextureSampler, UV );
}
)");
    program->link();
    pos_attr = (GLuint) program->attributeLocation("a_position");
    uv_attr = (GLuint) program->attributeLocation("a_vertexUV");
    u_mvp = (GLuint) program->uniformLocation("u_mvp");
    tex_attr = (GLuint) program->uniformLocation("myTextureSampler");
}

GLWidget::GLWidget(QWidget* parent)
        : QOpenGLWidget(parent), rot_x2(0), rot_y2(0) {
    setFocusPolicy(Qt::ClickFocus);

    refresh_mvp();

    setMouseTracking(true);
}


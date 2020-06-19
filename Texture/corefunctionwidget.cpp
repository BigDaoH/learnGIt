#include "corefunctionwidget.h"
#include <QDebug>
#include <QTimer>
#include <math.h>

#define ustepNum 20
#define vstepNum 20
#define PI 3.14
static GLuint VBO, VAO, EBO, texture1, texture2;
static GLuint arrayTexture[10];
CoreFunctionWidget::CoreFunctionWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_pTimer(new QTimer(this))
    , m_ftime(0.0f)
    , m_camera(new Camera())
{
    m_pTimer->setInterval(50);

    connect(m_pTimer, &QTimer::timeout, this, [ = ]
    {
        m_ftime += 1.0f;
        update();
    });
    m_pTimer->start();
}

CoreFunctionWidget::~CoreFunctionWidget()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(10, arrayTexture);
}

void CoreFunctionWidget::initializeGL()
{
    this->initializeOpenGLFunctions();

    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/textures.vert");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/textures.frag");
    shaderProgram.link();

    shaderLight.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/light.vert");
    shaderLight.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/light.frag");
    shaderLight.link();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    getSphere();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * points.size(), &points[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexes.size(), &indexes[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    QString fileName[10] = { ":/sun.jpg",
                             ":/mercury.jpg",
                             ":/venus.jpg",
                             ":/earth.jpg",
                             ":/mars.jpg",
                             ":/jupiter.jpg",
                             ":/saturn.jpg",
                             ":/uranus.jpg",
                             ":/neptune.jpg",
                             ":/moon.jpg"
                           };

    glGenTextures(10, arrayTexture);
    for(uint i = 0; i < 10; i++ )
    {
        glBindTexture(GL_TEXTURE_2D, arrayTexture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        QImage img = QImage(fileName[i]).convertToFormat(QImage::Format_RGB888).mirrored();
        if(!img.isNull())
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width(), img.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.bits());
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    shaderProgram.bind();
    shaderProgram.setUniformValue("light.position", 0.0f, 0.0f, 0.0f);
    shaderProgram.setUniformValue("light.ambient", 0.1f, 0.1f, 0.1f);
    shaderProgram.setUniformValue("light.diffuse", 0.8f, 0.8f, 0.8f);
    shaderProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);
    shaderProgram.setUniformValue("light.constant", 1.0f);
    shaderProgram.setUniformValue("light.linear", 0.045f);
    shaderProgram.setUniformValue("light.quadratic", 0.0075f);
    shaderProgram.setUniformValue("material.diffuse", 0);
    shaderProgram.setUniformValue("material.specular", 0);
    shaderProgram.setUniformValue("material.shininess", 32.0f);
    shaderProgram.release();
    glEnable(GL_DEPTH_TEST);
}

void CoreFunctionWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

static float cubeVelocity[] = {0.0f, 1.60f, 1.17f, 1.0f, 0.8f, 0.44f, 0.32f, 0.23f, 0.18f, 1.0f};
static float cubeVolume[] = {1.0f, 0.2f, 0.25f, 0.3f, 0.25f, 0.7f, 0.65f, 0.4f, 0.6f, 0.1f};
static float cubedistance[] = {0.0f, 2.0f, 3.0f, 4.5f, 6.0f, 8.0f, 11.0f, 14.0f, 16.0f, 4.5f};

void CoreFunctionWidget::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_camera->processInput(1.0f);

    shaderLight.bind();

    {
        QMatrix4x4 projection;
        projection.perspective(m_camera->zoom, width() / height(), 0.1f, 500.0f);
        shaderLight.setUniformValue("projection", projection);
        shaderLight.setUniformValue("view", m_camera->getViewMatrix());
        QMatrix4x4 model;
        float angle = cubeVelocity[0] * m_ftime;
        model.rotate(angle, QVector3D(0.0f, 0.0f, 1.0f));
        model.translate(QVector3D(cubedistance[0], 0.0f, 0.0f));
        model.rotate(angle, QVector3D(0.0f, 0.0f, 1.0f));
        model.scale(cubeVolume[0]);
        shaderLight.setUniformValue("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, arrayTexture[0]);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, arrayTexture[0]);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(unsigned int) * indexes.size(), GL_UNSIGNED_INT, 0);
    }

    shaderLight.release();

    shaderProgram.bind();

    QMatrix4x4 projection;
    projection.perspective(m_camera->zoom, width() / height(), 0.1f, 500.0f);
    shaderProgram.setUniformValue("projection", projection);
    shaderProgram.setUniformValue("view", m_camera->getViewMatrix());
    shaderProgram.setUniformValue("viewPos", m_camera->position);

    for(uint i = 1; i < 10; i++)
    {
        QMatrix4x4 model;
        float angle = cubeVelocity[i] * m_ftime;
        model.rotate(angle, QVector3D(0.0f, 0.0f, 1.0f));
        model.translate(QVector3D(cubedistance[i], 0.0f, 0.0f));
        if(9 == i)
        {
            model.rotate(angle * 12, QVector3D(0.0f, 0.0f, 1.0f));
            model.translate(QVector3D(0.7f, 0.0f, 0.0f));
        }
        model.rotate(angle*20, QVector3D(0.0f, 0.0f, 1.0f));
        model.scale(cubeVolume[i]);
        shaderProgram.setUniformValue("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, arrayTexture[i]);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(unsigned int) * indexes.size(), GL_UNSIGNED_INT, 0);
    }
    shaderProgram.release();
}

void CoreFunctionWidget::keyPressEvent(QKeyEvent *event)
{
    uint key = event->key();
    if(key < 1024)
    {
        m_camera->keys[key] = true;
    }
}

void CoreFunctionWidget::keyReleaseEvent(QKeyEvent *event)
{
    uint key = event->key();
    if(key < 1024)
    {
        m_camera->keys[key] = false;
    }
}

void CoreFunctionWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bLeftPressed = true;
        m_posLast = event->pos();
    }
}

void CoreFunctionWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_bLeftPressed = false;
}

void CoreFunctionWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bLeftPressed)
    {
        int xPos = event->pos().x();
        int yPos = event->pos().y();

        int xoffset = xPos - m_posLast.x();
        int yoffset = m_posLast.y() - yPos;
        m_posLast = event->pos();
        m_camera->processMouseMovement(xoffset, yoffset);
    }
}

void CoreFunctionWidget::wheelEvent(QWheelEvent *event)
{
    int offset = event->angleDelta().y();
    m_camera->processMouseScroll(offset / 20.0f);
}

void CoreFunctionWidget::getSphere()
{
    double ustep = 1 / (double)ustepNum, vstep = 1 / (double)vstepNum;
    points.push_back(sin(0)*cos(0));
    points.push_back(sin(0)*sin(0));
    points.push_back(cos(0));
    points.push_back(0.5);
    points.push_back(1);
    for (int i = 1; i < vstepNum; i++)
    {
        for (int j = 0; j <= ustepNum; j++)
        {
            points.push_back(sin(PI * vstep * i)*cos(2 * PI * ustep * j));
            points.push_back(sin(PI * vstep * i)*sin(2 * PI * ustep * j));
            points.push_back(cos(PI * vstep * i));
            points.push_back(vstep * j);
            points.push_back(1 - ustep * i);
        }
    }
    points.push_back(sin(PI * 1)*cos(2 * PI * 1));
    points.push_back(sin(PI * 1)*sin(2 * PI * 1));
    points.push_back(cos(PI * 1));
    points.push_back(0.5);
    points.push_back(0);

    //生成三角面片定点索引信息

    for (int i = 0; i <= ustepNum ; i++)
    {
        indexes.push_back(0);
        indexes.push_back(1 + i);
        indexes.push_back(1 + (i + 1) % (ustepNum + 1));
    }
    for (int i = 1; i < vstepNum - 1; i++)
    {
        int start = 1 + (i - 1) * (ustepNum + 1);
        for (int j = start; j < start + (1 + ustepNum); j++)
        {

            indexes.push_back(j);
            indexes.push_back(j + (1 + ustepNum));
            indexes.push_back(start + (1 + ustepNum) + (j + 1 - start) % (1 + ustepNum));

            indexes.push_back(j);
            indexes.push_back(start + (j + 1 - start) % (1 + ustepNum));
            indexes.push_back(start + ustepNum + 1 + (j + 1 - start) % (1 + ustepNum));
        }
    }
    int last = points.size() / 5 - 1;

    for (int i = last - ustepNum - 1; i < last; i++)
    {
        indexes.push_back(i);
        indexes.push_back(last);
        indexes.push_back(i + 1);
    }
}




















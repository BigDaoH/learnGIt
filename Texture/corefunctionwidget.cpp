#include "corefunctionwidget.h"
#include <QDebug>
#include <QTimer>
#include <math.h>

#define ustepNum 20
#define vstepNum 20
#define PI 3.14
static GLuint VBO, VAO, EBO, texture1, texture2;
CoreFunctionWidget::CoreFunctionWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_pTimer(new QTimer(this))
    , m_ftime(-1.5)
{
    m_pTimer->setInterval(200);

    connect(m_pTimer, &QTimer::timeout, this, [ = ]
    {
        m_ftime += 1.0f;
//        if(m_ftime >= 90.0f)
//            m_ftime = -90.0f;
        update();
    });
    m_pTimer->start();
}

CoreFunctionWidget::~CoreFunctionWidget()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void CoreFunctionWidget::initializeGL()
{
    this->initializeOpenGLFunctions();

    bool success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/textures.vert");
    if(!success)
    {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
        return;
    }

    success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/textures.frag");
    if(!success)
    {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
        return;
    }

    success = shaderProgram.link();
    if(!success)
    {
        qDebug() << "shaderProgram link failed!" << shaderProgram.log();
        return;
    }

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

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    QImage img1 = QImage(":/earth.jpg").convertToFormat(QImage::Format_RGB888).mirrored();
    if(!img1.isNull())
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img1.width(), img1.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img1.bits());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    shaderProgram.bind();
    glUniform1i(shaderProgram.uniformLocation("texture1"), 0);
    shaderProgram.release();
    glEnable(GL_DEPTH_TEST);
}

void CoreFunctionWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void CoreFunctionWidget::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    QMatrix4x4 model;
    model.rotate(m_ftime, QVector3D(1.0f, 0.0f, 0.0f));
    model.scale(0.8f);
    shaderProgram.setUniformValue("model", model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(unsigned int) * indexes.size(), GL_UNSIGNED_INT, 0);

    shaderProgram.release();
}

void CoreFunctionWidget::getSphere()
{
    double ustep = 1 / (double)ustepNum, vstep = 1 / (double)vstepNum;
    points.push_back(sin(0)*cos(0));
    points.push_back(cos(0));
    points.push_back(sin(0)*sin(0));
    points.push_back(0.5);
    points.push_back(1);
    for (int i = 1; i < vstepNum; i++)
    {
        for (int j = 0; j <= ustepNum; j++)
        {
            points.push_back(sin(PI * vstep * i)*cos(2 * PI * ustep * j));
            points.push_back(cos(PI * vstep * i));
            points.push_back(sin(PI * vstep * i)*sin(2 * PI * ustep * j));
            points.push_back(vstep * j);
            points.push_back(1 - ustep * i);

        }
    }
    points.push_back(sin(PI * 1)*cos(2 * PI * 1));
    points.push_back(cos(PI * 1));
    points.push_back(sin(PI * 1)*sin(2 * PI * 1));
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




















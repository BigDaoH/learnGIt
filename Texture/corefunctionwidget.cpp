#include "corefunctionwidget.h"
#include <QDebug>
#include <QTimer>

static GLuint VBO, VAO, EBO, texture1, texture2;
CoreFunctionWidget::CoreFunctionWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

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

    float vertices[] =
    {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] =    // note that we start from 0!
    {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    QImage img1 = QImage(":/container.jpg").convertToFormat(QImage::Format_RGB888);
    if(!img1.isNull())
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img1.width(), img1.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img1.bits());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    QImage img2 = QImage(":/awesomeface.png").convertToFormat(QImage::Format_RGB888).mirrored(true, true);
    if(!img2.isNull())
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img2.width(), img2.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img2.bits());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    shaderProgram.bind();
    glUniform1i(shaderProgram.uniformLocation("texture1"), 0);
    glUniform1i(shaderProgram.uniformLocation("texture2"), 1);
    shaderProgram.release();
}

void CoreFunctionWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void CoreFunctionWidget::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderProgram.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    shaderProgram.release();
}

void CoreFunctionWidget::getSphere()
{
    double ustep = 1 / (double)ustepNum, vstep = 1 / (double)vstepNum;
    getPoint(0, 0);
    points.push_back(0.5);
    points.push_back(1);
    for (int i = 1; i < vstepNum; i++)
    {
        for (int j = 0; j <= ustepNum; j++)
        {
            points.push_back(sin(PI * vstep * i)*cos(2 * PI * ustep * j));
            points.push_back(sin(PI * vstep * i)*sin(2 * PI * ustep * j));
            points.push_back(cos(PI * vstep * i));
            points.push_back(1- ustep*j);
            points.push_back(vstep*i);
        }
    }
    getPoint(1, 1);
    points.push_back(0.5);
    points.push_back(0);

    //生成三角面片定点索引信息

    for (int i = 0; i <= ustepNum ; i++)
    {
        indexes.push_back(0);
        indexes.push_back(1 + i);
        indexes.push_back(1 + i + 1);
    };

    int last = points.size() / 3 - 1;
    qDebug() << last;
    for (int i = last - ustepNum; i <= last; i++)
    {
        indexes.push_back(i);
        indexes.push_back(last);
        indexes.push_back(i + 1);
    }

    for (int i = 1; i < vstepNum - 1; i++)
    {
        int start = 1 + (i - 1) * (ustepNum + 1);
        for (int j = start; j < start + (1 + ustepNum); j++)
        {

            indexes.push_back(j);
            indexes.push_back(j + ustepNum + 1);
            indexes.push_back(j + ustepNum + 2);

            indexes.push_back(j);
            indexes.push_back(j + 1);
            indexes.push_back(j + ustepNum + 2);
        }
    }
}




















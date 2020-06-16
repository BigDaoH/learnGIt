#ifndef COREFUNCTIONWIDGET_H
#define COREFUNCTIONWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>


class CoreFunctionWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit CoreFunctionWidget(QWidget *parent = nullptr);
    ~CoreFunctionWidget();
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
private:
    QOpenGLShaderProgram shaderProgram;

    QTimer* m_pTimer;
    float m_ftime;

    QVector<float> points;
    QVector<uint> indexes;
    void getSphere();
};

#endif // COREFUNCTIONWIDGET_H

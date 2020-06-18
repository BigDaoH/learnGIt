#include "camera.h"
#include <math.h>

Camera::Camera(QVector3D position, QVector3D up, float yaw, float pitch) :
    position(position),
    worldUp(up),
    front(-position),
    picth(pitch),
    yaw(yaw),
    movementSpeed(SPEED),
    mouseSensitivity(SENSITIVITY),
    zoom(ZOOM)
{
    this->updateCameraVectors();

    for(uint i = 0; i != 1024; ++i)
    {
        keys[i] = false;
    }
}

Camera::~Camera()
{

}

QMatrix4x4 Camera::getViewMatrix()
{
    QMatrix4x4 view;
    view.lookAt(position,position+front,up);
    return view;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constraintPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw -= xoffset;
    picth -= yoffset;

    if (constraintPitch)
    {
        if (this->picth > 1.57f)
        {
            this->picth = 1.57f;
        }
        if (this->picth < -1.57f)
        {
            this->picth = -1.57f;
        }
    }
    if(!keys[Qt::Key_Z])
    {
        updateCameraVectors();
    }
    else
    {
        updateRoundCameraVector();
    }
}

void Camera::processMouseScroll(float yoffset)
{
    if (this->zoom >= 1.0f && this->zoom <= 45.0f)
    {
        this->zoom -= yoffset;
    }
    if (this->zoom > 45.0f)
    {
        this->zoom = 45.0f;
    }
    if (this->zoom < 1.0f)
    {
        this->zoom = 1.0f;
    }
}

void Camera::processInput(float dt)
{
    if (keys[Qt::Key_W])
    {
        processKeyboard(FORWARD, dt);
    }
    if (keys[Qt::Key_S])
    {
        processKeyboard(BACKWARD, dt);
    }
    if (keys[Qt::Key_A])
    {
        processKeyboard(LEFT, dt);
    }
    if (keys[Qt::Key_D])
    {
        processKeyboard(RIGHT, dt);
    }
    if (keys[Qt::Key_E])
    {
        processKeyboard(UP, dt);
    }
    if (keys[Qt::Key_Q])
    {
        processKeyboard(DOWN, dt);
    }
}

void Camera::updateCameraVectors()
{
    QVector3D front;
    front.setX(cos(this->yaw) * cos(this->picth));
    front.setY(sin(this->picth));
    front.setZ(sin(this->yaw) * cos(this->picth));
    this->front = front.normalized();
    this->right = QVector3D::crossProduct(this->front, this->worldUp).normalized();
    this->up = QVector3D::crossProduct(this->right, this->front).normalized();
}

void Camera::updateRoundCameraVector()
{
    float radius = (QVector3D(0.0,0.0,0.0)-position).length();
    position=QVector3D(cos(yaw)*cos(picth)*radius,sin(picth)*radius,cos(picth)*sin(yaw)*radius);
    this->front=QVector3D(0.0f,0.0f,0.0f)-position;
    this->right = QVector3D::crossProduct(front, this->worldUp).normalized();
    this->up = QVector3D::crossProduct(this->right, front).normalized();
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = this->movementSpeed * deltaTime;
    switch (direction)
    {
        case FORWARD:
            position += front*velocity;
            break;
        case BACKWARD:
            position -= front*velocity;
            break;
        case LEFT:
            position -= right*velocity;
            break;
        case RIGHT:
            position += right*velocity;
            break;
        case UP:
            position += worldUp*velocity;
            break;
        case DOWN:
            position -= worldUp*velocity;
            break;
        default:
            break;
    }
}



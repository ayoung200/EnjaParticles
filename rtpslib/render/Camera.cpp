#include "Camera.h"

namespace rtps
{
void Camera::move(float delX, float delY, float delZ)
{
    if(delX!=0.0f)
        pos += rotation * float3(delX*moveSpeed, 0.0f, 0.0f);
    if(delY!=0.0f)
        pos += rotation * float3(0.0f,delY*moveSpeed, 0.0f);
    if(delZ!=0.0f)
        pos += rotation * float3(0.0f, 0.0f, -delZ*moveSpeed);
    updateViewMatrix();
}

void Camera::moveX(float delX)
{
    pos += rotation * float3(delX*moveSpeed, 0.0f, 0.0f);
    updateViewMatrix();
}

void Camera::moveY(float delY)
{
    pos += rotation * float3(0.0f,delY*moveSpeed, 0.0f);
    updateViewMatrix();
}

void Camera::moveZ(float delZ)
{
    pos += rotation * float3(0.0f, 0.0f, -delZ*moveSpeed);
    updateViewMatrix();
}

void Camera::rotate(float rotateDelX, float rotateDelY)
{
    if(rotateDelX!=0.0f)
    {
        Quaternion xrot(float3(1.0f, 0.0f, 0.0f), rotateDelX * rotateSpeed * PIOVER180);
        //std::cout<<"rotation x radians = "<<rotateDelX * rotateSpeed * PIOVER180<<std::endl;
        rotation = rotation * xrot;
    }
    if(rotateDelY!=0.0f)
    {
        //Quaternion yrot(float3(0.0f, 0.0f, 1.0f), rotateDelZ * rotateSpeed * PIOVER180);
        Quaternion yrot(float3(0.0f, 1.0f, 0.0f), rotateDelY * rotateSpeed * PIOVER180);
        //std::cout<<"rotation z radians = "<<rotateDelZ * rotateSpeed * PIOVER180<<std::endl;
        rotation = yrot * rotation;
    }
    updateViewMatrix();
}

void Camera::setProjectionMatrixPerspective(double l, double r, double b, double t, double n, double f)
{
    projectionMatrix.loadIdentity();
    projectionMatrix[0]  = (2. * n) / (r - l);
    projectionMatrix[2]  = (r + l) / (r - l);
    projectionMatrix[5]  = (2. * n) / (t - b);
    projectionMatrix[6]  = (t + b) / (t - b);
    projectionMatrix[10] = -(f + n) / (f - n);
    projectionMatrix[11] = -(2. * f * n) / (f - n);
    projectionMatrix[14] = -1.;
    projectionMatrix[15] = 0;
    projectionMatrix.transpose();
}

void Camera::setProjectionMatrixOrthographic(double l, double r, double b, double t, double n,
                              double f)
{
    projectionMatrix.loadIdentity();
    projectionMatrix[0]  = 2. / (r - l);
    projectionMatrix[3]  = -(r + l) / (r - l);
    projectionMatrix[5]  = 2. / (t - b);
    projectionMatrix[7]  = -(t + b) / (t - b);
    projectionMatrix[10] = -2. / (f - n);
    projectionMatrix[11] = -(f + n) / (f - n);
    projectionMatrix.transpose();
}

const float16& Camera::getProjectionMatrix()
{
    return projectionMatrix;
}
const float16& Camera::getInverseProjectionMatrix()
{
    return invViewMatrix;
}

void Camera::updateProjectionMatrix()
{
    if(currentProjection==PERSPECTIVE_PROJECTION)
    {
        double tangent = tanf(fov *0.5 * PIOVER180); // tangent of half fovY
        double h = nearClip * tangent;         // half height of near plane
        double w = h * aspectRatio;          // half width of near plane

        setProjectionMatrixPerspective(-w, w, -h, h, nearClip, farClip);
    }
    else
    {
        setProjectionMatrixOrthographic(-width,width,-height,height,nearClip,farClip);
    }
    invProjectionMatrix = projectionMatrix;
    invProjectionMatrix.inverse();
}

const float16& Camera::getViewMatrix()
{
    return viewMatrix;
}

const float16& Camera::getInverseViewMatrix()
{
    return invViewMatrix;
}

void Camera::updateViewMatrix()
{
    rotation.normalize();
    viewMatrix = rotation.getMatrix();
    //FIXME: might not have to transpose.
    //viewMatrix.transpose();
    viewMatrix[12]=-pos.x;
    viewMatrix[13]=-pos.y;
    viewMatrix[14]=-pos.z;
    //viewMatrix[3]=pos.x;
    //viewMatrix[7]=pos.y;
    //viewMatrix[11]=pos.z;
    invViewMatrix=viewMatrix;
    invViewMatrix.inverse();
}
}

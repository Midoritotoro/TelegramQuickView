#pragma once

#include <QDebug>
#include <QtMath>
#include <QVector2D>
#include <QElapsedTimer>


#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>


#include "vertex.h"


class GLBlurFunctions : protected QOpenGLFunctions_3_3_Core
{
public:
    GLBlurFunctions();
    ~GLBlurFunctions();

    QImage blurImage_DualKawase(QImage imageToBlur, int offset, int iterations);
    float getGPUTime();
    float getCPUTime();

private:
    void renderToFBO(QOpenGLFramebufferObject* targetFBO, GLuint sourceTexture, QOpenGLShaderProgram* shader);
    void initFBOTextures();

    QOpenGLShaderProgram* m_ShaderProgram_kawase_up;
    QOpenGLShaderProgram* m_ShaderProgram_kawase_down;

    QVector<QOpenGLFramebufferObject*> m_FBO_vector;
    QOpenGLTexture* m_textureToBlur;

    QOpenGLVertexArrayObject m_VertexArrayObject;
    QOpenGLBuffer m_VertexBuffer;

    QOffscreenSurface* m_Surface;
    QOpenGLContext* m_Context;

    int m_iterations;
    QImage m_imageToBlur;

    //GPU timer
    GLuint GPUTimerQueries[1];
    GLint GPUTimerAvailable;
    GLuint64 GPUtimerElapsedTime;

    //CPU timer
    QElapsedTimer CPUTimer;
    quint64 CPUTimerElapsedTime;
};

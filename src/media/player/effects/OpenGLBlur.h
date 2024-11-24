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


class OpenGLBlur : protected QOpenGLFunctions_3_3_Core
{
private:
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

    GLuint GPUTimerQueries[1];
    GLint GPUTimerAvailable;
    GLuint64 GPUtimerElapsedTime;

    QElapsedTimer CPUTimer;
    quint64 CPUTimerElapsedTime;
public:
    OpenGLBlur();
    ~OpenGLBlur();

    [[nodiscard]] QImage blurImage(QImage imageToBlur, int offset, int iterations);

    [[nodiscard]] float getGPUTime() const;
    [[nodiscard]] float getCPUTime() const;
private:
    void renderToFBO(QOpenGLFramebufferObject* targetFBO, GLuint sourceTexture, QOpenGLShaderProgram* shader);
    void initFBOTextures();
};

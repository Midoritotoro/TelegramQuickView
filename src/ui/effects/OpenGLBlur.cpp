#include "OpenGLBlur.h"

#include <QCoreApplication>
#include <QDir>

static const Vertex sg_vertexes[] =
{
  Vertex(QVector3D(1.0f,  1.0f, 1.0f)),
  Vertex(QVector3D(-1.0f,  1.0f, 1.0f)),
  Vertex(QVector3D(-1.0f, -1.0f, 1.0f)),
  Vertex(QVector3D(1.0f, -1.0f, 1.0f))
};

OpenGLBlur::OpenGLBlur()
{
    m_Context = new QOpenGLContext();
    m_Context->setFormat(QSurfaceFormat::defaultFormat());
    m_Context->create();

    m_Surface = new QOffscreenSurface();
    m_Surface->create();

    QSurfaceFormat surfaceFormat;

    surfaceFormat.setVersion(3, 3);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    m_Surface->setFormat(surfaceFormat);

    m_Context->makeCurrent(m_Surface);

    initializeOpenGLFunctions();

    QString currentPath = QCoreApplication::applicationDirPath();
    QDir shadersDir(currentPath + "../../../src/parser/shaders");

    m_ShaderProgram_kawase_up = new QOpenGLShaderProgram();

    m_ShaderProgram_kawase_up->addShaderFromSourceFile(QOpenGLShader::Vertex, shadersDir.absolutePath() + "/simple.vert");
    m_ShaderProgram_kawase_up->addShaderFromSourceFile(QOpenGLShader::Fragment, shadersDir.absolutePath() + "/dual_kawase_up.frag");
    m_ShaderProgram_kawase_up->link();

    m_ShaderProgram_kawase_down = new QOpenGLShaderProgram();

    m_ShaderProgram_kawase_down->addShaderFromSourceFile(QOpenGLShader::Vertex, shadersDir.absolutePath() + "/simple.vert");
    m_ShaderProgram_kawase_down->addShaderFromSourceFile(QOpenGLShader::Fragment, shadersDir.absolutePath() + "/dual_kawase_down.frag");
    m_ShaderProgram_kawase_down->link();

    m_VertexBuffer.create();
    m_VertexBuffer.bind();
    m_VertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_VertexBuffer.allocate(sg_vertexes, sizeof(sg_vertexes));

    m_VertexArrayObject.create();
    m_VertexArrayObject.bind();

    m_ShaderProgram_kawase_up->enableAttributeArray(0);
    m_ShaderProgram_kawase_up->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());

    m_ShaderProgram_kawase_down->enableAttributeArray(0);
    m_ShaderProgram_kawase_down->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());

    m_textureToBlur = new QOpenGLTexture(QImage());
    m_textureToBlur->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textureToBlur->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

    m_iterations = -1;
    m_imageToBlur = QImage();
}

OpenGLBlur::~OpenGLBlur()
{
    delete m_ShaderProgram_kawase_up;
    delete m_ShaderProgram_kawase_down;

    for (int i = 0; i < m_FBO_vector.size(); i++)
        delete m_FBO_vector[i];

    delete m_textureToBlur;
}

QImage OpenGLBlur::blurImage_DualKawase(QImage imageToBlur, int offset, int iterations)
{
    if (iterations != m_iterations || imageToBlur != m_imageToBlur) {
        m_iterations = iterations;
        m_imageToBlur = imageToBlur;

        initFBOTextures();
    }

    glGenQueries(1, GPUTimerQueries);
    glBeginQuery(GL_TIME_ELAPSED, GPUTimerQueries[0]);

    CPUTimer.start();

    m_ShaderProgram_kawase_down->setUniformValue("offset", QVector2D(offset, offset));
    m_ShaderProgram_kawase_up->setUniformValue("offset", QVector2D(offset, offset));

    renderToFBO(m_FBO_vector[1], m_textureToBlur->textureId(), m_ShaderProgram_kawase_down);

    for (int i = 1; i < iterations; i++)
        renderToFBO(m_FBO_vector[i + 1], m_FBO_vector[i]->texture(), m_ShaderProgram_kawase_down);

    for (int i = iterations; i > 0; i--)
        renderToFBO(m_FBO_vector[i - 1], m_FBO_vector[i]->texture(), m_ShaderProgram_kawase_up);
    
    CPUTimerElapsedTime = CPUTimer.nsecsElapsed();

    glEndQuery(GL_TIME_ELAPSED);
    GPUTimerAvailable = 0;

    while (!GPUTimerAvailable)
        glGetQueryObjectiv(GPUTimerQueries[0], GL_QUERY_RESULT_AVAILABLE, &GPUTimerAvailable);
    
    glGetQueryObjectui64v(GPUTimerQueries[0], GL_QUERY_RESULT, &GPUtimerElapsedTime);
    glDeleteQueries(1, GPUTimerQueries);

    return m_FBO_vector[0]->toImage();
}

void OpenGLBlur::renderToFBO(QOpenGLFramebufferObject* targetFBO, GLuint sourceTexture, QOpenGLShaderProgram* shader)
{
    targetFBO->bind();

    glBindTexture(GL_TEXTURE_2D, sourceTexture);
    shader->bind();

    shader->setUniformValue("iResolution", QVector2D(targetFBO->size().width(), targetFBO->size().height()));
    shader->setUniformValue("halfpixel", QVector2D(0.5 / targetFBO->size().width(), 0.5 / targetFBO->size().height()));

    glViewport(0, 0, targetFBO->size().width(), targetFBO->size().height());
    glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
}

void OpenGLBlur::initFBOTextures()
{
    for (int i = 0; i < m_FBO_vector.size(); i++) {
        delete m_FBO_vector[i];
    }

    m_FBO_vector.clear();
    m_FBO_vector.append(new QOpenGLFramebufferObject(m_imageToBlur.size(), QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D));

    for (int i = 1; i <= m_iterations; i++) {
        m_FBO_vector.append(new QOpenGLFramebufferObject(m_imageToBlur.size() / qPow(2, i), QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D));

        glBindTexture(GL_TEXTURE_2D, m_FBO_vector.last()->texture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    delete m_textureToBlur;

    m_textureToBlur = new QOpenGLTexture(m_imageToBlur.mirrored(), QOpenGLTexture::DontGenerateMipMaps);

    m_textureToBlur->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textureToBlur->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
}

float OpenGLBlur::getGPUTime() const
{
    float gpuTime = GPUtimerElapsedTime / 1000000.;
    return roundf(gpuTime * 1000) / 1000;
}

float OpenGLBlur::getCPUTime() const
{
    float cpuTime = CPUTimerElapsedTime / 1000000.;
    return roundf(cpuTime * 1000) / 1000;
}
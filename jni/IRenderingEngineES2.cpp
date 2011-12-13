
#ifdef __APPLE__
#include "TargetConditionals.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif


#ifdef __ANDROID__
#ifndef ANDROID_NDK
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
#define  LOG_TAG    "NDKGL"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif /* !ANDROID_NDK */
#endif /* __ANDROID__ */

#include <stdio.h>
#include <cmath>
#include "IRenderingEngine.hpp"

static const float RevolutionsPerSecond = 1;

class RenderingEngine2 : public IRenderingEngine {
public:
    RenderingEngine2();
    void Initialize(int width, int height);
    void Render() const;
    void UpdateAnimation(float timeStep);
    void OnRotate(DeviceOrientation newOrientation);
private:
    float RotationDirection() const;
    GLuint BuildShader(const char* source, GLenum shaderType) const;
    GLuint BuildProgram(const char* vShader, const char* fShader) const;
    void ApplyOrtho(float maxX, float maxY) const;
    void ApplyRotation(float degrees) const;
    float m_desiredAngle;
    float m_currentAngle;
    GLuint m_simpleProgram;
    GLuint m_framebuffer;
    GLuint m_renderbuffer;
};


static const char SimpleVertexShader[] = 
"attribute vec4 Position;\n"
"attribute vec4 SourceColor;\n"
"varying vec4 DestinationColor;\n"
"uniform mat4 Projection;\n"
"uniform mat4 Modelview;\n"
"void main() {\n"
"  DestinationColor = SourceColor;\n"
"  gl_Position = Projection * Modelview * Position;\n"
"}\n";


static const char SimpleFragmentShader[] = 
"varying lowp vec4 DestinationColor;"
"void main() {\n"
"     gl_FragColor = DestinationColor;\n"
"}\n";


IRenderingEngine* CreateRenderer2()
{
    return new RenderingEngine2();
}

struct Vertex {
    float Position[2];
    float Color[4];
};

// Define the positions and colors of two triangles.
const Vertex Vertices[] = {
    {{-0.5, -0.866}, {1, 1, 0.5f, 1}},
    {{0.5, -0.866},  {1, 1, 0.5f, 1}},
    {{0, 1},         {1, 1, 0.5f, 1}},
    {{-0.5, -0.866}, {0.5f, 0.5f, 0.5f}},
    {{0.5, -0.866},  {0.5f, 0.5f, 0.5f}},
    {{0, -0.4f},     {0.5f, 0.5f, 0.5f}},
};

RenderingEngine2::RenderingEngine2()
{
    // Create & bind the color buffer so that the caller can allocate its space.
#ifdef __APPLE__
	glGenRenderbuffers(1, &m_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
#endif	
}

void RenderingEngine2::Initialize(int width, int height)
{
    // Create the framebuffer object and attach the color buffer.
#ifdef __APPLE__
	glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER,
                              m_renderbuffer);
#endif	
	
#ifndef ANDROID_NDK
	m_framebuffer = (GLuint)0;
#endif
    
    glViewport(0, 0, width, height);
	
	//    m_simpleProgram = BuildProgram(gVertexShader, gFragmentShader);
    m_simpleProgram = BuildProgram(SimpleVertexShader, SimpleFragmentShader);
	
    glUseProgram(m_simpleProgram);
    
    // Initialize the projection matrix.
    ApplyOrtho(2, 3);
    
    // Initialize rotation animation state.
    OnRotate(DeviceOrientationPortrait);
    m_currentAngle = m_desiredAngle;
}

void RenderingEngine2::ApplyOrtho(float maxX, float maxY) const
{
    float a = 1.0f / maxX;
    float b = 1.0f / maxY;
    float ortho[16] = {
        a, 0,  0, 0,
        0, b,  0, 0,
        0, 0, -1, 0,
        0, 0,  0, 1
    };
    
    GLint projectionUniform = glGetUniformLocation(m_simpleProgram, "Projection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &ortho[0]);
}

void RenderingEngine2::ApplyRotation(float degrees) const
{
    float radians = degrees * 3.14159f / 180.0f;
    float s = std::sin(radians);
    float c = std::cos(radians);
    float zRotation[16] = {
        c, s, 0, 0,
		-s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    
    GLint modelviewUniform = glGetUniformLocation(m_simpleProgram, "Modelview");
    glUniformMatrix4fv(modelviewUniform, 1, 0, &zRotation[0]);
}

void RenderingEngine2::Render() const
{
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ApplyRotation(m_currentAngle);
    
    GLuint positionSlot = glGetAttribLocation(m_simpleProgram, "Position");
    GLuint colorSlot = glGetAttribLocation(m_simpleProgram, "SourceColor");
    
    glEnableVertexAttribArray(positionSlot);
    glEnableVertexAttribArray(colorSlot);
    
    GLsizei stride = sizeof(Vertex);
    const GLvoid* pCoords = &Vertices[0].Position[0];
    const GLvoid* pColors = &Vertices[0].Color[0];
    
    glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, stride, pCoords);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, stride, pColors);
    
    GLsizei vertexCount = sizeof(Vertices) / sizeof(Vertex);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    
    glDisableVertexAttribArray(positionSlot);
    glDisableVertexAttribArray(colorSlot);
}

float RenderingEngine2::RotationDirection() const
{
    float delta = m_desiredAngle - m_currentAngle;
    if (delta == 0)
        return 0;
    
    bool counterclockwise = ((delta > 0 && delta <= 180) || (delta < -180));
    return counterclockwise ? +1 : -1;
}

void RenderingEngine2::UpdateAnimation(float timeStep)
{
    float direction = RotationDirection();
    if (direction == 0)
        return;
    
    float degrees = timeStep * 360 * RevolutionsPerSecond;
    m_currentAngle += degrees * direction;
	
    // Normalize the angle to [0, 360)
    if (m_currentAngle >= 360)
        m_currentAngle -= 360;
    else if (m_currentAngle < 0)
        m_currentAngle += 360;
    
    // If the rotation direction changed, then we overshot the desired angle.
    if (RotationDirection() != direction)
        m_currentAngle = m_desiredAngle;
}

void RenderingEngine2::OnRotate(DeviceOrientation orientation)
{
    float angle = 0;
    
    switch (orientation) {
        case DeviceOrientationLandscapeLeft:
            angle = 270;
            break;
			
        case DeviceOrientationPortraitUpsideDown:
            angle = 180;
            break;
			
        case DeviceOrientationLandscapeRight:
            angle = 90;
            break;
    }
    
    m_desiredAngle = angle;
	m_currentAngle = angle;
	
#ifdef __ANDROID__
	LOGI("Rotating source: %f   to %f  - %f  - %f", orientation, angle, m_currentAngle, m_desiredAngle);
#endif
}

GLuint RenderingEngine2::BuildShader(const char* source, GLenum shaderType) const
{
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &source, 0);
    glCompileShader(shaderHandle);
    
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    
    if (compileSuccess == GL_FALSE) {
        char messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
#ifdef __APPLE__
		printf("ERROR: %s",messages);   
#endif
#ifdef ANDROID_NDK
		LOGI("ERROR: %s",messages);   
#endif  
    }
	
    return shaderHandle;
}

GLuint RenderingEngine2::BuildProgram(const char* vertexShaderSource,
                                      const char* fragmentShaderSource) const
{
    GLuint vertexShader = BuildShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = BuildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);
    glLinkProgram(programHandle);
    
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
	
    if (linkSuccess == GL_FALSE) {
        char messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
#ifdef __APPLE__
		printf("ERROR: %s",messages);   
#endif
#ifdef ANDROID_NDK
		LOGI("ERROR: %s",messages);   
#endif
    }
   	
    return programHandle;
}

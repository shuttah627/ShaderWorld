#pragma once

// Acknowledgment: models taken and adapted from: 
//      https://3dexport.com/free-3dmodel-chess-pieces-54804.htm
//		https://www.cgtrader.com/


#define SIT354_OPENGL
#define SIT354_WINDOWS
//#define SIT354_LINUX

// THese are the flags to enable each of the stages.
// Uncomment each stage in sequence, as described in the
// workshop instructions.  

#define STAGE01
#define STAGE02
#define STAGE03
#define STAGE04
#define STAGE05

#ifdef SIT354_OPENGL

#ifdef SIT354_WINDOWS
#include "resource.h"

#include <GL/glew.h>
#endif

#include <stdlib.h>
#include <time.h>

#ifdef SIT354_LINUX
#include <QtGui/QOpenGLFunctions_4_1_Core>
#include <QtOpenGL/QtOpenGL>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
using glm::mat4;
using glm::mat3;
using glm::vec2;
using glm::vec3;
using glm::vec4;
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#endif

#ifdef SIT354_LINUX
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtOpenGL/QGLWidget>
#endif

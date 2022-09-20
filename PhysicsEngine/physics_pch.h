// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PHYSICS_PCH_H
#define PHYSICS_PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include <vector>
#include <memory>
#include <tuple>

#include <GL/glew.h>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#endif //PHYSICS_PCH_H

#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <fstream>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include <vector>
#include <map>

#include <functional>
#include <memory>

#include <string>
#include <cstring>

#ifdef WIN32
    #include <Random>
#else
    #include <random>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

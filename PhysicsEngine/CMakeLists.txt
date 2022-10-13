cmake_minimum_required(VERSION 3.10)

# set the project name
project(PhysicsEngine VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_library(PhysicsEngine SHARED PhysicsEngineMain.cpp physics_pch.cpp ../deps/Common/include/bullet/btBulletCollisionAll.cpp ../deps/Common/include/bullet/btBulletDynamicsAll.cpp ../deps/Common/include/bullet/btLinearMathAll.cpp)

target_compile_options(PhysicsEngine PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(PhysicsEngine PRIVATE -O2 -fPIC)

target_include_directories(PhysicsEngine PUBLIC
                           "${PROJECT_SOURCE_DIR}/../deps/Common/include/bullet"
                           "${PROJECT_SOURCE_DIR}/../deps/linux/include"
                           )

target_link_directories(PhysicsEngine PUBLIC
                        "${PROJECT_SOURCE_DIR}/../deps/linux/lib")

target_link_libraries(PhysicsEngine PUBLIC GL Bullet2FileLoader Bullet3Collision Bullet3Common Bullet3Dynamics Bullet3Geometry Bullet3OpenCL_clew BulletCollision BulletDynamics BulletInverseDynamics BulletSoftBody LinearMath)
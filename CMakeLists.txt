cmake_minimum_required(VERSION 3.17)
project(2_2_2SomaForLacoReducao)

add_executable(2_2_2SomaForLacoReducao SomaForLacoReducao.cpp)

#Acesso ao GSL-GNU
#target_link_libraries(2_2_2SomaForLacoReducao -lgsl -lgslcblas -lm)

#Acesso ao OpenMP
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")
set(SOURCE_FILES SomaForLacoReducao.cpp)
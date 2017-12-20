cmake_minimum_required(VERSION 3.8)

project(I2C)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        src/LineSegmentation/Components/Line.cpp
        src/LineSegmentation/Components/Line.h
        src/LineSegmentation/Components/Valley.cpp
        src/LineSegmentation/Components/Valley.h
        src/LineSegmentation/Components/Peak.cpp
        src/LineSegmentation/Components/Peak.h
        src/LineSegmentation/Components/Region.cpp
        src/LineSegmentation/Components/Region.h
        src/LineSegmentation/LineSegmentation.cpp
        src/LineSegmentation/LineSegmentation.h
        src/LineSegmentation/Components/Chunk.cpp
        src/LineSegmentation/Components/Chunk.h
        src/WordSegmentation/WordSegmentation.cpp
        src/WordSegmentation/WordSegmentation.h
        src/CharacterSegmentation/CharacterSegmentation.cpp
        src/CharacterSegmentation/CharacterSegmentation.h
        src/Utilities/Utilities.cpp
        src/Utilities/Utilities.h
        src/Utilities/Constants.h)

add_executable(I2C ${SOURCE_FILES})

target_link_libraries(I2C ${OpenCV_LIBS})
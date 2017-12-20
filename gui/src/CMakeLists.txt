cmake_minimum_required(VERSION 3.8)
project(Image2Code)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Segmentation/CharSegmentation.cpp
        Segmentation/CharSegmentation.h
        Segmentation/LineSegmentation.cpp
        Segmentation/LineSegmentation.h
        Segmentation/Segmentation.cpp
        Segmentation/Segmentation.h
        Segmentation/WordSegmentation.cpp
        Segmentation/WordSegmentation.h
        Preprocessing/Preprocessor.h
        Preprocessing/AdaptiveWienerFilter/WienerFilter.h
        Utilities/Constants.h
        Utilities/Utilities.cpp
        Utilities/Utilities.h
        Main.cpp)

add_executable(Image2Code ${SOURCE_FILES})

# Compile imgtxtenh
add_custom_target(
        imgtxtenh ALL
)

MESSAGE ("Building dependency: imgtxtenh")

add_custom_command (TARGET imgtxtenh
        COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/Preprocessing/imgtxtenh/ && make
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Preprocessing/imgtxtenh/)

target_link_libraries(Image2Code ${OpenCV_LIBS})
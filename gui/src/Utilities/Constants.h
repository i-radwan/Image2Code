#pragma once

//
// PATHS
//
#define INPUT_IMG               "../Data/Input/sample_0.jpg"

#define OUTPUT_PATH             "preprocessing/tmp/"
#define PREPROCESS_OUTPUT_PATH  "preprocessing/tmp/Preprocess/"
#define LINE_OUTPUT_PATH        "../Data/Output/1.Lines/"
#define WORD_OUTPUT_PATH        "../Data/Output/2.Words/"
#define CHARACTER_OUTPUT_PATH   "preprocessing/tmp/Characters/"

#define THRESH_IMG              "../Data/Output/0.Preprocess/0.thresholded.jpg"
#define LINES_IMG               "preprocessing/tmp/Preprocess/1.lines.jpg"
#define BOUNDING_BOX_IMG        "tmp/Preprocess/1.bounding_box.jpg"
#define ROTATED_IMG             "tmp/Preprocess/2.rotated.jpg"
#define RED_THRESH_IMG          "tmp/Preprocess/3.red.jpg"
#define SEGMENTED_IMG           "preprocessing/tmp/Preprocess/segmented.jpg"

//
// GLOBAL CONSTANTS
//
#define BACKCOLOR               0
#define FORECOLOR               255
#define WORD_RECT_COLOR         Vec3b(255, 0, 255) // Magenta color.
#define CHAR_RECT_COLOR			Vec3b(0, 255, 0)
#define SPECIAL_CHAR_RECT_COLOR	Vec3b(232, 162, 0)

#define IMG_MAX_WIDTH           1000
#define IMG_MAX_HEIGHT          1500
//
// Created by Ibrahim Radwan on 11/29/17.
//

#ifndef IMAGE2LINES2_CONSTANTS_H
#define IMAGE2LINES2_CONSTANTS_H

//
// PATHS
//
#define INPUT_IMG "../data/1.LineInput/input.jpg"
#define LINE_OUTPUT_PATH "../data/2.LineOutput_WordInput/"
#define WORD_OUTPUT_PATH "../data/3.WordOutput_CharacterInput/"
#define CHARACTER_OUTPUT_PATH "../data/4.CharacterOutput_ClassifierInput/"
#define CLASSIFIER_OUTPUT_PATH "../data/5.ClassifierOutput_PPInput/"
#define CODE_OUTPUT_PATH "../data/6.OutputCode/"

//
// LINE SEGMENTATION
//
#define CHUNKS_NUMBER 20
#define TEST_LINE_COLOR Vec3b(255, 0, 255) // Magenta color.

//
// WORD SEGMENTATION
//
#define WORD_SPACING_THRESHOLD 8

//
// CHAR SEGMENTATION
//
#define CHAR_COLOR_THRESHOLD 200

#endif //IMAGE2LINES2_CONSTANTS_H

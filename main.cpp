#include <iostream>
#include "src/LineSegmentation/LineSegmentation.h"
#include "src/WordSegmentation/WordSegmentation.h"
#include "src/CharacterSegmentation/CharacterSegmentation.h"

void
clear_output_folders();

int
main() {
    // ToDo: Read action from main args
    int action = 0; // 0 -> Segment, 1 -> Post process

    if (action == 0) {
        // Clear folders first
        clear_output_folders();

        // Call line segmentation
        LineSegmentation lineSegmentation(INPUT_IMG);
        lineSegmentation.segment();

        // Call word segmentation
        WordSegmentation wordSegmentation;
        wordSegmentation.segment();

        // Call character segmentation
        CharacterSegmentation::segment();
    } else {

    }
    return 0;
}

void
clear_output_folders() {
    system(("exec rm -r " + string(LINE_OUTPUT_PATH) + "*").c_str());
    system(("exec rm -r " + string(WORD_OUTPUT_PATH) + "*").c_str());
    system(("exec rm -r " + string(CHARACTER_OUTPUT_PATH) + "*").c_str());
    system(("exec rm -r " + string(CLASSIFIER_OUTPUT_PATH) + "*").c_str());
    system(("exec rm -r " + string(CODE_OUTPUT_PATH) + "*").c_str());
}

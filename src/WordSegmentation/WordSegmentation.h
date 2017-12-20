//
// Created by Ibrahim Radwan on 12/1/17.
//

#ifndef I2C_WORDSEGMENTATION_H
#define I2C_WORDSEGMENTATION_H

#include "../Utilities/Utilities.h"

class WordSegmentation {
public:
    /// Preform line segmentation
    void
    segment();

private:
    /// Divide the given line into words given the space threshold between words
    void
    divide_line(string line_path, string output_path, int threshold);
};


#endif //I2C_WORDSEGMENTATION_H

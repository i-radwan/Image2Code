#pragma once

// STL libraries
#include <vector>

// OpenCV libraries
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// Custom libraries
#include "Segmentation.h"
#include "../Utilities/Constants.h"

using namespace std;
using namespace cv;


class WordSegmentation {
	friend class Segmentation;

private:
	static int rows;
	static int cols;
	static int L;								// First black column from the left
	static int R;								// First black column from the right
	static int charsWidth, charsCount;
	static int spacesWidth, spacesCount;
	static cv::Mat line;						// Line image
	static vector<int> pixelsCount;				// Black pixels count in each column
	static vector<pair<int, int>> whiteSpaces;	// Vector holding intervals of white spaces

public:
	/**
	 * Segment the given line image into words.
	 */
	static int segment(cv::Mat& img, vector<cv::Mat>& words);

private:
	/**
	 * Private constructor (i.e. static class).
	 */
	WordSegmentation() {}

	/**
	 * Calculate average characters width in the given line image.
	 */
	static int calcAvgCharWidth();

	/**
	 * Calculate space threshold between words.
	 */
	static int calcAvgSpaceWidth(int avgCharWidth);

	/**
	 * Detect the white spaces between words in the given line image.
	 */
	static void detectWhiteSpaces(int threshold);

	/**
	 * Divide the given line into words according to the white spaces.
	 */
	static void divideLine(vector<cv::Mat>& words);

	/**
	 * Initialize word segmentation variables.
	 */
	static void init(cv::Mat& img);
};
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


class LineSegmentation {
	friend class Segmentation;

private:
	static int rows;
	static int cols;
	static int L;								// First black row from the top
	static int R;								// First black row from the bottom
	static cv::Mat img;							// Text image
	static vector<int> pixelsCount;				// Black pixels count in each row
	static vector<pair<int, int>> blankLines;	// Vector holding intervals of white rows

public:
	/**
	 * Segment the given text image into lines.
	 */
	static void segment(cv::Mat& img, vector<cv::Mat>& lines);

private:
	/**
	 * Private constructor (i.e. static class).
	 */
	LineSegmentation() {}

	/**
	 * Detect the white spaces between the lines in the given text image.
	 */
	static void detectBlankLines(int threshold);

	/**
	* Divide the given line into words according to the white spaces.
	*/
	static void divideImage(vector<cv::Mat>& lines);

	/**
	 * Initialize word segmentation variables.
	 */
	static void init(cv::Mat& img);
};
#include "LineSegmentation.h"

//
// Static members declaration
//
int LineSegmentation::rows;
int LineSegmentation::cols;
int LineSegmentation::L;
int LineSegmentation::R;
cv::Mat LineSegmentation::img;
vector<int> LineSegmentation::pixelsCount;
vector<pair<int, int>> LineSegmentation::blankLines;
//========================================================================


void LineSegmentation::segment(cv::Mat& img, vector<cv::Mat>& lines) {
	init(img);
	int spaceThreshold = 5; // TODO: to be calculated dynamically
	detectBlankLines(spaceThreshold);
	divideImage(lines);
}

void LineSegmentation::detectBlankLines(int threshold) {
	int prv = 0, cnt = 0;
	blankLines.clear();

	// Add top padding
	blankLines.push_back({ 0, L - 1 });

	// Detect white spaces
	for (int i = L; i <= R; ++i) {
		// Line
		if (pixelsCount[i] > 0) {
			if (cnt > threshold) {
				blankLines.push_back({ prv, i - 1 });
			}
			cnt = 0;
		}
		// Blank line
		else if (cnt++ == 0) {
			prv = i;
		}
	}

	// Add bottom padding
	blankLines.push_back({ R + 1, rows - 1 });
}

void LineSegmentation::divideImage(vector<cv::Mat>& lines) {
	// Detect words between two successive white spaces
	for (int i = 1; i < blankLines.size(); ++i) {
		int l = blankLines[i - 1].second + 1;
		int r = blankLines[i].first - 1;

		if (r <= l) {
			continue;
		}

		Mat line = img(cv::Rect(0, l, cols, r - l + 1));
		lines.push_back(line);
	}
}

void LineSegmentation::init(cv::Mat& img) {
	rows = img.rows;
	cols = img.cols;
	LineSegmentation::img = img;
	L = 0;
	R = rows - 1;
	pixelsCount.clear();
	pixelsCount.resize(rows, 0);

	// Calculate black pixels count in each row.
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			pixelsCount[i] += (img.at<uchar>(i, j) == FORECOLOR);
		}
	}

	// Detect image top and bottom paddings
	while (L <= R && pixelsCount[L] == 0) {
		L++;
	}
	while (R >= L && pixelsCount[R] == 0) {
		R--;
	}
}
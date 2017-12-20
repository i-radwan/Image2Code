#include "Segmentation.h"

//
// Static members declaration
//
cv::Mat Segmentation::colorImg;
cv::Mat Segmentation::binaryImg;
cv::Mat Segmentation::segmentedImg;
cv::Mat Segmentation::redImg;
int Segmentation::avgCharWidth = 0;
//========================================================================


void Segmentation::segment(cv::Mat& colorImg, cv::Mat& binaryImg) {
	// Set variables
	Segmentation::colorImg = colorImg;
    Segmentation::binaryImg = binaryImg;

	// Preprocessing
	Segmentation::preprocess();
    Segmentation::skewCorrectionByHoughTransform();
	Segmentation::redThresholding();
	cv::cvtColor(binaryImg, segmentedImg, cv::COLOR_GRAY2BGR);

	// Line segmentation
	vector<cv::Mat> lines;
	LineSegmentation::segment(binaryImg, lines);

	for (int i = 0; i < lines.size(); ++i) {
		processLine(lines[i], Utilities::toString(i, 2));
	}

	// Save segmented image
	imwrite(SEGMENTED_IMG, segmentedImg);
}

void Segmentation::processLine(cv::Mat& lineImg, const string& imgName) {
	// Save line image
	// imwrite(LINE_OUTPUT_PATH + imgName + ".jpg", lineImg);

	// Word segmentation
	vector<cv::Mat> words;
	avgCharWidth = WordSegmentation::segment(lineImg, words);

	for (int i = 0; i < words.size(); ++i) {
		processWord(words[i], imgName + Utilities::toString(i, 2));
	}
}

void Segmentation::processWord(cv::Mat& wordImg, const string& imgName) {
	// Save word image
	// imwrite(WORD_OUTPUT_PATH + imgName + ".jpg", wordImg);

	// Get the corresponding word from the original image
	cv::Size size;
	cv::Point offset;
	wordImg.locateROI(size, offset);
	cv::Mat redWordImg = redImg(cv::Rect(offset.x, offset.y, wordImg.cols, wordImg.rows)).clone();

	// Character segmentation
	vector<cv::Mat> chars;
	CharSegmentation::segment(wordImg, redWordImg, chars, avgCharWidth);

	for (int i = 0; i < chars.size(); ++i) {
		int x = offset.x + CharSegmentation::regions[i].L;
		int y = offset.y + CharSegmentation::regions[i].U;
		processChar(chars[i], imgName + Utilities::toString(i, 2), x, y, CharSegmentation::regions[i].type);
	}

	// Draw rectangle around the word and its chars
	Utilities::drawRect(segmentedImg, offset.x, offset.y, wordImg.cols, wordImg.rows, WORD_RECT_COLOR);
	Utilities::drawRect(segmentedImg, offset.x - 1, offset.y - 1, wordImg.cols + 2, wordImg.rows + 2, WORD_RECT_COLOR);
}

void Segmentation::processChar(cv::Mat& charImg, const string& imgName, int offsetX, int offsetY, bool special) {
	// Save character image
	string path = CHARACTER_OUTPUT_PATH + imgName;
	if (special) {
		path += 'x';
	}
	imwrite(path + ".jpg", charImg);

	// Choose color
	cv::Vec3b color = (special ? SPECIAL_CHAR_RECT_COLOR : CHAR_RECT_COLOR);
	
	// Draw rectanlge around the characte
	Utilities::drawRect(segmentedImg, offsetX, offsetY, charImg.cols, charImg.rows, color);
	Utilities::drawRect(segmentedImg, offsetX - 1, offsetY - 1, charImg.cols + 2, charImg.rows + 2, color);
}

void Segmentation::preprocess() {
	// Convert image to grayscale
	//cv::cvtColor(colorImg, binaryImg, cv::COLOR_BGR2GRAY);

	// medianBlur(binaryImg, binaryImg, 3);

	// Apply Otsu thresholding
	cv::threshold(binaryImg, binaryImg, 180, 255, CV_THRESH_BINARY_INV);
	//cv::imwrite(THRESH_IMG, binaryImg);

    // Assuming that we get a completely preprocessed image
    // Just invert the image
    //cv::bitwise_not(binaryImg, binaryImg);
}

void Segmentation::redThresholding() {
	// Threshold the HSV image to keep only the red pixels needed for special chars detection
	cv::Mat hsvImg, lowerRedImg, upperRedImg;
	cv::cvtColor(colorImg, hsvImg, cv::COLOR_BGR2HSV);
	cv::inRange(hsvImg, cv::Scalar(0, 50, 100), cv::Scalar(10, 255, 255), lowerRedImg);
	cv::inRange(hsvImg, cv::Scalar(160, 50, 100), cv::Scalar(179, 255, 255), upperRedImg);
	cv::addWeighted(lowerRedImg, 1.0, upperRedImg, 1.0, 0.0, redImg);
	// cv::imwrite(RED_THRESH_IMG, redImg);
}
void Segmentation::skewCorrectionByBoundingBox() {
	cv::Mat img = binaryImg.clone();

	// Push all foreground points
	vector<cv::Point> points;
	for (auto it = img.begin<uchar>(); it != img.end<uchar>(); ++it) {
		if (*it == FORECOLOR) {
			points.push_back(it.pos());
		}
	}

	// Get the bounding rectangle with the minimum area around the foreground pixels
	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

	// Draw the bounding rectangle (for visualization purpose)
	cv::Point2f vertices[4];
	box.points(vertices);
	for (int i = 0; i < 4; ++i) {
		cv::line(img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0), 1, CV_AA);
	}
	cv::imwrite(BOUNDING_BOX_IMG, img);

	// Calculate the skewness angle and get the corresponding rotation matrix
	double angle = box.angle;
	if (angle < -45.0) {
		cout << "Skew angle +90" << endl;
		angle += 90.0;
	}
	cout << "Skew angle = " << angle << endl;
	cv::Mat rotationMat = cv::getRotationMatrix2D(box.center, angle, 1);

	// Rotate the images by the calculated angle
	cv::warpAffine(binaryImg, binaryImg, rotationMat, binaryImg.size(), cv::INTER_CUBIC);
	cv::warpAffine(colorImg, colorImg, rotationMat, colorImg.size(), cv::INTER_CUBIC);

	// Re-thresholding (needed after the rotation interpolation)
	cv::threshold(binaryImg, binaryImg, 70, 255, CV_THRESH_BINARY);
	cv::imwrite(ROTATED_IMG, binaryImg);
}

void Segmentation::skewCorrectionByHoughTransform() {
	// Calculate skew angle
	double angle = calcSkewAngle();
	if (fabs(angle) < 1) {
		return;
	}

	// Push all foreground points
	vector<cv::Point> points;
	for (auto it = binaryImg.begin<uchar>(); it != binaryImg.end<uchar>(); ++it) {
		if (*it == FORECOLOR) {
			points.push_back(it.pos());
		}
	}

	// Get the bounding rectangle with the minimum area around the foreground pixels
	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

	// Get rotation matrix
	cv::Mat rotationMat = cv::getRotationMatrix2D(box.center, angle, 1);

	// Rotate the images by the calculated angle
	cv::warpAffine(binaryImg, binaryImg, rotationMat, binaryImg.size(), cv::INTER_CUBIC);
	cv::warpAffine(colorImg, colorImg, rotationMat, colorImg.size(), cv::INTER_CUBIC);

	// Re-thresholding (needed after the rotation interpolation)
	cv::threshold(binaryImg, binaryImg, 70, 255, CV_THRESH_BINARY);
	cv::imwrite(ROTATED_IMG, binaryImg);
}

double Segmentation::calcSkewAngle() {
	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(binaryImg, lines, 1, CV_PI / 180, 50, binaryImg.cols / 8.0, 50);

	double angle = 0.0;

	for (int i = 0; i < lines.size(); ++i) {
		int x1 = lines[i][0];
		int y1 = lines[i][1];
		int x2 = lines[i][2];
		int y2 = lines[i][3];

		angle += atan2(y2 - y1, x2 - x1);
	}

	// Calculate average angle in degrees
	angle = (angle / lines.size()) * 180 / CV_PI;

	cout << "Skew angle = " << angle << endl;

	return angle;
}
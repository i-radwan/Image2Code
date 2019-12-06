// STL libraries
#include <iostream>
#include <vector>
#include <ctime>

// Custom libraries
#include "Utilities/Constants.h"
#include "Utilities/Utilities.h"
#include "Segmentation/Segmentation.h"
#include "Preprocessing/Preprocessor.h"

using namespace std;
using namespace cv;

// Main program driver function
int main(int argc, char* argv[]) {
	int startTime = clock();

	try {
		// Clear previous output directory
		Utilities::removeDir(OUTPUT_PATH);
		Utilities::makeDir(OUTPUT_PATH);
		Utilities::makeDir(PREPROCESS_OUTPUT_PATH);
		Utilities::makeDir(CHARACTER_OUTPUT_PATH);
        // Utilities::makeDir(PREPROCESS_OUTPUT_PATH);
        // Utilities::makeDir(LINE_OUTPUT_PATH);
        // Utilities::makeDir(WORD_OUTPUT_PATH);
        // Utilities::makeDir(CHARACTER_OUTPUT_PATH);

		// Load image
		cout << "Loading image " << argv[1] << "..." << endl;
		cv::Mat img = Utilities::loadImage(argv[1]);

		// Pre-processing
		cout << "Pre-processing image..." << endl;
		Preprocessor::pre_process(img);

		// Segmentation
		cout << "Segmenting image..." << endl;

//		imwrite("/Users/ibrahimradwan/Desktop/lines/Image2Code/Image2Code/cmake-build-debug/preprocessing/tmp/Preprocess/t.jpg", Preprocessor::image_original_transformed);
// 		Segmentation::segment(Preprocessor::image_original_transformed, Preprocessor::image_sauvola);
	}
	catch (const exception& ex) {
		cout << "ERROR::" << ex.what() << endl;
	}

	// Output process time
	int stopTime = clock();
	cout << "Time: " << (stopTime - startTime) / double(CLOCKS_PER_SEC) << "sec" << endl;
	return 0;
}
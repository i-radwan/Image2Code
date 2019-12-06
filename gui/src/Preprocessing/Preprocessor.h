#pragma once

#include <iostream>
#include <string>
#include <vector>

// WienerFilter header
#include "AdaptiveWienerFilter/WienerFilter.h"

#define IMGTXTENH_PATH "preprocessing/imgtxtenh"

#define PREPROCESSOR_OUTPUT_PATH "preprocessing/tmp/Preprocess/"
#define IMAGE_WIENER "wiener.ppm"
#define IMAGE_SAUVOLA "paper.jpg"

using namespace std;

class Preprocessor
{
public:
    static Mat image_original;
    ///< Color image
    static Mat image_original_transformed;
    ///< Color image containing the paper only without background
    static Mat image_grey; // Is(x, y)
    ///< Grey scale image
    static Mat image_wiener; // I(x, y)
    ///< Grey scale image after applying Wiener filter
    static Mat image_sauvola; // S(x, y)
    ///< Grey scale image after applying Sauvola adaptive thresholding

    static void pre_process(Mat image)
    {
        image_original = image.clone();

        cvtColor(image_original, image_grey, COLOR_BGR2GRAY);

        // Apply Canny edge and transformations to get the paper out of the surrounding background
        get_paper_text();

        // Apply Wiener filter
        apply_wiener_filter();

        // Apply Sauvola binarization
        apply_sauvola_binarization();
    }

private:

    Preprocessor()
    {}

    /// Get the paper only without background in the original image
    static void get_paper_text()
    {
        Mat image_smoothed, image_blurred, image_edges;

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        vector<Point> original_corners, transformed_corners;

        double epsilon;

        // First step
        // Apply median blur
        medianBlur(image_grey, image_blurred, 15);

        // Canny detection
        Canny(image_blurred, image_edges, 40, 80);

        // Apply dilation
        dilate(image_edges, image_edges, Mat(), Point(-1, -1));
        dilate(image_edges, image_edges, Mat(), Point(-1, -1));

        imwrite("/Users/ibrahimradwan/Desktop/edges.jpg", image_edges);
        // Second step
        // Get contours (connected components in the image)
        findContours(image_edges, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));

        // Sort the contours
        sort(contours.begin(), contours.end(), compareContourAreas);

        // Save contours as poly-polygons
        vector<Rect> boundRect(contours.size());
        vector<vector<Point> > contours_poly(contours.size());
        for (int i = 0; i < contours.size(); i++)
        {
            approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
            boundRect[i] = boundingRect(Mat(contours_poly[i]));
        }

        int max_index = int(contours.size()) - 1;

        // Get the original 4 corners
        epsilon = 0.1 * arcLength(contours[max_index], true);
        approxPolyDP(contours[max_index], original_corners, epsilon, true);

        // Return if the number of corners is not 4
        if (original_corners.size() != 4)
        {
            image_original_transformed = image_original;
            perror("The number of corners is not 4!\n");
            return;
        } else
        {
            cv::RotatedRect box = cv::minAreaRect(cv::Mat(original_corners));
            if (box.size.width < 0.5 * image_original.cols)
            {
                image_original_transformed = image_original;
                perror("Tiny image!\n");
                return;
            }
        }

        // Draw the green rectangle around the original corners and save that corner
        //vector<vector<Point> > t = vector<vector<Point> >(1, original_corners);
        //drawContours(image_original, t, -1, Scalar(255, 255, 255), 2, 8);

        // Third step
        double width_a = sqrt((pow(abs(original_corners[3].x - original_corners[2].x), 2))
                              + (pow(abs(original_corners[3].y - original_corners[2].y), 2)));
        double width_b = sqrt((pow(abs(original_corners[1].x - original_corners[0].x), 2))
                              + (pow(abs(original_corners[1].y - original_corners[0].y), 2)));
        int max_width = max(int(width_a), int(width_b));

        double height_a = sqrt((pow(abs(original_corners[0].x - original_corners[3].x), 2))
                               + (pow(abs(original_corners[0].y - original_corners[3].y), 2)));
        double height_b = sqrt((pow(abs(original_corners[1].x - original_corners[2].x), 2))
                               + (pow(abs(original_corners[1].y - original_corners[2].y), 2)));
        int max_height = max(int(height_a), int(height_b));

        if ((image_original.cols > image_original.rows) != (max_width > max_height)) {
            swap(max_width, max_height);
        }

        Mat transformed; // Anti-clock wise, Add the corners
        transformed_corners.push_back(Point(max_width - 1, 0));
        transformed_corners.push_back(Point(0, 0));
        transformed_corners.push_back(Point(0, max_height - 1));
        transformed_corners.push_back(Point(max_width - 1, max_height - 1));

        Preprocessor::alignTransformationPoints(original_corners, transformed_corners);

        Point2f original_corners_arr[4];
        copy(original_corners.begin(), original_corners.end(), original_corners_arr);
        Point2f transformed_corners_arr[4];
        copy(transformed_corners.begin(), transformed_corners.end(), transformed_corners_arr);

        // Apply transformation to get the text paper
        transformed = getPerspectiveTransform(original_corners_arr, transformed_corners_arr);
        warpPerspective(image_original, image_original_transformed, transformed, Point(max_width, max_height));

        cvtColor(image_original_transformed, image_grey, cv::COLOR_BGR2GRAY);

        // Rotate if not rotated 😅
        // imwrite("/Users/ibrahimradwan/Desktop/test.jpg", image_original_transformed);
//        if (image_original_transformed.cols > image_original_transformed.rows)
//        {
//            cv::rotate(image_original_transformed, image_original_transformed, ROTATE_90_COUNTERCLOCKWISE);
//            cv::rotate(image_grey, image_grey, ROTATE_90_COUNTERCLOCKWISE);
//        }

        if (image_grey.cols - 80 > 0 && image_grey.rows - 80 > 0)
        {
            image_grey = image_grey(cv::Rect(40, 40, image_grey.cols - 80, image_grey.rows - 80));
            image_original_transformed = image_original_transformed(
                    cv::Rect(40, 40, image_original_transformed.cols - 80, image_original_transformed.rows - 80));
        }
    }

    /// Apply Wiener filter of window size 3x3
    static void apply_wiener_filter()
    {
        // Call to WienerFilter
        WienerFilter(image_grey, image_wiener, Size(5, 5));

        // Save as PPM to be used by Doxa library that doesn't support OpenCV
        imwrite(string(PREPROCESSOR_OUTPUT_PATH) + IMAGE_WIENER, image_wiener);
    }

    /// Apply Sauvola adaptive thresholding using k = 0.2
    static void apply_sauvola_binarization()
    {

        // Apply sauvola binarization.
        string command = string(IMGTXTENH_PATH) + " -k 0.2 -p " + string(PREPROCESSOR_OUTPUT_PATH)
                         + string(IMAGE_WIENER) + " " + string(PREPROCESSOR_OUTPUT_PATH)
                         + string(IMAGE_SAUVOLA);
        system(command.c_str());

        image_sauvola = imread(string(PREPROCESSOR_OUTPUT_PATH) + IMAGE_SAUVOLA, CV_LOAD_IMAGE_GRAYSCALE);
    }

    // Helper functions
    /// Used in sorting contours
    static bool compareContourAreas(vector<Point> &contour1, vector<Point> &contour2)
    {
        double i = fabs(contourArea(Mat(contour1)));
        double j = fabs(contourArea(Mat(contour2)));
        return (i < j);
    }

    /// Repair transformation points
    static void alignTransformationPoints(vector<Point> &original_corners, vector<Point> &transformed_corners)
    {
        vector<bool> vis(transformed_corners.size(), 0);

        vector<Point> res_corners(original_corners.size());
        for (int i = 0; i < original_corners.size(); ++i)
        {
            int minDis = 1e9;
            int minJ = 10;

            for (int j = 0; j < transformed_corners.size(); ++j)
            {
                if (vis[j]) continue;

                int dx = (transformed_corners[j].x - original_corners[i].x);
                int dy = (transformed_corners[j].y - original_corners[i].y);
                int dis = dx * dx + dy * dy;

                if (minDis > dis) {
                    minDis = dis;
                    minJ = j;
                }
            }

            vis[minJ] = true;
            res_corners[minJ] = original_corners[i];
        }

        original_corners.swap(res_corners);
    }
};

cv::Mat Preprocessor::image_original;
cv::Mat Preprocessor::image_original_transformed;
cv::Mat Preprocessor::image_grey;
cv::Mat Preprocessor::image_wiener;
cv::Mat Preprocessor::image_sauvola;
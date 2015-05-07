#ifndef Isothetic_h
#define Isothetic_h

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define ISO_LIGHT CV_RGB(255, 0, 255)
#define ISO_DARK CV_RGB(255, 180, 255)

using namespace std;
using namespace cv;

void drawGrid(Mat& image, int size, Scalar color);
vector<Point2i> makeOIP(Mat& img, int gsize);
vector<Point2i> animateOIP(Mat& img, Mat& final, int gsize);
void drawOIC(Mat& img, vector<Point2i> vertices, bool filled);
void patternRandRGB(Mat& src, Mat& dest, int gsize, bool animate);
void rainbowFill(Mat src, Mat dest, Mat thresMask, int gsize, bool animate);
void primsAlgoFill(Mat src, Mat dest, Mat thresMask, int gsize, bool animate);
void effect1(Mat &src, Mat &dest, int gsize);
void effect2(Mat &src, Mat &dest, int gsize);
#endif
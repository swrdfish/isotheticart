#include <stdio.h>
#include <math.h>
#include <opencv2/opencv.hpp>
// #include "makeoip.hpp"

using namespace cv;
using namespace std;

int size = 10, threshval = 200;
Mat image, result, final, steps;

void DrawGrid(Mat& image, int size);
void onSizeChange(int, void*);
int getPointType(Mat& img, Point2i q, int gsize);
bool ObjectInUGB(Mat& img, Point2i q, int ugb, int gsize);
Point2i getTopLeftPoint(Mat& image);
Point2i getStartPoint(Mat& img, Point2i p, int gsize);
vector<Point2i> makeOIP(Mat& img, Point2i topleftpoint, int gsize);
Point2i getNextPoint(Point2i currentpoint, int d, int gsize);

int main(int argc, char** argv) {

  if (argc < 2) {
    cout << "Usage: cvsetup <path to image>" << endl;
    return 0;
  }

  image = imread(argv[1]);

  if (!image.data) {
    printf("No image data \n");
    return -1;
  }

  // Convert to greyscale
  cvtColor(image, result, CV_RGB2GRAY);

  // Binarise the image
  threshold(result, result, threshval, 255, CV_THRESH_BINARY);

  // Draw the grid
  final = image.clone();
  DrawGrid(final, 10);

  // Display the binary image
  namedWindow("Intermediate image", CV_WINDOW_AUTOSIZE);
  namedWindow("Final image", CV_WINDOW_AUTOSIZE);
  imshow("Intermediate image", result);
  imshow("Final image", final);

  // Create a trakbar to control the grid size
  createTrackbar("Grid size", "Final image", &size, 50, onSizeChange);
  createTrackbar("Threshold", "Final image", &threshval, 255, onSizeChange);

  waitKey(1000);
  // cout << p << endl;
  // p = getStartPoint(result, p, size);
  // cout << p << endl;
  // Point2i arbit(150, 50);
  // cout << getPointType(result, p, size) << endl;
  Point2i p = getTopLeftPoint(result);
  vector<Point2i> isotheticcover = makeOIP(result, p, size);

  cout << isotheticcover << endl;
  waitKey(1000);
  return 0;
}

Point2i getTopLeftPoint(Mat& image) {
  int nRows = image.rows;
  int nCols = image.cols;

  // if the image has multiple channels
  // int nCols = image.cols*channels;

  if (image.isContinuous()) {
    nCols *= nRows;
    nRows = 1;
  }

  uchar* p;

  for (int i = 0; i < nRows; ++i) {
    p = image.ptr(i);
    for (int j = 0; j < nCols; ++j) {
      // Check for black pixel
      if (p[j] == 0) {
        if (image.isContinuous()) {
          nRows = image.rows;
          Point2i P(j % nRows, j / nRows);
          return P;
        } else {
          Point2i P(i, j);
          return P;
        }
      }
    }
  }

  Point2i P(-1, -1);
  return P;
}

void DrawGrid(Mat& img, int gsize) {
  if (gsize < 2)
    return;
  int nRows = img.rows;
  int nCols = img.cols;

  Point2i p1(0, 0);
  Point2i p2(0, nRows);
  Point2i stepx(gsize, 0);
  for (int i = gsize; i < nCols; i += gsize) {
    p1 += stepx;
    p2 += stepx;
    line(img, p1, p2, CV_RGB(255, 0, 255), 1, CV_AA, 0);
  }

  p1 = Point2i(0, 0);
  p2 = Point2i(nCols, 0);
  Point2i stepy(0, gsize);
  for (int i = gsize; i < nRows; i += gsize) {
    p1 += stepy;
    p2 += stepy;
    line(img, p1, p2, CV_RGB(255, 0, 255), 1, CV_AA, 0);
  }
}

void onSizeChange(int, void*) {

  final = image.clone();
  cvtColor(image, result, CV_RGB2GRAY);
  threshold(result, result, threshval, 255, CV_THRESH_BINARY);

  DrawGrid(final, size);
  imshow("Intermediate image", result);
  imshow("Final image", final);
}

Point2i getStartPoint(Mat& img, Point2i p, int gsize) {
  int qx, qy;
  qx = (ceil(float(p.x) / gsize) - 1) * gsize;
  qy = (floor(float(p.y) / gsize)) * gsize;
  Point2i q(qx, qy);
  return q;
}

int getPointType(Mat& img, Point2i q, int gsize) {
  int m = 0, r = 0, t = 10;
  for (int k = 1; k < 5; k++) {
    if (ObjectInUGB(img, q, k, gsize)) {
      m++;
      r += k;
      cout << "debug info: inside" << endl;
    }
  }
  if (m = 2 && (r == 4 || r == 6)) {
    t = -2;
  } else if (m == 0 || m == 4) {
    t = 0;
    // cout << "debug info: " << m << endl;
  } else {
    t = 2 - m;
  }
  return t;
}

bool ObjectInUGB(Mat& img, Point2i q, int ugb, int gsize) {
  Point2i pt;
  switch (ugb) {
    case 1:
      pt.x = q.x;
      pt.y = q.y - gsize;
      // cout << "debug info: 1 " << pt << endl;
      break;
    case 2:
      pt.x = q.x - gsize;
      pt.y = q.y - gsize;
      // cout << "debug info: 2 " << pt << endl;
      break;
    case 3:
      pt.x = q.x - gsize;
      pt.y = q.y;
      // cout << "debug info: 3 " << pt << endl;
      break;
    case 4:
      pt.x = q.x;
      pt.y = q.y;
      // cout << "debug info: 4 " << pt << endl;
      break;
    default:
      break;
  }
  uchar* p;
  for (int i = pt.y; i < pt.y + gsize; i++) {
    p = img.ptr(i);
    for (int j = pt.x; j < pt.x + gsize; ++j) {
      // cout << "(" << i <<"," << j << ")" << int(p[j]);
      if (p[j] == 0) {
        return false;
      }
    }
  }
  return true;
}

Point2i getNextPoint(Point2i currentpoint, int d, int gsize) {
  Point2i nextpoint;
  switch (d) {
    case 0:
      nextpoint.x = currentpoint.x + gsize;
      nextpoint.y = currentpoint.y;
      break;
    case 1:
      nextpoint.x = currentpoint.x;
      nextpoint.y = currentpoint.y + gsize;
      break;
    case 2:
      nextpoint.x = currentpoint.x - gsize;
      nextpoint.y = currentpoint.y;
      break;
    case 3:
      nextpoint.x = currentpoint.x;
      nextpoint.y = currentpoint.y - gsize;
      break;
  }
  return nextpoint;
}

vector<Point2i> makeOIP(Mat& img, Point2i topleftpoint, int gsize) {
  vector<Point2i> vertices;
  Point2i startpoint = getStartPoint(img, topleftpoint, gsize);
  Point2i q = startpoint;
  int type = getPointType(img, q, gsize);
  int d = (2 + type) % 4;

  // show the steps
  steps = final.clone();
  circle(steps, q, 1, CV_RGB(0, 255, 100), 1, CV_AA, 0);
  imshow("Final image", steps);
  waitKey(1000);

  do {
    if (type == 1 || type == -1) {
      vertices.push_back(q);
      circle(steps, q, 3, CV_RGB(255, 0, 0), 1, CV_AA, 0);
    }
    q = getNextPoint(q, d, gsize);
    type = getPointType(img, q, gsize);
    circle(steps, q, 1, CV_RGB(0, 0, 200), 1, CV_AA, 0);
    if (type == -2) {
      type = -1;
    }
    d = (d + type) % 4;
    imshow("Final image", steps);
    waitKey(1000);
  } while (q != startpoint);
  return vertices;
}

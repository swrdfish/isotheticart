#include <iostream>
#include <stdio.h>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
// #include "makeoip.hpp"
// void fillPoly(Mat& img, const Point** pts, const int* npts, int ncontours, const Scalar& color, int lineType=8, int shift=0, Point offset=Point() );
using namespace cv;
using namespace std;

int size, threshval = 200;
Mat image, result, final, steps;

void copyMat(Mat&);
void DrawGrid(Mat& image, int size);
void onSizeChange(int, void*);
int getPointType(Mat& img, Point2i q, int gsize);
bool ObjectInUGB(Mat& img, Point2i q, int ugb, int gsize);
void drawCover(Mat& img, vector<Point2i> vertices);
Point2i getTopLeftPoint(Mat& image);
Point2i getStartPoint(Mat& img, Point2i p, int gsize);
vector<Point2i> makeOIP(Mat& img, Point2i topleftpoint, int gsize);
Point2i getNextPoint(Point2i currentpoint, int d, int gsize);
void DrawOIP(vector<Point2i>);

int main(int argc, char** argv) {
  Mat res;
  char c;
  if (argc < 2) {
    cout << "Usage: cvsetup <path to image>" << endl;
    return 0;
  }
  cout<<"size : ";
  cin>>size;
  image = imread(argv[1]);

  if (!image.data) {
    printf("No image data \n");
    return -1;
  }
  
  // Convert to greyscale
  cvtColor(image, res, CV_RGB2GRAY);
  result.create(image.rows+4*size,image.cols+4*size,res.depth());
  // Binarise the image
  threshold(res, res, threshval, 255, CV_THRESH_BINARY);
  threshold(result, result, threshval, 255, 1);
  // Draw the grid
  copyMat(res);
  final = image.clone();
  DrawGrid(final, size);
  
  // Mat res (result, Rect(10,10, 500,500) ); 
  // Display the binary image
  namedWindow("Intermediate image", CV_WINDOW_AUTOSIZE);
  namedWindow("Final image", CV_WINDOW_AUTOSIZE);
  imshow("Intermediate image", result);
  imshow("Final image", final);
  // namedWindow("Final im", CV_WINDOW_AUTOSIZE);
  // imshow("Final im", res);
  // Create a trakbar to control the grid size
  // createTrackbar("Grid size", "Final image", &size, 50, onSizeChange);
  // createTrackbar("Threshold", "Final image", &threshval, 255, onSizeChange);

  waitKey(1000);
  
  uchar *pm = result.ptr(550);
  cout<<(int)pm[550]<<endl;
  // cout<<image<<endl;
  // cout << p << endl;
  // p = getStartPoint(result, p, size);
  // cout << p << endl;
  // Point2i arbit(150, 50);
  // cout << getPointType(result, p, size) << endl;
  Point2i p = getTopLeftPoint(result);
  cout << p << endl;
  vector<Point2i> isotheticcover = makeOIP(result, p, size);
  // namedWindow("OIC", CV_WINDOW_AUTOSIZE);
  // cout<<isotheticcover[0]<<endl;;
  // cout << isotheticcover << endl;
  // cout<<isotheticcover.size();
  DrawOIP(isotheticcover);
  waitKey(10000);
  // cin>>c;
  return 0;
}
void copyMat(Mat& src){
  int i,j;
  uchar *p,*q;
  for(i=0;i<src.rows;i++){
    p=result.ptr(i);
    for(j=0;j<src.cols;j++){
      p[j]=255;
    }
  }
  for(i=0;i<src.rows;i++){
    p=src.ptr(i);
    q=result.ptr(i+2*size);
    for(j=0;j<src.cols;j++){
      q[j+2*size]=p[j];
    }
  }
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
          nCols = image.cols;
          Point2i P(j % nCols, j / nCols);
          return P;
        } 
        else {
          Point2i P(j, i);
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
  qx = (ceil(float(p.x)/ gsize)-1) * gsize;
  qy = (ceil(float(p.y)/ gsize)-1) * gsize;
  Point2i q(qx, qy);
  return q;
}

int getPointType(Mat& img, Point2i q, int gsize) {
  int m = 0, r = 0, t = 10;
  for (int k = 1; k < 5; k++) {
    if (ObjectInUGB(img, q, k, gsize)) {
      m++;
      r += k;
      // cout << "debug info: inside" << endl;
    }
  }
  if (m == 2 && (r == 4 || r == 6)) {
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
  for (int i = pt.y; i <= pt.y + gsize; i++) {
    p = img.ptr(i);
    for (int j = pt.x; j <= pt.x + gsize; ++j) {
      // cout << "(" << i <<"," << j << ")" << int(p[j]);
      if (p[j] == 0) {
        return true;
      }
    }
  }
  return false;
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
      nextpoint.y = currentpoint.y - gsize;
      break;
    case 2:
      nextpoint.x = currentpoint.x - gsize;
      nextpoint.y = currentpoint.y;
      break;
    case 3:
      nextpoint.x = currentpoint.x;
      nextpoint.y = currentpoint.y + gsize;
      break;
  }
  return nextpoint;
}

vector<Point2i> makeOIP(Mat& img, Point2i topleftpoint, int gsize) {
  char c;
  vector<Point2i> vertices;
  Point2i startpoint = getStartPoint(img, topleftpoint, gsize);
  Point2i q = startpoint,sum;
  int type = getPointType(img, q, gsize);
  int d = (2 + type) % 4;
  sum.x=sum.y=2*size;
  // show the steps
  steps = final.clone();
  circle(steps, q-sum, 1, CV_RGB(0, 255, 100), 1, CV_AA, 0);
  imshow("Final image", steps);
  waitKey(500);

  do {
    cout << q << " type: " << type << " direction: " << d <<endl;
    if (type == 1||type==-1 ) {
      vertices.push_back(q);
      circle(steps, q-sum, 3, CV_RGB(255, 0, 0), 1, CV_AA, 0);
    }
    q = getNextPoint(q, d, gsize);
    type = getPointType(img, q, gsize);
    circle(steps, q-sum, 1, CV_RGB(0, 0, 200), 1, CV_AA, 0);
    if (type == -2) {
      type = -1;
    }
    d = (d + type) % 4;
    if (d < 0){
      d += 4;
    }
    imshow("Final image", steps);
    c = waitKey(10);
    if (c == 113)
      break;
    else if(c == 112)
      waitKey(0);
  } while (q != startpoint);
  return vertices;
}

void drawCover(Mat& img, vector<Point2i> vertices){
  int i;
  for (i = 0; i < vertices.size() -1; ++i)
  {
    line(img, vertices[i], vertices[i+1], CV_RGB(50, 50, 200), 2, CV_AA, 0);
  }
  line(img, vertices[i], vertices[0], CV_RGB(50, 50, 200), 2, CV_AA, 0);
}

void DrawOIP(vector<Point2i> pt){
  bool arr[1000][1000];
  Mat OIP;
  int i;
  OIP.create(result.rows,result.cols,result.depth());
  threshold(OIP,OIP, threshval, 255, 1);
  // uchar *p;
  // cout<<pt.size();
  for(i=0;i<pt.size()-1;i++){
    line(OIP, pt[i],pt[i+1], CV_RGB(0,0,0), 1, CV_AA, 0);
  }
  line(OIP, pt[pt.size()-1],pt[0], CV_RGB(0,0,0), 1, CV_AA, 0);
  
  int npt[] = {pt.size()};
	// Point rook_points[1][20];
	// rook_points[0]=pt;
  Point *p;
  p = pt.get_allocator().allocate(pt.size());

  // construct values in-place on the array:
  for (i=0; i<pt.size(); i++) pt.get_allocator().construct(&p[i],pt[i]);
  // cout<<p[1]<<endl;
  // pt.
  const Point* ppt[1] = { p };
  fillPoly( OIP,ppt,npt,1,Scalar( 0, 0, 0 ),1);
  namedWindow("OIP", CV_WINDOW_AUTOSIZE);
  imshow("OIP", OIP);
  // getcha
  waitKey(10000);
}

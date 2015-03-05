#include "isothetic.hpp"

void drawGrid(Mat& img, int gsize) {
  if (gsize < 2) return;
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
        } else {
          Point2i P(j, i);
          return P;
        }
      }
    }
  }

  Point2i P(-1, -1);
  return P;
}

Point2i getStartPoint(Mat& img, Point2i p, int gsize) {
  int qx, qy;
  qx = (ceil(float(p.x) / gsize) - 1) * gsize;
  qy = (ceil(float(p.y) / gsize) - 1) * gsize;
  Point2i q(qx, qy);
  return q;
}

bool objectInUGB(Mat& img, Point2i q, int ugb, int gsize) {
  Point2i pt;
  switch (ugb) {
    case 1:
      pt.x = q.x;
      pt.y = q.y - gsize;
      break;
    case 2:
      pt.x = q.x - gsize;
      pt.y = q.y - gsize;
      break;
    case 3:
      pt.x = q.x - gsize;
      pt.y = q.y;
      break;
    case 4:
      pt.x = q.x;
      pt.y = q.y;
      break;
    default:
      break;
  }
  // handle boundary conditions
  if (pt.x < 0 || pt.y < 0 || pt.x >= img.cols || pt.y >= img.rows) {
    return false;
  }

  uchar* p;
  for (int i = pt.y; i <= pt.y + gsize; i++) {
    p = img.ptr(i);
    for (int j = pt.x; j <= pt.x + gsize; ++j) {
      if (p[j] == 0) {
        return true;
      }
    }
  }
  return false;
}

int getPointType(Mat& img, Point2i q, int gsize) {
  int m = 0, r = 0, t = 10;
  for (int k = 1; k < 5; k++) {
    if (objectInUGB(img, q, k, gsize)) {
      m++;
      r += k;
    }
  }
  if (m == 2 && (r == 4 || r == 6)) {
    t = -2;
  } else if (m == 0 || m == 4) {
    t = 0;
  } else {
    t = 2 - m;
  }
  return t;
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

vector<Point2i> makeOIP(Mat& img, int gsize) {
  char c;
  vector<Point2i> vertices;
  Mat steps;
  Point2i topleftpoint = getTopLeftPoint(img);
  Point2i startpoint = getStartPoint(img, topleftpoint, gsize);
  Point2i q = startpoint;
  int type = getPointType(img, q, gsize);
  int d = (2 + type) % 4;
  do {
    // cout << q << " type: " << type << " direction: " << d <<endl;
    if (type == 1 || type == -1) {
      vertices.push_back(q);
    }
    q = getNextPoint(q, d, gsize);
    type = getPointType(img, q, gsize);
    if (type == -2) {
      type = -1;
    }
    d = (d + type) % 4;
    if (d < 0) {
      d += 4;
    }
  } while (q != startpoint);

  return vertices;
}

void drawOIC(Mat& img, vector<Point2i> vertices, bool isfilled) {
  const Point* pts[1] = {&vertices.at(0)};
  int npts[1] = {int(vertices.size())};
  if (isfilled) {
    fillPoly(img, pts, npts, 1, Scalar(0, 0, 0), 1, 0);
  } else {
    int iii;
    for (iii = 0; iii < vertices.size() - 1; ++iii) {
      line(img, vertices[iii], vertices[iii + 1], CV_RGB(50, 50, 200), 2, CV_AA,
           0);
    }
    line(img, vertices[iii], vertices[0], CV_RGB(50, 50, 200), 2, CV_AA, 0);
  }
}

vector<Point2i> animateOIP(Mat& img, Mat& final, int gsize) {
  char c;
  vector<Point2i> vertices;
  Mat steps;
  Point2i topleftpoint = getTopLeftPoint(img);
  Point2i startpoint = getStartPoint(img, topleftpoint, gsize);
  Point2i q = startpoint;
  int type = getPointType(img, q, gsize);
  // cout << "topleft: " << topleftpoint << " startpoint: " << startpoint << "
  // type: " << type << endl;
  int d = (2 + type) % 4;
  // show the steps
  steps = final.clone();
  circle(steps, q, 1, CV_RGB(0, 255, 100), 1, CV_AA, 0);
  imshow("Final image", steps);
  waitKey(500);

  do {
    // cout << q << " type: " << type << " direction: " << d <<endl;
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
    if (d < 0) {
      d += 4;
    }
    imshow("Final image", steps);
    c = waitKey(10);
    if (c == 113)
      break;
    else if (c == 112)
      waitKey(500);
  } while (q != startpoint);
  return vertices;
}

void patternRandRGB(Mat& src, Mat& dest, int gsize, bool animate) {
  int i, j, blue, green, red;
  int nRows = src.rows;
  int nCols = src.cols;
  Point2i tmp[5];
  int npts[1] = {4};
  const Point2i* pts[1];

  char c;
  uchar* p;
  for (j = 0; j < nRows; j += gsize) {
    p = src.ptr(j + 1);
    for (i = 0; i < nCols; i += gsize) {
      if (p[i + 1] == 0) {
        tmp[0].x = i;
        tmp[0].y = j;
        tmp[1].x = (i + gsize - 1);
        tmp[1].y = j;
        tmp[2].x = (i + gsize - 1);
        tmp[2].y = (j + gsize - 1);
        tmp[3].x = i;
        tmp[3].y = (j + gsize - 1);
        tmp[4].x = i;
        tmp[4].y = j;
        pts[0] = tmp;
        blue = rand() % 256;
        green = rand() % 256;
        red = rand() % 256;
        fillPoly(dest, pts, npts, 1, Scalar(blue, green, red), 1, 0);
        if (animate) {
          imshow("intermediate", dest);
          c = waitKey(10);
          if (c == 113) {
            animate = false;
          }
        }
      }
    }
  }
}


void rainbowFill(Mat src, Mat dest, Mat thresMask, int gsize, bool animate) {
  int nRows = dest.rows;
  int nCols = dest.cols;
  int nChannels = dest.channels();
  int n = 0; char c;
  float distance = 150;
  Point2i tmp[5];
  int npts[1] = {4};
  const Point2i* pts[1];
  Point2i topleftpoint = getTopLeftPoint(thresMask);
  cvtColor(dest, dest, CV_BGR2HSV, 0);

  // matrix to keep track of the visited nodes
  int visited[nRows * nCols];

  // queue to hold the nodes
  vector<Point2i> frontier;
  Point2i p, q;
  p = topleftpoint;
  cout << "debug: " << p << endl;
  frontier.push_back(p);
  visited[p.y * nRows + p.x] = 1;

  while (n = frontier.size()) {
    // convert to hsv space
    
    int i = rand() % n;
    Point2i p = frontier[i];
    frontier[i] = frontier[n - 1];
    frontier[n - 1] = p;
    frontier.pop_back();

    // adjacent nodes
    q.x = p.x + gsize;
    q.y = p.y;
    if (q.x < nCols && thresMask.ptr(q.y)[q.x] == 0 && visited[q.y * nRows + q.x] != 1) {
      frontier.push_back(q);
      visited[q.y * nRows + q.x] = 1;
    }
    q.x = p.x;
    q.y = p.y + gsize;
    if (q.y < nRows && thresMask.ptr(q.y)[q.x] == 0 && visited[q.y * nRows + q.x] != 1) {
      frontier.push_back(q);
      visited[q.y * nRows + q.x] = 1;
    }
    q.x = p.x - gsize;
    q.y = p.y;
    if (q.x >= 0 && thresMask.ptr(q.y)[q.x] == 0 && visited[q.y * nRows + q.x] != 1) {
      frontier.push_back(q);
      visited[q.y * nRows + q.x] = 1;
    }
    q.x = p.x;
    q.y = p.y - gsize;
    if (q.y >= 0 && thresMask.ptr(q.y)[q.x] == 0 && visited[q.y * nRows + q.x] != 1) {
      frontier.push_back(q);
      visited[q.y * nRows + q.x] = 1;
    }

    tmp[0].x = p.x;
    tmp[0].y = p.y;
    tmp[1].x = (p.x + gsize - 1);
    tmp[1].y = p.y;
    tmp[2].x = (p.x + gsize - 1);
    tmp[2].y = (p.y + gsize - 1);
    tmp[3].x = p.x;
    tmp[3].y = (p.y + gsize - 1);
    tmp[4].x = p.x;
    tmp[4].y = p.y;
    pts[0] = tmp;
    fillPoly(dest, pts, npts, 1, Scalar(int(distance += gsize*0.03) % 255, 180, 180), 1, 0);
    // if(thresMask.ptr(p.y)[p.x] == 0){
    // }
    // dest.ptr(p.y)[p.x * 3 + 0] = int(distance += 0.005) % 255;
    // dest.ptr(p.y)[p.x * 3 + 1] = 180;
    // dest.ptr(p.y)[p.x * 3 + 2] = 180;  
    if (animate) {
          imshow("intermediate", dest);
          c = waitKey(10);
          if (c == 113) {
            animate = false;
          }
        }
  }
  cvtColor(dest, dest, CV_HSV2BGR, 0);

}
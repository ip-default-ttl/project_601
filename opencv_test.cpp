#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct cam
{
  string ip;
  string port;
  string url;
  string login;
  string password;
  string proto;
}

inline void generate_url()
{
  cam.url=cam.proto+"://"+cam.login+':'+cam.password+'@'+cam.ip+':'+cam.port+"/live/0/MAIN";
}

int main()
{
  VideoCapture cap (cam.url);
  Mat img;
  while (true)
  {
    cap>>img;
    Mat edges;
    cvtColor(img, edges, COLOR_BGR2GRAY);
    Canny(edges, edges, 30, 60);
    namedWindow("main stream", WINDOW_AUTOSIZE);
    imshow("main stream", edges);
    waitKey(0);
  }
}

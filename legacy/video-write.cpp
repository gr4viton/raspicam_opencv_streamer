#include <iostream>	// for standard I/O
#include <string>   // for strings

#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat)
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>  // Video write

#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv;

static void help()
{
    cout
        << "------------------------------------------------------------------------------" << endl
        << "This program shows how to write video files."                                   << endl
        << "You can extract the R or G or B color channel of the input video."              << endl
        << "Usage:"                                                                         << endl
        << "./video-write <input_video_name> [ R | G | B] [Y | N]"                          << endl
        << "------------------------------------------------------------------------------" << endl
        << endl;
}





raspicam::RaspiCam_Cv Camera;
Size cap_size;

int init_raspicam(void)
{
    cv::Mat image;
    int nCount=10;
    //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );
   // Camera.set( CV_CAP_PROP_FORMAT, CV_8UC4 );
    Camera.set( CV_CAP_PROP_FORMAT, CV_8U );    // grayscale unsigned 8bit = byte = jpg
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );


    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

    int x = 320;
    int y = 240;
    printf("Setting resolution to %ix%i\n",x,y);
    Camera.set( CV_CAP_PROP_FRAME_WIDTH, x);
    Camera.set( CV_CAP_PROP_FRAME_HEIGHT, y);

    printf("Resolution set\n");


    //Start capture
    cout<<"Capturing "<<nCount<<" frames ...."<<endl;


    for ( int i=0; i<nCount; i++ ) {
        Camera.grab();
        Camera.retrieve ( image);
        if ( i%2==0 )
            cout<<"\r captured "<<i<<" images" \
                <<" of size: "<<image.size() \
                <<" img type: "<<image.type() \
                <<std::flush;

    }   
    cap_size = image.size();

    cout<<"\nStop camera..."<<endl;
        return 0;
}


int main(int argc, char *argv[])
{
    init_raspicam();

  const string NAME = "/home/pi/mjpg_out2/out.avi"; // functional writer for MJPG

 //   const string NAME = source.substr(0, pAt) + argv[2][0] + ".avi";   // Form the new name with container
 //   int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form

    int ex = CV_FOURCC('M','J','P','G');


    double fps = 20;


    int channel = 2; // Select the channel to save
    Mat src, res;
    vector<Mat> spl;

    for(;;) //Show the image captured in the window and repeat
    {
        Camera.grab();
        Camera.retrieve(src);

 //       inputVideo >> src;              // read

        if (src.empty()) 
        {
            cout<< "grabbed empty\n";
            break;         // check if at end
        }

        Size S = src.size();
        cout << "siz = " << src.cols << "x" << src.rows << endl;
        split(src, spl);                // process - extract only the correct channel
        for (int i =0; i < 3; ++i)
            if (i != channel)
                spl[i] = Mat::zeros(S, spl[0].type());
       merge(spl, res);
        
        res = src.clone();



    bool is_colored = true;
        S = cap_size;

    //    is_colored = false;

   //     S = Size(cap_size.height*3, cap_size.width);
  //      S = Size(cap_size.height, cap_size.width);

//        cvtColor(src, res, cv::COLOR_BGR2GRAY);

        VideoWriter outputVideo;                                        // Open the output
        cout << "videoWriter openning" << endl;
            outputVideo.open(NAME, ex, fps, S, is_colored);

        if (!outputVideo.isOpened())
        {
            cout  << "Could not open the output video for write: " << NAME << endl;
            return -1;
        }
        else
            cout << "outputVideo is opened" << endl;

       outputVideo.write(res); //save or
       //outputVideo << res;

        cout<<"output written"<<endl;
    }

    cout << "Finished writing" << endl;
    return 0;
}

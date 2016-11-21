// source: http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html

//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/video/background_segm.hpp"

#include <iostream>
#include <cstring>
#include <string>

#include <iostream>
#include <fstream>


#include <time.h>
#include <limits.h>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <raspicam/raspicam_cv.h>

using namespace cv;
using namespace std;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void printusage(){
    cout<<"Usage:\n";
    cout<<"  ./raspicam_cv_stream /var/www/output.mjpeg";
    cout<<"\twill write stream file at given location\n";
    cout<<endl;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// global variables
raspicam::RaspiCam_Cv Camera;

Mat output;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function declarations
void rename_it();
int write_video(string NAME, int codec_id, bool is_colored);
int write_image(string NAME);

int set_cam_format(int format);
void print_type();




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function definitions
int init_raspicam(void)
{
    cv::Mat image;
    int nCount=10;
    //set camera params
    // Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );
    // Camera.set( CV_CAP_PROP_FORMAT, CV_8UC4 );
    // grayscale unsigned 8bit = byte = jpg
    Camera.set( CV_CAP_PROP_FORMAT, CV_8U );    
//    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );

    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

    int x = 320;
    int y = 240;
    printf("Setting resolution to %ix%i\n",x,y);
    Camera.set( CV_CAP_PROP_FRAME_WIDTH, x);
    Camera.set( CV_CAP_PROP_FRAME_HEIGHT, y);

    printf("Resolution set");


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
    cout<<endl<<"Stop camera..."<<endl;
	return 0;
}

int set_cam_format(int format){
    cout<<"Releasing Camera..."<<endl;
    Camera.release();
    cout<<"Seting Camera format..."<<endl;
    Camera.set( CV_CAP_PROP_FORMAT, CV_8U );    
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
}


int main(int argc, char** argv)
{

    init_raspicam();
	
    // fps counter begin
    time_t start, end;
    int counter = 0;
    double sec;
    double fps;
    // fps counter end

    int color = 0;


    int cam_code = 0;
    switch(color){
        case(0): cam_code=CV_8U; break;
        case(4): cam_code=CV_8UC4; break;
        case(3): cam_code=CV_8UC3; break;
        case(1): cam_code=CV_8UC1; break;
    };
    set_cam_format(cam_code);

    while(1)
    {
        if (counter == 0){
                time(&start);
        }
        Mat frame;
        Mat grabbed;
    

        Camera.grab();
        Camera.retrieve(grabbed);
        
        frame = grabbed.clone();        

        if(frame.empty())
            printf("Retrieved empty frame!\n");    
        else
            printf("Retrieved image! (%ix%i)\n", frame.cols, frame.rows);

        output = frame;
        int code = 0;
        if (color!=0){
            switch(color){
                case(3): code = COLOR_BGR2GRAY; break;
                case(4): code = COLOR_RGBA2GRAY; break;
                case(6): code = COLOR_GRAY2RGBA; break;
            };
            cvtColor(frame, output, cv::COLOR_BGR2GRAY);
        }
        //output = frame;
        
        print_type();


 //      "/home/pi/mjpg_out2/out.avi" 
        write_image("out.jpg");
 //       write_video("out.avi", CV_FOURCC('M','J','P','G'), false);
 //       write_video("out.avi", CV_FOURCC('M','J','P','G'), true);
        rename_it();        

        // fps counter begin
        time(&end);
        counter++;
        sec = difftime(end, start);
        fps = counter/sec;
        cout.precision(2);
        cout<<fixed<<fps<<" fps\n";
        if (counter == (INT_MAX - 1000)) counter = 0;
        // fps counter end
    }

    Camera.release();
//    cap.release();
    return 0;
}

int write_image(string NAME )
{
    //write the image to file
    //bool bSuccess = imwrite(NAME, output, compression_params); 
    bool bSuccess = imwrite(NAME, output); 
    if ( !bSuccess )
    {
         cout << "ERROR : Failed to save the image" << endl;
    }
}

int write_video(string NAME, int codec_id, bool is_colored)
{
    Size S = output.size();
    VideoWriter outputVideo;
    double out_fps = 5;
    try
    {
        outputVideo.open(NAME, codec_id, out_fps, S, is_colored);
    } 
    catch (cv::Exception & e)
    {
        std::cout << e.what() << std::endl;
    }

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << NAME << endl;
        return -1;
    }

    outputVideo.write(output);
    outputVideo.release();
    return 0;
}



void rename_it()
{
    bool out = rename("/home/pi/DEV/bprp/raspicam_cv_stream/build/out.jpg", 
                        "/home/pi/DEV/bprp/raspicam_cv_stream/build/out.mjpg");

//    bool out = rename("build/out.avi", "build/out.mjpg");
//    bool out = rename("/home/pi/mjg_out2/out.avi", "/home/pi/mjpg_out2/out.mjpg");

    if (!out)
        cout<<"Could not rename!"<<endl;
        //cout<<"Could not rename ["<<".avi"<<"] to [.mjpg]"<<endl;
    else
        cout<<"Renamed!"<<endl;

/*
    ofstream f;
    f.open("build/out.mjpg", ios::out | ios::app | ios::binary);
    f<<endl;
    f.close();
  */  
}


void print_type()
{
    int type_i = output.type();
    printf("Size is %ix%i, type_i=%i, type = %s\n",  
                    output.cols, output.rows, type_i, (
                        (type_i && CV_8U) ? "CV_8U" : (
                        (type_i && CV_8UC1) ? "CV_8UC1" : (
                        (type_i && CV_8UC3) ? "CV_8UC3" : (
                        (type_i && CV_8UC4) ? "CV_8UC4" : (
                                            "not known" )))) )
                );
}

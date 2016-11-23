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
// global variables
raspicam::RaspiCam_Cv Camera;

Mat output;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function declarations
void rename_it();
int write_image(string NAME);

int set_cam_format(int format);
void print_type();
Mat get_x_edges(Mat img_gray);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function definitions
int init_raspicam(void)
{
    cv::Mat image;
    int nCount=10;
    //set camera params
    // grayscale unsigned 8bit = byte 
    Camera.set( CV_CAP_PROP_FORMAT, CV_8U );    

    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

    int x = 320;
    int y = 240;
    printf("Setting resolution to %ix%i\n", x, y);
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
    cout<<"Releasing Camera to change parameters..."<<endl;
    Camera.release();
    cout<<"Seting Camera format..."<<endl;
    Camera.set( CV_CAP_PROP_FORMAT, format);    
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
}


int main(int argc, char** argv)
{
    // init camera input via raspicam library and try to grab images
    init_raspicam();
	
    // fps counter begin
    time_t start, end;
    int counter = 0;
    double sec;
    double fps;
    // fps counter end

    int color_index= 3;
    int cvt_index = 3;
    if (argc>1)
    {   
        color_index = atoi(argv[1]);
        if (argc>2)
            cvt_index = atoi(argv[2]);
        else
            cvt_index = color_index;
    }
    cout<<"color_index = "<<color_index<<endl;
    cout<<"cvt_index = "<<cvt_index<<endl;

    int cam_code = 0;
    switch(color_index){
        case(0): cam_code=CV_8U; break; // 8-bit unsigned int - no color = grayscale
        case(1): cam_code=CV_8UC1; break; 
        case(3): cam_code=CV_8UC3; break; // 8-bit usnsigned int - 3 colors = rgb or bgr (from cam)
        case(4): cam_code=CV_8UC4; break;
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
        
        if (cvt_index!=0){
            int cvt_code = COLOR_BGR2GRAY;
            switch(cvt_index){
                case(1): cvt_code = COLOR_BGR2RGB; cout<<"COLOR_BGR2RGB"<<endl;break;
                case(2): cvt_code = COLOR_BGR2GRAY; cout<<"COLOR_BGR2GRAY"<<endl;break;

                // as the CV_8UC3 has color channels in BGR order (color_index = 3)
                //we must switch them to RGBfor the viewing 
                case(3): cvt_code = COLOR_BGR2RGB; cout<<"COLOR_BGR2RGB"<<endl;break;
                case(4): cvt_code = COLOR_RGBA2BGR; cout<<"COLOR_RGBA2BGR"<<endl;break;
                case(6): cvt_code = COLOR_GRAY2RGB; cout<<"COLOR_GRAY2RGB"<<endl;break;
            };
            Mat colored;
            cvtColor(frame, colored, cvt_code);
            output = colored;
        }
		
		// Region of interest = get subimage
        Rect roi(0,50,frame.cols, 100);
        Mat img_roi = output(roi);
        output = img_roi;
        
        // detect edges
        //output = get_x_edges(output);
        
        // print image type
        print_type();
		
		// write file on disk to be able to stream it
        write_image("../stream/out.jpg");
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

void rename_it()
{
    int rc = rename("../stream/out.jpg", 
                    "../stream/out.mjpg");
    if (rc)
        cout<<"Could not rename!"<<endl;
        cerr<<"Return code ["<<rc<<"]renaming jpg to mjpg"<<endl;
}



Mat get_x_edges(Mat img_gray)
// the input and return arguments are not optimal as it is passed by value
// this copies the image everytime
// you should use pointers (or C++ references) to pass the value by refference
// this would only copy the address (pointer) to the image = faster for processing
{
	// if not grayscale - convert it
    if(!(img_gray.type() && CV_8U))
    {
		Mat colored;
		cvtColor(img_gray, img_gray, COLOR_RGB2GRAY);
	}
        
	int scale = 1; 
	int delta = 0; 
	int ddepth = CV_16S;

	Mat grad_x;

	//Scharr( img_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT ); 
	Sobel( img_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT ); 
	
	return grad_x;
}

void print_type()
{
    int type_i = output.type();
    printf("Size is %ix%i, type_i=%i, type = %s\n",  
                    output.cols, output.rows, type_i, (
                        (type_i && CV_8U || 1) ? "CV_8U" : (
                        (type_i && CV_8UC1) ? "CV_8UC1" : (
                        (type_i && CV_8UC3) ? "CV_8UC3" : (
                        (type_i && CV_8UC4) ? "CV_8UC4" : (
                                            "not known" )))) )
                );
}

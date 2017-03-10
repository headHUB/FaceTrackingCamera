/****************************************************************************************************
						Face Tracking Camera Using OpenCV, C++ and Arduino
					Haar Feature-based Cascade Classifier for Face Detection
									[ CPP and OpenCV ]
*****************************************************************************************************/


/**	Header Files	*/
#include<opencv2/objdetect/objdetect.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include <iostream>
#include "Tserial.h"

using namespace std;
using namespace cv;

/** Function Headers */
void detectFace(Mat frame);

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "Face Tracking Camera";

/**	Serial Communication through Arduino COM port*/
Tserial *arduino_com;

/**	Main Function*/
int main()
{
	// Initilize WebCam 0
	VideoCapture capture(0);
	Mat frame;

	// Setting Up Serial Comm to Arduino 
	arduino_com = new Tserial();
	if (arduino_com != 0) 
	{
		arduino_com->connect("COM6", 57600, spNONE);	//	Arduino connected to port : COM6 ; Baud Rate : 57600
	}
	

	//	Load the cascades
	if (!face_cascade.load(face_cascade_name))	// Check if cascades are loaded
	{
		printf("\n\tERROR: Cascades are not loaded,\n Copy 'haarcascade_frontalface_alt.xml' to current directory\n");
		return -1; 
	};
	
	//	Read the video stream
	if (capture.isOpened() == false)							// Check if VideoCapture object was associated to webcam successfully
	{
		std::cout << "\n\tERROR: Webcam not accessable !\n\n";	// If not, print error message to std out and exit program
		return(0);											
	}
	if (capture.isOpened())					// If true, continue the program
	{
		while (true)
		{
			capture.read(frame);		//	Capture the frame
			if (!frame.empty())			// If the frame is not empty detect the face in it.
			{
				detectFace(frame);
			}
			else
			{
				printf("\n\tNo captured frame !"); 
				break;
			}

			int c = waitKey(10);
			if ((char)c == 'c')				// Press ' C ' to close the program
			{
				arduino_com->sendChar(0);	// Set the servo back to normal position
				break; 
			}
		}
	}
	// Disconnect Arduino
	arduino_com->disconnect();
	delete arduino_com;
	arduino_com = 0;
	return 0;
}

/**	Function to Detect and Track Face*/

void detectFace(Mat frame)
{
	//	Local variables for detecting and tracking face
	char tiltChannel = 1;
	char panChannel = 2;
	char servoTiltPosition = 90;	// Servo Normal Tilt and Pan Position
	char servoPanPosition = 90;
	int midFaceX = 0;			
	int midFaceY = 0;
	int midScreenX = 300;		//	Mid position of screen, Size of screen 600x600
	int midScreenY = 300;
	int midScreenWindow = 1;	//	Acceptable error for center of window.
	int stepSize = 1;			//	Step size
	
	std::vector<Rect> faces;	//	Vector of Rectangles , Face
	Mat frame_gray;
	
	//	Convert the captured frame from BGR to GreyScale
	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	//	Equalizes the histogram of the grayscale image
	equalizeHist(frame_gray, frame_gray);
	
	//	Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (int i = 0; i < faces.size(); i++)
	{
		// Finding the center of the face
		midFaceX = faces[i].x + (faces[i].width / 2);
		midFaceY = faces[i].y + (faces[i].height / 2);
		Point center(midFaceX, midFaceY);	

		// Drawing a rectangle around the face
		rectangle(frame, Point( faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar(255, 0, 255), 1, 8, 0);
		
		//Tracing the center of the face
		
		//If the Y component of the face is below the middle of the screen, update the tilt position variable to lower the tilt servo.
		if (midFaceY < (midScreenY - midScreenWindow)) 
		{
			if (servoTiltPosition >= 5)servoTiltPosition -= stepSize; 
		}
		//If the Y component of the face is above the middle of the screen, update the tilt position variable to raise the tilt servo.
		else if (midFaceY >(midScreenY + midScreenWindow)) 
		{
			if (servoTiltPosition <= 175)servoTiltPosition += stepSize; 
		}
		//If the X component of the face is to the left of the middle of the screen, update the pan position variable to move the servo to the left.
		if (midFaceX < (midScreenX - midScreenWindow)) 
		{
			if (servoPanPosition >= 5)servoPanPosition -= stepSize; 
		}
		//If the X component of the face is to the right of the middle of the screen, update the pan position variable to move the servo to the right.
		else if (midFaceX > (midScreenX + midScreenWindow) )
		{
			if (servoPanPosition <= 175)servoPanPosition += stepSize; 
		}

		// Send Servo Positions
		arduino_com->sendChar(tiltChannel);
		arduino_com->sendChar(servoTiltPosition);
		arduino_com->sendChar(panChannel);
		arduino_com->sendChar(servoPanPosition);
		//	Shows the center of face and transmitted data to Arduino (Pan and Tilt)
		cout << "X:" << midFaceX << "  y:" << midFaceY <<" Tilt : "<<int(servoTiltPosition)<<" Pan : "<<int(servoPanPosition)<<endl;

	}

	//	Show the result
	imshow(window_name, frame);

}

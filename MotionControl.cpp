#include "MotionControl.h"

using namespace std;

Video::Video(void)
{
	capture = VideoCapture(0);
	capture.read(initializingMat);
}

void Video::TakeCameraInput(void)
{
	capture.read(cameraInput);
}

Threshold::Threshold(void)
{
	for (int i = 0; i < 10; i++)
	{
		lowH[i] = 0;
		highH[i] = 0;
		lowS[i] = 0;
		highS[i] = 0;
		lowV[i] = 0;
		highV[i] = 0;
		erodeVal[i] = 5;
		dilateVal[i] = 5;

	}
}

void Threshold::ThresholdRange(int colorN,char* window)
{
	namedWindow(window, CV_WINDOW_AUTOSIZE);

	createTrackbar("LowH", window, &lowH[colorN], 179);
	createTrackbar("HighH", window, &highH[colorN], 179);
	
	createTrackbar("LowS", window, &lowS[colorN], 255);
	createTrackbar("HighS", window, &highS[colorN], 255);

	createTrackbar("LowV", window, &lowV[colorN], 255);
	createTrackbar("HighV", window, &highV[colorN], 255);
}

void Threshold::ERRange(int colorN,char* window)
{
	namedWindow(window, CV_WINDOW_AUTOSIZE);

	createTrackbar("Erode", window, &erodeVal[colorN], 20);
	createTrackbar("Dilate", window, &dilateVal[colorN], 20);
}

void Threshold::ThresholdColor(int colorN)
{
	cvtColor(cameraInput, imgHSV, COLOR_BGR2HSV);
	inRange(imgHSV, Scalar(lowH[colorN], lowS[colorN], lowV[colorN]), Scalar(highH[colorN], highS[colorN], highV[colorN]), thresholdImg[colorN]);

	erode(thresholdImg[colorN], thresholdImg[colorN], getStructuringElement(MORPH_ELLIPSE, Size(erodeVal[colorN], erodeVal[colorN])));
	dilate(thresholdImg[colorN], thresholdImg[colorN], getStructuringElement(MORPH_ELLIPSE, Size(dilateVal[colorN], dilateVal[colorN])));

	dilate(thresholdImg[colorN], thresholdImg[colorN], getStructuringElement(MORPH_ELLIPSE, Size(dilateVal[colorN], dilateVal[colorN])));
	erode(thresholdImg[colorN], thresholdImg[colorN], getStructuringElement(MORPH_ELLIPSE, Size(erodeVal[colorN], erodeVal[colorN])));

}

MotionData::MotionData(void)
{
	for (int i = 0; i < 10; i++)
	{
		motionLineMat[i] = Mat::zeros(initializingMat.size(), CV_8UC3);
		lastPosX[i] = -1;
		lastPosY[i] = -1;
	}
}

void MotionData::SetPositionOfColor(int colorN)
{
	int R=0, G=0, B=0;
	if (colorN % 3 == 0)
	{
		R = 255 / (colorN + 1);
	}
	if (colorN % 3 == 1)
	{
		G = 255 / (colorN + 1);
	}
	if (colorN % 3 == 2)
	{
		B = 255 / (colorN + 1);
	}
	if (colorN == 9)
	{
		R = 255;
		G = 255;
		B = 255;
	}
	moment = moments(thresholdImg[colorN]);
	area[colorN] = moment.m00;
	if (moment.m00 > 10000)
	{
		lastPosX[colorN] = posX[colorN];
		lastPosY[colorN] = posY[colorN];
		posX[colorN] = moment.m10 / moment.m00;
		posY[colorN] = moment.m01 / moment.m00;
		if (lastPosX[colorN] >= 0 && lastPosY[colorN] >= -1)
		{
			line(motionLineMat[colorN], Point(posX[colorN], posY[colorN]), Point(lastPosX[colorN], lastPosY[colorN]), Scalar(R, G, B), 2);
		}
	}
}

GestureControl::GestureControl(void)
{
	for (int i = 0; i < 10; i++)
	{
		counter[i] = 0;
	}
	ClickRange = 3000;
	Condition = 0;
}

void GestureControl::GestureRectangle(int colorN)
{
	if (counter[colorN] == 0)
	{
		XposX[colorN] = posX[colorN];
		XposY[colorN] = posY[colorN];
	}
	counter[colorN] = (counter[colorN] + 1) % 2;
	if (counter[colorN] == 1)	//logic for gesture, Xpos is location at 10 frames before, pos is current pos
	{
		int temp = 0;
		if ((posX[colorN] - XposX[colorN]) == 0)
		{
			temp = 1;
		}
		float m = ((float)posY[colorN] - (float)XposY[colorN]) / ((float)posX[colorN] - (float)XposX[colorN] + temp);
		cout << endl << m;
	}

}

void GestureControl::MoveCursor(int colorN)
{
	SetCursorPos((2000 - (2.5 * posX[colorN]) - 360), 2.5 * posY[colorN] - 200);
}

void GestureControl::Click(int colorN1, int colorN2)
{
	int x1 = posX[colorN1];
	int x2 = posX[colorN2];
	int y1 = posY[colorN1];
	int y2 = posY[colorN2];
	if (((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) <= ClickRange) && Condition == 0)
	{
		INPUT Input = { 0 };
		Input.type = INPUT_MOUSE;

		Input.mi.dx = (LONG)100;
		Input.mi.dy = (LONG)100;

		Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		SendInput(1, &Input, sizeof(INPUT));
		Condition = 1;
	}
	else if (Condition == 1 && ((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) > ClickRange))
	{
		INPUT Input = { 0 };
		Input.type = INPUT_MOUSE;

		Input.mi.dx = (LONG)100;
		Input.mi.dy = (LONG)100;

		Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;

		SendInput(1, &Input, sizeof(INPUT));
		Condition = 0;
	}

}

void ChangeVariable::ChangeVar(char* WinName, int* Variable, int Range)
{
	namedWindow(WinName, CV_WINDOW_AUTOSIZE);
	createTrackbar("Variable", WinName, Variable, Range);
}

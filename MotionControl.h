#pragma once

#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

class Video
{
public :
	VideoCapture capture;
	Mat cameraInput;
	Mat initializingMat;

	Video();
	void TakeCameraInput(void);
};

class Threshold : public Video
{
public :
	int lowH[10];
	int highH[10];
	int lowS[10];
	int highS[10];
	int lowV[10];
	int highV[10];

	int erodeVal[10];
	int dilateVal[10];

	Mat thresholdImg[10];
	Mat imgHSV;

	Threshold();

	void ThresholdRange(int,char*);

	void ERRange(int,char*);

	void ThresholdColor(int);
};

class MotionData : public Threshold
{
public :
	Mat motionLineMat[10];

	Moments moment;

	int posX[10];
	int posY[10];
	int lastPosX[10];
	int lastPosY[10];
	double area[10];

	MotionData();
	void SetPositionOfColor(int);

};

class GestureControl : public MotionData
{
public:
	int counter[10];
	int XposX[10];
	int XposY[10];
	int Condition, ClickRange;

	GestureControl();
	void GestureRectangle(int);
	bool GestureTriangle(int);
	void MoveCursor(int);
	void Click(int,int);
};

class ChangeVariable
{
public:
	void ChangeVar(char*,int*,int);
};

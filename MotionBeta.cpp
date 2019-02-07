// MotionBeta.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MotionControl.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	GestureControl M;
	ChangeVariable C;

	int tracking = 0;

	C.ChangeVar("Change Var", &tracking, 1);

	M.ThresholdRange(0,"color 0");
	M.ThresholdRange(1,"color 1");
	M.ERRange(0, "color 0 ER");
	M.ERRange(1, "color 1 ER");
	while (true)
	{
		
		M.TakeCameraInput();

		M.ThresholdColor(0);
		M.ThresholdColor(1);

		M.SetPositionOfColor(0);
		M.SetPositionOfColor(1);

		imshow("Threshold color 0 & 1", M.thresholdImg[0] + M.thresholdImg[1]);
		imshow("lines", M.motionLineMat[0] + M.motionLineMat[1]);
		if (tracking == 1 && M.area[0] > 10000)
		{
			M.MoveCursor(0);
			M.Click(0,1);
		}

		waitKey(30);
	}
	return 0;
}


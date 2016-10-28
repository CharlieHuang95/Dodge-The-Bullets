#include "Header.h"

/** @function main */
int main(int argc, char** argv)
{
	Mat image;
	Mat imageOverlay;
	image = imread("C:/Black.png");

	// Initialize a Stigure Figure to do our bidding
	StickFigure Steve = StickFigure();
	Steve.Set_Initial(image.rows, Point_F(20, 80), 30.0);

	// Initialize a Background
	Background back = Background();
	char key = 'f';

	// Initialize webcam
	VideoCapture cap(0);

	vector<Vec2f> lines;
	Mat dst, cdst;
	Mat frame;
	Point p1, p2;
	Point pt1, pt2;
	int dilation_type = MORPH_ELLIPSE;
	int dilation_size1 = 1;
	int dilation_size2 = 5;
	int horizontalsize = 5;
	Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1));
	Mat element1 = getStructuringElement(dilation_type, Size(2 * dilation_size1 + 1, 2 * dilation_size1 + 1), Point(dilation_size1, dilation_size1));
	Mat element2 = getStructuringElement(dilation_type, Size(2 * dilation_size2 + 1, 2 * dilation_size2 + 1), Point(dilation_size2, dilation_size2));
	Mat dilation_dst;

	while (1) {

		if (useImage == 1) { frame = image; }
		else { cap >> frame; }
		Canny(frame, dst, 180, 200, 3);
		cvtColor(dst, cdst, CV_GRAY2BGR);

		if (0) {
			HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);
			for (size_t i = 0; i < lines.size(); i++)
			{
				float rho = lines[i][0], theta = lines[i][1];
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;
				pt1.x = cvRound(x0 + 1000 * (-b));
				pt1.y = cvRound(y0 + 1000 * (a));
				pt2.x = cvRound(x0 - 1000 * (-b));
				pt2.y = cvRound(y0 - 1000 * (a));
				if (pt1.x - pt2.x != 0 && abs((pt1.y - pt2.y) / (pt1.x - pt2.x)) < 0.5) {
					p1 = pt1;
					p2 = pt2;
					//line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
				}
			}
		}

		//erode(cdst, cdst, horizontalStructure);
		//dilate(cdst, dilation_dst, horizontalStructure);
		//imshow("Dilation Demo", cdst);
		vector<Vec4i> lines;
		if (1) {
			HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);
			for (size_t i = 0; i < lines.size(); i++)
			{
				Vec4i l = lines[i];
				p1 = Point(l[0], l[1]);
				p2 = Point(l[2], l[3]);

				line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
			}
		} 
		int* my = FindTheGround(lines, dst);
		for (size_t i = 0; i < dst.cols - 1; i++) {
			cout << my[i];
		}

		imshow("detected lines", cdst);
		imageOverlay = Steve.Draw_Overlay(frame);

		back.ground.l_end = Point_F(p1.x, p1.y);
		back.ground.r_end = Point_F(p2.x, p2.y);
		back.left_end = 20;
		back.right_end = 600;

		//DrawLine(imageOverlay, back.ground.l_end, back.ground.r_end);
		imshow("Steve", imageOverlay);
		Steve.FSM(back);

		Steve.adjustReference(back.ground.l_end, back.ground.r_end);
		if (key == 'a') { Steve.Set_Initial(image.rows, Point_F(20, 80), 30.0); Steve.currentState = REPOSITION_F; }
		key = waitKey(5);
	}
	return 0;
}

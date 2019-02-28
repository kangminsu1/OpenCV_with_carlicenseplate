#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <Windows.h>
#include <iostream>

#define WIN_NAME "�ڵ��� ��ȣ�� �ν�"
#define IMAGE_PATH "..\\car_license_plate_images\\1.jpg"
#define CANNY_THRESHOLD_1 100
#define CANNY_THRESHOLD_2 400
#define MAX_WIDTH_HEIGHT_RATIO 3.5
#define MIN_WIDTH_HEIGHT_RATIO 0.5
#define MAX_RECT_AREA 2000
#define MIN_RECT_AREA 300
#define GRADIENT_THRESHOLD 0.1

using namespace std;
using namespace cv;

/**
*	���� �޼ҵ�
*/
int main(int argc, char *argv[]) {
	// �̹��� �ε�
	Mat image = imread(IMAGE_PATH);

	// �׷��� �����Ϸ� ��ȯ
	Mat proccessedImage;
	cvtColor(image, proccessedImage, CV_BGR2GRAY);

	// canny �˰����� �̿��Ͽ� �������� ����
	Canny(proccessedImage, proccessedImage, CANNY_THRESHOLD_1, CANNY_THRESHOLD_2);

	// �������� ������
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(proccessedImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point());
	vector<vector<Point>> contoursPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Rect> boundRect2(contours.size());

	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours[i]), contoursPoly[i], 1, true);
		boundRect[i] = boundingRect(Mat(contoursPoly[i]));
	}

	// ��ȣ�� �ĺ��� ���� ���� ����
	int refineryCount = 0;

	// ������ �� ��ȣ�� �ĺ��� �� �� �ִ� �͸� ���� (����, ���� ���� �� ũ��� �Ǵ�)
	for (int i = 0; i < contours.size(); i++) {
		double widthHeightRatio = (double)boundRect[i].height / boundRect[i].width;

		if (
			widthHeightRatio <= MAX_WIDTH_HEIGHT_RATIO &&
			widthHeightRatio >= MIN_WIDTH_HEIGHT_RATIO &&
			boundRect[i].area() <= MAX_RECT_AREA &&
			boundRect[i].area() >= MIN_RECT_AREA
		) {
			refineryCount++;
			boundRect2[refineryCount] = boundRect[i];
		}
	}

	boundRect2.resize(refineryCount);

	// �߷��� ��ȣ�� �ĺ��� �������� x ��ǥ �������� ����
	for (int i = 0; i < boundRect2.size(); i++) {
		for (int j = 0; j < boundRect2.size() - 1; j++) {
			if (boundRect2[j].tl().x > boundRect2[j + 1].tl().x) {
				Rect tmpRect = boundRect2[j];
				boundRect2[j] = boundRect2[j + 1];
				boundRect2[j + 1] = tmpRect;
			}
		}
	}

	// ��ȣ�� ������ ��ü�� ���� �ε���
	int startIdx = 0;

	// ���� ������ ���� ���� ����
	int adjecentCount = 0;

	// ��ȣ�� ���� ���� ���� ����
	double carLicensePlateWidth = 0;

	for (int i = 0; i < boundRect2.size(); i++) {
		int count = 0;
		double deltaX = 0.0;

		for (int j = i + 1; j < boundRect2.size(); j++) {
			// ���� �������� x ��ǥ�� �ִ� �Ÿ� ���� �������� x ��ǥ ������ �Ÿ��� ����
			deltaX = abs(boundRect2[j].tl().x - boundRect2[i].tl().x);

			// �Ÿ��� �ʹ� ������ �ִ� ��� �ĺ������� ���� ��Ŵ
			if (deltaX > MIN_RECT_AREA) {
				break;
			}

			// ���⸦ ���� �� 0���� ������ ���� ����
			if (deltaX == 0.0) {
				deltaX = 1.0;
			}

			// ���� �������� y ��ǥ�� �ִ� �Ÿ� ���� �������� y ��ǥ ������ �Ÿ��� ����
			double deltaY = abs(boundRect2[j].tl().y - boundRect2[i].tl().y);

			// ���⸦ ���� �� 0���� ������ ���� ����
			if (deltaY == 0.0) {
				deltaY = 1.0;
			}

			// �� �������� ���⸦ ����
			double gradient = deltaY / deltaX;

			// ���Ⱑ �̼��ϰ� ���̰� �ִ� ��� ���� ���������� �߰�
			if (gradient < GRADIENT_THRESHOLD) {
				count++;
			}
		}

		// �ִ� ���� ������ ���� ���� ����
		if (count > adjecentCount) {
			startIdx = i;
			adjecentCount = count;
			carLicensePlateWidth = deltaX;
		}
	}

	// ������ ����
	namedWindow(WIN_NAME);

	// �����쿡 ��� ǥ��
	imshow(
		WIN_NAME, 
		image(
			Rect(
				boundRect2[startIdx].tl().x - 20, 
				boundRect2[startIdx].tl().y - 20, 
				carLicensePlateWidth, 
				carLicensePlateWidth * 0.3
			)
		)
	);

	cout << "�ƹ� Ű�� �����ø� �����մϴ�." << endl;

	waitKey();

	return 0;
}
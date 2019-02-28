#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <Windows.h>
#include <iostream>

#define WIN_NAME "자동차 번호판 인식"
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
*	메인 메소드
*/
int main(int argc, char *argv[]) {
	// 이미지 로드
	Mat image = imread(IMAGE_PATH);

	// 그레이 스케일로 변환
	Mat proccessedImage;
	cvtColor(image, proccessedImage, CV_BGR2GRAY);

	// canny 알고리즘을 이용하여 윤곽선만 남김
	Canny(proccessedImage, proccessedImage, CANNY_THRESHOLD_1, CANNY_THRESHOLD_2);

	// 윤곽선을 가져옴
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

	// 번호판 후보군 갯수 저장 변수
	int refineryCount = 0;

	// 윤곽선 중 번호판 후보가 될 수 있는 것만 남김 (가로, 세로 비율 및 크기로 판단)
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

	// 추려낸 번호판 후보군 윤곽선을 x 좌표 기준으로 정렬
	for (int i = 0; i < boundRect2.size(); i++) {
		for (int j = 0; j < boundRect2.size() - 1; j++) {
			if (boundRect2[j].tl().x > boundRect2[j + 1].tl().x) {
				Rect tmpRect = boundRect2[j];
				boundRect2[j] = boundRect2[j + 1];
				boundRect2[j + 1] = tmpRect;
			}
		}
	}

	// 번호판 윤곽선 객체의 시작 인덱스
	int startIdx = 0;

	// 인접 윤곽선 갯수 저장 변수
	int adjecentCount = 0;

	// 번호판 가로 길이 저장 변수
	double carLicensePlateWidth = 0;

	for (int i = 0; i < boundRect2.size(); i++) {
		int count = 0;
		double deltaX = 0.0;

		for (int j = i + 1; j < boundRect2.size(); j++) {
			// 현재 윤곽선의 x 좌표와 최단 거리 인접 윤곽선의 x 좌표 사이의 거리를 구함
			deltaX = abs(boundRect2[j].tl().x - boundRect2[i].tl().x);

			// 거리가 너무 떨어져 있는 경우 후보군에서 제외 시킴
			if (deltaX > MIN_RECT_AREA) {
				break;
			}

			// 기울기를 구할 때 0으로 나누는 예외 방지
			if (deltaX == 0.0) {
				deltaX = 1.0;
			}

			// 현재 윤곽선의 y 좌표와 최단 거리 인접 윤곽선의 y 좌표 사이의 거리를 구함
			double deltaY = abs(boundRect2[j].tl().y - boundRect2[i].tl().y);

			// 기울기를 구할 때 0으로 나누는 예외 방지
			if (deltaY == 0.0) {
				deltaY = 1.0;
			}

			// 두 윤곽선의 기울기를 구함
			double gradient = deltaY / deltaX;

			// 기울기가 미세하게 차이가 있는 경우 인접 윤곽선으로 추가
			if (gradient < GRADIENT_THRESHOLD) {
				count++;
			}
		}

		// 최대 인접 윤곽선 갯수 상태 저장
		if (count > adjecentCount) {
			startIdx = i;
			adjecentCount = count;
			carLicensePlateWidth = deltaX;
		}
	}

	// 윈도우 생성
	namedWindow(WIN_NAME);

	// 윈도우에 결과 표시
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

	cout << "아무 키나 누르시면 종료합니다." << endl;

	waitKey();

	return 0;
}
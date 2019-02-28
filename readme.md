이번 프로젝트는 차량 번호판 사진을 이용하여 차량 번호판만을 추출하는 프로그램입니다.
C++를 기반으로 만들게 되었으며 OpenCV를 이용하여 개발하였습니다.

[준비단계]
1. OpenCV 다운
Web-Site: https://sourceforge.net/projects/opencvlibrary/
2. Visual Studio 2017을 다운
3. Visual Studio에 Opencv 헤더파일 추가
※ 헤더파일 추가하는 자세한 설명은 WORD 파일 참조
4. OPENCV 헤더파일을 include "opencv/cv.h" , "opencv/highgui.h" 로 추가

[작성]
1. opencv의 image_read인 imread 를 사용하여 이미지를 불러온다
2. image를 흑백처리를 시작하여 vector로 행렬 처리를 한다
3. 흑백처리 된 이미지의 윤곽선을 추출하여 vector로 행렬 처리를 한다
4. 윤곽처리 된 이미지와 흑백처리 된 이미지를 가져와 사각형 처리 된 이미지를 추출한다
※ 이해하기 쉽게 참조하면 좋은 지식: 히스토그램, 컨벌루션, 공분산 행렬, DoG 필터링, 가우시안 블러 등등..

5. 추출 된 사각형 후보들 중 챠랑 윤곽선 안의 사각형 이외의 후보들은 제외시킨다
6. 추출 된 하나의 사각형의 영역의 원 이미지를 추출한 뒤 새로 imagewrite인 imwrite를 통하여 새로 이미지를 만들어 저장한다.

감사합니다.

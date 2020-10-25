#include <iostream>
#include <opencv.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <opencv2\core\types_c.h>
#include <opencv2\core\core_c.h>
#include <opencv2\imgproc\imgproc_c.h>
#include <opencv2\highgui\highgui_c.h>


using namespace std;
using namespace cv;



//ȫ�ֺ�������
//Mat src, 
Mat dst;
int g_nTrackbarNmuer = 0;//0��ʾ��ʴ��1��ʾ����  
int g_nStructElementSize = 4;//�ں˾���ĳߴ�      
Mat imreconstruct(Mat marker, Mat mask);
Mat diskStrel(int radius);
CvPoint centerPoint;
//Mat RegionGrow(Mat srcImage, Point pt);
//void on_mouse(int event, int x, int y, int flags, void* prarm);
void mouseHandler(int event, int x, int y, int flags, void* param);


void cv::erode(
    InputArray src,
    OutputArray dst,
    InputArray kernel,
    Point anchor,
    int iterations,
    int borderType,
    const Scalar& borderValue
);


void cv::dilate(
    InputArray src,
    OutputArray dst,
    InputArray kernel,
    Point anchor,
    int iterations,
    int borderType,
    const Scalar& borderValue
);

Mat diskStrel(int radius)
{
    Mat sel(2 * radius - 1, 2 * radius - 1, CV_8UC1, Scalar(1));
    /* the same as MATLAB function 'strel('disk',radius)' */
    int borderWidth = 0;
    switch (radius)
    {
    case 1: borderWidth = 0; break;
    case 3: borderWidth = 0; break;
    case 5: borderWidth = 2; break;
    case 7: borderWidth = 2; break;
    case 9: borderWidth = 4; break;
    case 11: borderWidth = 6; break;
    case 13: borderWidth = 6; break;
    case 15: borderWidth = 8; break;
    case 17: borderWidth = 8; break;
    case 19: borderWidth = 10; break;
    case 21: borderWidth = 10; break;
    default: borderWidth = 2; break;
    }

    for (int i = 0; i < borderWidth; i++) {
        for (int j = 0; j < borderWidth - i; j++) {
            sel.at<uchar>(i, j) = 0;
            sel.at<uchar>(i, sel.cols - 1 - j) = 0;
            sel.at<uchar>(sel.rows - 1 - i, j) = 0;
            sel.at<uchar>(sel.rows - 1 - i, sel.cols - 1 - j) = 0;
        }
    }

    return sel;
}


Mat imreconstruct(Mat marker, Mat mask)
{
    /*the same as MATLAB function imreconstruct*/
    Mat dst;
    marker.copyTo(dst);

    dilate(dst, dst, Mat());
    cv::min(dst, mask, dst);
    Mat temp1 = Mat(marker.size(), CV_32FC1);
    Mat temp2 = Mat(marker.size(), CV_32FC1);
    do
    {
        dst.copyTo(temp1);
        dilate(dst, dst, Mat());
        cv::min(dst, mask, dst);
        compare(temp1, dst, temp2, CV_CMP_NE);
    } while (sum(temp2).val[0] != 0);
    return dst;
}



void mouseHandler(int event, int x, int y, int flags, void* param)
{
    IplImage* img0, * img1;
    img0 = (IplImage*)param;
    img1 = cvCreateImage(cvSize(img0->width, img0->height), img0->depth, img0->nChannels);
    cvCopy(img0, img1, NULL);

    CvFont font;
    uchar* ptr;
    char label[20];
    char label2[20];

    cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1, 1);    //��ʼ������

    if (event == CV_EVENT_LBUTTONDOWN)
    {
        //��ȡ����
        ptr = cvPtr2D(img0, y, x, NULL);

        sprintf_s(label, "Grey Level:%d", ptr[0]);
        sprintf_s(label2, "Pixel: (%d, %d)", x, y);
        //������ʾλ��
        if (img0->width - x <= 180 || img0->height - y <= 20)
        {
            cvRectangle(img1, cvPoint(x - 180, y - 40), cvPoint(x - 10, y - 10), CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
            cvPutText(img1, label, cvPoint(x - 180, y - 30), &font, CV_RGB(255, 255, 255));
            cvPutText(img1, label2, cvPoint(x - 180, y - 10), &font, CV_RGB(255, 255, 255));
        }
        else
        {
            cvRectangle(img1, cvPoint(x + 10, y - 12), cvPoint(x + 180, y + 20), CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
            cvPutText(img1, label, cvPoint(x + 10, y), &font, CV_RGB(255, 255, 255));
            cvPutText(img1, label2, cvPoint(x + 10, y + 20), &font, CV_RGB(255, 255, 255));
        }
        //�����Ϊ���Ļ���

        centerPoint.x = x;
        centerPoint.y = y;
        cvCircle(img1, centerPoint, 3, CV_RGB(0, 0, 0), 1, 8, 3);


        cvShowImage("img", img1);
    }
}



int main(int argc, char** argv)
{

    Mat src = imread("1.jpg");

    if (src.empty())
    {
        std::cout << "Couldn't open file." << std::endl;
        system("pause");
        return -1;
    }
    int g_nStrutElement = 3;  //�ṹԪ�أ��ں˾��󣩵ĳߴ�
    Mat Iobr, Iobrd, J, pt;
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * g_nStructElementSize + 1, 2 * g_nStructElementSize + 1),
        Point(g_nStructElementSize, g_nStructElementSize));

    erode(src, dst, element);
    Iobr = imreconstruct(dst, src); //imshow("Image", dst);//�������ع�

    dilate(Iobr, Iobrd, element);
    Iobrd = ~Iobrd;
    Iobr = ~Iobr;
    cv::Mat Iobrcbr = imreconstruct(Iobrd, Iobr);
    dst = ~Iobrcbr;//�����ع��Ŀ�������
    //dst.convertTo(dst, CV_32F);
    //cv::normalize(dst, dst, 1.0, 0.0, cv::NORM_MINMAX);
    //dst.convertTo(dst, CV_32F);
    //Mat dst1 = Mat::zeros(dst.size(), CV_32FC1);
    //normalize(dst, dst1, 1.0, 0, NORM_MINMAX);
    //Mat result = dst1 * 255;
    //result.convertTo(dst1, CV_8UC1);
    //imshow("2--NORM_MINMAX", dst1);
    //imshow("IMAGE",dst);
    //waitKey();



    int exit = 0;
    int c;
    IplImage* img_dst = (IplImage*)&IplImage(dst);
    assert(img_dst);
    cvNamedWindow("img_dst", 1);
    cvSetMouseCallback("img_dst", mouseHandler, (void*)img_dst);
    cvShowImage("img", img_dst);
    cvWaitKey();
    while (!exit)
    {
        c = cvWaitKey(0);
        switch (c)
        {
        case 'q':
            exit = 1;
            break;
        default:
            continue;
        }
    }


    int x, y;
    x = centerPoint.x;
    y = centerPoint.y;
    //namedWindow("dst", CV_WINDOW_NORMAL);
    //ѡ�����ӵ�Ĵ���
   //cvSetMouseCallback("dst", on_mouse, 0);
   // cvShowImage("src", dst);
    //WaitKey();
   // cv::imshow("pic", dst);

    //J = RegionGrow(dst, x,y);

    Mat growImage = Mat::zeros(dst.size(), CV_8UC1);   //����һ���հ��������Ϊ��ɫ
   //double reg_mean = 0; //��ʾ�ָ�õ������ڵ�ƽ��ֵ����ʼ��Ϊ���ӵ�ĻҶ�ֵ
    int reg_size = 1;//�ָ�ĵ������򣬳�ʼ��ֻ�����ӵ�һ��
    int neg_pos = 0; //���ڼ�¼neg_list�еĴ����������ص�ĸ���
    double pixdist = 0;//��¼�������ص����ӵ��ָ������ľ�����,��һ�δ��������ĸ��������ص�͵�ǰ���ӵ�ľ���.�����ǰ����Ϊ��x, y����ôͨ��neigb���ǿ��Եõ����ĸ��������ص�λ��
    double dist, dist_min;
    int index;
    Point pToGrowing;                       //��������λ��
    int neg_free = 90000;// ��̬�����ڴ��ʱ��ÿ������������ռ��С
    Mat neg_list = Mat::zeros(neg_free, 3, CV_8UC1);



    uchar* ptr = dst.ptr<uchar>(y, x);
    double reg_mean = 0;

    reg_mean = (int)*(ptr);//��ʾ�ָ�õ������ڵ�ƽ��ֵ����ʼ��Ϊ���ӵ�ĻҶ�ֵ
    //reg_mean = dst.at<uchar>(y, x);
    int DIR[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} };//�ĸ�����λ��
    //index = neg_list.at<uchar>(neg_pos, 1);
    int width = dst.cols;
    int height = dst.rows;
    int numel = width * height;
    while (pixdist < 25 && reg_size < numel)
    {
        for (int j = 0; j < 4; j++)
        {
            pToGrowing.x = x + DIR[j][0];
            pToGrowing.y = y + DIR[j][1];
            int ins = (pToGrowing.x >= 0) && (pToGrowing.y >= 0) && (pToGrowing.x <= (dst.rows - 1)) && (pToGrowing.y <= (dst.cols - 1));
            if (ins && growImage.at<uchar>(pToGrowing.x, pToGrowing.y) == 0)
            {
                neg_pos = neg_pos + 1;
                neg_list.at<uchar>(neg_pos - 1, 0) = pToGrowing.x;
                neg_list.at<uchar>(neg_pos - 1, 1) = pToGrowing.y;
                neg_list.at<uchar>(neg_pos - 1, 2) = (int)*(dst.ptr<uchar>(pToGrowing.y, pToGrowing.x));// �洢��Ӧ��ĻҶ�ֵ
                growImage.at<uchar>(pToGrowing.x, pToGrowing.y) = 1;
            }
        }

        int z = neg_list.at<uchar>(0, 2);



        if (neg_pos + 10 > neg_free)
        {
            neg_free = neg_free + 100000;
            for (int i = neg_pos + 1; i <= neg_free; i++)
            {
                for (int j = 0; j < 3; j++)
                    neg_list.at<uchar>(i, j) = 0;
            }
        }
        //�����д����������ص���ѡ��һ�����ص㣬�õ�ĻҶ�ֵ���Ѿ��ָ������ҶȾ�ֵ��
        //��ľ���ֵʱ����������������С��
        dist_min = abs((int)*(neg_list.ptr<uchar>(neg_pos, 2)) - reg_mean);
        for (int j = 0; j < neg_pos; ++j)
        {
            dist = abs((int)*(neg_list.ptr<uchar>(neg_pos, 2)) - reg_mean);
            if (dist < dist_min)
            {
                index = neg_pos;
                dist_min = dist;
            }
            else
                continue;
        }
        reg_size = reg_size + 1;
        reg_mean = (reg_mean * reg_size + neg_list.at<uchar>(index, 3)) / (reg_size);

        growImage.at<uchar>(x, y) = 2;

        x = neg_list.at<uchar>(index, 0);
        y = neg_list.at<uchar>(index, 1);

        neg_list.at<uchar>(index, 0) = neg_list.at<uchar>(neg_pos, 0);
        neg_list.at<uchar>(index, 1) = neg_list.at<uchar>(neg_pos, 1);
        neg_list.at<uchar>(index, 2) = neg_list.at<uchar>(neg_pos, 2);
        neg_pos = neg_pos - 1;
    }


    J = (J == 2);
    imshow("Image", J);






    waitKey();








    // J = RegionGrow(src, pt);
     //
     //imshow("Image", J);
     //waitKey();
}





#include <opencv/cv.h>  
#include <opencv/highgui.h>   
#include<opencv/cxcore.h>
#include<iostream>
#include <fstream>
#include "math.h"
#define pi 3.1415926

using namespace std;

float max(float x, float y);
void prewitt(IplImage *src, IplImage *dst);//Prewitt����  
void roberts(IplImage *src, IplImage *dst); //roberts����  
void sobel(IplImage *src, IplImage *dst);  //sobel����  
void canny(IplImage *src, IplImage *dst);//canny����  
void Laplacian(IplImage *src, IplImage *dst32, IplImage *dst); //Laplacian����

typedef struct
{
	double x;
	double y;
}P;
int getmax(int*array, int n)
{
	int i;
	int max = 0;
	for (i = 0; i<n; i++)
	{
		if (array[i]>array[max])
		{
			max = i;
		}
	}
	return max;
}

int main()
{
	//����ͼ��  
	IplImage *src = NULL;
	src = cvLoadImage("081_0062.jpg", 0);
	cvNamedWindow("��ԭͼ��", 1);
	cvShowImage("��ԭͼ��", src);

	IplImage *pCannyImg = cvCreateImage(cvGetSize(src), 8, 1);
	canny(src, pCannyImg);

	IplImage *pLaplaceImg_32 = cvCreateImage(cvGetSize(src), 32, 1);
	IplImage *pLaplaceImg = cvCreateImage(cvGetSize(src), 8, 1);
	Laplacian(src, pLaplaceImg_32, pLaplaceImg);

	IplImage *pSobelImg = cvCreateImage(cvGetSize(src), 8, 1);
	sobel(src, pSobelImg);
	cvReleaseImage(&pSobelImg);

	IplImage *pRobertsImg = cvCreateImage(cvGetSize(src), 8, 1);
	roberts(src, pRobertsImg);
	cvReleaseImage(&pRobertsImg);

	IplImage *pPrewittImg = cvCreateImage(cvGetSize(src), 8, 1);
	prewitt(src, pPrewittImg);
	cvReleaseImage(&pPrewittImg);

	cvWaitKey(0);
	//���ٴ���,�ͷ�ͼ��  
	cvDestroyWindow("��ԭͼ��");
	cvReleaseImage(&src);
	return -1;
}
float max(float x, float y)
{
	float z;
	if (x>y)z = x;
	else z = y;
	return(z);
}
//Prewitt����  
void prewitt(IplImage *src, IplImage *dst)
{
	//����prewitt���ӵ�ģ��  
	float prewittx[9] =
	{
		-1, 0, 1,
		-1, 0, 1,
		-1, 0, 1
	};
	float prewitty[9] =
	{
		1, 1, 1,
		0, 0, 0,
		-1, -1, -1
	};
	CvMat px;
	px = cvMat(3, 3, CV_32F, prewittx);
	CvMat py;
	py = cvMat(3, 3, CV_32F, prewitty);

	IplImage *dstx = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage *dsty = cvCreateImage(cvGetSize(src), 8, 1);

	//��ͼ��ʹ��ģ�壬�Զ����߽�  
	cvFilter2D(src, dstx, &px, cvPoint(-1, -1));
	cvFilter2D(src, dsty, &py, cvPoint(-1, -1));

	//�����ݶȣ�  
	int i, j, temp;
	float tempx, tempy;  //����Ϊ��������Ϊ�˱���sqrt������������  
	uchar* ptrx = (uchar*)dstx->imageData;
	uchar* ptry = (uchar*)dsty->imageData;
	for (i = 0; i<src->width; i++)
	{
		for (j = 0; j<src->height; j++)
		{
			tempx = ptrx[i + j*dstx->widthStep];   //tempx,tempy��ʾ����ָ����ָ�������  
			tempy = ptry[i + j*dsty->widthStep];
			temp = (int)sqrt(tempx*tempx + tempy*tempy);
			dst->imageData[i + j*dstx->widthStep] = temp;
		}
	}
	double min_val = 0, max_val = 0;//ȡͼ����ʾ���е������С����ֵ  
	cvMinMaxLoc(dst, &min_val, &max_val);
	printf("max_val = %f\nmin_val = %f\n", max_val, min_val);

	cvSaveImage("PrewittImg.jpg", dst);//��ͼ������ļ�  
	cvReleaseImage(&dstx);
	cvReleaseImage(&dsty);
	cvNamedWindow("��Ч��ͼ��prewitt����", 1);
	cvShowImage("��Ч��ͼ��prewitt����", dst);
}

//roberts����  
void roberts(IplImage *src, IplImage *dst)
{
	dst = cvCloneImage(src);
	int x, y, i, w, h;
	int temp, temp1;

	uchar* ptr = (uchar*)(dst->imageData);
	int ptr1[4] = { 0 };
	int indexx[4] = { 0, 1, 1, 0 };
	int indexy[4] = { 0, 0, 1, 1 };
	w = dst->width;
	h = dst->height;
	for (y = 0; y<h - 1; y++)
		for (x = 0; x<w - 1; x++)
		{

			for (i = 0; i<4; i++)    //ȡÿ��2*2����Ԫ�ص�ָ��      0 | 1  
			{                   //                             3 | 2  
				ptr1[i] = *(ptr + (y + indexy[i])*dst->widthStep + x + indexx[i]);

			}
			temp = abs(ptr1[0] - ptr1[2]);    //����2*2������0��2λ�õĲȡ����ֵtemp  
			temp1 = abs(ptr1[1] - ptr1[3]);   //����2*2������1��3λ�õĲȡ����ֵtemp1  
			temp = (temp>temp1 ? temp : temp1); //��temp1>temp,����temp1��ֵ�滻temp  
			temp = (int)sqrt(float(temp*temp) + float(temp1*temp1));  //���ֵ  
			*(ptr + y*dst->widthStep + x) = temp;       //�����ֵ�����dst���صĶ�Ӧλ��  
		}
	double min_val = 0, max_val = 0;//ȡͼ����ʾ���е������С����ֵ  
	cvMinMaxLoc(dst, &min_val, &max_val);
	printf("max_val = %f\nmin_val = %f\n", max_val, min_val);
	cvSaveImage("RobertsImg.jpg", dst);
	cvNamedWindow("��Ч��ͼ��robert����", 1);
	cvShowImage("��Ч��ͼ��robert����", dst);
}
//sobel����  
void sobel(IplImage *src, IplImage *dst)
{
	IplImage *pSobelImg_dx = cvCreateImage(cvGetSize(src), 32, 1);
	IplImage *pSobelImg_dy = cvCreateImage(cvGetSize(src), 32, 1);
	IplImage *pSobelImg_dxdy = cvCreateImage(cvGetSize(src), 32, 1);

	//��sobel���Ӽ������������΢��  
	cvSobel(src, pSobelImg_dx, 1, 0, 3);
	cvSobel(src, pSobelImg_dy, 0, 1, 3);

	FILE * pFile = fopen("gt_081_0062.jpg.txt", "r");
	if (pFile == NULL) return;
	double v1, v2, v;
	int n = 30;                          //the number of vertexes
	int i;
	int j = 0;
	double ivl;                          //interval angel
	double alpha = 0;                    //alpha
	
	double a = 107.462600;
	double b = 42.238400;
	double x = 163.968900;
	double y = 224.070400;
/*
	double a, b, x, y;
	
	fscanf(pFile, "%f", &a);			//read data from text file
	fscanf(pFile, "%f", &b);
	fscanf(pFile, "%f", &x);
	fscanf(pFile, "%f", &y)

	ifstream pfile("gt_081_0062.jpg.txt");
	pfile >> a >> b >> x >> y;
	cout << "******" << endl;
	cout << a << endl;
	cout << b << endl;
	cout << x << endl;
	cout << y << endl;;*/

	double tempx = 0;
	double tempy = 0;
	double theta = (180-166.777280)*pi / 180; //��ʱ��
	double radius = 0;                   //for rotating
	double temp = 0;                     //compare theta and alpha
	double start = 0;                    //the start of the bow
	double end = 2 * pi;                   //the end of the bow
	int unqualified = 0;

	P vertex[30];                         //vertex
	int index[30];                        //record the indexes of unqualified vertex
	int index_dis[30];                    //record the distance of the unqualified vertex's index
	double alpha_vs[30];                  //the alpha of each vertex
	int temp1;                           //save the interval index
	int start_index;                     //record the start index then to find the start alpha
	int end_index;                       //record the end index then to find the end alpha
	int flag[30];
										 /* rotated uniform choosing vertex */
	do
	{
		ivl = (end - start) / n;
		for (i = 0; i<n; i++)
		{
			alpha = ivl*i;
			alpha_vs[i] = alpha + start;
			vertex[i].x = a*cos(alpha + start)*cos(theta) + b*sin(alpha + start)*sin(theta) + x;
			vertex[i].y = y - (a*cos(alpha + start)*sin(theta) + b*sin(alpha + start)*cos(theta));
		}
		for (i = 0; i < n; i++)
		{
			printf("Point(%f,%f),", vertex[i].x, vertex[i].y);
		}
		
		for (i = 0; i < n; i++)
		{
			v1 = cvGetReal2D(pSobelImg_dx, vertex[i].y, vertex[i].x);
			v2 = cvGetReal2D(pSobelImg_dy, vertex[i].y, vertex[i].x);
			v = sqrt(v1*v1 + v2*v2);
			if (v > 100)
				flag[i] = 1;   //��ֵ
			else {
				flag[i] = 0;
			}
			//cvSetReal2D(pSobelImg_dxdy, vertex[i].y, vertex[i].x, v);
		}
		for (i = 0; i < n; i++)
		{
			if (flag[i] == 0)
			{
				unqualified++;
				index[j] = i;
				j++;
			}
		}
		for (i = 1; i<unqualified; i++)
		{
			index_dis[i-1] = index[i] - index[i - 1];
		}
		temp1 = getmax(index_dis, unqualified - 1);
		start_index = index[temp1];
		end_index = index[temp1 + 1];
		start = alpha_vs[start_index];
		end = alpha_vs[end_index];
		for (int i = 0; i < n; i++)
			cout << flag[i] << endl;
		n = n / 2;
		printf("-----");
		
	} while (unqualified != 0 && n >= 6);
	/*
	for (i = 0; i<src->height; i++)
	{
		for (j = 0; j<src->width; j++)
		{
			v1 = cvGetReal2D(pSobelImg_dx, i, j);
			v2 = cvGetReal2D(pSobelImg_dy, i, j);
			v = sqrt(v1*v1 + v2*v2);
			// if(v>100) v = 255;
			//else v = 0;
			cvSetReal2D(pSobelImg_dxdy, i, j, v);
		}
	}
	*/
	for (int i = 0; i < 15; i++)
	{
		cvSetReal2D(pSobelImg_dxdy, vertex[i].y, vertex[i].x, 255);
		//cout << "Point(" << vertex[i].x << "," << vertex[i].y << ")," << endl;
	}

	cvConvertScale(pSobelImg_dxdy, dst);   //��ͼ��ת��Ϊ8λ  
	double min_val = 0, max_val = 0;//ȡͼ����ʾ���е������С����ֵ  
	cvMinMaxLoc(pSobelImg_dxdy, &min_val, &max_val);
	printf("max_val = %f\nmin_val = %f\n", max_val, min_val);

	//��һ��  
	cvNormalize(dst, dst, 0, 255, CV_MINMAX, 0);

	cvReleaseImage(&pSobelImg_dx);
	cvReleaseImage(&pSobelImg_dy);
	cvReleaseImage(&pSobelImg_dxdy);
	cvSaveImage("SobelImg.jpg", dst);//��ͼ������ļ�  
	cvNamedWindow("��Ч��ͼ��sobel����", 1);
	cvShowImage("��Ч��ͼ��sobel����", dst);
}
//canny����  
void canny(IplImage *src, IplImage *dst)
{
	cvCanny(src, dst, 100, 150, 3),
		cvNamedWindow("��Ч��ͼ����֣��Խǣ�", 1);
	cvShowImage("��Ч��ͼ����֣��Խǣ�", dst);
	cvSaveImage("CannyImg.jpg", dst);//��ͼ������ļ�  
	cvReleaseImage(&dst);
}
//Laplacian����  
void Laplacian(IplImage *src, IplImage *dst32, IplImage *dst)
{
	double min_val = 0, max_val = 0;//ȡͼ����ʾ���е������С����ֵ  
	cvLaplace(src, dst32, 5);
	cvConvertScale(dst32, dst);   //��ͼ��ת��Ϊ8λ  
	cvMinMaxLoc(dst, &min_val, &max_val);
	printf("max_val = %f\nmin_val = %f\n", max_val, min_val);
	cvNormalize(dst, dst, 0, 255, CV_MINMAX, 0);
	cvNamedWindow("��Ч��ͼ��laplacian����", 1);
	cvShowImage("��Ч��ͼ��laplacian����", dst);
	cvSaveImage("LaplaceImg.jpg", dst);//��ͼ������ļ�  
	cvReleaseImage(&dst);
}
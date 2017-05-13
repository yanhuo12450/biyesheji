// OpenGLShow.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "OpenGLShow.h"
#include "afxdialogex.h"
#include <GL\GL.h>
#include <GL\GLU.h>
#include <GL\glut.h>
#include <opencv2/highgui.hpp>  
#include <opencv2/core.hpp>  
// OpenGLShow 对话框
using namespace cv;
using namespace std;

vector<Point3f> worldpoint;
static float xrot = 0.0;
static float yrot = 0.0;
static float zrot = 0.0;
static int currcoutn = 0;
static int timecount = 0;
vector<Point3d> Triinfo;
float distancexgl(Point3f p);
int pointsisemptygl(int* p, int length);
void DelunayTrigl(vector<Point3f> worldpoint, vector<Point3d> &Triinfo);

IMPLEMENT_DYNAMIC(OpenGLShow, CDialogEx)

OpenGLShow::OpenGLShow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

OpenGLShow::~OpenGLShow()
{
}

void OpenGLShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(OpenGLShow, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void OpenGLShow::OnTimer(UINT nIDEvent) //实时绘制场景 
{
	// TODO: Add your message handler code here and/or call default 
	RenderScene();
	m_yRotate += 3;
	CDialog::OnTimer(nIDEvent);
}

BOOL OpenGLShow::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;

	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;

	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER |
		PFD_TYPE_RGBA;

	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 0;
	pixelDesc.cRedShift = 0;
	pixelDesc.cGreenBits = 0;
	pixelDesc.cGreenShift = 0;
	pixelDesc.cBlueBits = 0;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 0;
	pixelDesc.cAccumRedBits = 0;
	pixelDesc.cAccumGreenBits = 0;
	pixelDesc.cAccumBlueBits = 0;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 0;
	pixelDesc.cStencilBits = 1;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;

	PixelFormat = ChoosePixelFormat(hDC, &pixelDesc);
	if (PixelFormat == 0) // Choose default 
	{
		PixelFormat = 1;
		if (DescribePixelFormat(hDC, PixelFormat,
			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
		{
			return FALSE;
		}
	}

	if (SetPixelFormat(hDC, PixelFormat, &pixelDesc) == FALSE)

	{
		return FALSE;
	}

	return TRUE;
}

BOOL OpenGLShow::CreateViewGLContext(HDC hDC)
{
	hrenderRC = wglCreateContext(hDC);

	if (hrenderRC == NULL)
		return FALSE;

	if (wglMakeCurrent(hDC, hrenderRC) == FALSE)
		return FALSE;



	return TRUE;
}

void OpenGLShow::RenderScene()
{

	vector<Point3d>::iterator itt = Triinfo.begin();
	///////////////////////////////////////////////// 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(0.0f, 1.0f, 0.0f);
	glPointSize(5.0f);
	glLoadIdentity();
	glTranslatef(0.0f, -0.3f, -2.5f);      // Move Left 1.5 Units And Into The Screen 6.0 
	glRotated(m_yRotate, 0.0, 1.0, 0.0);
	glBegin(GL_POINTS);
	vector<Point3f>::iterator ipt = worldpoint.begin();
	float xmax = 0, ymax = 0, zmax = 0, xmin = 10000, ymin = 10000, zmin = 10000;
	float xfactor, yfactor, zfactor;
	for (ipt = worldpoint.begin(); ipt != worldpoint.end(); ipt++) {
		if (ipt->x > xmax)xmax = ipt->x;
		if (ipt->y > ymax)ymax = ipt->y;
		if (ipt->z > zmax)zmax = ipt->z;
		if (ipt->x < xmin)xmin = ipt->x;
		if (ipt->y < ymin)ymin = ipt->y;
		if (ipt->z < zmin)zmin = ipt->z;
	}
	xfactor = (xmax - 0.8*xmin)*1.3;
	yfactor = (ymax - 0.8*ymin)*2;
	zfactor = (zmax - 0.8*zmin)*1.3;

	for (ipt = worldpoint.begin(); ipt != worldpoint.end(); ipt++) {
		glVertex3f((ipt->x-xmin*0.8)/ xfactor, (ipt->y-ymin*0.8)/yfactor, (ipt->z-zmin*0.8) /zfactor);
		//cout << " " << ipt->x << " " << ipt->y << " " << ipt->z<<endl;
	}
	glEnd();
	glBegin(GL_TRIANGLES);
	for (; itt != Triinfo.begin() + currcoutn; itt++) {
		glVertex3f((worldpoint[itt->x].x - xmin*0.8) / xfactor,  (worldpoint[itt->x].y - ymin*0.8) / yfactor, (worldpoint[itt->x].z - zmin*0.8) / zfactor);
		glVertex3f((worldpoint[itt->y].x - xmin*0.8) / xfactor,  (worldpoint[itt->y].y - ymin*0.8) / yfactor, (worldpoint[itt->y].z - zmin*0.8) / zfactor);
		glVertex3f((worldpoint[itt->z].x - xmin*0.8) / xfactor,  (worldpoint[itt->z].y - ymin*0.8) / yfactor, (worldpoint[itt->z].z - zmin*0.8) / zfactor);
		//cout << itt->x << "   " << itt->y << "   " << itt->z << endl;
	}
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();
	timecount++;
	if (currcoutn == Triinfo.size() + 1)currcoutn = 0;
	if (timecount == 20) {
		currcoutn++;
		timecount = 0;
	}
	//glBegin(GL_TRIANGLES); // Drawing Using Triangles 

	//glVertex3f(0.0f, 1.0f, 0.0f);     // Top 
	//glVertex3f(-1.0f, -1.0f, 0.0f);     // Bottom Left 
	//glVertex3f(1.0f, -1.0f, 0.0f);     // Bottom Right 
	//glEnd();           // Finished Drawing The Triangle 
	SwapBuffers(hrenderDC);
}

// OpenGLShow 消息处理程序


BOOL OpenGLShow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CWnd *wnd = GetDlgItem(IDC_RENDER);
	hrenderDC = ::GetDC(wnd->m_hWnd);
	if (SetWindowPixelFormat(hrenderDC) == FALSE)
		return 0;

	if (CreateViewGLContext(hrenderDC) == FALSE)
		return 0;

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	/////////////////////////////////////////// 
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glViewport(0, 0, 259, 231);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glShadeModel(GL_SMOOTH);       // Enable Smooth Shading 
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);    // Black Background 
	glClearDepth(1.0f);         // Depth Buffer Setup 
	glEnable(GL_DEPTH_TEST);       // Enables Depth Testing 
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do 
							///////////////////////////////////////////////////////////////////////// 
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	SetTimer(1, 10, 0);

	////////////////////////////////////////////////////////////////

	FILE *fs;
	fopen_s(&fs,"3Dpoint.txt", "r");
	float X, Y, Z;

	fscanf_s(fs, "%f %f %f", &X, &Y, &Z);
	worldpoint.resize(0);
	Triinfo.resize(0);
	while (!feof(fs)) {
		worldpoint.push_back(Point3f(X, Y, Z));
		fscanf_s(fs, "%f %f %f", &X, &Y, &Z);
	}
	fclose(fs);
	if (worldpoint.size() < 3) {
		return TRUE;
	}
	else {
		DelunayTrigl(worldpoint, Triinfo);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void  DelunayTrigl(vector<Point3f> worldpoint, vector<Point3d>& Triinfo) {
	vector<Point2d> Lineinfo;
	int count = worldpoint.size();
	int minnum = 1000;
	int minnoright = 0;
	int minl = 0;
	int minnoleft = 0;
	for (int j = 0; j < count; j++) {

		for (int i = 1; i < count; i++) {
			float di = distancexgl(worldpoint[j] - worldpoint[i]);
			if (di < minnum) {
				minnum = di;
				minnoright = i;
				minnoleft = j;
			}
		}
	}
	int *points;
	points = new int[count];
	for (int i = 0; i < count; i++) {
		points[i] = 1;
	}
	points[minnoleft] = 0;
	points[minnoright] = 0;
	Lineinfo.push_back(Point2d(minnoleft, minnoright));
	while (pointsisemptygl(points, count)) {
		minnum = 1000;
		minnoright = 0;
		minl = 0;
		for (int i = 0; i < count; i++) {
			if (points[i] == 1) {
				for (size_t l = 0; l < Lineinfo.size(); l++) {
					Point3f linezhong = worldpoint[Lineinfo[l].x] + worldpoint[Lineinfo[l].y];
					linezhong.x /= 2;
					linezhong.y /= 2;
					linezhong.z /= 2;
					float di = distancexgl(worldpoint[i] - linezhong);
					if (di < minnum) {
						minnum = di;
						minnoright = i;
						minl = l;
					}

				}
			}
		}
		int p1 = Lineinfo[minl].x;
		int p2 = Lineinfo[minl].y;
		Triinfo.push_back(Point3d(minnoright, p1, p2));
		points[minnoright] = 0;
		std::vector<Point2d>::iterator lin = Lineinfo.begin();
		Lineinfo.erase(lin + minl);
		Lineinfo.push_back(Point2d(minnoright, p1));
		Lineinfo.push_back(Point2d(minnoright, p2));
	}


}
float distancexgl(Point3f p) {
	return sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
}
int pointsisemptygl(int* p, int length) {
	int count = 0;
	for (int i = 0; i < length; i++) {
		count += p[i];
	}
	return count;
}
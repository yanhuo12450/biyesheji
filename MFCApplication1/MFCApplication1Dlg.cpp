
// MFCApplication1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#define WM_MY_MESSAGE (WM_USER+100)
#define WM_3D_MESSAGE (WM_USER+101) 
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

float distancex(Point3f p);
int pointsisempty(int* p, int length);
void DelunayTri(vector<Point3f> worldpoint, vector<Point3d> &Triinfo);
void Tri2Line(vector<Point3d> &tri, vector<Point2d> &line);
void FindMidpoint(vector<Point2d>&line, vector<Point>&lp, vector<Point>&rp, Mat & lgray, Mat &rgray);
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CMFCApplication1Dlg::ReleaseIplImage() {
	cvReleaseImage(&TheImage);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 对话框



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
	, TheImage(NULL)
	, C_left_value(_T(""))
	, C_right_value(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, C_left);
	DDX_Control(pDX, IDC_COMBO2, C_right);
	DDX_CBString(pDX, IDC_COMBO1, C_left_value);
	DDX_CBString(pDX, IDC_COMBO2, C_right_value);
	DDX_Control(pDX, IDC_EDIT1, E_rows);
	DDX_Control(pDX, IDC_EDIT2, E_cols);
	DDX_Control(pDX, IDC_EDIT3, E_lenght);
	DDX_Control(pDX, IDC_EDIT4, E_count);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_MESSAGE(WM_MY_MESSAGE, OnMyMessage)
	ON_MESSAGE(WM_3D_MESSAGE,On3DMessage)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication1Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication1Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCApplication1Dlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFCApplication1Dlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMFCApplication1Dlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CMFCApplication1Dlg::OnBnClickedButton7)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMFCApplication1Dlg 消息处理程序

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CMFCApplication1App *app = (CMFCApplication1App*)AfxGetApp();
	//app->m_exitThreadEvent = CreateEvent(NULL, FALSE,FALSE,NULL);//初始化线程关闭信号
	app->cameraswitch = false;
	app->WorkMode = 0; //初始化工作模式
	CvSize ImgSize; //初始化显示的图片
	ImgSize.height = IMAGE_HEIGHT;
	ImgSize.width = IMAGE_WIDTH;
	TheImage = cvCreateImage(ImgSize, IPL_DEPTH_8U, IMAGE_CHANNELS);
	TheImage_right = cvCreateImage(ImgSize, IPL_DEPTH_8U, IMAGE_CHANNELS);
	ShowImage(TheImage, IDC_ShowImg);    // 重绘图片函数
	ShowImage(TheImage_right, IDC_ShowImg_rigth);

	broad.width = 7;
	broad.height = 9;//初始化标定参数
	currcount = 0;
	piccount = 20;
	broadlenght = 0;
	

	vector<int> capnum;//初始化摄像机选择
	if (EnumerateCameras(capnum)) {
		for (size_t i = 0; i < capnum.size(); i++) {
			char temp[60];
			snprintf(temp, 60, "%d", capnum[i]);
			C_left.AddString((LPCTSTR)temp);
			C_right.AddString((LPCTSTR)temp);
		}
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();                    // 重绘对话框
		CDialog::UpdateWindow();                // 更新windows窗口，如果无这步调用，图片显示还会出现问题
		ShowImage(TheImage, IDC_ShowImg);    // 重绘图片函数
		ShowImage(TheImage_right, IDC_ShowImg_rigth);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//LRESULT CMFCApplication1Dlg::On3DMessage(WPARAM wParam, LPARAM lParam) {
//	CvMat *worldpoint = (CvMat *)lParam;
//	FILE *fs;
//	fopen_s(&fs,"3Dpoint.txt", "w");
//	for (int i = 0; i < worldpoint->rows; i++) {
//		fprintf(fs, "%f %f %f\n", CV_MAT_ELEM(*worldpoint,float,i,0), CV_MAT_ELEM(*worldpoint, float, i, 1), CV_MAT_ELEM(*worldpoint, float, i, 2));
//	}
//	fclose(fs);
//	CString str;
//	str.Format(_T("刚才的图片上有\n排数 = %d\n列数 = %d\n个特征点"), worldpoint->rows,worldpoint->cols);
//	AfxMessageBox(str);
//	return 0;
//}
LRESULT CMFCApplication1Dlg::On3DMessage(WPARAM wParam, LPARAM lParam) {
	IplImage * framel, *framer;
	framel = (IplImage *)wParam;
	framer = (IplImage *)lParam;
	Mat rimg = Mat(framel);
	Mat cimg = Mat(framer);


	CMFCApplication1App *app = (CMFCApplication1App*)AfxGetApp();
	vector<KeyPoint> keypoints1, keypoints2;
	keypoints1.resize(0);
	keypoints2.resize(0);
	keypoints1.resize(500);
	keypoints2.resize(500);
	SurfFeatureDetector detector(1000);
	detector.detect(framel, keypoints1);
	detector.detect(framer, keypoints2);
	SurfDescriptorExtractor surfDesc;
	Mat descriptros1, descriptros2;
	surfDesc.compute(framel, keypoints1, descriptros1);
	surfDesc.compute(framer, keypoints2, descriptros2);
	BruteForceMatcher<L2<float>>matcher;
	vector<DMatch>matches;
	matcher.match(descriptros1, descriptros2, matches);		
	std::nth_element(matches.begin(), matches.begin() + 40, matches.end());
	matches.erase(matches.begin() + 41, matches.end());

	vector<Point> l_points, r_points;
	vector<DMatch>::iterator match = matches.begin();
	vector<Point3f> worldpoint;
	vector<Point3d> Triinfo;


	//FILE *fs;
	//fopen_s(&fs,"3Dpoint.txt", "wt");
	cvtColor(rimg, rimg, CV_BGR2GRAY);
	cvtColor(cimg, cimg, CV_BGR2GRAY);
	for (;match != matches.end(); match++) {
		if ((abs((int)keypoints1[match->queryIdx].pt.y - (int)keypoints2[match->trainIdx].pt.y) <= 2)) {
			l_points.push_back(keypoints1[match->queryIdx].pt);
			r_points.push_back(keypoints2[match->trainIdx].pt);
			double d = keypoints1[match->queryIdx].pt.x - keypoints2[match->trainIdx].pt.x;
			double Z = app->T*app->f / d;
			double X = (keypoints1[match->queryIdx].pt.x + app->cx)*Z / app->f;
			double Y = -(keypoints1[match->queryIdx].pt.y + app->cy)*Z / app->f;
			worldpoint.push_back(Point3f(X, Y, Z));
			//fprintf(fs, "%f %f %f\n", X, Y, Z);
			//worldpoint.push_back(Point3d(X, Y, Z));
		}
	}
	DelunayTri(worldpoint, Triinfo);
	vector<Point2d> Lineinfo;
	Tri2Line(Triinfo, Lineinfo);

	FindMidpoint(Lineinfo, l_points, r_points, rimg, cimg);
	vector<Point>::iterator lpointit = l_points.begin();
	vector<Point>::iterator rpointit = r_points.begin();
	worldpoint.resize(0);
	FILE *fs;
	fopen_s(&fs,"3Dpoint.txt", "wt");
	for (;lpointit != l_points.end();lpointit++) {
		double d = lpointit->x - rpointit->x;
		float Z = app->T*app->f / d;
		float X = -(lpointit->x + app->cx)*Z / app->f;
		float Y = -(lpointit->y + app->cy)*Z / app->f;
		worldpoint.push_back(Point3f(X, Y, Z));
		rpointit++;
		fprintf(fs, "%f %f %f\n", X, Y, Z);
	}
	fclose(fs);
	//fclose(fs);
	Triinfo.resize(0);
	DelunayTri(worldpoint, Triinfo);
	for (size_t i = 0; i < l_points.size(); i++) {
		circle(rimg, Point(l_points[i].x, l_points[i].y), 2, Scalar(255, 255, 255));
	}
	for (size_t i = 0; i < r_points.size(); i++) {
		circle(cimg, Point(r_points[i].x, r_points[i].y), 2, Scalar(255, 255, 255));
	}
	for (vector<Point3d>::iterator it = Triinfo.begin(); it != Triinfo.end(); it++) {
		line(rimg, l_points[it->x], l_points[it->y], Scalar(255, 255, 255));
		line(rimg, l_points[it->y], l_points[it->z], Scalar(255, 255, 255));
		line(rimg, l_points[it->x], l_points[it->z], Scalar(255, 255, 255));
	}
	for (vector<Point3d>::iterator it = Triinfo.begin(); it != Triinfo.end(); it++) {
		line(cimg, r_points[it->x], r_points[it->y], Scalar(255, 255, 255));
		line(cimg, r_points[it->y], r_points[it->z], Scalar(255, 255, 255));
		line(cimg, r_points[it->x], r_points[it->z], Scalar(255, 255, 255));
	}

	OpenGLShow *test = new OpenGLShow;
	test->Create(IDD_DIALOG1, this);
	test->ShowWindow(SW_SHOW);
	imshow("left", rimg);
	imshow("right", cimg);
	imwrite("left.jpg", rimg);
	imwrite("right.jpg", cimg);

	int c = waitKey(0);
	//CvMat * worldP = cvCreateMat(worldpoint.size(), 3, CV_32FC1);
	//int i = 0;
	//vector<Point3d>::iterator it = worldpoint.begin();
	//for (;it != worldpoint.end();it++) {
	//	CV_MAT_ELEM(*worldP, float, i, 0) = (float)it->x;
	//	CV_MAT_ELEM(*worldP, float, i, 1) = (float)it->y;
	//	CV_MAT_ELEM(*worldP, float, i, 2) = (float)it->z;
	//	i++;
	//}
	return 0;
}
LRESULT CMFCApplication1Dlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	IplImage *framel, *framer;
	framel = (IplImage *)wParam;
	framer = (IplImage *)lParam;
	//Mat aaa = cv::Mat(framel);
	//imshow("asdf", aaa);
	//waitKey(0);
	//Mat bb = cv::Mat(framer);
	//imshow("ads", bb);
	//waitKey(0);
	if (TheImage)                                // 对上一幅显示的图片数据清零
		cvZero(TheImage);
	if (TheImage_right)
		cvZero(TheImage_right);
	ResizeImage(framel, &TheImage);
	ResizeImage(framer, &TheImage_right);
	ShowImage(TheImage, IDC_ShowImg);
	ShowImage(TheImage_right, IDC_ShowImg_rigth);

	CMFCApplication1App *app = (CMFCApplication1App*)AfxGetApp();
	if (app->WorkMode == 1) {
		if (toru) {
			if (currcount == 0) {
				if (fs.isOpened()) { fs.release(); }
				fs.open("pic.xml", FileStorage::WRITE);
				fs << "strings";
				fs << "[";
				char *leftpath = new char[50];
				snprintf(leftpath, 50, "left%03d.jpg", currcount);
				char *rightpath = new char[50];
				snprintf(rightpath, 50, "right%03d.jpg", currcount);
				cvSaveImage(leftpath, framel);
				cvSaveImage(rightpath, framer);
				fs << leftpath;
				fs << rightpath;
				currcount++;
			}
			else if(currcount!=piccount){
				char *leftpath = new char[50];
				snprintf(leftpath, 50, "left%03d.jpg", currcount);
				char *rightpath = new char[50];
				snprintf(rightpath, 50, "right%03d.jpg", currcount);
				cvSaveImage(leftpath,framel);
				cvSaveImage(rightpath, framer);
				fs << leftpath;
				fs << rightpath;
				currcount++;
			}
			else if (currcount == piccount) {
				fs << "]";
				fs.release();
				currcount++;
				AfxMessageBox(_T("拍照完毕，开始标定"));
				vector<string> imagelist;
				char* path = "pic.xml";
				double rms;
				double err;
				readStringListx(path,imagelist);
				try {
					StereoCalibx(imagelist, broad, broadlenght, rms, err);
				}
				catch (Exception e) {
					;
				}
				CString str;
				str.Format(_T("标定结束\nrms = %lf\nerr = %lf"), rms, err);
				AfxMessageBox(str);
				app->WorkMode = 2;
			}
			toru = false;
		}
	}



	return 0;
}


void CMFCApplication1Dlg::OnBnClickedButton1()
{
	//CFileDialog dlg(
	//	TRUE, _T("*.bmp;*.jpg;*.jpeg;*.png"), NULL,
	//	OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
	//	_T("image files (*.bmp; *.jpg;*.jpeg;*.png) |*.bmp; *.jpeg;*.jpg;*.png | All Files (*.*) |*.*||"), NULL
	//);                                        // 选项图片的约定
	//dlg.m_ofn.lpstrTitle = _T("Open Image");    // 打开文件对话框的标题名
	//if (dlg.DoModal() != IDOK)                    // 判断是否获得图片
	//	return;

	//CString mPath = dlg.GetPathName();            // 获取图片路径
	////CFile file;
	////file.Open("log.log", CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL);
	////file.Write(mPath, mPath.GetLength());
	////file.Close();
	//IplImage* ipl = cvLoadImage(mPath, 1);    // 读取图片、缓存到一个局部变量 ipl 中
	//if (!ipl)                                    // 判断是否成功载入图片
	//	return;
	//if (TheImage)                                // 对上一幅显示的图片数据清零
	//	cvZero(TheImage);

	//ResizeImage(ipl);    // 对读入的图片进行缩放，使其宽或高最大值者刚好等于 256，再复制到 TheImage 中
	//ShowImage(TheImage, IDC_ShowImg);            // 调用显示图片函数    
	//cvReleaseImage(&ipl);                        // 释放 ipl 占用的内存
	CMFCApplication1App *app = (CMFCApplication1App*)AfxGetApp();
	//ResetEvent(app->m_exitThreadEvent);
	
	if (app->cameraswitch == true) {
		AfxMessageBox(_T("已经打开摄像头了"));
		return;
	}

	CString left_value,right_value;
	C_left.GetWindowText(left_value);
	C_right.GetWindowText(right_value);
	int *capnum = new int[2];
	capnum[0]= _ttoi(left_value);
	capnum[1] = _ttoi(right_value);
	if (capnum[0] == capnum[1]) {
		AfxMessageBox(_T("左右摄像机不能是同一摄像机"));
		return;
	}
	m_pThread = AfxBeginThread(ThreadFunc,(LPVOID)capnum); //启动线程
	app->cameraswitch = true;
}


void CMFCApplication1Dlg::ShowImage(IplImage* img, UINT ID)
{
	CDC* pDC = GetDlgItem(ID)->GetDC();        // 获得显示控件的 DC
	HDC hDC = pDC->GetSafeHdc();                // 获取 HDC(设备句柄) 来进行绘图操作

	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	int rw = rect.right - rect.left;            // 求出图片控件的宽和高
	int rh = rect.bottom - rect.top;
	int iw = img->width;                        // 读取图片的宽和高
	int ih = img->height;
	int tx = (int)(rw - iw) / 2;                    // 使图片的显示位置正好在控件的正中
	int ty = (int)(rh - ih) / 2;
	SetRect(rect, tx, ty, tx + iw, ty + ih);


	CvvImage cimg;
	cimg.CopyOf(img);                            // 复制图片
	cimg.DrawToHDC(hDC, &rect);                // 将图片绘制到显示控件的指定区域内

	ReleaseDC(pDC);
}


void CMFCApplication1Dlg::ResizeImage(IplImage* img,IplImage **theimage)
{
	// 读取图片的宽和高
	int w = img->width;
	int h = img->height;

	// 找出宽和高中的较大值者
	int max = (w > h) ? w : h;

	// 计算将图片缩放到TheImage区域所需的比例因子
	float scale = (float)((float)max / 256.0f);

	// 缩放后图片的宽和高
	int nw = (int)(w / scale);
	int nh = (int)(h / scale);

	// 为了将缩放后的图片存入 TheImage 的正中部位，需计算图片在 TheImage 左上角的期望坐标值
	int tlx = (nw > nh) ? 0 : (int)(256 - nw) / 2;
	int tly = (nw > nh) ? (int)(256 - nh) / 2 : 0;

	// 设置 TheImage 的 ROI 区域，用来存入图片 img
	cvSetImageROI(*theimage, cvRect(tlx, tly, nw, nh));

	// 对图片 img 进行缩放，并存入到 TheImage 中
	cvResize(img, *theimage);

	// 重置 TheImage 的 ROI 准备读入下一幅图片
	cvResetImageROI(*theimage);
}

bool CMFCApplication1Dlg::EnumerateCameras(vector<int> &camIdx)
{
	camIdx.clear();
	struct CapDriver {
		int enumValue; string enumName; string comment;
	};
	// list of all CAP drivers (see highgui_c.h)  
	vector<CapDriver> drivers;

	drivers.push_back({ CV_CAP_DSHOW, "CV_CAP_DSHOW", "DirectShow (via videoInput)" });

	std::string winName, driverName, driverComment;
	int driverEnum;
	Mat frame;
	//bool found;
	for (size_t drv = 0; drv < drivers.size(); drv++)
	{
		driverName = drivers[drv].enumName;
		driverEnum = drivers[drv].enumValue;
		driverComment = drivers[drv].comment;

		int maxID = 10; //100 IDs between drivers  

		for (int idx = 0; idx <maxID; idx++)
		{
			VideoCapture cap(driverEnum + idx);  // open the camera  
			if (cap.isOpened())                  // check if we succeeded  
			{
				camIdx.push_back(driverEnum + idx);  // vector of all available cameras  
			}
			cap.release();
		}
		cout << endl;
	}


	return (camIdx.size()>0); // returns success  
}


UINT CMFCApplication1Dlg::ThreadFunc(LPVOID pParam) {
	int* capnum = (int*)pParam;
	CMFCApplication1App *app =(CMFCApplication1App*)AfxGetApp();
	VideoCapture capl(capnum[0]);
	capl.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capl.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	VideoCapture capr(capnum[1]);
	capr.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capr.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	Mat M[2], D[2], R[2], P[2], rmap[2][2],Q;
	

	Size imageSize;
	imageSize.height = 480;
	imageSize.width = 640;
	Mat frame0, frame2;
	//vector<KeyPoint> keypoints1, keypoints2;
	//keypoints1.resize(500);
	//keypoints2.resize(500);//http://blog.csdn.net/u012327581/article/details/51351780
	//while (WaitForSingleObject(app->m_exitThreadEvent, 1) != WAIT_OBJECT_0) {
	while(app->cameraswitch){
		capl >> frame0;
		capr >> frame2;
		if (app->WorkMode == 0 || app->WorkMode == 1) {
			IplImage *framel = &IplImage(frame0);
			char* datao = framel->imageData;
			IplImage *framer = &IplImage(frame2);
			framel->imageData = datao;
			::SendMessage(
				AfxGetMainWnd()->m_hWnd
				, WM_MY_MESSAGE
				, (WPARAM)framel
				, (LPARAM)framer);
		}else 
		if (app->WorkMode == 2) {
			
			M[0] = Mat_<double>::zeros(3, 3);
			M[1] = Mat_<double>::zeros(3, 3);
			D[0] = Mat_<double>::zeros(1, 8);
			D[1] = Mat_<double>::zeros(1, 8);
			R[0] = Mat_<double>::zeros(3, 3);
			R[1] = Mat_<double>::zeros(3, 3);
			P[0] = Mat_<double>::zeros(3, 4);
			P[1] = Mat_<double>::zeros(3, 4);
			FileStorage fs("intrinsics.yml", FileStorage::READ);
			if (fs.isOpened()) {
				cout << "is opened" << endl;
			}
			fs["M1"] >> M[0];
			fs["M2"] >> M[1];
			fs["D1"] >> D[0];
			fs["D2"] >> D[1];
			fs.release();
			fs.open("extrinsics.yml", FileStorage::READ);
			fs["R1"] >> R[0];
			fs["R2"] >> R[1];
			fs["P1"] >> P[0];
			fs["P2"] >> P[1];
			fs["Q"] >> Q;
			fs.release();
			app->cx = Q.at<double>(0, 3);
			app->cy = Q.at<double>(1, 3);
			app->f = Q.at<double>(2, 3);
			app->T = 1 / Q.at<double>(3, 2);
			app->WorkMode = 3;
		}else 
		if (app->WorkMode == 3) {
			initUndistortRectifyMap(M[0], D[0], R[0], P[0], imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
			initUndistortRectifyMap(M[1], D[1], R[1], P[1], imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);
			remap(frame0, frame0, rmap[0][0], rmap[0][1], CV_INTER_LINEAR);
			remap(frame2, frame2, rmap[1][0], rmap[1][1], CV_INTER_LINEAR);
			rectangle(frame0, Rect(0, 0, 0, 0), Scalar(0, 0, 255), 3, 8);
			rectangle(frame2, Rect(0, 0, 0, 0), Scalar(0, 0, 255), 3, 8);

			for (int j = 0; j < frame0.rows; j += 16)
				line(frame0, Point(0, j), Point(frame0.cols, j), Scalar(0, 255, 0), 1, 8);
			for (int j = 0; j < frame2.rows; j += 16)
				line(frame2, Point(0, j), Point(frame2.cols, j), Scalar(0, 255, 0), 1, 8);
			IplImage *framel = &IplImage(frame0);
			char* datao = framel->imageData;
			IplImage *framer = &IplImage(frame2);
			framel->imageData = datao;
			::SendMessage(
				AfxGetMainWnd()->m_hWnd
				, WM_MY_MESSAGE
				, (WPARAM)framel
				, (LPARAM)framer);

		}else 
		if (app->WorkMode == 4) {
			initUndistortRectifyMap(M[0], D[0], R[0], P[0], imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
			initUndistortRectifyMap(M[1], D[1], R[1], P[1], imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);
			remap(frame0, frame0, rmap[0][0], rmap[0][1], CV_INTER_LINEAR);
			remap(frame2, frame2, rmap[1][0], rmap[1][1], CV_INTER_LINEAR);
			rectangle(frame0, Rect(0, 0, 0, 0), Scalar(0, 0, 255), 3, 8);
			rectangle(frame2, Rect(0, 0, 0, 0), Scalar(0, 0, 255), 3, 8);
			IplImage *framel = &IplImage(frame0);
			char* datao = framel->imageData;
			IplImage *framer = &IplImage(frame2);
			framel->imageData = datao;
			app->WorkMode = 3;
			::SendMessage(
				AfxGetMainWnd()->m_hWnd
				, WM_3D_MESSAGE
				, (WPARAM)framel
				, (LPARAM)framer);
			
		}
		//if (app->WorkMode == 4) {
		//	//vector<KeyPoint> keypoints1, keypoints2;
		//		initUndistortRectifyMap(M[0], D[0], R[0], P[0], imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
		//		initUndistortRectifyMap(M[1], D[1], R[1], P[1], imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);
		//		remap(frame0, frame0, rmap[0][0], rmap[0][1], CV_INTER_LINEAR);
		//		remap(frame2, frame2, rmap[1][0], rmap[1][1], CV_INTER_LINEAR);
		//		rectangle(frame0, Rect(0, 0, 0, 0), Scalar(0, 0, 255), 3, 8);
		//		rectangle(frame2, Rect(0, 0, 0, 0), Scalar(0, 0, 255), 3, 8);
		//		
		//		SurfFeatureDetector detector(400);
		//		detector.detect(frame0, keypoints1);
		//		detector.detect(frame2, keypoints2);
		//		SurfDescriptorExtractor surfDesc;
		//		Mat descriptros1, descriptros2;
		//		surfDesc.compute(frame0, keypoints1, descriptros1);
		//		surfDesc.compute(frame2, keypoints2, descriptros2);
		//		BruteForceMatcher<L2<float>>matcher;
		//		vector<DMatch>matches;
		//		matcher.match(descriptros1, descriptros2, matches);
		//		std::nth_element(matches.begin(), matches.begin() + 35, matches.end());
		//		matches.erase(matches.begin() + 36, matches.end());

		//		vector<DMatch>::iterator match = matches.begin();
		//		vector<Point3d> worldpoint;
		//		worldpoint.resize(0);
		//		for (;match != matches.end(); match++) {
		//			if (abs((int)keypoints1[match->queryIdx].pt.y - (int)keypoints2[match->trainIdx].pt.y) <= 2) {
		//				//l_points.push_back(keypoints1[match->queryIdx].pt);
		//				//r_points.push_back(keypoints2[match->trainIdx].pt);
		//				double d = keypoints1[match->queryIdx].pt.x - keypoints2[match->trainIdx].pt.x;
		//				double Z = T*f / d;
		//				double X = (keypoints1[match->queryIdx].pt.x + cx)*Z / f;
		//				double Y = -(keypoints1[match->queryIdx].pt.y + cy)*Z / f;
		//				worldpoint.push_back(Point3d(X, Y, Z));
		//			}
		//		}
		//		CvMat * worldP = cvCreateMat(worldpoint.size(), 3, CV_32FC1);
		//		int i = 0;
		//		vector<Point3d>::iterator it = worldpoint.begin();
		//		for (;it != worldpoint.end();it++) {
		//			CV_MAT_ELEM(*worldP, float, i, 0) = (float)it->x;
		//			CV_MAT_ELEM(*worldP, float, i, 1) = (float)it->y;
		//			CV_MAT_ELEM(*worldP, float, i, 2) = (float)it->z;
		//			i++;

		//		}
		//		::SendMessage(
		//			AfxGetMainWnd()->m_hWnd
		//			, WM_3D_MESSAGE
		//			, 0
		//			, (LPARAM)worldP);
		//		app->WorkMode = 3;
		//	}
	}
	//vector<KeyPoint>().swap(keypoints1);
	//vector<KeyPoint>().swap(keypoints2);
	capl.release();
	capr.release();
	return 0;
}

void CMFCApplication1Dlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCApplication1App *app = (CMFCApplication1App*)AfxGetApp();
	app->cameraswitch = false;
	//SetEvent(app->m_exitThreadEvent);
	if (TheImage)                                // 对上一幅显示的图片数据清零
		cvZero(TheImage);
	if (TheImage_right)
		cvZero(TheImage_right);
	ShowImage(TheImage, IDC_ShowImg);
	ShowImage(TheImage_right, IDC_ShowImg_rigth);
	if (TheImage)                                // 对上一幅显示的图片数据清零
		cvZero(TheImage);
	if (TheImage_right)
		cvZero(TheImage_right);
	ShowImage(TheImage, IDC_ShowImg);
	ShowImage(TheImage_right, IDC_ShowImg_rigth);
	return;
}


void CMFCApplication1Dlg::OnBnClickedButton3()
{
	if(m_pThread==NULL){ AfxMessageBox(_T("并没有打开摄像头")); return;}
	CMFCApplication1App *app = (CMFCApplication1App*)AfxGetApp();
	if (app->WorkMode != 1) {
		
		CString str;
			E_rows.GetWindowText(str);
			if (str == "") { AfxMessageBox(_T("请输入行数")); return; }
			broad.height = _ttoi(str);
			E_cols.GetWindowText(str);
			if (str == "") { AfxMessageBox(_T("请输入列数")); return;
			}
			broad.width = _ttoi(str);
			E_count.GetWindowText(str);
			if (str == "") { AfxMessageBox(_T("请输入拍照数")); return;
			}
			piccount = _ttoi(str);
			E_lenght.GetWindowText(str);
			if (str == "") { AfxMessageBox(_T("请输入长度")); return;
			}
			int temp = _ttoi(str);
			broadlenght = (float)temp/10;
		
		app->WorkMode = 1;
		AfxMessageBox(_T("开始标定"));
	}
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCApplication1Dlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	toru = true;
}

void CMFCApplication1Dlg::StereoCalibx(const vector<string>& imagelist, Size boardSize, float squareSize,double &rms,double &err)
{
	bool useCalibrated = true;
	bool showRectified = false;
	if (imagelist.size() % 2 != 0)
	{
		//cout << "Error: the image list contains odd (non-even) number of elements\n";
		return;
	}

	bool displayCorners = true;//true;
	const int maxScale = 2;
	  // Set this to your actual square size
									   // ARRAY AND VECTOR STORAGE:

	vector<vector<Point2f> > imagePoints[2];
	vector<vector<Point3f> > objectPoints;
	Size imageSize;

	int i, j, k, nimages = (int)imagelist.size() / 2;

	imagePoints[0].resize(nimages);
	imagePoints[1].resize(nimages);
	vector<string> goodImageList;

	for (i = j = 0; i < nimages; i++)
	{
		for (k = 0; k < 2; k++)
		{
			const string& filename = imagelist[i * 2 + k];
			Mat img = imread(filename, 0);
			if (img.empty())
				break;
			if (imageSize == Size())
				imageSize = img.size();
			else if (img.size() != imageSize)
			{
				//cout << "The image " << filename << " has the size different from the first image size. Skipping the pair\n";
				break;
			}
			bool found = false;
			vector<Point2f>& corners = imagePoints[k][j];
			for (int scale = 1; scale <= maxScale; scale++)
			{
				Mat timg;
				if (scale == 1)
					timg = img;
				else
					resize(img, timg, Size(), scale, scale);
				found = findChessboardCorners(timg, boardSize, corners,
					CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
				if (found)
				{
					if (scale > 1)
					{
						Mat cornersMat(corners);
						cornersMat *= 1. / scale;
					}
					break;
				}
			}
			//if (displayCorners)
			//{
			//	cout << filename << endl;
			//	Mat cimg, cimg1;
			//	cvtColor(img, cimg, COLOR_GRAY2BGR);
			//	drawChessboardCorners(cimg, boardSize, corners, found);
			//	double sf = 640. / MAX(img.rows, img.cols);
			//	resize(cimg, cimg1, Size(), sf, sf);
			//	imshow("corners", cimg1);
			//	char c = (char)waitKey(50);
			//	if (c == 27 || c == 'q' || c == 'Q') //Allow ESC to quit
			//		exit(-1);
			//}
			//else;
			//	//putchar('.');
			if (!found)
				break;
			cornerSubPix(img, corners, Size(11, 11), Size(-1, -1),
				TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,
					30, 0.01));
		}
		if (k == 2)
		{
			goodImageList.push_back(imagelist[i * 2]);
			goodImageList.push_back(imagelist[i * 2 + 1]);
			j++;
		}
	}
	//cout << j << " pairs have been successfully detected.\n";
	nimages = j;
	if (nimages < 2)
	{
		//cout << "Error: too little pairs to run the calibration\n";
		return;
	}

	imagePoints[0].resize(nimages);
	imagePoints[1].resize(nimages);
	objectPoints.resize(nimages);

	for (i = 0; i < nimages; i++)
	{
		for (j = 0; j < boardSize.height; j++)
			for (k = 0; k < boardSize.width; k++)
				objectPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
	}

	//cout << "Running stereo calibration ...\n";

	Mat cameraMatrix[2], distCoeffs[2];
	cameraMatrix[0] = Mat::eye(3, 3, CV_64F);
	cameraMatrix[1] = Mat::eye(3, 3, CV_64F);
	Mat R, T, E, F;

	rms = stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
		cameraMatrix[0], distCoeffs[0],
		cameraMatrix[1], distCoeffs[1],
		imageSize, R, T, E, F,
		TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5),
		CV_CALIB_FIX_ASPECT_RATIO +
		CV_CALIB_ZERO_TANGENT_DIST +
		CV_CALIB_SAME_FOCAL_LENGTH +
		CV_CALIB_RATIONAL_MODEL +
		CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);
	//cout << "done with RMS error=" << rms << endl;

	// CALIBRATION QUALITY CHECK
	// because the output fundamental matrix implicitly
	// includes all the output information,
	// we can check the quality of calibration using the
	// epipolar geometry constraint: m2^t*F*m1=0
	int npoints = 0;
	vector<Vec3f> lines[2];
	for (i = 0; i < nimages; i++)
	{
		int npt = (int)imagePoints[0][i].size();
		Mat imgpt[2];
		for (k = 0; k < 2; k++)
		{
			imgpt[k] = Mat(imagePoints[k][i]);
			undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], Mat(), cameraMatrix[k]);
			computeCorrespondEpilines(imgpt[k], k + 1, F, lines[k]);
		}
		for (j = 0; j < npt; j++)
		{
			double errij = fabs(imagePoints[0][i][j].x*lines[1][j][0] +
				imagePoints[0][i][j].y*lines[1][j][1] + lines[1][j][2]) +
				fabs(imagePoints[1][i][j].x*lines[0][j][0] +
					imagePoints[1][i][j].y*lines[0][j][1] + lines[0][j][2]);
			err += errij;
		}
		npoints += npt;
	}
	//cout << "average reprojection err = " << err / npoints << endl;
	err /= npoints;
	// save intrinsic parameters
	FileStorage fs("intrinsics.yml", CV_STORAGE_WRITE);
	if (fs.isOpened())
	{
		fs << "M1" << cameraMatrix[0] << "D1" << distCoeffs[0] <<
			"M2" << cameraMatrix[1] << "D2" << distCoeffs[1];
		fs.release();
	}
	else;
		//cout << "Error: can not save the intrinsic parameters\n";

	Mat R1, R2, P1, P2, Q;
	Rect validRoi[2];

	stereoRectify(cameraMatrix[0], distCoeffs[0],
		cameraMatrix[1], distCoeffs[1],
		imageSize, R, T, R1, R2, P1, P2, Q,
		CALIB_ZERO_DISPARITY, 0, imageSize, &validRoi[0], &validRoi[1]);

	fs.open("extrinsics.yml", CV_STORAGE_WRITE);
	if (fs.isOpened())
	{
		fs << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
		fs.release();
	}
	else;
		//cout << "Error: can not save the extrinsic parameters\n";

	//// OpenCV can handle left-right
	//// or up-down camera arrangements
	//bool isVerticalStereo = fabs(P2.at<double>(1, 3)) > fabs(P2.at<double>(0, 3));

	//// COMPUTE AND DISPLAY RECTIFICATION
	//if (!showRectified)
	//	return;

	//Mat rmap[2][2];
	//// IF BY CALIBRATED (BOUGUET'S METHOD)
	//if (useCalibrated)
	//{
	//	// we already computed everything
	//}
	//// OR ELSE HARTLEY'S METHOD
	//else
	//	// use intrinsic parameters of each camera, but
	//	// compute the rectification transformation directly
	//	// from the fundamental matrix
	//{
	//	vector<Point2f> allimgpt[2];
	//	for (k = 0; k < 2; k++)
	//	{
	//		for (i = 0; i < nimages; i++)
	//			std::copy(imagePoints[k][i].begin(), imagePoints[k][i].end(), back_inserter(allimgpt[k]));
	//	}
	//	F = findFundamentalMat(Mat(allimgpt[0]), Mat(allimgpt[1]), FM_8POINT, 0, 0);
	//	Mat H1, H2;
	//	stereoRectifyUncalibrated(Mat(allimgpt[0]), Mat(allimgpt[1]), F, imageSize, H1, H2, 3);

	//	R1 = cameraMatrix[0].inv()*H1*cameraMatrix[0];
	//	R2 = cameraMatrix[1].inv()*H2*cameraMatrix[1];
	//	P1 = cameraMatrix[0];
	//	P2 = cameraMatrix[1];
	//}

	////Precompute maps for cv::remap()
	//initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
	//initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);

	//Mat canvas;
	//double sf;
	//int w, h;
	//if (!isVerticalStereo)
	//{
	//	sf = 600. / MAX(imageSize.width, imageSize.height);
	//	w = cvRound(imageSize.width*sf);
	//	h = cvRound(imageSize.height*sf);
	//	canvas.create(h, w * 2, CV_8UC3);
	//}
	//else
	//{
	//	sf = 300. / MAX(imageSize.width, imageSize.height);
	//	w = cvRound(imageSize.width*sf);
	//	h = cvRound(imageSize.height*sf);
	//	canvas.create(h * 2, w, CV_8UC3);
	//}

	//for (i = 0; i < nimages; i++)
	//{
	//	for (k = 0; k < 2; k++)
	//	{
	//		Mat img = imread(goodImageList[i * 2 + k], 0), rimg, cimg;
	//		remap(img, rimg, rmap[k][0], rmap[k][1], CV_INTER_LINEAR);
	//		cvtColor(rimg, cimg, COLOR_GRAY2BGR);
	//		Mat canvasPart = !isVerticalStereo ? canvas(Rect(w*k, 0, w, h)) : canvas(Rect(0, h*k, w, h));
	//		resize(cimg, canvasPart, canvasPart.size(), 0, 0, CV_INTER_AREA);
	//		if (useCalibrated)
	//		{
	//			Rect vroi(cvRound(validRoi[k].x*sf), cvRound(validRoi[k].y*sf),
	//				cvRound(validRoi[k].width*sf), cvRound(validRoi[k].height*sf));
	//			rectangle(canvasPart, vroi, Scalar(0, 0, 255), 3, 8);
	//		}
	//	}

	//	if (!isVerticalStereo)
	//		for (j = 0; j < canvas.rows; j += 16)
	//			line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
	//	else
	//		for (j = 0; j < canvas.cols; j += 16)
	//			line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
	//	imshow("rectified", canvas);
	//	char c = (char)waitKey();
	//	if (c == 27 || c == 'q' || c == 'Q')
	//		break;
	//}
	return;
}


bool CMFCApplication1Dlg::readStringListx(const string& filename, vector<string>& l)
{
	l.resize(0);
	FileStorage fs(filename, FileStorage::READ);
	if (!fs.isOpened())
		return false;
	FileNode n = fs.getFirstTopLevelNode();
	if (n.type() != FileNode::SEQ)
		return false;
	FileNodeIterator it = n.begin(), it_end = n.end();
	for (; it != it_end; ++it)
		l.push_back((string)*it);
	return true;
}

void CMFCApplication1Dlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCApplication1App *app = (CMFCApplication1App*)AfxGetApp();
	app->WorkMode = 2;
	return;
}



void CMFCApplication1Dlg::OnBnClickedButton6()
{
	CMFCApplication1App *app = (CMFCApplication1App*)AfxGetApp();
	if (app->WorkMode != 3) { AfxMessageBox(_T("请先读入参数或者进行标定")); return; }
	app->WorkMode = 4;
	return;
	// TODO: 在此添加控件通知处理程序代码
}


void  DelunayTri(vector<Point3f> worldpoint, vector<Point3d>& Triinfo) {
	vector<Point2d> Lineinfo;
	int count = worldpoint.size();
	int minnum = 1000;
	int minnoright = 0;
	int minl = 0;
	int minnoleft = 0;
	for (int j = 0; j < count; j++) {

		for (int i = 1; i < count; i++) {
			float di = distancex(worldpoint[j] - worldpoint[i]);
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
	while (pointsisempty(points, count)) {
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
					float di = distancex(worldpoint[i] - linezhong);
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
float distancex(Point3f p) {
	return sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
}
int pointsisempty(int* p, int length) {
	int count = 0;
	for (int i = 0; i < length; i++) {
		count += p[i];
	}
	return count;
}

void Tri2Line(vector<Point3d> &tri, vector<Point2d> &line) {
	vector<Point3d>::iterator trit = tri.begin();
	vector<Point2d>::iterator lineit = line.begin();
	for (; trit != tri.end(); trit++) {
		int flag1 = 0, flag2 = 0, flag3 = 0;
		for (lineit = line.begin(); lineit != line.end(); lineit++) {
			if ((lineit->x == trit->x&&lineit->y == trit->y) || (lineit->y == trit->x&&lineit->x == trit->y)) { flag1 = 1; }
			if ((lineit->x == trit->x&&lineit->y == trit->z) || (lineit->y == trit->x&&lineit->x == trit->z)) { flag2 = 1; }
			if ((lineit->x == trit->z&&lineit->y == trit->y) || (lineit->y == trit->z&&lineit->x == trit->y)) { flag3 = 1; }
		}
		if (flag1 == 0) {
			line.push_back(Point2d(trit->x, trit->y));
		}
		if (flag2 == 0) {
			line.push_back(Point2d(trit->x, trit->z));
		}
		if (flag3 == 0) {
			line.push_back(Point2d(trit->z, trit->y));
		}
	}
	return;
}

void FindMidpoint(vector<Point2d>&line, vector<Point>&lp, vector<Point>&rp, Mat & lgray, Mat &rgray) {
	vector<Point> lptemp, rptemp;
	vector<Point2d>::iterator lineit = line.begin();
	for (; lineit != line.end(); lineit++) {
		float ly = lp[lineit->x].x + lp[lineit->y].x;
		ly /= 2;
		float lx = lp[lineit->x].y + lp[lineit->y].y;
		lx /= 2;
		float ry = rp[lineit->x].x + rp[lineit->y].x;
		ry /= 2;
		float rx = rp[lineit->x].y + rp[lineit->y].y;
		rx /= 2;
		float min = 255, minx, miny;
		if (abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx, ry)) < min) {
			min = abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx, ry));
			minx = rx;
			miny = ry;
		}
		if (abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx - 1, ry)) < min) {
			min = abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx - 1, ry));
			minx = rx - 1;
			miny = ry;
		}
		if (abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx - 1, ry - 1)) < min) {
			min = abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx - 1, ry - 1));
			minx = rx - 1;
			miny = ry - 1;
		}
		if (abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx - 1, ry + 1)) < min) {
			min = abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx - 1, ry + 1));
			minx = rx - 1;
			miny = ry + 1;
		}
		if (abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx, ry - 1)) < min) {
			min = abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx, ry - 1));
			minx = rx;
			miny = ry - 1;
		}
		if (abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx, ry + 1)) < min) {
			min = abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx, ry + 1));
			minx = rx;
			miny = ry + 1;
		}
		if (abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx + 1, ry)) < min) {
			min = abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx + 1, ry));
			minx = rx + 1;
			miny = ry;
		}
		if (abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx + 1, ry - 1)) < min) {
			min = abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx + 1, ry - 1));
			minx = rx + 1;
			miny = ry - 1;
		}
		if (abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx + 1, ry + 1)) < min) {
			min = abs(lgray.at<float>(lx, ly) - rgray.at<float>(rx + 1, ry + 1));
			minx = rx + 1;
			miny = ry + 1;
		}
		lptemp.push_back(Point(ly, lx));
		rptemp.push_back(Point(ry, rx));
	}
	vector<Point>::iterator lptempit = lptemp.begin();
	vector<Point>::iterator rptempit = rptemp.begin();
	for (;lptempit != lptemp.end();lptempit++) {
		lp.push_back(Point(lptempit->x, lptempit->y));
	}
	for (;rptempit != rptemp.end();rptempit++) {
		rp.push_back(Point(rptempit->x, rptempit->y));
	}
}

void CMFCApplication1Dlg::OnBnClickedButton7()
{
	imshow("left",imread("left.jpg"));
	imshow("right", imread("right.jpg"));
	OpenGLShow *test = new OpenGLShow;
	test->Create(IDD_DIALOG1, this);
	test->ShowWindow(SW_SHOW);
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCApplication1Dlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnBnClickedButton2();
	OnBnClickedButton2();
	OnBnClickedButton2();

	AfxMessageBox(_T("正在关闭"));

	CDialogEx::OnClose();
}

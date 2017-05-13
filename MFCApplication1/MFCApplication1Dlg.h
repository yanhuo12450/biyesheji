
// MFCApplication1Dlg.h : 头文件
//

#pragma once
#include <opencv2/opencv.hpp>
#include "highgui.h"
#include "Resource.h"
#include "CvvImage.h"
#include "afxwin.h"
#include <opencv2/highgui.hpp>  
#include <opencv2/core.hpp>  
#include <iostream>  
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include "OpenGLShow.h"

using namespace cv;
using namespace std;

// CMFCApplication1Dlg 对话框
class CMFCApplication1Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication1Dlg(CWnd* pParent = NULL);	// 标准构造函数
	void ReleaseIplImage();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	
	

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On3DMessage(WPARAM wParam, LPARAM lParam);

private:
	IplImage* TheImage;
public:
	static UINT ThreadFunc(LPVOID pParam);

	afx_msg void OnBnClickedButton1();
	void ShowImage(IplImage* img, UINT ID);
	void ResizeImage(IplImage* img,IplImage **theimage);
	CComboBox C_left;
	CComboBox C_right;
	CString C_left_value;
	CString C_right_value;
	bool EnumerateCameras(vector<int> &camIdx);
	IplImage* TheImage_right;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	Size broad;
	float broadlenght;
	int piccount;
	int currcount;
	boolean toru;
	FileStorage fs;
	CEdit E_rows;
	CEdit E_cols;
	CEdit E_lenght;
	CEdit E_count;
	afx_msg void OnBnClickedButton4();
	CWinThread *m_pThread; //线程指针
	static bool readStringListx(const string& filename, vector<string>& l);
	static void StereoCalibx(const vector<string>& imagelist, Size boardSize, float squareSize, double &rms, double &err);
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnClose();
};
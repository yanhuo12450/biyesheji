#pragma once


// OpenGLShow 对话框

class OpenGLShow : public CDialogEx
{
	DECLARE_DYNAMIC(OpenGLShow)

public:

	HDC hrenderDC;  //设备上下文
	HGLRC hrenderRC;  //渲染上下文
	float m_yRotate;  //转速
	int PixelFormat;    //像素格式
	OpenGLShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~OpenGLShow();
	BOOL SetWindowPixelFormat(HDC hDC);  //设定像素格式
	BOOL CreateViewGLContext(HDC hDC);   //view GL Context
	void RenderScene();  //绘制场景

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

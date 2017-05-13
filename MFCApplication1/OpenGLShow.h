#pragma once


// OpenGLShow �Ի���

class OpenGLShow : public CDialogEx
{
	DECLARE_DYNAMIC(OpenGLShow)

public:

	HDC hrenderDC;  //�豸������
	HGLRC hrenderRC;  //��Ⱦ������
	float m_yRotate;  //ת��
	int PixelFormat;    //���ظ�ʽ
	OpenGLShow(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~OpenGLShow();
	BOOL SetWindowPixelFormat(HDC hDC);  //�趨���ظ�ʽ
	BOOL CreateViewGLContext(HDC hDC);   //view GL Context
	void RenderScene();  //���Ƴ���

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

// myworkView.h : interface of the CmyworkView class
//

#pragma once

#include "handle.h"

class CmyworkView : public CView
{
protected: // create from serialization only
	CmyworkView();
	DECLARE_DYNCREATE(CmyworkView);

public:
	//CmyworkDoc* CmyworkView::GetDocument(void);

// Operations
public:
	handle* phandle;
	BOOL	m_Cull;
	BOOL	m_Show;
	BOOL	m_Z;
	BOOL    m_Smooth;	
	BOOL    m_Texture;
	int     m_color;
	CString	m_FaceCount;
	CString	m_PointCount;
	int		m_ShowStyle;   
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CmyworkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileOpen();
	afx_msg void OnAxes();
	afx_msg void OnUpdateAxes(CCmdUI* pCmdUI);

private:
	void DrawAxis();
	bool SetUpPiexlFormat(HDC hDC);
	void InitalEnvironment();
	void InitalLigth(); //π‚’’
	void UpdatamyworkView();

	HGLRC m_hGLContext;
	int nPixelFormat;
	float xRot, yRot, zRot ;
	double zoom;
	CPoint m_Pos;
	BOOL b_Axis;	
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnStylePoint();
	afx_msg void OnStyleXian();
	afx_msg void OnStyleSmooth();
	afx_msg void OnStyleZ();
	afx_msg void OnStyleSolid();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnStyleCull();
	afx_msg void OnColor();
	afx_msg void OnColorw();
	afx_msg void OnColorg();
	afx_msg void OnColorp();
	afx_msg void OnTexture();
};

#ifndef _DEBUG  // debug version in myworkView.cpp
inline CmyworkDoc* CmyworkView::GetDocument()
   { return reinterpret_cast<CmyworkDoc*>(m_pDocument); }
#endif


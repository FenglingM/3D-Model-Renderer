// OpenglView.cpp : implementation of the CmyworkView class
//

#include "stdafx.h"
#include "mywork.h"

#include "myworkDoc.h"
#include "myworkView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CmyworkView

IMPLEMENT_DYNCREATE(CmyworkView, CView)

BEGIN_MESSAGE_MAP(CmyworkView, CView)
	//{{AFX_MSG_MAP(CmyworkView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_AXES, OnUpdateAxes)
	ON_COMMAND(ID_AXES, OnAxes)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CmyworkView::OnFileOpen)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
    ON_COMMAND(ID_STYLE_POINT, &CmyworkView::OnStylePoint)
    ON_COMMAND(ID_STYLE_XIAN, &CmyworkView::OnStyleXian)
    ON_COMMAND(ID_STYLE_SMOOTH, &CmyworkView::OnStyleSmooth)
    ON_COMMAND(ID_STYLE_Z, &CmyworkView::OnStyleZ)
    ON_COMMAND(ID_STYLE_SOLID, &CmyworkView::OnStyleSolid)
	ON_COMMAND(ID_STYLE_CULL, &CmyworkView::OnStyleCull)
	ON_COMMAND(ID_COLORW, &CmyworkView::OnColorw)
	ON_COMMAND(ID_COLORG, &CmyworkView::OnColorg)
	ON_COMMAND(ID_COLORP, &CmyworkView::OnColorp)
	ON_COMMAND(ID_TEXTURE, &CmyworkView::OnTexture)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CmyworkView construction/destruction

CmyworkView::CmyworkView()
{
	// TODO: add construction code here
	phandle = NULL;

	xRot = yRot = zRot = 0.0; 

	zoom = -10.0; 

	m_Pos.x=0;
	m_Pos.y=0;
	m_Z = TRUE;
	m_Cull = TRUE;
	m_Smooth = FALSE;
	m_ShowStyle = 2;
	m_color = 0;
	//m_Texture =FALSE;
}

CmyworkView::~CmyworkView()
{
	if ( phandle )
		delete phandle;

}

BOOL CmyworkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CmyworkView drawing

void CmyworkView::OnDraw(CDC* pDC)
{
	// TODO: add draw code for native data here
	CDC* pDc=this->GetDC();
	InitalEnvironment();

	InitalLigth(); //光照 

	glPushMatrix(); //保存变换以前的位置
	glTranslatef(0.0f,0.0f,zoom);  

	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);

	if ( b_Axis )  //是否显示坐标轴
		DrawAxis();

	if( phandle)
	    phandle->Drawhandle(m_Show,m_Cull,m_ShowStyle,m_Z,m_color,m_Smooth,m_Texture); //绘制模型

	glPopMatrix();  //恢复变换之前的位置

	SwapBuffers(pDc->m_hDC);
	ReleaseDC(pDc);
}

/////////////////////////////////////////////////////////////////////////////
// CmyworkView printing

BOOL CmyworkView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CmyworkView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CmyworkView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CmyworkView diagnostics

#ifdef _DEBUG
void CmyworkView::AssertValid() const
{
	CView::AssertValid();
}

void CmyworkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CmyworkView message handlers

void CmyworkView::OnFileOpen()  //打开文件操作
{
	// TODO: Add your command handler code here
	CString filter="PLY2 File (*.PLY2)|*.ply2||";
    
	CFileDialog fileDlg(TRUE,NULL,NULL,NULL,filter,NULL);  
	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle="读取ply2文件";
	if (fileDlg.DoModal()==IDOK) 
	{
		if ( phandle )
			delete phandle;
		phandle = new handle();

		CString filepath = fileDlg.GetPathName();  //获取文件路径
		phandle->ReadhandleFile(filepath);  //读取文件
		CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
		pMain->SetWindowText(filepath); //在窗口显示文件名

		CString str;

		str.Format("%d",phandle->faceCount);  //显示点数量和面数量对话框
		m_FaceCount = _T("面数量:") + str+"\n";

		str.Format("%d",phandle->vertexCount);
		m_PointCount = _T("点数量:") + str;

		::MessageBox(NULL, +m_FaceCount+m_PointCount, "模型参数", MB_OK);

		UpdateData(FALSE);
	}	
}

bool CmyworkView::SetUpPiexlFormat(HDC hDC)    //设置像素格式
{
		PIXELFORMATDESCRIPTOR pfd = { 
	    sizeof(PIXELFORMATDESCRIPTOR),    // pfd结构的大小 
	    1,                                // 版本号 
	    PFD_DRAW_TO_WINDOW |              // 支持在窗口中绘图 
	    PFD_SUPPORT_OPENGL |              // 支持 OpenGL 
	    PFD_DOUBLEBUFFER,                 // 双缓存模式 
	    PFD_TYPE_RGBA,                    // RGBA 颜色模式 
	    24,                               // 24 位颜色深度 
	    0, 0, 0, 0, 0, 0,                 // 忽略颜色位 
	    0,                                // 没有非透明度缓存 
	    0,                                // 忽略移位位 
	    0,                                // 无累加缓存 
	    0, 0, 0, 0,                       // 忽略累加位 
	    16,                               // 32 位深度缓存     
	    0,                                // 无模板缓存 
	    0,                                // 无辅助缓存 
	    PFD_MAIN_PLANE,                   // 主层 
	    0,                                // 保留 
	    0, 0, 0                           // 忽略层,可见性和损毁掩模 
	}; 
	if (!(nPixelFormat = ChoosePixelFormat(hDC, &pfd)))
		     return false;
	if(!SetPixelFormat(hDC,nPixelFormat,&pfd))//设置当前设备的像素点格式
		return false;
	
	m_hGLContext = wglCreateContext(hDC); //获取渲染描述句柄
	if(m_hGLContext==NULL)
		return false;
	if(!wglMakeCurrent(hDC, m_hGLContext))		//激活渲染描述句柄
		return false;
	return true;
}

int CmyworkView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CDC* cDc=this->GetDC();
	
	if(!SetUpPiexlFormat(cDc->m_hDC))
	{
		MessageBox(TEXT("设备环境初始化错误,程序不能运行"),TEXT("错误"),MB_OK | MB_ICONERROR);
		SendMessage(WM_QUIT,0,0);
	}
	ReleaseDC(cDc);	

	return 0;
}

void CmyworkView::InitalEnvironment()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f); //设置背景颜色
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void CmyworkView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	GLfloat fAspect;

    if(cy == 0) 
        cy = 1;
    glMatrixMode(GL_PROJECTION);  //模型的投影矩阵
    glLoadIdentity(); //重置当前指定的矩阵为单位矩阵
    glViewport(0, 0, cx,cy);  //设置视口为当前窗口

    fAspect = (GLfloat)cx/(GLfloat)cy;  //窗口的纵横比

    gluPerspective(35, fAspect,0.1, 2000);  //视角为35度，近处裁面0.1，远处裁面2000

    glMatrixMode(GL_MODELVIEW); //模型的视景体矩阵
    glLoadIdentity();
		
}

void CmyworkView::InitalLigth() //光照
{
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空颜色和深度缓冲  
 
    GLfloat m_LightPostion[4]={16.0f, 12.0f, 10.0f, 1.0f};
	GLfloat ambientLight[]={0.2f,0.2f,0.2f,1.0f};
	GLfloat diffuseLight[]={0.8f,0.0f,0.0f,1.0f};
	GLfloat specularLight[]={1.0f,1.0f,1.0f,1.0f};

	//白炽灯
	//GLfloat ambientLight[]={0.25f,0.25f,0.25f,1.0f};
	//GLfloat diffuseLight[]={0.5,0.5f,0.5f,1.0f};
	//GLfloat specularLight[]={0.5f,0.5f,0.5f,1.0f};
		
	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT1,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT1,GL_SPECULAR,specularLight);
	glLightfv(GL_LIGHT1,GL_POSITION,m_LightPostion);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST); //启用深度测试 	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
}

void CmyworkView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	::ReleaseCapture();		
	CView::OnLButtonUp(nFlags, point);
}

void CmyworkView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	HWND hWnd=this->GetSafeHwnd();
	::SetCapture(hWnd);
	if(nFlags==MK_LBUTTON)
		m_Pos=point;		
	CView::OnLButtonDown(nFlags, point);
}

void CmyworkView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(nFlags==MK_LBUTTON)
	{
		//模型XY
		zRot -= (float)(m_Pos.x - point.x) / 3.0f;
		xRot += (float)(point.y - m_Pos.y) / 3.0f;
		m_Pos = point;
		this->InvalidateRect(NULL,FALSE);
	}		
	CView::OnMouseMove(nFlags, point);
}


BOOL CmyworkView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	double a = zDelta / 120 ;
	if ( (zoom + a * 0.1)  <  1 )
		zoom += a * 0.1;
	
	this->InvalidateRect(NULL,FALSE);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CmyworkView::DrawAxis()  //画坐标轴
{
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL );
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    glScalef(0.6f, 0.6f, 0.6f);
	glBegin(GL_LINES);	
	// x axis
	glColor3f ( 1.0f, 0.0f,  0.0f);  //设置颜色
	glVertex3f( 0.0f , 0.0f , 0.0f );
	glVertex3f( 1.0f , 0.0f , 0.0f );

	// y axis
	glColor3f ( 0.0f, 1.0f,  0.0f);
	glVertex3f( 0.0f , 0.0f , 0.0f );
	glVertex3f( 0.0f , 1.0f , 0.0f );

	// z axis
	glColor3f ( 0.0f, 0.0f,  1.0f);
	glVertex3f( 0.0f , 0.0f , 0.0f );
	glVertex3f( 0.0f , 0.0f , 1.0f );
	glEnd();

	glBegin(GL_TRIANGLES);	

	// x
	glColor3f ( 1.0f, 0.0f,  0.0f);
	glVertex3f(1.0f  ,    0.0f ,    0.0f );
	glVertex3f(0.9f ,    0.0f ,    0.04f  );
	glVertex3f(0.9f ,    0.028f ,  0.028f );
	glVertex3f(0.9f ,    0.0f ,    0.04f );
	glVertex3f(0.9f ,    0.028f ,  0.028f );
	glVertex3f(0.9f ,    0.0f ,    0.0f );
	glVertex3f(1.0f ,    0.0f ,    0.0f );
	glVertex3f(0.9f ,    0.028f ,  0.028f );
	glVertex3f(0.9f ,    0.04f ,   0.0f );
	glVertex3f(0.9f ,    0.028f ,  0.028f );
	glVertex3f(0.9f ,    0.04f ,   0.0f );
	glVertex3f(0.9f ,    0.0f ,    0.0f );
	glVertex3f(1.0f ,    0.0f ,    0.0f );
	glVertex3f(0.9f ,    0.04f ,   0.0f );
	glVertex3f(0.9f ,    0.028f ,  -0.028f );
	glVertex3f(0.9f ,    0.04f ,   0.0f );
	glVertex3f(0.9f ,    0.028f ,  -0.028f );
	glVertex3f(0.9f ,    0.0f ,    0.0f );
	glVertex3f(1.0f ,    0.0f ,    0.0f );
	glVertex3f(0.9f ,    0.028f ,  -0.028f );
	glVertex3f(0.9f ,    0.0f ,    -0.04f );
	glVertex3f(0.9f ,    0.028f ,  -0.028f );
	glVertex3f(0.9f ,    0.0f ,    -0.04f );
	glVertex3f(0.9f ,    0.0f ,    0.0f );
	glVertex3f(1.0f ,    0.0f ,    0.0f );
	glVertex3f(0.9f ,    0.0f ,    -0.04f );
	glVertex3f(0.9f ,    -0.028f , -0.028f );
	glVertex3f(0.9f ,    0.0f ,    -0.04f );
	glVertex3f(0.9f ,    -0.028f , -0.028f );
	glVertex3f(0.9f ,    0.0f ,    0.0f );
	glVertex3f(1.0f ,    0.0f ,    0.0f );
	glVertex3f(0.9f ,    -0.028f , -0.028f );
	glVertex3f(0.9f ,    -0.04f ,  0.0f );
	glVertex3f(0.9f ,    -0.028f , -0.028f );
	glVertex3f(0.9f ,    -0.04f ,  0.0f );
	glVertex3f(0.9f ,    0.0f ,    0.0f );
	glVertex3f(1.0f ,    0.0f ,    0.0f );
	glVertex3f(0.9f ,    -0.04f ,  0.0f );
	glVertex3f(0.9f ,    -0.028f , 0.028f );
	glVertex3f(0.9f ,    -0.04f ,  0.0f );
	glVertex3f(0.9f ,    -0.028f , 0.028f );
	glVertex3f(0.9f ,    0.0f ,    0.0f );
	glVertex3f(1.0f ,    0.0f ,    0.0f );
	glVertex3f(0.9f ,    -0.028f , 0.028f );
	glVertex3f(0.9f ,    0.0f ,    0.04f );
	glVertex3f(0.9f ,    -0.028f , 0.028f );
	glVertex3f(0.9f ,    0.0f ,    0.04f );
	glVertex3f(0.9f ,    0.0f ,    0.0f );

	// y
	glColor3f ( 0.0f, 1.0f,  0.0f);
	glVertex3f(0.0f ,    1.0f ,    0.0f );
	glVertex3f(0.0f ,    0.9f ,    0.04f );
	glVertex3f(0.028f ,  0.9f ,    0.028f );
	glVertex3f(0.0f ,    0.9f ,    0.04f );
	glVertex3f(0.028f ,  0.9f ,    0.028f );
	glVertex3f(0.0f ,    0.9f ,    0.0f );
	glVertex3f(0.0f ,    1.0f ,    0.0f );
	glVertex3f(0.028f ,  0.9f ,    0.028f );
	glVertex3f(0.04f ,   0.9f ,    0.0f );
	glVertex3f(0.028f ,  0.9f ,    0.028f );
	glVertex3f(0.04f ,   0.9f ,    0.0f );
	glVertex3f(0.0f ,    0.9f ,    0.0f );
	glVertex3f(0.0f ,    1.0f ,    0.0f );
	glVertex3f(0.04f ,   0.9f ,    0.0f );
	glVertex3f(0.028f ,  0.9f ,    -0.028f );
	glVertex3f(0.04f ,   0.9f ,    0.0f );
	glVertex3f(0.028f ,  0.9f ,    -0.028f );
	glVertex3f(0.0f ,    0.9f ,    0.0f );
	glVertex3f(0.0f ,    1.0f ,    0.0f );
	glVertex3f(0.028f ,  0.9f ,    -0.028f );
	glVertex3f(0.0f ,    0.9f ,    -0.04f );
	glVertex3f(0.028f ,  0.9f ,    -0.028f );
	glVertex3f(0.0f ,    0.9f ,    -0.04f );
	glVertex3f(0.0f ,    0.9f ,    0.0f );
	glVertex3f(0.0f ,    1.0f ,    0.0f );
	glVertex3f(0.0f ,    0.9f ,    -0.04f );
	glVertex3f(-0.028f , 0.9f ,    -0.028f );
	glVertex3f(0.0f ,    0.9f ,    -0.04f );
	glVertex3f(-0.028f , 0.9f ,    -0.028f );
	glVertex3f(0.0f ,    0.9f ,    0.0f );
	glVertex3f(0.0f ,    1.0f ,    0.0f );
	glVertex3f(-0.028f , 0.9f ,    -0.028f );
	glVertex3f(-0.04f ,  0.9f ,    0.0f );
	glVertex3f(-0.028f , 0.9f ,    -0.028f );
	glVertex3f(-0.04f ,  0.9f ,    0.0f );
	glVertex3f(0.0f ,    0.9f ,    0.0f );
	glVertex3f(0.0f ,    1.0f ,    0.0f );
	glVertex3f(-0.04f ,  0.9f ,    0.0f );
	glVertex3f(-0.028f , 0.9f ,    0.028f );
	glVertex3f(-0.04f ,  0.9f ,    0.0f );
	glVertex3f(-0.028f , 0.9f ,    0.028f );
	glVertex3f(0.0f ,    0.9f ,    0.0f );
	glVertex3f(0.0f ,    1.0f ,    0.0f );
	glVertex3f(-0.028f , 0.9f ,    0.028f );
	glVertex3f(0.0f ,    0.9f ,    0.04f );
	glVertex3f(-0.028f , 0.9f ,    0.028f );
	glVertex3f(0.0f ,    0.9f ,    0.04f );
	glVertex3f(0.0f ,    0.9f ,    0.0f );

	// z
	glColor3f ( 0.0f, 0.0f,  1.0f);
	glVertex3f(0.0f ,    0.0f ,    1.0f );
	glVertex3f(0.0f ,    0.04f ,   0.9f );
	glVertex3f(0.028f ,  0.028f ,  0.9f );
	glVertex3f(0.0f ,    0.04f ,   0.9f );
	glVertex3f(0.028f ,  0.028f ,  0.9f );
	glVertex3f(0.0f ,    0.0f ,    0.9f );
	glVertex3f(0.0f ,    0.0f ,    1.0f );
	glVertex3f(0.028f ,  0.028f ,  0.9f );
	glVertex3f(0.04f ,   0.0f ,    0.9f );
	glVertex3f(0.028f ,  0.028f ,  0.9f );
	glVertex3f(0.04f ,   0.0f ,    0.9f );
	glVertex3f(0.0f ,    0.0f ,    0.9f );
	glVertex3f(0.0f ,    0.0f ,    1.0f );
	glVertex3f(0.04f ,   0.0f ,    0.9f );
	glVertex3f(0.028f ,  -0.028f , 0.9f );
	glVertex3f(0.04f ,   0.0f ,    0.9f );
	glVertex3f(0.028f ,  -0.028f , 0.9f );
	glVertex3f(0.0f ,    0.0f ,    0.9f );
	glVertex3f(0.0f ,    0.0f ,    1.0f );
	glVertex3f(0.028f ,  -0.028f , 0.9f );
	glVertex3f(0.0f ,    -0.04f ,  0.9f );
	glVertex3f(0.028f ,  -0.028f , 0.9f );
	glVertex3f(0.0f ,    -0.04f ,  0.9f );
	glVertex3f(0.0f ,    0.0f ,    0.9f );
	glVertex3f(0.0f ,    0.0f ,    1.0f );
	glVertex3f(0.0f ,    -0.04f ,  0.9f );
	glVertex3f(-0.028f , -0.028f , 0.9f );
	glVertex3f(0.0f ,    -0.04f ,  0.9f );
	glVertex3f(-0.028f , -0.028f , 0.9f );
	glVertex3f(0.0f ,    0.0f ,    0.9f );
	glVertex3f(0.0f ,    0.0f ,    1.0f );
	glVertex3f(-0.028f , -0.028f , 0.9f );
	glVertex3f(-0.04f ,  0.0f ,    0.9f );
	glVertex3f(-0.028f , -0.028f , 0.9f );
	glVertex3f(-0.04f ,  0.0f ,    0.9f );
	glVertex3f(0.0f ,    0.0f ,    0.9f );
	glVertex3f(0.0f ,    0.0f ,    1.0f );
	glVertex3f(-0.04f ,  0.0f ,    0.9f );
	glVertex3f(-0.028f , 0.028f ,  0.9f );
	glVertex3f(-0.04f ,  0.0f ,    0.9f );
	glVertex3f(-0.028f , 0.028f ,  0.9f );
	glVertex3f(0.0f ,    0.0f ,    0.9f );
	glVertex3f(0.0f ,    0.0f ,    1.0f );
	glVertex3f(-0.028f , 0.028f ,  0.9f );
	glVertex3f(0.0f ,    0.04f ,   0.9f );
	glVertex3f(-0.028f , 0.028f ,  0.9f );
	glVertex3f(0.0f ,    0.04f ,   0.9f );
	glVertex3f(0.0f ,    0.0f ,    0.9f );

	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

void CmyworkView::OnUpdateAxes(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(b_Axis);
	this->InvalidateRect(NULL,FALSE);
}

void CmyworkView::OnAxes() 
{
	// TODO: Add your command handler code here
	b_Axis = !b_Axis;
}

void CmyworkView::OnStylePoint()
{
	// TODO: Add your command handler code here
	m_ShowStyle = 0;
	this->UpdateData();	
	this->Invalidate();
}


void CmyworkView::OnStyleXian()
{
	// TODO: Add your command handler code here
	m_ShowStyle = 1;
	this->UpdateData();	
	this->Invalidate();
}

void CmyworkView::OnStyleSolid()
{
	// TODO: Add your command handler code here
	m_ShowStyle = 2;
	this->UpdateData();	
	this->Invalidate();
}

void CmyworkView::OnStyleSmooth()
{
	// TODO: Add your command handler code here
	m_Smooth = !m_Smooth;
    this->UpdateData();	
	this->Invalidate();
}

void CmyworkView::OnStyleZ()
{
	// TODO: Add your command handler code here
	m_Z = !m_Z;
	this->UpdateData();	
	this->Invalidate();
}

void CmyworkView::OnStyleCull()
{
  // TODO: Add your command handler code here
	m_Cull = !m_Cull;
	this->UpdateData();	
	this->Invalidate();
}

void CmyworkView::OnColorw()
{
	// TODO: Add your command handler code here
	m_color = 0;
	this->UpdateData();	
	this->Invalidate();
}

void CmyworkView::OnColorg()
{
	// TODO: Add your command handler code here
	m_color = 1;
	this->UpdateData();	
	this->Invalidate();
}

void CmyworkView::OnColorp()
{
	// TODO: Add your command handler code here
	m_color = 2;
	this->UpdateData();	
	this->Invalidate();
}

void CmyworkView::OnTexture()
{
	// TODO: Add your command handler code here
	m_Texture = !m_Texture;
	this->UpdateData();	
	this->Invalidate();
}

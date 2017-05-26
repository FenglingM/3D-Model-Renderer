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

	InitalLigth(); //���� 

	glPushMatrix(); //����任��ǰ��λ��
	glTranslatef(0.0f,0.0f,zoom);  

	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);

	if ( b_Axis )  //�Ƿ���ʾ������
		DrawAxis();

	if( phandle)
	    phandle->Drawhandle(m_Show,m_Cull,m_ShowStyle,m_Z,m_color,m_Smooth,m_Texture); //����ģ��

	glPopMatrix();  //�ָ��任֮ǰ��λ��

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

void CmyworkView::OnFileOpen()  //���ļ�����
{
	// TODO: Add your command handler code here
	CString filter="PLY2 File (*.PLY2)|*.ply2||";
    
	CFileDialog fileDlg(TRUE,NULL,NULL,NULL,filter,NULL);  
	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle="��ȡply2�ļ�";
	if (fileDlg.DoModal()==IDOK) 
	{
		if ( phandle )
			delete phandle;
		phandle = new handle();

		CString filepath = fileDlg.GetPathName();  //��ȡ�ļ�·��
		phandle->ReadhandleFile(filepath);  //��ȡ�ļ�
		CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
		pMain->SetWindowText(filepath); //�ڴ�����ʾ�ļ���

		CString str;

		str.Format("%d",phandle->faceCount);  //��ʾ���������������Ի���
		m_FaceCount = _T("������:") + str+"\n";

		str.Format("%d",phandle->vertexCount);
		m_PointCount = _T("������:") + str;

		::MessageBox(NULL, +m_FaceCount+m_PointCount, "ģ�Ͳ���", MB_OK);

		UpdateData(FALSE);
	}	
}

bool CmyworkView::SetUpPiexlFormat(HDC hDC)    //�������ظ�ʽ
{
		PIXELFORMATDESCRIPTOR pfd = { 
	    sizeof(PIXELFORMATDESCRIPTOR),    // pfd�ṹ�Ĵ�С 
	    1,                                // �汾�� 
	    PFD_DRAW_TO_WINDOW |              // ֧���ڴ����л�ͼ 
	    PFD_SUPPORT_OPENGL |              // ֧�� OpenGL 
	    PFD_DOUBLEBUFFER,                 // ˫����ģʽ 
	    PFD_TYPE_RGBA,                    // RGBA ��ɫģʽ 
	    24,                               // 24 λ��ɫ��� 
	    0, 0, 0, 0, 0, 0,                 // ������ɫλ 
	    0,                                // û�з�͸���Ȼ��� 
	    0,                                // ������λλ 
	    0,                                // ���ۼӻ��� 
	    0, 0, 0, 0,                       // �����ۼ�λ 
	    16,                               // 32 λ��Ȼ���     
	    0,                                // ��ģ�建�� 
	    0,                                // �޸������� 
	    PFD_MAIN_PLANE,                   // ���� 
	    0,                                // ���� 
	    0, 0, 0                           // ���Բ�,�ɼ��Ժ������ģ 
	}; 
	if (!(nPixelFormat = ChoosePixelFormat(hDC, &pfd)))
		     return false;
	if(!SetPixelFormat(hDC,nPixelFormat,&pfd))//���õ�ǰ�豸�����ص��ʽ
		return false;
	
	m_hGLContext = wglCreateContext(hDC); //��ȡ��Ⱦ�������
	if(m_hGLContext==NULL)
		return false;
	if(!wglMakeCurrent(hDC, m_hGLContext))		//������Ⱦ�������
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
		MessageBox(TEXT("�豸������ʼ������,����������"),TEXT("����"),MB_OK | MB_ICONERROR);
		SendMessage(WM_QUIT,0,0);
	}
	ReleaseDC(cDc);	

	return 0;
}

void CmyworkView::InitalEnvironment()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f); //���ñ�����ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void CmyworkView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	GLfloat fAspect;

    if(cy == 0) 
        cy = 1;
    glMatrixMode(GL_PROJECTION);  //ģ�͵�ͶӰ����
    glLoadIdentity(); //���õ�ǰָ���ľ���Ϊ��λ����
    glViewport(0, 0, cx,cy);  //�����ӿ�Ϊ��ǰ����

    fAspect = (GLfloat)cx/(GLfloat)cy;  //���ڵ��ݺ��

    gluPerspective(35, fAspect,0.1, 2000);  //�ӽ�Ϊ35�ȣ���������0.1��Զ������2000

    glMatrixMode(GL_MODELVIEW); //ģ�͵��Ӿ������
    glLoadIdentity();
		
}

void CmyworkView::InitalLigth() //����
{
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�����ɫ����Ȼ���  
 
    GLfloat m_LightPostion[4]={16.0f, 12.0f, 10.0f, 1.0f};
	GLfloat ambientLight[]={0.2f,0.2f,0.2f,1.0f};
	GLfloat diffuseLight[]={0.8f,0.0f,0.0f,1.0f};
	GLfloat specularLight[]={1.0f,1.0f,1.0f,1.0f};

	//�׳��
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
	glEnable(GL_DEPTH_TEST); //������Ȳ��� 	
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
		//ģ��XY
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

void CmyworkView::DrawAxis()  //��������
{
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL );
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    glScalef(0.6f, 0.6f, 0.6f);
	glBegin(GL_LINES);	
	// x axis
	glColor3f ( 1.0f, 0.0f,  0.0f);  //������ɫ
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

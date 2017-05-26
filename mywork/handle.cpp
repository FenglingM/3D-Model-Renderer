#include "stdafx.h"
#include "mywork.h"
#include "handle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

handle::handle()   //���캯��
{
	pPoint = NULL;
	pFaceIndex= NULL;
	pPointNormal= NULL;
	pFaceNormal= NULL;
	p_PointFace= NULL;
	faceCount = 0;
	vertexCount = 0;
}

handle::~handle()
{
	delete [] pPoint;
	delete [] pFaceIndex;
	delete [] pPointNormal;
	delete [] pFaceNormal;
	delete [] p_PointFace;
}

//����ģ�͵Ĵ�С���������������������ڴ�ռ�
BOOL handle::MemoryAllocate(long pointcount, long facecount)  //�����ڴ�
{
    pPoint = new CPoint3D[pointcount];  //���ģ�����е������
	pFaceIndex = new CFaceIndex[facecount];  //��������������
	pPointNormal = new CPoint3D[pointcount];  //
	pFaceNormal = new CPoint3D[facecount];  //���������ķ���
	p_PointFace = new QLinkList<long> [pointcount];  //������е������

	return TRUE;
}

//���õ�����
BOOL handle::SetPointTopPoint(const double x, const double y, const double z,long const pointID)
{
	return pPoint[pointID].SetPoint(x,y,z);
}

//�����������������
BOOL handle::SetFaceIndexTopFaceIndex(const long f1, const long f2, const long f3, const long faceID)
{
	return pFaceIndex[faceID].SetFaceIndex(f1,f2,f3);
}

BOOL handle::ReadhandleFile(CString path)  //���ݻ�ȡ��·������ȡ�ļ�
{
	FILE *in=NULL;  
	in = fopen(path,"r");  

	if( !in ) return FALSE;  //�ļ���ȡʧ��

	fscanf_s(in,"%d",&vertexCount);  //�ļ��ĵ�һ���ǵ�������������
	fscanf_s(in,"%d",&faceCount);

	MemoryAllocate(vertexCount,faceCount);  

	double dx,dy,dz;
	long i;
	for(i=0 ; i < vertexCount ; i++)  //��ȡ���еĵ������
	{
		fscanf_s(in,"%lf %lf %lf",&dx,&dy,&dz);
		SetPointTopPoint(dx,dy,dz,i);  //�����ڵ�������
	}

	long temp,di,dj,dk;  
	for(i=0 ; i < faceCount ; i++)  //��ȡÿ����������������
	{
		fscanf_s(in,"%d %d %d %d",&temp,&di,&dj,&dk);
		SetFaceIndexTopFaceIndex(di,dj,dk,i);

		p_PointFace[di].InsertNodeHead(i);  //ÿ��������ڶ�����У�������Щ�湹����������ڵ�ֵΪ�������
		p_PointFace[dj].InsertNodeHead(i);
		p_PointFace[dk].InsertNodeHead(i);
	}
	fclose(in);

	for( i = 0 ; i < faceCount ; i++)  
	{
		CFaceIndex f;
		
		pFaceIndex[i].GetFaceIndex(f);  

		CPoint3D vertex[3],normal;

		pPoint[ f.v1Index ].GetPoint(vertex[0]); //����ÿ���棬��ȡ���������ά����
		pPoint[ f.v2Index ].GetPoint(vertex[1]);
		pPoint[ f.v3Index ].GetPoint(vertex[2]);

		CalcFaceNormal(vertex,normal);  //������ķ���
		SetNormalTopFaceNormal(normal,i); //������ķ���
	}

	for( i = 0 ; i < vertexCount ; i++)  
	{
		CPoint3D m_normal;
		CalVertexNormal(i,m_normal);  //�����ķ���
		SetNormalTopPointNormal(m_normal,i);  //�����ķ���
	}
	return TRUE;
}

BOOL handle::CalcFaceNormal(const double v[][3], double out[])
{
	double v1[3],v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// ��������ȷ����ƽ��������ཻ����
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	//������ķ���
	out[x] = v1[y]*v2[z] - v1[z]*v2[y];
	out[y] = v1[z]*v2[x] - v1[x]*v2[z];
	out[z] = v1[x]*v2[y] - v1[y]*v2[x];

	//��������λ��
	ReduceToUnit(out);
	return TRUE;
}


BOOL handle::CalcFaceNormal(const CPoint3D point[], CPoint3D &out)
{
	double m_vertex[3][3],out1[3];

	m_vertex[0][0]=point[0].x;
	m_vertex[0][1]=point[0].y;
	m_vertex[0][2]=point[0].z;

	m_vertex[1][0]=point[1].x;
	m_vertex[1][1]=point[1].y;
	m_vertex[1][2]=point[1].z;

	m_vertex[2][0]=point[2].x;
	m_vertex[2][1]=point[2].y;
	m_vertex[2][2]=point[2].z;

	CalcFaceNormal(m_vertex,out1);

	out.x = out1[0];
	out.y = out1[1];
	out.z = out1[2];

	return TRUE;
}

//��λ������
void handle::ReduceToUnit(double vector[])
{
	double length;
	
	// ����������ģ	
	length = (double)sqrt((vector[0]*vector[0]) + 
						(vector[1]*vector[1]) +
						(vector[2]*vector[2]));

	// �����Ƚӽ�0ʱ����ֵΪ1��ֹ���
	if(length == 0.0f)
		length = 1.0f;

	// ��λ��
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}

//���������ķ��򱣴�
BOOL handle::SetNormalTopFaceNormal(const double x, const double y, const double z, const long faceID)
{
	return pFaceNormal[faceID].SetPoint(x,y,z);
}
//���������ķ��򱣴�
BOOL handle::SetNormalTopFaceNormal(const CPoint3D normal, const long faceID)
{
	return pFaceNormal[faceID].SetPoint(normal);
}

//������ĵ�ķ��򱣴�
BOOL handle::SetNormalTopPointNormal(const double x, const double y, const double z, const long pointID)
{
	return pPointNormal[pointID].SetPoint(x,y,z);
}

BOOL handle::SetNormalTopPointNormal(const CPoint3D normal, const long pointID)
{
	return pPointNormal[pointID].SetPoint(normal);
}

//����ĳ����ķ���
BOOL handle::CalVertexNormal(long pointID, double &x, double &y, double &z)
{
	ListNode<long>* pList = p_PointFace[pointID].Head;  //��������

	CPoint3D normal,normalSave;
	normal.SetPoint(0.0,0.0,0.0);  
	normalSave.SetPoint(0.0,0.0,0.0);

	int count=0;
	
	while( pList )  //���������е�ÿһ����
	{
		long faceID;
		faceID = pList->GetData();  
		
		if ( pFaceNormal[faceID].x != -1 &&      
			 pFaceNormal[faceID].y != -1 &&
			 pFaceNormal[faceID].z != -1 )
		{
			CFaceIndex faceindex;
			pFaceIndex[faceID].GetFaceIndex(faceindex);

			CPoint3D point[3];
			//�����������������������
			pPoint[faceindex.v1Index].GetPoint(point[0]);
			pPoint[faceindex.v2Index].GetPoint(point[1]);
			pPoint[faceindex.v3Index].GetPoint(point[2]);
			
			CalcFaceNormal(point,normalSave);   //�������ķ���
		}
		else
			normalSave =  pFaceNormal[faceID]; 

		normal += normalSave;
		count++;
		pList = pList->next;
	}
	if ( count == 0 )  //�õ㲻���κ�һ������
	{
		normal.x = 0;
		normal.y = 0;
		normal.z = 0;
	}
	else   //ÿ������ķ�����ڸõ����ڵ��������ƽ������
	{
		normal.x = normal.x / count;
		normal.y = normal.y / count;
		normal.z = normal.z / count;
	}

	x = normal.x;  //����ķ����������Ϊ������꣬��������ģ�͸�ƽ��
	y = normal.y;
	z = normal.z;

	return TRUE;
}

//�����ķ���
BOOL handle::CalVertexNormal(long pointID, CPoint3D &point)
{
	double x,y,z;

	CalVertexNormal(pointID,x,y,z);

	point.x = x;
	point.y = y;
	point.z = z;

	return TRUE;
}

//��ͼ����
BOOL handle::Drawhandle(bool show, bool cull, int style, bool zbuffer,int color,bool smooth,bool texture)
{
	if ( !show ) return TRUE;

	switch (style) //����������ģʽ
	{
		case 0:  //��ģʽ
			glBegin(GL_POINTS );
			   for(long point = 0; point < vertexCount; point++)
				   glVertex3f(pPoint[point].x , pPoint[point].y , pPoint[point].z);
		   glEnd();
		   return TRUE;
		case 1:   //�߿�ģʽ
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);break;
		case 2:   //���ģʽ
		    glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL );break;
		default:  //Ĭ��ʵ����ʾ�������ģʽ
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);break;
	}

	//�����������
	if(cull)  //�����޳�
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if(zbuffer)  //z����
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	switch(color) //��ɫ
	{
	   case 0:glColor3f(1.0f,1.0f,1.0f);break;   //��ɫ
	   case 1:glColor3f(0.5f,1.0f,0.5f);break;   //��ɫ
	   case 2:glColor3f(1.0f,0.5f,0.5f);break;   //��ɫ
	}

	if(smooth) //ƽ��
	{
		 glEnable (GL_SMOOTH);
	}

	glShadeModel(GL_SMOOTH); //������ɫģʽ����������ÿ���������ɫ

	for(long face = 0; face < faceCount; face++)  
	{
		CFaceIndex f;
		CPoint3D point[3],normal;

		pFaceIndex[face].GetFaceIndex(f);

		pPoint[ f.v1Index ].GetPoint(point[0]);  //ͨ��������ȡ���������
		pPoint[ f.v2Index ].GetPoint(point[1]);
		pPoint[ f.v3Index ].GetPoint(point[2]);

	    //���÷���
		glNormal3f(pFaceNormal[face].x,pFaceNormal[face].y,pFaceNormal[face].z);
		glBegin(GL_TRIANGLES);	//����������
			glVertex3f(point[0].x,point[0].y,point[0].z);
			glVertex3f(point[1].x,point[1].y,point[1].z);
			glVertex3f(point[2].x,point[2].y,point[2].z);				
		glEnd();
	}
	return TRUE;
}

/*
AUX_RGBImageRec* handle::LoadBMP(char *Filename)//����ResizeGLScene()֮ǰ  ��������λͼ
{
    FILE *File = NULL;
    if(!Filename)
    {
       return NULL;
     }
     File = fopen(Filename , "r");
     if(File)
     {
         fclose(File);
         return auxDIBImageLoad(Filename);
     }
     return NULL;
}


int handle::LoadGLTextures()
{
    int status = false;
    AUX_RGBImageRec *TextureImage[1];//��������Ĵ洢�ռ�
    memset(TextureImage, 0, sizeof(void *)*1);//��ָ����ΪNULL

   if(TextureImage[0] = LoadBMP("texture.bmp"))
   {
       status = TRUE;
       glGenTextures(1, &texture[0]);//��������

       glBindTexture(GL_TEXTURE_2D, texture[0]); //ʹ������λͼ�������ɵĵ�������

	   glTexImage2D(GL_TEXTURE_2D, 0 ,3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 
		  0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data );//��������

   }
   if(TextureImage[0])
   {
       if(TextureImage[0]->data)
      {
         free(TextureImage[0]->data);
      }
      free(TextureImage[0]);
   }
   return status;
 }*/


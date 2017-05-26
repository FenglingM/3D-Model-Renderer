#include "stdafx.h"
#include "mywork.h"
#include "handle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

handle::handle()   //构造函数
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

//根据模型的大小（点和面的数量），分配内存空间
BOOL handle::MemoryAllocate(long pointcount, long facecount)  //分配内存
{
    pPoint = new CPoint3D[pointcount];  //存放模型所有点的坐标
	pFaceIndex = new CFaceIndex[facecount];  //存放所有面的索引
	pPointNormal = new CPoint3D[pointcount];  //
	pFaceNormal = new CPoint3D[facecount];  //存放所有面的法向
	p_PointFace = new QLinkList<long> [pointcount];  //存放所有点的索引

	return TRUE;
}

//设置点坐标
BOOL handle::SetPointTopPoint(const double x, const double y, const double z,long const pointID)
{
	return pPoint[pointID].SetPoint(x,y,z);
}

//设置面的三个点索引
BOOL handle::SetFaceIndexTopFaceIndex(const long f1, const long f2, const long f3, const long faceID)
{
	return pFaceIndex[faceID].SetFaceIndex(f1,f2,f3);
}

BOOL handle::ReadhandleFile(CString path)  //根据获取的路径，读取文件
{
	FILE *in=NULL;  
	in = fopen(path,"r");  

	if( !in ) return FALSE;  //文件读取失败

	fscanf_s(in,"%d",&vertexCount);  //文件的第一行是点数量和面数量
	fscanf_s(in,"%d",&faceCount);

	MemoryAllocate(vertexCount,faceCount);  

	double dx,dy,dz;
	long i;
	for(i=0 ; i < vertexCount ; i++)  //读取所有的点的坐标
	{
		fscanf_s(in,"%lf %lf %lf",&dx,&dy,&dz);
		SetPointTopPoint(dx,dy,dz,i);  //保存在点数组中
	}

	long temp,di,dj,dk;  
	for(i=0 ; i < faceCount ; i++)  //读取每个面的三个点的索引
	{
		fscanf_s(in,"%d %d %d %d",&temp,&di,&dj,&dk);
		SetFaceIndexTopFaceIndex(di,dj,dk,i);

		p_PointFace[di].InsertNodeHead(i);  //每个点可能在多个面中，所有这些面构成链表，链表节点值为面的索引
		p_PointFace[dj].InsertNodeHead(i);
		p_PointFace[dk].InsertNodeHead(i);
	}
	fclose(in);

	for( i = 0 ; i < faceCount ; i++)  
	{
		CFaceIndex f;
		
		pFaceIndex[i].GetFaceIndex(f);  

		CPoint3D vertex[3],normal;

		pPoint[ f.v1Index ].GetPoint(vertex[0]); //对于每个面，获取三个点的三维坐标
		pPoint[ f.v2Index ].GetPoint(vertex[1]);
		pPoint[ f.v3Index ].GetPoint(vertex[2]);

		CalcFaceNormal(vertex,normal);  //计算面的法向
		SetNormalTopFaceNormal(normal,i); //保存面的法向
	}

	for( i = 0 ; i < vertexCount ; i++)  
	{
		CPoint3D m_normal;
		CalVertexNormal(i,m_normal);  //计算点的法向
		SetNormalTopPointNormal(m_normal,i);  //保存点的法向
	}
	return TRUE;
}

BOOL handle::CalcFaceNormal(const double v[][3], double out[])
{
	double v1[3],v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// 由三个点确定该平面的两个相交向量
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	//计算面的法向
	out[x] = v1[y]*v2[z] - v1[z]*v2[y];
	out[y] = v1[z]*v2[x] - v1[x]*v2[z];
	out[z] = v1[x]*v2[y] - v1[y]*v2[x];

	//将向量单位化
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

//单位化法向
void handle::ReduceToUnit(double vector[])
{
	double length;
	
	// 计算向量的模	
	length = (double)sqrt((vector[0]*vector[0]) + 
						(vector[1]*vector[1]) +
						(vector[2]*vector[2]));

	// 当长度接近0时，赋值为1防止溢出
	if(length == 0.0f)
		length = 1.0f;

	// 单位化
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}

//将计算的面的法向保存
BOOL handle::SetNormalTopFaceNormal(const double x, const double y, const double z, const long faceID)
{
	return pFaceNormal[faceID].SetPoint(x,y,z);
}
//将计算的面的法向保存
BOOL handle::SetNormalTopFaceNormal(const CPoint3D normal, const long faceID)
{
	return pFaceNormal[faceID].SetPoint(normal);
}

//将计算的点的法向保存
BOOL handle::SetNormalTopPointNormal(const double x, const double y, const double z, const long pointID)
{
	return pPointNormal[pointID].SetPoint(x,y,z);
}

BOOL handle::SetNormalTopPointNormal(const CPoint3D normal, const long pointID)
{
	return pPointNormal[pointID].SetPoint(normal);
}

//计算某个点的法向
BOOL handle::CalVertexNormal(long pointID, double &x, double &y, double &z)
{
	ListNode<long>* pList = p_PointFace[pointID].Head;  //索引队列

	CPoint3D normal,normalSave;
	normal.SetPoint(0.0,0.0,0.0);  
	normalSave.SetPoint(0.0,0.0,0.0);

	int count=0;
	
	while( pList )  //遍历队列中的每一个面
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
			//由面的索引获得三个点的索引
			pPoint[faceindex.v1Index].GetPoint(point[0]);
			pPoint[faceindex.v2Index].GetPoint(point[1]);
			pPoint[faceindex.v3Index].GetPoint(point[2]);
			
			CalcFaceNormal(point,normalSave);   //计算该面的法向
		}
		else
			normalSave =  pFaceNormal[faceID]; 

		normal += normalSave;
		count++;
		pList = pList->next;
	}
	if ( count == 0 )  //该点不在任何一个面中
	{
		normal.x = 0;
		normal.y = 0;
		normal.z = 0;
	}
	else   //每个顶点的法向等于该点所在的所有面的平均法向
	{
		normal.x = normal.x / count;
		normal.y = normal.y / count;
		normal.z = normal.z / count;
	}

	x = normal.x;  //将点的法向坐标更新为点的坐标，画出来的模型更平滑
	y = normal.y;
	z = normal.z;

	return TRUE;
}

//计算点的法向
BOOL handle::CalVertexNormal(long pointID, CPoint3D &point)
{
	double x,y,z;

	CalVertexNormal(pointID,x,y,z);

	point.x = x;
	point.y = y;
	point.z = z;

	return TRUE;
}

//画图部分
BOOL handle::Drawhandle(bool show, bool cull, int style, bool zbuffer,int color,bool smooth,bool texture)
{
	if ( !show ) return TRUE;

	switch (style) //绘制三角形模式
	{
		case 0:  //点模式
			glBegin(GL_POINTS );
			   for(long point = 0; point < vertexCount; point++)
				   glVertex3f(pPoint[point].x , pPoint[point].y , pPoint[point].z);
		   glEnd();
		   return TRUE;
		case 1:   //线框模式
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);break;
		case 2:   //填充模式
		    glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL );break;
		default:  //默认实物显示，即填充模式
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);break;
	}

	//隐藏面的消除
	if(cull)  //背面剔除
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if(zbuffer)  //z缓冲
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	switch(color) //颜色
	{
	   case 0:glColor3f(1.0f,1.0f,1.0f);break;   //白色
	   case 1:glColor3f(0.5f,1.0f,0.5f);break;   //绿色
	   case 2:glColor3f(1.0f,0.5f,0.5f);break;   //红色
	}

	if(smooth) //平滑
	{
		 glEnable (GL_SMOOTH);
	}

	glShadeModel(GL_SMOOTH); //设置着色模式，单独处理每个顶点的颜色

	for(long face = 0; face < faceCount; face++)  
	{
		CFaceIndex f;
		CPoint3D point[3],normal;

		pFaceIndex[face].GetFaceIndex(f);

		pPoint[ f.v1Index ].GetPoint(point[0]);  //通过索引获取面的三个点
		pPoint[ f.v2Index ].GetPoint(point[1]);
		pPoint[ f.v3Index ].GetPoint(point[2]);

	    //设置法向
		glNormal3f(pFaceNormal[face].x,pFaceNormal[face].y,pFaceNormal[face].z);
		glBegin(GL_TRIANGLES);	//绘制三角形
			glVertex3f(point[0].x,point[0].y,point[0].z);
			glVertex3f(point[1].x,point[1].y,point[1].z);
			glVertex3f(point[2].x,point[2].y,point[2].z);				
		glEnd();
	}
	return TRUE;
}

/*
AUX_RGBImageRec* handle::LoadBMP(char *Filename)//放在ResizeGLScene()之前  用来加载位图
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
    AUX_RGBImageRec *TextureImage[1];//创建纹理的存储空间
    memset(TextureImage, 0, sizeof(void *)*1);//将指针设为NULL

   if(TextureImage[0] = LoadBMP("texture.bmp"))
   {
       status = TRUE;
       glGenTextures(1, &texture[0]);//创建纹理

       glBindTexture(GL_TEXTURE_2D, texture[0]); //使用来自位图数据生成的典型纹理

	   glTexImage2D(GL_TEXTURE_2D, 0 ,3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 
		  0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data );//创建纹理

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


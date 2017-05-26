// handle.h: interface for the handle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_handle_H__E09906D8_CE0B_4D70_9149_7501AE5BD751__INCLUDED_)
#define AFX_handle_H__E09906D8_CE0B_4D70_9149_7501AE5BD751__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPoint3D  //3ά�ռ�����
{
public:
	double x,y,z;
	CPoint3D(double X = -1 ,double Y = -1 ,double Z = -1)  //��ʼ��
	{
		x = X;
		y = Y;
		z = Z;
	}

	BOOL SetPoint(double const X ,double const Y ,double const Z )//���õ������
	{
		x = X;
		y = Y;
		z = Z;
		return TRUE;
	}

	BOOL SetPoint(CPoint3D const point ) 
	{
		x = point.x;
		y = point.y;
		z = point.z;
		return TRUE;
	}

	BOOL GetPoint(CPoint3D &point)  //��ȡ�����ά����
	{
		point.x = x;
		point.y = y;
		point.z = z;
		return TRUE;
	}

	BOOL GetPoint(double &X, double &Y, double &Z)
	{
		X = x;
		Y = y;
		Z = z;
		return TRUE;
	}

	void operator = ( CPoint3D const point) //��ĸ�ֵ
	{
		x = point.x;
		y = point.y;
		z = point.z;
	}

	void operator += ( CPoint3D const point)  //����ӷ����㣬����ƽ��
	{
		x += point.x;
		y += point.y;
		z += point.z;
	}

	void operator  /= ( long len ) //����������㣬��������
	{
		x = x / len;
		y = y / len;
		z = z / len;
	}

	virtual ~CPoint3D(){};
};

class CFaceIndex  //���������
{
public:
	long v1Index,v2Index,v3Index;
	
	CFaceIndex(long index1 = -1 ,long index2 = -1,long index3 = -1)
	{
		v1Index = index1;
		v2Index = index2;
		v3Index = index3;
	}

	BOOL SetFaceIndex(long const index1 ,long const index2 ,long const index3 )
	{
		v1Index = index1;
		v2Index = index2;
		v3Index = index3;
		return TRUE;
	}

	BOOL SetFaceIndex(CFaceIndex const faceindex)
	{
		v1Index = faceindex.v1Index;
		v2Index = faceindex.v2Index;
		v3Index = faceindex.v3Index;
		return TRUE;
	}

	BOOL GetFaceIndex(CFaceIndex &faceindex)
	{
		faceindex.v1Index = v1Index ;
		faceindex.v2Index = v2Index;
		faceindex.v3Index = v3Index;
		return TRUE;
	}

	BOOL GetFaceIndex(long &index1 ,long &index2 ,long &index3 )
	{
		index1 = v1Index;
		index2 = v2Index;
		index3 = v3Index ;
		return TRUE;
	}

	virtual ~CFaceIndex(){};

};

#include "linklist.h"

class handle  
{
private:
	void ReduceToUnit(double vector[]);  //�����ĵ�λ��
	BOOL CalcFaceNormal(const CPoint3D point[3], CPoint3D &out);  //������ķ���
	BOOL CalcFaceNormal(double const v[3][3], double out[3]);  
	BOOL CalVertexNormal(long pointID,double &x,double &y,double &z); //����ߵķ���
	BOOL CalVertexNormal(long pointID, CPoint3D &point);
	
	BOOL MemoryAllocate(long pointcount,long facecount); //����ģ�ʹ�СΪ���������ڴ�

	BOOL SetFaceIndexTopFaceIndex(long const f1,long const f2,long const f3,long const  faceID);
	BOOL SetPointTopPoint(double const x,double const  y,double const  z,long const pointID);
	BOOL SetNormalTopPointNormal(const double x, const double y, const double z, const long pointID);
	BOOL SetNormalTopPointNormal(const CPoint3D normal, const long pointID);
	BOOL SetNormalTopFaceNormal(CPoint3D const normal,long const faceID);
	BOOL SetNormalTopFaceNormal(const double x, const double y, const double z,long const faceID);
	//AUX_RGBImageRec* LoadBMP(char *Filename);
	//int LoadGLTextures();

	CPoint3D* pPoint;	//������
	CFaceIndex* pFaceIndex;	//������
	CPoint3D* pPointNormal;//�淨��
	CPoint3D* pFaceNormal;//�㷨��
	GLuint texture[3]; //����
	
	QLinkList<long>* p_PointFace;//����ڽ�������
public:
	//����ģ�Ͳ���
	BOOL Drawhandle(bool show,bool cull,int style,bool zbuffer,int color,bool smooth,bool texture);
	BOOL ReadhandleFile(CString path);  //��ȡ�ļ����ݲ�����
	
	handle();
	virtual ~handle();

	long faceCount;  //������
	long vertexCount;  //������

};

#endif // !defined(AFX_handle_H__E09906D8_CE0B_4D70_9149_7501AE5BD751__INCLUDED_)

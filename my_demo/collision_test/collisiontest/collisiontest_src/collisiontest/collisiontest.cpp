#include <windows.h>
#include <d3dx9.h>
#include <cstdio>
#include <limits>
#include <utility>
#include <cstdlib>
#include <algorithm>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")

using namespace std;

enum TIMER_COMMAND { TIMER_RESET, TIMER_START, TIMER_STOP, TIMER_ADVANCE,
TIMER_GETABSOLUTETIME, TIMER_GETAPPTIME, TIMER_GETELAPSEDTIME };

FLOAT __stdcall DXUtil_Timer( TIMER_COMMAND command )
{
	static BOOL     m_bTimerInitialized = FALSE;
	static BOOL     m_bUsingQPF         = FALSE;
	static BOOL     m_bTimerStopped     = TRUE;
	static LONGLONG m_llQPFTicksPerSec  = 0;

	// Initialize the timer
	if( FALSE == m_bTimerInitialized )
	{
		m_bTimerInitialized = TRUE;

		// Use QueryPerformanceFrequency() to get frequency of timer.  If QPF is
		// not supported, we will timeGetTime() which returns milliseconds.
		LARGE_INTEGER qwTicksPerSec;
		m_bUsingQPF = QueryPerformanceFrequency( &qwTicksPerSec );
		if( m_bUsingQPF )
			m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
	}

	if( m_bUsingQPF )
	{
		static LONGLONG m_llStopTime        = 0;
		static LONGLONG m_llLastElapsedTime = 0;
		static LONGLONG m_llBaseTime        = 0;
		double fTime;
		double fElapsedTime;
		LARGE_INTEGER qwTime;

		// Get either the current time or the stop time, depending
		// on whether we're stopped and what command was sent
		if( m_llStopTime != 0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
			qwTime.QuadPart = m_llStopTime;
		else
			QueryPerformanceCounter( &qwTime );

		// Return the elapsed time
		if( command == TIMER_GETELAPSEDTIME )
		{
			fElapsedTime = (double) ( qwTime.QuadPart - m_llLastElapsedTime ) / (double) m_llQPFTicksPerSec;
			m_llLastElapsedTime = qwTime.QuadPart;
			return (FLOAT) fElapsedTime;
		}

		// Return the current time
		if( command == TIMER_GETAPPTIME )
		{
			double fAppTime = (double) ( qwTime.QuadPart - m_llBaseTime ) / (double) m_llQPFTicksPerSec;
			return (FLOAT) fAppTime;
		}

		// Reset the timer
		if( command == TIMER_RESET )
		{
			m_llBaseTime        = qwTime.QuadPart;
			m_llLastElapsedTime = qwTime.QuadPart;
			m_llStopTime        = 0;
			m_bTimerStopped     = FALSE;
			return 0.0f;
		}

		// Start the timer
		if( command == TIMER_START )
		{
			if( m_bTimerStopped )
				m_llBaseTime += qwTime.QuadPart - m_llStopTime;
			m_llStopTime = 0;
			m_llLastElapsedTime = qwTime.QuadPart;
			m_bTimerStopped = FALSE;
			return 0.0f;
		}

		// Stop the timer
		if( command == TIMER_STOP )
		{
			if( !m_bTimerStopped )
			{
				m_llStopTime = qwTime.QuadPart;
				m_llLastElapsedTime = qwTime.QuadPart;
				m_bTimerStopped = TRUE;
			}
			return 0.0f;
		}

		// Advance the timer by 1/10th second
		if( command == TIMER_ADVANCE )
		{
			m_llStopTime += m_llQPFTicksPerSec/10;
			return 0.0f;
		}

		if( command == TIMER_GETABSOLUTETIME )
		{
			fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;
			return (FLOAT) fTime;
		}

		return -1.0f; // Invalid command specified
	}
	else
	{
		// Get the time using timeGetTime()
		static double m_fLastElapsedTime  = 0.0;
		static double m_fBaseTime         = 0.0;
		static double m_fStopTime         = 0.0;
		double fTime;
		double fElapsedTime;

		// Get either the current time or the stop time, depending
		// on whether we're stopped and what command was sent
		if( m_fStopTime != 0.0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
			fTime = m_fStopTime;
		else
			fTime = timeGetTime() * 0.001;

		// Return the elapsed time
		if( command == TIMER_GETELAPSEDTIME )
		{   
			fElapsedTime = (double) (fTime - m_fLastElapsedTime);
			m_fLastElapsedTime = fTime;
			return (FLOAT) fElapsedTime;
		}

		// Return the current time
		if( command == TIMER_GETAPPTIME )
		{
			return (FLOAT) (fTime - m_fBaseTime);
		}

		// Reset the timer
		if( command == TIMER_RESET )
		{
			m_fBaseTime         = fTime;
			m_fLastElapsedTime  = fTime;
			m_fStopTime         = 0;
			m_bTimerStopped     = FALSE;
			return 0.0f;
		}

		// Start the timer
		if( command == TIMER_START )
		{
			if( m_bTimerStopped )
				m_fBaseTime += fTime - m_fStopTime;
			m_fStopTime = 0.0f;
			m_fLastElapsedTime  = fTime;
			m_bTimerStopped = FALSE;
			return 0.0f;
		}

		// Stop the timer
		if( command == TIMER_STOP )
		{
			if( !m_bTimerStopped )
			{
				m_fStopTime = fTime;
				m_fLastElapsedTime  = fTime;
				m_bTimerStopped = TRUE;
			}
			return 0.0f;
		}

		// Advance the timer by 1/10th second
		if( command == TIMER_ADVANCE )
		{
			m_fStopTime += 0.1f;
			return 0.0f;
		}

		if( command == TIMER_GETABSOLUTETIME )
		{
			return (FLOAT) fTime;
		}

		return -1.0f; // Invalid command specified
	}
}
using namespace std;

#define D3DFVF_MY_VERTEX	( D3DFVF_XYZ | D3DFVF_DIFFUSE )

//自定义的mesh的顶点格式
struct Vertex 
{
	float x, y, z; //顶点的坐标
	DWORD diffuse; //顶点的漫反射色
};


//OBB数据结构
struct OBB
{
	D3DXVECTOR3 vCenter;		//obb的中点坐标，基于世界坐标系
	D3DXVECTOR3 vVertex[8];		//obb的8个顶点
	float		fDims[3];		//obb的三个轴的半长
};

//AABB数据结构
struct AABB
{
	D3DXVECTOR3 vMin;	//AABB左下角坐标，其x，y，z都为最小值
	D3DXVECTOR3 vMax;	//AABB右上角坐标，其x，y，z都为最大值
};

//球包围体的数据结构
struct Sphere
{
	D3DXVECTOR3 vCenter;  //球心坐标，基于世界坐标系
	float		fRadius;  //球的半径
};

HWND					g_hWnd = NULL;
LPDIRECT3D9				g_pD3D = NULL;
LPDIRECT3DDEVICE9		g_pDevice = NULL;
LPD3DXMESH				g_pOBB1Mesh = NULL;
LPD3DXMESH				g_pOBB2Mesh = NULL;
LPD3DXMESH				g_pAABBMesh = NULL;
LPD3DXMESH				g_pSphereMesh = NULL;
LPD3DXMESH				g_pCylinderMesh = NULL;
LPD3DXFONT				g_pFont = NULL;
AABB					g_AABB;					//AABBMesh的包络盒
OBB						g_OBB1;					//obbmesh1的包络盒
OBB						g_OBB2;					//obbmesh2的包络盒
Sphere					g_Sphere;				//obbmesh3的包络盒
D3DXPLANE				g_Plane;				//plane对象
LPDIRECT3DVERTEXBUFFER9	g_pAABBVtx = NULL;		//用来进行显示绘制的AABB的顶点
LPDIRECT3DVERTEXBUFFER9	g_pOBB1Vtx = NULL;		//用来进行显示绘制的OBB1的顶点
LPDIRECT3DVERTEXBUFFER9	g_pOBB2Vtx = NULL;		//用来进行显示绘制的AABB的顶点
LPDIRECT3DVERTEXBUFFER9 g_pLineVtx = NULL;		//用来显示一个单独点的顶点缓冲区
LPDIRECT3DVERTEXBUFFER9 g_pPointVtx = NULL;		//用来显示一个线段的顶点缓冲区
LPDIRECT3DVERTEXBUFFER9 g_pPlaneVtx = NULL;		//用来显示一个平面的顶点缓冲区

float					g_fOBB1Width = 2.0f;
float					g_fOBB1Height = 1.0f;
float					g_fOBB1Depth = 1.0f;
float					g_fOBB2Width = 1.0f;
float					g_fOBB2Height = 1.0f;
float					g_fOBB2Depth = 1.0f;
float					g_fAABBWidth =12.0f;
float					g_fAABBHeight = 1.0f;
float					g_fAABBDepth = 1.0f;
D3DXMATRIX				g_matOBB1Rot;
D3DXMATRIX				g_matOBB2Rot;
float                   g_fOBB1RotAngle = 0.0f;
float					g_fOBB2RotAngle = 0.0f;
float					g_fFOO = 0.0f;
BOOL					g_bOBB1OBB2 = FALSE;
BOOL					g_bOBB1Sphere = FALSE;
BOOL					g_bOBB1Point = FALSE;
BOOL					g_bOBB1Line = FALSE;
BOOL					g_bOBB1Plane = FALSE;
BOOL					g_bOBB1Circle = FALSE;

D3DXVECTOR3				g_vPointPos(0.7f,0.0f,0.6f);
D3DXVECTOR3				g_vLineStart(3.0f,0.0f,0.6f);
D3DXVECTOR3				g_vLineEnd(0.0f,0.0f,1.0f);
D3DXVECTOR3				g_vPlaneVtx[4];


void DrawCylinder();
BOOL Intersection( const D3DXPLANE& plane , const OBB& obb );
BOOL Intersection( const D3DXVECTOR3& vCenter , 
				   const D3DXVECTOR3& vRadiusDir ,
				   const D3DXVECTOR3& vVerticalDir , 
				   const float fRadius , const OBB& obb );
BOOL IsSegmentIntersectOBB( const OBB& obb , const D3DXVECTOR3& vStart , const D3DXVECTOR3& vEnd );
BOOL IsPointInOBB( const D3DXVECTOR3& vPos , const OBB& obb );
BOOL Intersection( const OBB& obb , const D3DXVECTOR3& vPos );
BOOL Intersection( const OBB& OBB1 , const OBB& OBB2 );
BOOL Intersection( const D3DXVECTOR3& vCenter , const float fRadius , const OBB& obb );
BOOL Init();
void Shutdown();
BOOL InitMesh();
void Render();
void ComputeOBBVertex( const OBB& obb , Vertex* pVtx );
void ComputeOBBVertex( const OBB& obb , Vertex* pVtx , int nVtxNum );
BOOL UpdateBoundingBoxVertices();
void InitBoundingBox();
void UpdateOBBData( OBB& obb , const D3DXMATRIX& matTransform );
void DrawBoundingBox();
void DrawText();
void DrawOBB1Mesh();
void DrawOBB2Mesh();
void DrawSphereMesh();

void DrawCylinder()
{
	D3DXMATRIX mat1 , mat2 , mat3;
	D3DXMatrixIdentity( &mat1 );
	D3DXMatrixIdentity( &mat2 );
	D3DXMatrixRotationX( &mat1 , D3DXToRadian(90.0f));
	D3DXMatrixTranslation( &mat2 ,3.0f , 0.0f , 0.0f );
	mat3 = mat2 * mat1;
	g_pDevice->SetTransform( D3DTS_WORLD , &mat3 );
	g_pCylinderMesh->DrawSubset(0);
}
//某圆是否是和OBB包络盒相交，方法是先算出该圆所处的平面是否和OBB相交，然后再
//看根据相交区域进行判断
//vCenter是圆心坐标，vRadiusDir是某半径的方向,vOtherRadiusDir是另一个半径的方向，
//fRadius是半径的长度.该圆的方向是使用左手，从vRadiusDir转向vOtherRadiusDir的叉积，
BOOL Intersection( const D3DXVECTOR3& vCenter , 
				  const D3DXVECTOR3& vRadiusDir ,const D3DXVECTOR3& vOtherRadiusDir , const float fRadius , const OBB& obb )
{
	D3DXPLANE Circle;//首先算出圆所在的平面
	
	D3DXVECTOR3 tmp;
	D3DXVec3Cross(&tmp,&vRadiusDir , &vOtherRadiusDir );
	D3DXPlaneFromPointNormal( &Circle , &vCenter , &tmp );
	
	//如果obb和圆盘所在的平面都不相交，就直接返回false
	if( !Intersection( Circle , obb ) )
		return FALSE;
	
	//否则，就计算obb8个顶点点到平面的投影到圆心的距离，只要有一个顶点的
	//投影到圆心的距离小于半径，就证明obb和圆盘相交

	float distance_square = 0.0f; //投影到圆心的距离的平方
	D3DXVECTOR3 circle_normal(Circle.a , Circle.b , Circle.c );
	float diff = 0.0f;//OBB顶点到圆心的距离
	float diff_normal = 0.0f; //OBB顶点到圆心的距离在圆的法线上的投影
	for( int i = 0 ; i < 8 ; ++i )
	{
		diff = D3DXVec3Length( &(obb.vVertex[i]-vCenter) );
		diff_normal = D3DXVec3Dot( &(obb.vVertex[i]-vCenter) , &circle_normal);

		distance_square = diff * diff - diff_normal * diff_normal;

		if( fRadius*fRadius > distance_square )
			return TRUE;
	}

	return FALSE;
}

//计算某无限平面是否和OBB相交，
//方法是利用OBB的三个边界的半长在平面法线上的投影之和与OBB的中心点到平面的距离来比较
BOOL Intersection( const D3DXPLANE& plane , const OBB& obb )
{
	D3DXVECTOR3 vPlaneNormal(plane.a , plane.b , plane.c);
	D3DXVECTOR3 vAxis[3];
	vAxis[0] = 0.5f * ( obb.vVertex[7] - obb.vVertex[6] );
	vAxis[1] = 0.5f * ( obb.vVertex[4] - obb.vVertex[6] );
	vAxis[2] = 0.5f * ( obb.vVertex[2] - obb.vVertex[6] );

	//OK,这就是OBB对角线在平面法线的投影的半长
	float d = fabsf( D3DXVec3Dot( &vAxis[0] , &vPlaneNormal ) )+
			  fabsf( D3DXVec3Dot( &vAxis[1] , &vPlaneNormal ) )+
			  fabsf( D3DXVec3Dot( &vAxis[2] , &vPlaneNormal ) );

	//再求点到平面的距离。
	D3DXVECTOR4 v1 , v2;
	v1.x = obb.vCenter.x ;
	v1.y = obb.vCenter.y ; 
	v1.z = obb.vCenter.z ;
	v1.w = 1.0f;

	v2.x = -plane.a;
	v2.y = -plane.b;
	v2.z = -plane.c;
	v2.w = 0.0f;

	float dot = D3DXPlaneDot(&plane,&v2 );

	if( dot == 0.0f )
		return FALSE;

	//利用公式：距离D = (平面P 点乘 点Q的位置向量 ）/ (平面P 点乘 直线的方向向量)
	float diff = fabs( D3DXPlaneDot( &plane , &v1 )/dot );
	
	if( diff > d )
		return FALSE;

	return TRUE;
	
}

BOOL IsSegmentIntersectOBB(const OBB& obb , const D3DXVECTOR3& vStart , const D3DXVECTOR3& vEnd )
{
	float fNear = FLT_MIN;
	float fFar = FLT_MAX;
	const float fEpsilon = 0.00001f;

	//计算线段的长度和方向矢量
	D3DXVECTOR3 vSegmentDir;
	vSegmentDir = vEnd-vStart;
	float fSegmentLen = D3DXVec3Length( &vSegmentDir );
	D3DXVec3Normalize( &vSegmentDir , &vSegmentDir );

	//计算从OBB中点到线段起始点的连线
	D3DXVECTOR3 vDiff = obb.vCenter - vStart;

	//计算obb的沿着其X轴的边（即是width边）的方向矢量
	D3DXVECTOR3 vAxisDir[3];
	D3DXVec3Normalize( &vAxisDir[0] , &(obb.vVertex[7] - obb.vVertex[6]) );
	D3DXVec3Normalize( &vAxisDir[1] , &(obb.vVertex[4] - obb.vVertex[6]) );
	D3DXVec3Normalize( &vAxisDir[2] , &(obb.vVertex[2] - obb.vVertex[6]) );

	for( int i = 0 ; i < 3 ; ++i )
	{
		float fDot = D3DXVec3Dot( &vAxisDir[i] , &vSegmentDir );
		float r = D3DXVec3Dot( &vAxisDir[i] , &vDiff );

		//如果连线与obb的含有width边的面平行的话
		//if( fabsf(fDot) < fEpsilon )
		//{
		//	/*if( -r - obb.fDims[i] > 0.0f || -r + obb.fDims[i] > 0.0f )
		//		return FALSE;*/
		//}

		float s = D3DXVec3Dot( &vAxisDir[i] , &vSegmentDir );

		//得到射线和obb的某两个平行面之间的相交点，即最远相交点和最近相交点
		float t0 = ( r + obb.fDims[i] )/s;
		float t1 = ( r - obb.fDims[i] )/s;

		if( t0 > t1 )
			swap(t0,t1);

		if( t0 > fNear )
			fNear = t0;
		if( t1 < fFar )
			fFar = t1;

		if( fNear > fFar || fFar < 0.0f )
			return FALSE;
	}

	float fIntersect = 0.0f; //fIntersect是线段起始点与最近的交点的距离
	if( fNear > 0.0f )
		fIntersect = fNear;
	else
		fIntersect = fFar;

	if( fSegmentLen + fEpsilon > fIntersect )//还要比较线段起始点与最近的交点的距离与真实线段的长度的距离
		return TRUE;

	return FALSE;
}

BOOL IsPointInOBB( const D3DXVECTOR3& vPos , const OBB& obb )
{
	//要严格注意构造平面的参数的顺序。
	D3DXPLANE plane;
	//上顶面
	D3DXPlaneFromPoints( &plane , &obb.vVertex[2] , &obb.vVertex[0] , &obb.vVertex[3] );
	//计算n*p+d的值
	float result = D3DXPlaneDotCoord( &plane , &vPos );
	if( result > 0 )//表示已经在某个面的正侧，表示肯定在该点OBB外
		return FALSE;
	else
	{
		//下顶面
		D3DXPlaneFromPoints( &plane , &obb.vVertex[6] , &obb.vVertex[7] , &obb.vVertex[4] );
		float result = D3DXPlaneDotCoord( &plane , &vPos );
		if( result > 0 )//表示已经在某个面的正侧，表示肯定在该点OBB外
			return FALSE;
		else
		{
			//后顶面
			D3DXPlaneFromPoints( &plane , &obb.vVertex[7] , &obb.vVertex[6] , &obb.vVertex[3] );
			float result = D3DXPlaneDotCoord( &plane , &vPos );
			if( result > 0 )//表示已经在某个面的正侧，表示肯定在该点OBB外
				return FALSE;
			else
			{
				//前顶面
				D3DXPlaneFromPoints( &plane , &obb.vVertex[5] , &obb.vVertex[1] , &obb.vVertex[4] );
				float result = D3DXPlaneDotCoord( &plane , &vPos );
				if( result > 0 )//表示已经在某个面的正侧，表示肯定在该点OBB外
					return FALSE;
				else
				{
					//左顶面
					D3DXPlaneFromPoints( &plane , &obb.vVertex[2] , &obb.vVertex[6] , &obb.vVertex[0] );
					float result = D3DXPlaneDotCoord( &plane , &vPos );
					if( result > 0 )//表示已经在某个面的正侧，表示肯定在该点OBB外
						return FALSE;
					else
					{
						D3DXPlaneFromPoints( &plane , &obb.vVertex[3] , &obb.vVertex[1] , &obb.vVertex[7] );
						float result = D3DXPlaneDotCoord( &plane , &vPos );
						if( result > 0 )//表示已经在某个面的正侧，表示肯定在该点OBB外
							return FALSE;
					}
				}
			}
		}
	}

	return TRUE;
}

//绘制球体
void DrawSphereMesh()
{
	D3DXMATRIX mat;
	D3DXMatrixIdentity( &mat );
	D3DXMatrixTranslation( &mat , 0.0f , 0.0f , 1.5f );
	g_pDevice->SetTransform( D3DTS_WORLD , &mat );
	g_pSphereMesh->DrawSubset(0);
}


//判断球是否和obb相交
BOOL Intersection( const D3DXVECTOR3& vCenter , const float fRadius , const OBB& obb )
{
	//第一步，首先获取OBB中点到球中点的连线
	D3DXVECTOR3 vDiff = vCenter - obb.vCenter;
	float fS[3]    = { 0.0f, 0.0f, 0.0f };
	float fD       = 0.0f;
	
	//2,得到obb的三个轴，并且规格化
	D3DXVECTOR3 OBBAxisDir[3];
	OBBAxisDir[0] = obb.vVertex[7] - obb.vVertex[6];
	OBBAxisDir[1] = obb.vVertex[4] - obb.vVertex[6];
	OBBAxisDir[2] = obb.vVertex[2] - obb.vVertex[6];
	
	for( int i = 0 ; i < 3 ; ++i )
		D3DXVec3Normalize( &OBBAxisDir[i] , &OBBAxisDir[i] );

	const D3DXVECTOR3& OBBCenter    = obb.vCenter;
	const D3DXVECTOR3& SphereCenter = vCenter;

	D3DXVECTOR3 tmp;
	//计算连线和obb的一个轴的点积，小于零表示球心的x分量还要小于包围盒min_x端点的分量
	if( D3DXVec3Dot( &vDiff , &OBBAxisDir[0] ) < 0.0f )//也就是说把连线投影到OBBAxisDir.[0]上
	{
		//OBBCenter - OBBAxisDir.[0]*rkOBBDi.x 就是球obbX轴上的min_x端点
		//（球体的中心点－（obbX轴上的min_x端点））* OBBAxisDir.[0]，表示
		// 求从球心到obb轴上的min_x端点的连线在OBBAxisDir.[0]的投影
		tmp = SphereCenter - ( OBBCenter - ( OBBAxisDir[0] * obb.fDims[0] ) );
		fS[0] = D3DXVec3Dot( &tmp , &OBBAxisDir[0] ); 

		if( fS[0] <= -fRadius ) //如果小于的话那自然就是不相交
			return FALSE;
		else if( fS[0] < 0.0f ) //否则就计算它在X方向的分量  
			fD += fS[0] * fS[0];

		fS[0] += fRadius;
	}
	else//表示球心的x分量要大于包围盒max_x端点的分量
	{
		//同上，都是求球心到obb轴上的max_x端点的连线在OBBAxisDir.[0]的投影
		tmp = OBBCenter + OBBAxisDir[0] * obb.fDims[0]  - SphereCenter;
		fS[0] = D3DXVec3Dot( &tmp , &OBBAxisDir[0] );

		if( fS[0] <= -fRadius ) //小于的话就是不相交的
			return FALSE;
		else if( fS[0] < 0.0f )
			fD += fS[0] * fS[0];

		fS[0] += fRadius;
	}

	if( D3DXVec3Dot( &vDiff , &OBBAxisDir[1] ) < 0.0f )
	{
		tmp = SphereCenter - ( OBBCenter - OBBAxisDir[1] * obb.fDims[1] ); 
		fS[1] = D3DXVec3Dot( &tmp , &OBBAxisDir[1] );

		if( fS[1] <= -fRadius )
			return FALSE;
		else if( fS[1] < 0.0f )
			fD += fS[1] * fS[1];

		fS[1] += fRadius;
	}
	else
	{
		tmp = OBBCenter + OBBAxisDir[1] * obb.fDims[1] - SphereCenter;
		fS[1] = D3DXVec3Dot( &tmp , &OBBAxisDir[1]);

		if( fS[1] <= -fRadius )
			return FALSE;
		else if( fS[1] < 0.0f )
			fD += fS[1] * fS[1];

		fS[1] += fRadius;
	}

	if( D3DXVec3Dot( &vDiff , &OBBAxisDir[2] ) < 0.0f )
	{
		tmp = SphereCenter - ( OBBCenter - OBBAxisDir[2] * obb.fDims[2] );
		fS[2] = D3DXVec3Dot( &tmp , &OBBAxisDir[2] );

		if( fS[2] <= -fRadius )
			return FALSE;
		else if( fS[2] < 0.0f )
			fD += fS[2] * fS[2];

		fS[2] += fRadius;
	}
	else
	{
		tmp = OBBCenter + OBBAxisDir[2] * obb.fDims[2] - SphereCenter; 
		fS[2] = D3DXVec3Dot( &tmp,&OBBAxisDir[2]);

		if( fS[2] <= -fRadius )
			return FALSE;
		else if( fS[2] < 0.0f )
			fD += fS[2] * fS[2];

		fS[2] += fRadius;

	}

	
	if( fD <= ( fRadius * fRadius ) )
		return TRUE;

	return FALSE;
}
//判断两OBB是否相交
BOOL Intersection( const OBB& OBB1 , const OBB& OBB2 )
{
	const float fParallellCutoff = 0.99999f;
	BOOL bParallellAxes = FALSE;

	//第一步，首先算出两个OBB他们的3个轴的方向向量
	//局部变量OBB1AxisDir，OBB2AxisDir。存储顺序为x轴,y轴,z轴
	D3DXVECTOR3 OBB1AxisDir[3],OBB2AxisDir[3];
	//计算两个OBB的X轴方向矢量，注意OBB的顶点的顺序：从上到下，从里到外，从左到右
	OBB1AxisDir[0] = OBB1.vVertex[7] - OBB1.vVertex[6];
	OBB1AxisDir[1] = OBB1.vVertex[4] - OBB1.vVertex[6];
	OBB1AxisDir[2] = OBB1.vVertex[2] - OBB1.vVertex[6];

	OBB2AxisDir[0] = OBB2.vVertex[7] - OBB2.vVertex[6];
	OBB2AxisDir[1] = OBB2.vVertex[4] - OBB2.vVertex[6];
	OBB2AxisDir[2] = OBB2.vVertex[2] - OBB2.vVertex[6];

	//把轴矢量normalize
	for( int i = 0 ; i < 3 ; ++i )
	{
		D3DXVec3Normalize( &OBB1AxisDir[i] , &OBB1AxisDir[i] );
		D3DXVec3Normalize( &OBB2AxisDir[i] , &OBB2AxisDir[i] );
	}

	//第二步，计算两OBB中点的连线，从OBB1指向OBB2,以及各个轴之间的点积
	D3DXVECTOR3 vCenterDiff = OBB2.vCenter - OBB1.vCenter;

	float afAxisDot[3][3];    // afAxisDot[i][j] = OBB1AxisDir[i] * OBB2AxisDir[j];
	float afAbsAxisDot[3][3]; // afAbsAxisDot[i][j] = |afAxisDot[i][j]|
	float afAxesADotDiff[3];  // OBB1AxisDir[i]  = OBBAxisDir.A[i] * vDiff

	float fProj;

	/*第三步，根据分离轴定理,分别以OBB的三个轴为分离轴*/
	float fOverlap;
	//1 判断以OBB1的X轴为分离轴的情况
	{
		afAxisDot[0][0] = D3DXVec3Dot( &OBB1AxisDir[0] , &OBB2AxisDir[0] );
		afAxisDot[0][1] = D3DXVec3Dot( &OBB1AxisDir[0] , &OBB2AxisDir[1] );
		afAxisDot[0][2] = D3DXVec3Dot( &OBB1AxisDir[0] , &OBB2AxisDir[2] );
		//获取点积的绝对值，如果点积的绝对值>0.9999f,也就是说cos值接近于1，就表示
		//这两个轴之间实质上是平行的（或者是非常近似于平行）
		afAbsAxisDot[0][0] = fabsf( afAxisDot[0][0] );  
		if( afAbsAxisDot[0][0] > fParallellCutoff ) 
			bParallellAxes = TRUE;
		afAbsAxisDot[0][1] = fabsf( afAxisDot[0][1] ); 
		if( afAbsAxisDot[0][1] > fParallellCutoff ) 
			bParallellAxes = TRUE;
		afAbsAxisDot[0][2] = fabsf( afAxisDot[0][2] ); 
		if( afAbsAxisDot[0][2] > fParallellCutoff ) 
			bParallellAxes = TRUE;

		//计算1盒X轴与1盒与2盒子的中点连线的点积，目的是
		//为了计算中点连线在1盒的其中一条轴上的投影的长度
		afAxesADotDiff[0]  = D3DXVec3Dot( &OBB1AxisDir[0] , &vCenterDiff );
		//检查中点连线在1盒子X轴上的投影是否大于2盒子的半边与2盒子的半边在
		//轴上的投影之和，如果大于的话，那么就表示这两个OBB盒子是相离的。
		fOverlap = fabsf( afAxesADotDiff[0] ) - ( OBB1.fDims[0] + 
			OBB2.fDims[0] * afAbsAxisDot[0][0] + 
			OBB2.fDims[1] * afAbsAxisDot[0][1] + 
			OBB2.fDims[2] * afAbsAxisDot[0][2]);
		if( fOverlap > 0.0f )
			return false;
	}

	//2 判断以OBB1的Z轴为分离轴的情况，和以X轴为分离轴的是一样的
	{
		afAxisDot[1][0] = D3DXVec3Dot( &OBB1AxisDir[1] , &OBB2AxisDir[0] );
		afAxisDot[1][1] = D3DXVec3Dot( &OBB1AxisDir[1] , &OBB2AxisDir[1] );
		afAxisDot[1][2] = D3DXVec3Dot( &OBB1AxisDir[1] , &OBB2AxisDir[2] );
		//获取点积的绝对值，如果点积的绝对值>0.9999f,也就是说cos值接近于1，就表示
		//这两个轴之间实质上是平行的（或者是非常近似于平行）
		afAbsAxisDot[1][0] = fabsf( afAxisDot[1][0] );  
		if( afAbsAxisDot[1][0] > fParallellCutoff ) 
			bParallellAxes = TRUE;

		afAbsAxisDot[1][1] = fabsf( afAxisDot[1][1] ); 
		if( afAbsAxisDot[1][1] > fParallellCutoff ) 
			bParallellAxes = TRUE;

		afAbsAxisDot[1][2] = fabsf( afAxisDot[1][2] ); 
		if( afAbsAxisDot[1][2] > fParallellCutoff ) 
			bParallellAxes = TRUE;


		afAxesADotDiff[1] = D3DXVec3Dot( &OBB1AxisDir[1] , &vCenterDiff );

		fOverlap = fabsf( afAxesADotDiff[1] ) - ( OBB1.fDims[1] + 
			OBB2.fDims[0] * afAbsAxisDot[1][0] + 
			OBB2.fDims[1] * afAbsAxisDot[1][1] + 
			OBB2.fDims[2] * afAbsAxisDot[1][2] );
		if( fOverlap > 0.0f )
			return FALSE;
	}
	//3 判断以OBB1的Y轴为分离轴的情况
	{
		afAxisDot[2][0] = D3DXVec3Dot( &OBB1AxisDir[2] , &OBB2AxisDir[0] );
		afAxisDot[2][1] = D3DXVec3Dot( &OBB1AxisDir[2] , &OBB2AxisDir[1] );
		afAxisDot[2][2] = D3DXVec3Dot( &OBB1AxisDir[2] , &OBB2AxisDir[2] );

		afAbsAxisDot[2][0] = fabsf( afAxisDot[2][0] );  
		if( afAbsAxisDot[2][0] > fParallellCutoff ) 
			bParallellAxes = TRUE;

		afAbsAxisDot[2][1] = fabsf( afAxisDot[2][1] ); 
		if( afAbsAxisDot[2][1] > fParallellCutoff ) 
			bParallellAxes = TRUE;

		afAbsAxisDot[2][2] = fabsf( afAxisDot[2][2] ); 
		if( afAbsAxisDot[2][2] > fParallellCutoff ) 
			bParallellAxes = TRUE;

		afAxesADotDiff[2] = D3DXVec3Dot( &OBB1AxisDir[2] , &vCenterDiff );

		fOverlap = fabsf( afAxesADotDiff[2] ) - ( OBB1.fDims[2] + 
			OBB2.fDims[0] * afAbsAxisDot[2][0] + 
			OBB2.fDims[1] * afAbsAxisDot[2][1] + 
			OBB2.fDims[2] * afAbsAxisDot[2][2]);
		if( fOverlap > 0.0f )
			return FALSE;
	}

	//4 判断以0BB2的X轴为分离轴的情况,和OB1的原理一致
	{
		//因为前面现在已经计算了轴和轴之间的点积了。
		//现在可以直接计算中点连线在B盒X轴的投影
		fProj = D3DXVec3Dot( &OBB2AxisDir[0] , &vCenterDiff );
		fOverlap = fabsf( fProj ) - ( OBB2.fDims[0] + 
			OBB1.fDims[0] * afAbsAxisDot[0][0] + 
			OBB1.fDims[1] * afAbsAxisDot[1][0] + 
			OBB1.fDims[2] * afAbsAxisDot[2][0]);
		if( fOverlap > 0.0f )
			return FALSE;
	}
	//5 判断以0BB2的z轴为分离轴的情况
	{
		fProj = D3DXVec3Dot( &OBB2AxisDir[1] , &vCenterDiff );
		fOverlap = fabsf( fProj ) - ( OBB2.fDims[1] + 
			OBB1.fDims[0] * afAbsAxisDot[0][1] + 
			OBB1.fDims[1] * afAbsAxisDot[1][1] + 
			OBB1.fDims[2] * afAbsAxisDot[2][1]);
		if( fOverlap > 0.0f )
			return FALSE;

	}
	//6 判断以0BB2的Y轴为分离轴的情况
	{
		fProj = D3DXVec3Dot( &OBB2AxisDir[2] , &vCenterDiff );
		fOverlap = fabsf( fProj ) - ( OBB2.fDims[2] + 
			OBB1.fDims[0] * afAbsAxisDot[0][2] + 
			OBB1.fDims[1] * afAbsAxisDot[1][2] + 
			OBB1.fDims[2] * afAbsAxisDot[2][2]);
		if( fOverlap > 0.0f )
			return FALSE;
	}
	//如果两OBB是平行的话就以其轴与轴之间的叉积为分离轴
	if( !bParallellAxes )
	{
		//7 判断以OBB1的X轴和OBB2的X轴的叉积为分离轴的情况
		{
			fProj = afAxisDot[1][0] * afAxesADotDiff[2] - afAxisDot[2][0] * afAxesADotDiff[1];
			fOverlap = fabsf(fProj) - ( ( OBB1.fDims[1] * afAbsAxisDot[2][0] + OBB1.fDims[2] * afAbsAxisDot[1][0] ) + 
				( OBB2.fDims[1] * afAbsAxisDot[0][2] + OBB2.fDims[2] * afAbsAxisDot[0][1] ) );
			if( fOverlap > 0.0f )
				return false;
		}
		//8 判断以OBB1的X轴和OBB2的Y轴的叉积为分离轴的情况
		{
			fProj = afAxisDot[1][1] * afAxesADotDiff[2] - afAxisDot[2][1] * afAxesADotDiff[1];
			fOverlap = fabsf(fProj) - ( (OBB1.fDims[1] * afAbsAxisDot[2][1] + OBB1.fDims[2] * afAbsAxisDot[1][1] ) + 
				(OBB2.fDims[0] * afAbsAxisDot[0][2] + OBB2.fDims[2] * afAbsAxisDot[0][0] ) );
			if( fOverlap > 0.0f )
				return false;
		}
		//9 判断以OBB1的X轴和OBB2的Z轴的叉积为分离轴的情况
		{
			fProj = afAxisDot[1][2] * afAxesADotDiff[2] - afAxisDot[2][2] * afAxesADotDiff[1];
			fOverlap = fabsf(fProj) - ( ( OBB1.fDims[1] * afAbsAxisDot[2][2] + OBB1.fDims[2] * afAbsAxisDot[1][2] ) + 
				( OBB2.fDims[0] * afAbsAxisDot[0][1] + OBB2.fDims[1] * afAbsAxisDot[0][0] ) );
			if( fOverlap > 0.0f )
				return false;
		}
		//10 判断以OBB1的Y轴和OBB2的X轴的叉积为分离轴的情况
		{
			fProj = afAxisDot[2][0] * afAxesADotDiff[0] - afAxisDot[0][0] * afAxesADotDiff[2];
			fOverlap = fabsf( fProj ) - ( ( OBB1.fDims[0] * afAbsAxisDot[2][0] + OBB1.fDims[2] * afAbsAxisDot[0][0] ) + 
				( OBB2.fDims[1] * afAbsAxisDot[1][2] + OBB2.fDims[2] * afAbsAxisDot[1][1] ) );
			if( fOverlap > 0.0f )
				return false;
		}
		//11 判断以OBB2的Y轴和OBB2的Y轴的叉积为分离轴的情况
		{
			fProj = afAxisDot[2][1] * afAxesADotDiff[0] - afAxisDot[0][1] * afAxesADotDiff[2];
			fOverlap = fabsf(fProj) - ( (OBB1.fDims[0] * afAbsAxisDot[2][1] + OBB1.fDims[2] * afAbsAxisDot[0][1] ) + 
				(OBB2.fDims[0] * afAbsAxisDot[1][2] + OBB2.fDims[2] * afAbsAxisDot[1][0] ) );
			if( fOverlap > 0.0f )
				return false;
		}
		//12 判断以OBB3的Y轴和OBB2的Z轴的叉积为分离轴的情况
		{
			fProj = afAxisDot[2][2] * afAxesADotDiff[0] - afAxisDot[0][2] * afAxesADotDiff[2];
			fOverlap = fabsf( fProj ) - ( ( OBB1.fDims[0] * afAbsAxisDot[2][2] + OBB1.fDims[2] * afAbsAxisDot[0][2] ) + 
				( OBB2.fDims[0] * afAbsAxisDot[1][1] + OBB2.fDims[1] * afAbsAxisDot[1][0] ) );
			if( fOverlap > 0.0f )
				return false;
		}
		//13 判断以OBB1的Z轴和OBB2的X轴的叉积为分离轴的情况
		{
			fProj = afAxisDot[0][0] * afAxesADotDiff[1] - afAxisDot[1][0] * afAxesADotDiff[0];
			fOverlap = fabsf(fProj) - ( ( OBB1.fDims[0] * afAbsAxisDot[1][0] + OBB1.fDims[1] * afAbsAxisDot[0][0] ) +
				( OBB2.fDims[1] * afAbsAxisDot[2][2] + OBB2.fDims[2] * afAbsAxisDot[2][1] ) );
			if( fOverlap > 0.0f )
				return false;
		}
		//14 判断以OBB2的Z轴和OBB2的Y轴的叉积为分离轴的情况
		{
			fProj = afAxisDot[0][1] * afAxesADotDiff[1] - afAxisDot[1][1] * afAxesADotDiff[0];
			fOverlap = fabsf(fProj) - ( ( OBB1.fDims[0] * afAbsAxisDot[1][1] + OBB1.fDims[1] * afAbsAxisDot[0][1] ) + 
				( OBB2.fDims[0] * afAbsAxisDot[2][2] + OBB2.fDims[2] * afAbsAxisDot[2][0] ) );
			if( fOverlap > 0.0f )
				return false;
		}
		//15 判断以OBB3的Z轴和OBB2的Z轴的叉积为分离轴的情况
		{
			fProj = afAxisDot[0][2] * afAxesADotDiff[1] - afAxisDot[1][2] * afAxesADotDiff[0];
			fOverlap = fabsf(fProj) - ( ( OBB1.fDims[0] * afAbsAxisDot[1][2] + OBB1.fDims[1] * afAbsAxisDot[0][2] ) + 
				( OBB2.fDims[0] * afAbsAxisDot[2][1] + OBB2.fDims[1] * afAbsAxisDot[2][0] ) );
			if( fOverlap > 0.0f )
				return false;
		}
	}

	return TRUE;
}


//绘制包络盒
void DrawBoundingBox()
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	g_pDevice->SetTransform( D3DTS_WORLD , &matIdentity );
	//绘制obb1的包络盒
	g_pDevice->SetFVF( D3DFVF_MY_VERTEX );
	g_pDevice->SetStreamSource( 0 , g_pOBB1Vtx , 0 , sizeof(Vertex) );
	g_pDevice->DrawPrimitive( D3DPT_LINELIST , 0 , 12 );

	//绘制obb2的包络盒
	g_pDevice->SetFVF( D3DFVF_MY_VERTEX );
	g_pDevice->SetStreamSource( 0 , g_pOBB2Vtx , 0 , sizeof(Vertex) );
	g_pDevice->DrawPrimitive( D3DPT_LINELIST , 0 , 12 );

}

//更新包络盒的数据信息,注意，现在是直接把obb的坐标，方向轴等等都进行改变
void UpdateOBBData( OBB& obb , const D3DXMATRIX& matTransform )
{
	D3DXVECTOR4 out;
	for( int i = 0 ; i < 8 ; ++i )
	{
		D3DXVec3Transform( &out , &obb.vVertex[i] , &matTransform );
		obb.vVertex[i].x = out.x;
		obb.vVertex[i].y = out.y;
		obb.vVertex[i].z = out.z;
	}
	//更新中点
	D3DXVec3Transform( &out , &obb.vCenter , &matTransform );
	obb.vCenter.x = out.x;
	obb.vCenter.y = out.y;
	obb.vCenter.z = out.z;	
	//更新半长
	obb.fDims[0] = 0.5f * D3DXVec3Length( &(obb.vVertex[7]-obb.vVertex[6]) );
	obb.fDims[1] = 0.5f * D3DXVec3Length( &(obb.vVertex[4]-obb.vVertex[6]) );
	obb.fDims[2] = 0.5f * D3DXVec3Length( &(obb.vVertex[2]-obb.vVertex[6]) );
}
//初始化所有包络盒信息
void InitBoundingBox()
{
	//初始化obb1
	g_OBB1.fDims[0] = g_fOBB1Width * 0.5f;
	g_OBB1.fDims[1] = g_fOBB1Depth * 0.5f;
	g_OBB1.fDims[2] = g_fOBB1Height * 0.5f;
	g_OBB1.vCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
	//初始化8个顶点
	g_OBB1.vVertex[0] = D3DXVECTOR3(-1.0f,0.5f,0.5f);
	g_OBB1.vVertex[1] = D3DXVECTOR3(1.0f,0.5f,0.5f);
	g_OBB1.vVertex[2] =	D3DXVECTOR3(-1.0f,0.5f,-0.5f);
	g_OBB1.vVertex[3] = D3DXVECTOR3(1.0f,0.5f,-0.5f);
	g_OBB1.vVertex[4] = D3DXVECTOR3(-1.0f,-0.5f,0.5f);
	g_OBB1.vVertex[5] = D3DXVECTOR3(1.0f,-0.5f,0.5f);
	g_OBB1.vVertex[6] =	D3DXVECTOR3(-1.0f,-0.5f,-0.5f);
	g_OBB1.vVertex[7] = D3DXVECTOR3(1.0f,-0.5f,-0.5f);
	
	//初始化obb2
	g_OBB2.fDims[0] = g_fOBB2Width * 0.5f;
	g_OBB2.fDims[1] = g_fOBB2Depth * 0.5f;
	g_OBB2.fDims[2] = g_fOBB2Height * 0.5f;
	g_OBB2.vCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
	
	//初始化8个顶点
	g_OBB2.vVertex[0] = D3DXVECTOR3(-0.5f,0.5f,0.5f);
	g_OBB2.vVertex[1] = D3DXVECTOR3(0.5f,0.5f,0.5f);
	g_OBB2.vVertex[2] =	D3DXVECTOR3(-0.5f,0.5f,-0.5f);
	g_OBB2.vVertex[3] = D3DXVECTOR3(0.5f,0.5f,-0.5f);
	g_OBB2.vVertex[4] = D3DXVECTOR3(-0.5f,-0.5f,0.5f);
	g_OBB2.vVertex[5] = D3DXVECTOR3(0.5f,-0.5f,0.5f);
	g_OBB2.vVertex[6] =	D3DXVECTOR3(-0.5f,-0.5f,-0.5f);
	g_OBB2.vVertex[7] = D3DXVECTOR3(0.5f,-0.5f,-0.5f);
	
	//把obb平移到(1.0f,0.0f,1.0f)
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	D3DXMatrixTranslation( &matIdentity ,0.0f , 0.0f , -1.5f );
	UpdateOBBData( g_OBB2 , matIdentity );

	//初始化aabb
	g_AABB.vMin = D3DXVECTOR3( -g_fOBB2Width*0.5f , -g_fOBB2Height*0.5f , -g_fOBB2Height*0.5f);
	g_AABB.vMax = D3DXVECTOR3( g_fOBB2Width*0.5f , g_fOBB2Height*0.5f , g_fOBB2Height*0.5f);

	//初始化好包络盒的相关代码
	UpdateBoundingBoxVertices();
}



BOOL Init()
{
	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	D3DDISPLAYMODE d3ddm;

	g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	d3dpp.Windowed               = TRUE;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat       = d3ddm.Format;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

	g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pDevice );

	//设置渲染状态
	g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	//设置world，view，projection矩阵
	D3DXMATRIX matTransform;
	D3DXMatrixPerspectiveFovLH( &matTransform, D3DXToRadian( 45.0f ), 640.0f / 480.0f, 1.0f, 100.0f );
	g_pDevice->SetTransform( D3DTS_PROJECTION, &matTransform );
	D3DXMatrixLookAtLH( &matTransform , &D3DXVECTOR3(0.0f,15.0f,0.0f),
						&D3DXVECTOR3(0.0f,0.0f,0.0f),&D3DXVECTOR3(0.0f,0.0f,1.0f));
	g_pDevice->SetTransform( D3DTS_VIEW , &matTransform );
	D3DXMatrixIdentity( &matTransform );
	g_pDevice->SetTransform( D3DTS_WORLD , &matTransform );
	
	//初始化线段顶点和单个顶点的坐标，缓冲区
	
	BYTE* pVertices = NULL;
	Vertex linevtx[2];
	Vertex pointvtx;
	Vertex planevtx[8];

	linevtx[0].diffuse = 0xFF0000FF;
	linevtx[0].x = g_vLineStart.x; 
	linevtx[0].y = g_vLineStart.y;
	linevtx[0].z = g_vLineStart.z;
	linevtx[1].diffuse = 0xFF0000FF;
	linevtx[1].x = g_vLineEnd.x; 
	linevtx[1].y = g_vLineEnd.y; 
	linevtx[1].z = g_vLineEnd.z;
	
	
	pointvtx.diffuse = 0xFFFF0000;
	pointvtx.x = g_vPointPos.x;
	pointvtx.y = g_vPointPos.y ;
	pointvtx.z = g_vPointPos.z;

	//平面的四个顶点
	g_vPlaneVtx[0] = D3DXVECTOR3(-0.5f,-0.5f,1.9f);
	g_vPlaneVtx[1] = D3DXVECTOR3(0.5f,-0.5f,1.9f);
	g_vPlaneVtx[2] = D3DXVECTOR3(-0.5f,0.5f,0.9f);
	g_vPlaneVtx[3] = D3DXVECTOR3(0.5f,0.5f,0.9f);

	//初始化平面
	D3DXPlaneFromPoints( &g_Plane , &g_vPlaneVtx[2],
						 &g_vPlaneVtx[0] , &g_vPlaneVtx[3] );

	//初始化平面的四个顶点
	const int index[8] = {0,1,1,3,3,2,2,0};
	
	for( int i = 0 ; i < 8 ; ++i )
	{
		planevtx[i].diffuse = 0xFFFF00FF;
		planevtx[i].x = g_vPlaneVtx[index[i]].x;
		planevtx[i].y = g_vPlaneVtx[index[i]].y;
		planevtx[i].z = g_vPlaneVtx[index[i]].z;
	}

	HRESULT hr = g_pDevice->CreateVertexBuffer( 2*sizeof(Vertex), 0, D3DFVF_MY_VERTEX,
		D3DPOOL_DEFAULT, &g_pLineVtx,NULL );

	hr = g_pLineVtx->Lock( 0, 2*sizeof(Vertex), (void**)&pVertices, 0 );
	memcpy( pVertices, linevtx, sizeof(linevtx) );
	g_pLineVtx->Unlock();

	g_pDevice->CreateVertexBuffer( sizeof(Vertex), 0, D3DFVF_MY_VERTEX,
		D3DPOOL_DEFAULT, &g_pPointVtx,NULL );

	hr = g_pPointVtx->Lock( 0, sizeof(Vertex), (void**)&pVertices, 0 );
	memcpy( pVertices, &pointvtx, sizeof(pointvtx) );
	g_pPointVtx->Unlock();

	g_pDevice->CreateVertexBuffer( 8*sizeof(Vertex), 0, D3DFVF_MY_VERTEX,
		D3DPOOL_DEFAULT, &g_pPlaneVtx,NULL );

	hr = g_pPlaneVtx->Lock( 0, 8*sizeof(Vertex), (void**)&pVertices, 0 );
	memcpy( pVertices, &planevtx, sizeof(planevtx) );
	g_pPlaneVtx->Unlock();




	//初始化mesh
	InitMesh();
	InitBoundingBox();
	UpdateOBBData( g_OBB1 , matTransform );


	D3DXMatrixIdentity( &g_matOBB1Rot );
	D3DXMatrixIdentity( &g_matOBB2Rot );

	HDC hDC;
	int nHeight;
	int nPointSize = 18;
	hDC = GetDC( NULL );
	nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );
	ReleaseDC( NULL, hDC );

	hr = D3DXCreateFont( g_pDevice, nHeight*0.5f, 0, FW_BOLD, 0, FALSE, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), 
		&g_pFont );

	return TRUE;
}

//释放所有的D3D对象
void Shutdown()
{
	if( g_pCylinderMesh )
		g_pCylinderMesh->Release();

	if( g_pPlaneVtx )
		g_pPlaneVtx->Release();

	if( g_pPointVtx )
		g_pPointVtx->Release();

	if( g_pLineVtx )
		g_pLineVtx->Release();

	if( g_pFont )
		g_pFont->Release();

	if( g_pSphereMesh )
		g_pSphereMesh->Release();
	
	if( g_pAABBMesh )
		g_pAABBMesh->Release();

	if( g_pOBB1Mesh )
		g_pOBB1Mesh->Release();

	if( g_pOBB2Mesh )
		g_pOBB2Mesh->Release();

	if( g_pAABBVtx )
		g_pAABBVtx->Release();

	if( g_pOBB1Vtx )
		g_pOBB1Vtx->Release();

	if( g_pOBB2Vtx )
		g_pOBB2Vtx->Release();

	if( g_pDevice )
		g_pDevice->Release();

	if( g_pD3D )
		g_pD3D->Release();
}
//初始化所有的mesh
BOOL InitMesh()
{
	LPD3DXMESH pTmpMesh = NULL;
	LPDIRECT3DVERTEXBUFFER9 pTempVertexBuffer = NULL;
	Vertex *pVertices = NULL;
	int nNumVerts = 0;
	
	//创建OBB1
	D3DXCreateBox(g_pDevice, g_fOBB1Width, g_fOBB1Height, g_fOBB1Depth , &pTmpMesh, NULL);
	pTmpMesh->CloneMeshFVF( 0, D3DFVF_MY_VERTEX, g_pDevice, &g_pOBB1Mesh );
	if( SUCCEEDED( g_pOBB1Mesh->GetVertexBuffer( &pTempVertexBuffer ) ) )
	{
		nNumVerts = g_pOBB1Mesh->GetNumVertices();
		pTempVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );

		for( int i = 0; i < nNumVerts; ++i )
				pVertices[i].diffuse = D3DCOLOR_COLORVALUE( 0.0, 0.0, 1.0, 1.0 );

		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
		pTmpMesh->Release();
	}
	else
		return FALSE;

	//创建OBB2
	D3DXCreateBox(g_pDevice, g_fOBB2Width, g_fOBB2Height, g_fOBB2Depth , &pTmpMesh, NULL);
	pTmpMesh->CloneMeshFVF( 0, D3DFVF_MY_VERTEX, g_pDevice, &g_pOBB2Mesh );
	if( SUCCEEDED( g_pOBB2Mesh->GetVertexBuffer( &pTempVertexBuffer ) ) )
	{
		nNumVerts = g_pOBB2Mesh->GetNumVertices();

		pTempVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );
		{
			for( int i = 0; i < nNumVerts; ++i )
				pVertices[i].diffuse = D3DCOLOR_COLORVALUE( 1.0, 1.0, 0.0, 1.0 );
		}
		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
		pTmpMesh->Release();
	}
	else
		return FALSE;

	//创建AABB
	D3DXCreateBox(g_pDevice, g_fAABBWidth, g_fAABBHeight, g_fAABBDepth , &pTmpMesh, NULL);
	pTmpMesh->CloneMeshFVF( 0, D3DFVF_MY_VERTEX, g_pDevice, &g_pAABBMesh );
	if( SUCCEEDED( g_pAABBMesh->GetVertexBuffer( &pTempVertexBuffer ) ) )
	{
		nNumVerts = g_pAABBMesh->GetNumVertices();

		pTempVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );
		{
			for( int i = 0; i < nNumVerts; ++i )
				pVertices[i].diffuse = D3DCOLOR_COLORVALUE( 1.0, 1.0, 0.0, 1.0 );
		}
		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
		pTmpMesh->Release();
	}
	else
		return FALSE;

	//创建Sphere
	D3DXCreateSphere(g_pDevice, 0.5f, 20, 20 , &pTmpMesh, NULL);
	pTmpMesh->CloneMeshFVF( 0, D3DFVF_MY_VERTEX, g_pDevice, &g_pSphereMesh );
	if( SUCCEEDED( g_pSphereMesh->GetVertexBuffer( &pTempVertexBuffer ) ) )
	{
		nNumVerts = g_pSphereMesh->GetNumVertices();

		pTempVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );
		{
			for( int i = 0; i < nNumVerts; ++i )
				pVertices[i].diffuse = D3DCOLOR_COLORVALUE( 1.0, 1.0, 0.0, 1.0 );
		}
		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
		pTmpMesh->Release();
	}
	else
		return FALSE;

	//创建cylinder
	D3DXCreateCylinder( g_pDevice , 2.0f , 2.0f , 0.0f,20,20 , &g_pCylinderMesh,NULL );
	pTmpMesh->CloneMeshFVF( 0, D3DFVF_MY_VERTEX, g_pDevice, &g_pCylinderMesh );
	if( SUCCEEDED( g_pCylinderMesh->GetVertexBuffer( &pTempVertexBuffer ) ) )
	{
		nNumVerts = g_pCylinderMesh->GetNumVertices();

		pTempVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );
		{
			for( int i = 0; i < nNumVerts; ++i )
				pVertices[i].diffuse = D3DCOLOR_COLORVALUE( 1.0, 0.0, 0.0, 1.0 );
		}
		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
		pTmpMesh->Release();
	}
	else
		return FALSE;
	
	return TRUE;
}


//初始化/更新包络盒顶点,注意这是绘制用的顶点
BOOL UpdateBoundingBoxVertices()
{
	HRESULT		hr;
	Vertex		bounding_box_vtx[24];
	const int	index[24] = {0,1,1,3,3,2,2,0,4,5,5,7,7,6,6,4,0,4,1,5,3,7,2,6};
	
	//更新24个顶点
	if( !g_pOBB1Vtx )
	{
		hr = g_pDevice->CreateVertexBuffer( 24*sizeof(Vertex), 0, D3DFVF_MY_VERTEX,
												D3DPOOL_DEFAULT, &g_pOBB1Vtx,NULL );
		if( FAILED(hr) )
			return FALSE;
	}
	
	//接着把这些值赋值到绘制包络盒的临时顶点缓冲区去
	for( int i = 0 ; i < 24 ; ++i )
	{
		bounding_box_vtx[i].x = g_OBB1.vVertex[index[i]].x;
		bounding_box_vtx[i].y = g_OBB1.vVertex[index[i]].y;
		bounding_box_vtx[i].z = g_OBB1.vVertex[index[i]].z;
		bounding_box_vtx[i].diffuse = 0xFFFFFFFF;
	}
	//OK,现在可以拷贝的真正的顶点缓冲区中去了
	BYTE* pVertices = NULL;
	hr = g_pOBB1Vtx->Lock( 0, 24*sizeof(Vertex), (void**)&pVertices, 0 );
	if( FAILED(hr) )
		return FALSE;
	memcpy( pVertices, bounding_box_vtx, sizeof(bounding_box_vtx) );
	g_pOBB1Vtx->Unlock();
	
	
	//更新24个顶点
	if( !g_pOBB2Vtx )
	{
		hr = g_pDevice->CreateVertexBuffer( 24*sizeof(Vertex), 0, D3DFVF_MY_VERTEX,
			D3DPOOL_DEFAULT, &g_pOBB2Vtx,NULL );
		if( FAILED(hr) )
			return FALSE;
	}
	
	//接着把这些值赋值到绘制包络盒的临时顶点缓冲区去
	for( i = 0 ; i < 24 ; ++i )
	{
		bounding_box_vtx[i].x = g_OBB2.vVertex[index[i]].x;
		bounding_box_vtx[i].y = g_OBB2.vVertex[index[i]].y;
		bounding_box_vtx[i].z = g_OBB2.vVertex[index[i]].z;
		bounding_box_vtx[i].diffuse = 0xFFFFFFFF;
	}
	//OK,现在可以拷贝的真正的顶点缓冲区中去了
	pVertices = NULL;
	hr = g_pOBB2Vtx->Lock( 0, 24*sizeof(Vertex), (void**)&pVertices, 0 );
	if( FAILED(hr) )
		return FALSE;
	memcpy( pVertices, bounding_box_vtx, sizeof(bounding_box_vtx) );
	g_pOBB2Vtx->Unlock();
	
	//获取AABB包围盒的代码在此在此
	return TRUE;
	
}
//显示文字
void DrawText()
{
	const char* msg1 = "OBB1 intersect with sphere";
	const char* msg2 = "OBB1 intersect with OBB2";
	const char* msg3 = "Single point is in the OBB1";
	const char* msg4 = "Segment intersect with obb1";
	const char* msg5 = "Plane intersect with OBB1";
	const char* msg6 = "circle intersect with obb1";
	RECT destRect;
	SetRect( &destRect, 0, 0, 100, 100 );

	if( g_bOBB1OBB2 )
	{
		g_pFont->DrawText( NULL,msg2, -1, &destRect, DT_NOCLIP, 
		D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) );
	}
	if( g_bOBB1Sphere )
	{
		SetRect( &destRect, 0, 20, 100, 120 );
		g_pFont->DrawText( NULL,msg1, -1, &destRect, DT_NOCLIP, 
			D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) );
	}
	if( g_bOBB1Point )
	{
		SetRect( &destRect, 0, 30, 100, 130 );
		g_pFont->DrawText( NULL,msg3, -1, &destRect, DT_NOCLIP, 
			D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) );
	}
	if( g_bOBB1Line )
	{
		SetRect( &destRect, 0, 40, 100, 140 );
		g_pFont->DrawText( NULL,msg4, -1, &destRect, DT_NOCLIP, 
			D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) );
	}
	if( g_bOBB1Plane )
	{
		SetRect(&destRect, 0, 50, 100, 150 );
		g_pFont->DrawText( NULL,msg5, -1, &destRect, DT_NOCLIP, 
			D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) );
	}
	if( g_bOBB1Circle )
	{
		SetRect(&destRect, 0, 60, 100, 160 );
		g_pFont->DrawText( NULL,msg6, -1, &destRect, DT_NOCLIP, 
			D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) );
	}
}

//绘制obb1的mesh和bounding box
void DrawOBB1Mesh()
{
//	g_pOBB1Mesh->DrawSubset(0);
	g_pDevice->SetFVF( D3DFVF_MY_VERTEX );
	g_pDevice->SetStreamSource( 0 , g_pOBB1Vtx , 0 , sizeof(Vertex) );
	g_pDevice->DrawPrimitive( D3DPT_LINELIST , 0 , 12 );
}

//绘制obb2的mesh和bounding box
void DrawOBB2Mesh()
{
	D3DXMATRIX matIdentity;
	/*D3DXMatrixIdentity( &matIdentity );
	D3DXMatrixTranslation( &matIdentity ,1.5f , 0.0f , 1.0f );
	g_pDevice->SetTransform( D3DTS_WORLD , &matIdentity );
	g_pOBB2Mesh->DrawSubset(0);*/

	D3DXMatrixIdentity( &matIdentity );
	g_pDevice->SetTransform( D3DTS_WORLD , &matIdentity );
	g_pDevice->SetFVF( D3DFVF_MY_VERTEX );
	g_pDevice->SetStreamSource( 0 , g_pOBB2Vtx , 0 , sizeof(Vertex) );
	g_pDevice->DrawPrimitive( D3DPT_LINELIST , 0 , 12 );
}
//渲染
void Render()
{
	
	
	
	//
	D3DXMATRIX matOBB1Rot;
	D3DXMatrixIdentity( &matOBB1Rot );
	D3DXMatrixRotationY( &matOBB1Rot , D3DXToRadian(0.1f) );
	UpdateOBBData( g_OBB1 , matOBB1Rot );
	//根新绘制的点
	UpdateBoundingBoxVertices();

	g_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
							D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f), 1.0f, 0 );
	g_pDevice->BeginScene();
	
	D3DXMatrixIdentity( &matOBB1Rot );
	g_pDevice->SetTransform( D3DTS_WORLD , &matOBB1Rot );

	DrawOBB1Mesh();
//	DrawOBB2Mesh();
//	DrawSphereMesh();
	DrawCylinder();

	//绘制单独点
	D3DXMatrixIdentity( &matOBB1Rot );
	g_pDevice->SetTransform( D3DTS_WORLD , &matOBB1Rot );

	g_pDevice->SetFVF( D3DFVF_MY_VERTEX );
	g_pDevice->SetStreamSource( 0 , g_pLineVtx , 0 , sizeof(Vertex) );
	g_pDevice->DrawPrimitive( D3DPT_LINELIST , 0 , 1 );

	g_pDevice->SetFVF( D3DFVF_MY_VERTEX );
	g_pDevice->SetStreamSource( 0 , g_pPointVtx , 0 , sizeof(Vertex) );
	g_pDevice->DrawPrimitive( D3DPT_POINTLIST , 0 , 1 );

	g_pDevice->SetFVF( D3DFVF_MY_VERTEX );
	g_pDevice->SetStreamSource( 0 , g_pPlaneVtx , 0 , sizeof(Vertex) );
	g_pDevice->DrawPrimitive( D3DPT_LINELIST , 0 , 4 );

	//Sleep(100);
	g_bOBB1Point = IsPointInOBB( g_vPointPos , g_OBB1 );
	g_bOBB1Line = IsSegmentIntersectOBB( g_OBB1 , g_vLineStart , g_vLineEnd );
	
	g_bOBB1Plane = Intersection(g_Plane , g_OBB1 );
	g_bOBB1Circle = Intersection( D3DXVECTOR3(3.0f,0.0f,0.0f),
								  D3DXVECTOR3(1.0f,0.0f,0.0f),
								  D3DXVECTOR3(0.0f,0.0f,0.1f),2.0f,g_OBB1 );

	static FLOAT fLastTime = 0.0f;
	static DWORD dwFrames  = 0;
	FLOAT fTime = DXUtil_Timer( TIMER_GETABSOLUTETIME );
	++dwFrames;
	static float m_fFPS = 0.0f;

	
	
	if( fTime - fLastTime > 1.0f )
	{
		m_fFPS    = dwFrames / (fTime - fLastTime);
		fLastTime = fTime;
		dwFrames  = 0;
	}
	RECT destRect;
	SetRect( &destRect , 0 , 0 , 100, 100 );
	const char* fps = "fps = %f";
	char displayfps[32];
	memset(displayfps,NULL,32);
	sprintf( displayfps , fps , m_fFPS );
	g_pFont->DrawText( NULL,displayfps, -1, &destRect, DT_NOCLIP, 
		D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) );

	DrawText();

	g_pDevice->EndScene();
	g_pDevice->Present( NULL, NULL, NULL, NULL );
}
LRESULT CALLBACK WindowProc( HWND   hWnd, UINT   msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{   
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case VK_SPACE:
				break;
			case VK_F1:
				break;
			case VK_F2:
				break;
			}
		}
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc( hWnd, msg, wParam, lParam );
	}

	return 0;
}

int WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow )
{
	WNDCLASSEX winClass; 
	MSG        uMsg;

	memset(&uMsg,0,sizeof(uMsg));

	winClass.lpszClassName = "MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon         = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	winClass.hIconSm       = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClassEx(&winClass) )
		return E_FAIL;

	g_hWnd = CreateWindowEx( NULL, "MY_WINDOWS_CLASS", 
		"Collision Test --- Written by Shinco Xiong",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 640, 480, NULL, NULL, hInstance, NULL );

	if( g_hWnd == NULL )
		return E_FAIL;

	ShowWindow( g_hWnd, nCmdShow );
	UpdateWindow( g_hWnd );

	Init();
	

	while( uMsg.message != WM_QUIT )
	{
		if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
		{ 
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
		}
		else
		{
			Render();
		}
	}

	UnregisterClass( "MY_WINDOWS_CLASS", winClass.hInstance );
	Shutdown();
	return uMsg.wParam;
}
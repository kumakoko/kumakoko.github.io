  /************************************************************************
 *	@file		：	ModelDemo/XKMD2Model.h
 *	@brief		：	定义和读取了MD2文件格式的类
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-26 16:54:17
 ************************************************************************/
#ifndef __MODELDEMO_XKMD2MODEL_H__
#define __MODELDEMO_XKMD2MODEL_H__

#include "XKGlobals.h"
#include <vector>

using namespace std;


//MD2模型的每个三角形的顶点和纹理映射坐标的索引列表
struct MD2_IndexList
{
	int		m_nA , m_nB, m_nC;
	float	m_fUA , m_fVA;
	float	m_fUB , m_fVB;
	float	m_fUC , m_fVC;
};

//MD2模型的顶点列表
struct MD2_VertexList
{
	float m_fX , m_fY , m_fZ;
};

//MD2模型的帧列表,每一帧指向一个顶点列表
struct MD2_FrameList
{
	MD2_VertexList* m_pVtxList;
};

//三维矢量
struct MD2_Vector3
{
	float m_fX , m_fY , m_fZ;
};

//保存纹理映射的坐标
struct MD2_UVTexCoord
{
	short int m_nU , m_nV;
};

//MD2模型中的三角形面片，用来保持这
//些顶点和纹理映射坐标在文件中索引
struct MD2_Triangle
{
	short int m_nIndexA;
	short int m_nIndexB;
	short int m_nIndexC;
	short int m_nIndexA_UV;
	short int m_nIndexB_UV;
	short int m_nIndexC_UV;
};

//MD2模型中的三角形的顶点，其坐标是单字节的
struct MD2_TriVertex
{
	unsigned char m_X , m_Y , m_Z;
	unsigned char m_LightNormalIndex;
};

//MD2用来保持动画帧的缩放(scale)，
//平移(translate)和缩放等信息
struct MD2_AliasFrame
{
	float m_fScaleX;
	float m_fScaleY;
	float m_fScaleZ;
	float m_fTranslateX;
	float m_fTranslateY;
	float m_fTranslateZ;
	char  m_szName[16];
	MD2_TriVertex m_TriVtx[1];
};

//MD2的文件头数据，这个数据结构的成员
//顺序是要严格定义不能调乱的
struct MD2_FileHeader
{
	int m_nIdentify;		//识别号
	int m_nVersion;			//版本号
	int m_nSkinWidth;		//
	int m_nSkinHeight;		//
	int m_nFrameSize;		//每帧的大小，以字节为单位
	int m_nSkinNum;			//与模型相关联的皮肤数量
	int m_nVertexNum;		//模型的顶点数
	int m_nUVNum;			//纹理坐标的数量
	int m_nTriangleNum;		//模型中三角形面片的数量
	int m_nOpenGLCmdNum;	//OpenGL的命令数目
	int m_nFrameNum;		//帧数目
	int m_nSkinsOffset;		//皮肤数据在文件中的偏移地址
	int m_nTexCoordOffset;	//纹理映射坐标数据在文件中的偏移量
	int m_nTrianglesOffset; //三角形面片数据在文件中的偏移量
	int m_nFrameOffset;     //帧数据在文件中的偏移量
	int m_nOpenGLCmdOffset; //OpenGL命令在文件中的偏移量
	int m_nEndOffset;		//文件结束位置在文件中的偏移量
};

struct MD2_TriFloatVtx
{
	MD2_Vector3	m_V;
	int			m_nLightNormalIndex;
};

struct MD2_Frame
{
	MD2_Vector3		m_vMin;
	MD2_Vector3		m_vMax;
	char			m_szName[16];
	MD2_TriFloatVtx	m_V;
};

//MD2的FVF顶点模型
struct MD2_ModelVertex 
{
	D3DXVECTOR3   m_vPos;
	D3DCOLOR      m_dwDiffuse;
	D3DXVECTOR2   m_vTex;
} ;

#define FVF_MD2_MODELVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0) )

struct MD2_Mesh
{
	vector<MD2_ModelVertex>	m_Vertices;
};

//MD2的动画帧
struct MD2_Animation
{
	int		m_nStart;		//MD2动画的起始帧
	int		m_nEnd;			//MD2动画的结束帧
	int		m_nIndex;		//
	int		m_nCurrent;		//
	string	m_strName;
};


class XKMD2Model
{
private:
	MD2_IndexList*	m_pIndexList;		//顶点索引
	MD2_FrameList*	m_pFrameList;		//帧索引列表
	int				m_nFrames;			//帧数
	int				m_nVertices;		//每帧的顶点数
	int				m_nTriangles;		//三角形面片数
	int				m_nSkins;			//皮肤数
	int				m_nUVs;				//纹理坐标数
	int				m_nFrameSize;		//帧大小
	MD2_Mesh		m_Meshes[MD2_MAX_FRAMES];			 //每一个就对应一个mesh
	MD2_ModelVertex	m_VertexBuffer[MD2_MAX_TRIANGLES*3]; //模型的顶点数
public:
	vector<MD2_Animation>	m_Animation;
private:
	int Init();
public:
	XKMD2Model();
	~XKMD2Model();
	int   ReturnCurrentAnim( int nCurrent );
	float ReturnCurrentTime(int nSelect,int nNextFrame);
	int	  LoadFile( const char* szFileName );
	int	  Render( int nCurrent );
	inline int GetFrameCount();
	inline int GetVertexCount();
	inline int GetTriangleCount();
};

#endif /* __MODELDEMO_XKMD2MODEL_H__ */

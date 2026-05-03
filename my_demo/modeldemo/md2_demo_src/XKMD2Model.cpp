 /************************************************************************
 *	@file		：	ModelDemo/XKMD2Model.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-26 16:54:06
 ************************************************************************/

#include "XKMD2Model.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;



XKMD2Model::XKMD2Model()
{
	m_pIndexList = NULL;
	m_pFrameList = NULL;
}

XKMD2Model::~XKMD2Model()
{
	if( m_pIndexList )
		delete m_pIndexList;

	if( m_pFrameList )
	{
		for( int i = 0 ; i < m_nFrames ; ++i )
		{
			delete m_pFrameList[i].m_pVtxList;
		}

		delete m_pFrameList;
	}
}

/**
 * @name  XKMD2Model::LoadFile
 * @brief     载入MD2模型文件
 * @return    int
 * @param     const char* szFileName
 * @remark    
 **/
int XKMD2Model::LoadFile( const char* szFileName )
{
	FILE*			md2_file = NULL;
	MD2_FileHeader	md2_file_head;
	MD2_UVTexCoord	md2_tex_coord[MD2_MAX_VERTS];
	char			md2_skins[MD2_MAX_SKINS][64];
	unsigned char	buffer[MD2_MAX_VERTS*4+128];

	md2_file = fopen( szFileName , "rb" );

	if( NULL == md2_file )
	{
		MessageBox( g_hWnd , "无法打开MD2文件",
					DEMOMSGBOXCAPTION , MB_OK|MB_ICONERROR );
		return 0;
	}

	//读取MD2文件头信息块
	fread( &md2_file_head, 1, 
		   sizeof(MD2_FileHeader), md2_file );
	
	//要重新计算每帧数据的大小
	//md2_file_head.m_nFrameSize =
	//读取MD2文件的帧数，顶点数。还有三角形面片数
	m_nFrames = md2_file_head.m_nFrameNum;
	m_nVertices = md2_file_head.m_nVertexNum;
	m_nTriangles = md2_file_head.m_nTriangleNum;
	m_nSkins = md2_file_head.m_nSkinNum;
	m_nUVs = md2_file_head.m_nUVNum;
	//根据三角形的面数和帧数,创建
	m_pIndexList = new MD2_IndexList[ m_nTriangles ];
	m_pFrameList = new MD2_FrameList[ m_nFrames ];

	//给每一帧都开辟顶点缓冲区
	for( int i = 0 ; i < m_nFrames ; ++i )
	{
		m_pFrameList[i].m_pVtxList = new MD2_VertexList[m_nVertices];
	}

	//读取skin数据和纹理映射坐标数据
	fread( md2_skins, 1, m_nSkins * MD2_MAX_SKINNAME, md2_file );
	fread( md2_tex_coord , 1, m_nUVs * sizeof(MD2_UVTexCoord), md2_file );

	//根据三角形的面数读取三角形面片信息并填充
	MD2_Triangle	triangle;
	
	//检测最大的纹理映射坐标，看看有没有超出的
	int max_tex_u , max_tex_v;
	for( i = 0 ; i < m_nTriangles ; ++i )
	{
		//读取三角形面片的顶点数据,并且填充三角形的顶点和纹理映射坐标的索引列表
		fread( &triangle, 1, sizeof(MD2_Triangle), md2_file );
		
		//要注意磁盘的数据存储顺序和内存中的顺序的不同
		m_pIndexList[i].m_nA = triangle.m_nIndexC;
		m_pIndexList[i].m_nB = triangle.m_nIndexB;
		m_pIndexList[i].m_nC = triangle.m_nIndexA;

		m_pIndexList[i].m_fUA = md2_tex_coord[triangle.m_nIndexC_UV].m_nU;
		m_pIndexList[i].m_fVA = md2_tex_coord[triangle.m_nIndexC_UV].m_nV;
		m_pIndexList[i].m_fUB = md2_tex_coord[triangle.m_nIndexB_UV].m_nU;
		m_pIndexList[i].m_fVB = md2_tex_coord[triangle.m_nIndexB_UV].m_nV;
		m_pIndexList[i].m_fUC = md2_tex_coord[triangle.m_nIndexA_UV].m_nU;
		m_pIndexList[i].m_fVC = md2_tex_coord[triangle.m_nIndexA_UV].m_nV;

		max_tex_u = max( max_tex_u, md2_tex_coord[triangle.m_nIndexA_UV].m_nU );
		max_tex_u = max( max_tex_u, md2_tex_coord[triangle.m_nIndexB_UV].m_nU );
		max_tex_u = max( max_tex_u, md2_tex_coord[triangle.m_nIndexC_UV].m_nU );
		max_tex_v = max( max_tex_v, md2_tex_coord[triangle.m_nIndexA_UV].m_nV );
		max_tex_v = max( max_tex_v, md2_tex_coord[triangle.m_nIndexB_UV].m_nV );
		max_tex_v = max( max_tex_v, md2_tex_coord[triangle.m_nIndexC_UV].m_nV );
	}

	//使纹理坐标有效
	for( i = 0 ; i < m_nTriangles ; ++i )
	{
		m_pIndexList[i].m_fUA /= max_tex_u;
		m_pIndexList[i].m_fUB /= max_tex_u;
		m_pIndexList[i].m_fUC /= max_tex_u;
		m_pIndexList[i].m_fVA /= max_tex_v;
		m_pIndexList[i].m_fVB /= max_tex_v;
		m_pIndexList[i].m_fVC /= max_tex_v;
	}

	MD2_Animation	animation;
	string			last_anim_name;
	int				current_frame = 0;
	int				last_frame = 0;
	//读取所有动画的顶点数
	MD2_AliasFrame* out = NULL;
	m_nFrameSize = md2_file_head.m_nFrameSize;
	
	for( i = 0 ; i < m_nFrames ; ++i )
	{
		out = (MD2_AliasFrame*)buffer;
		//根据每帧的大小读取相关的数据
		fread( out , 1 , m_nFrameSize , md2_file );

		string anim_name = out->m_szName;

		for( int j = 0 ; j < anim_name.length() ; ++j )
		{
			//如果索引j所指的字符为数字字符，并
			//且它的位置在字符串倒数第2个之后
			if( isdigit( anim_name[j]) && 
				j >= anim_name.length() - 2 )
			{
				anim_name.erase(j, anim_name.length() - j);
				break;
			}
		}

		// 如果这动画名字和上一帧的名字不同的话，
		// 或者我们在这个model的最后的一帧动画
		if( anim_name.compare(last_anim_name) || 
			i == m_nFrames - 1)
		{
			//如果这一帧不是第一帧的话
			if(last_anim_name.compare(""))
			{
				//把上一帧的名字复制到本帧中
				animation.m_strName = last_anim_name;

				// Set the last frame of this animation to i
				animation.m_nEnd = i;
				animation.m_nIndex = last_frame;
				current_frame = 0;
				last_frame++;
				m_Animation.push_back(animation);
			}

			// 把当前获取的帧编号加1后作为动画的起始编号
			animation.m_nStart = current_frame + i;
			animation.m_nCurrent = animation.m_nStart;
		}

		last_anim_name = anim_name;
		current_frame++;

		for( j = 0; j < m_nVertices; j++ ) 
		{
			(m_pFrameList)[i].m_pVtxList[j].m_fX = (out->m_TriVtx[j]).m_X * out->m_fScaleX + out->m_fTranslateX;
			(m_pFrameList)[i].m_pVtxList[j].m_fY = (out->m_TriVtx[j]).m_Y * out->m_fScaleY + out->m_fTranslateY;
			(m_pFrameList)[i].m_pVtxList[j].m_fZ = (out->m_TriVtx[j]).m_Z * out->m_fScaleZ + out->m_fTranslateZ;
		}
	}

	fclose( md2_file );
	return Init();
}

/**
 * @name  XKMD2Model::GetFrameCount
 * @brief     
 * @return    int 
 * @remark    
 **/
int XKMD2Model::GetFrameCount()
{
	return m_nFrames;
}

/**
 * @name  XKMD2Model::GetVertexCount
 * @brief     获取模型的顶点数
 * @return    int 
 * @remark    
 **/
int XKMD2Model::GetVertexCount()
{
	return m_nVertices;
}

/**
 * @name  XKMD2Model::GetTriangleCount
 * @brief     获取模型的三角形面片数
 * @return    int 
 * @remark    
 **/
int XKMD2Model::GetTriangleCount()
{
	return m_nTriangles;
}


/**
 * @name  XKMD2Model::Init
 * @brief     把MD2的相关数据转用DX的数据结构存储
 * @return    int 
 * @remark    
 **/
int XKMD2Model::Init()
{
	//把每一帧顶点等转换成D3D的mesh
	for( int i = 0 ; i < GetFrameCount() ; ++i )
	{
		MD2_ModelVertex	model_vertex;
		D3DXCOLOR		diffuse_color(1.0f,1.0f,1.0f,1.0f);//顶点漫反射的颜色
		
		//把每一帧的三角形的顶点都存储进自定义的mesh中去
		for( int j = 0 ; j < GetTriangleCount() ; ++j )
		{
			//注意。MD2文件的坐标轴和D3D的有所不同，所以要注意赋值的对应
			// D3D.X = MD2.X D3D.Y = MD2.Z D3D.Z = MD2.Y

			//存储三角形顶点A
			model_vertex.m_vPos.x = 
				m_pFrameList[i].m_pVtxList[m_pIndexList[j].m_nA].m_fX;
			model_vertex.m_vPos.y = 
				m_pFrameList[i].m_pVtxList[m_pIndexList[j].m_nA].m_fZ;
			model_vertex.m_vPos.z = 
				m_pFrameList[i].m_pVtxList[m_pIndexList[j].m_nA].m_fY;
			
			model_vertex.m_vTex.x = m_pIndexList[j].m_fUA;
			model_vertex.m_vTex.y = m_pIndexList[j].m_fVA;
			
			model_vertex.m_dwDiffuse = diffuse_color;

			m_Meshes[i].m_Vertices.push_back( model_vertex );
			//-------------------------------------------

			//存储三角形顶点B
			model_vertex.m_vPos.x = 
				m_pFrameList[i].m_pVtxList[m_pIndexList[j].m_nB].m_fX;
			model_vertex.m_vPos.y = 
				m_pFrameList[i].m_pVtxList[m_pIndexList[j].m_nB].m_fZ;
			model_vertex.m_vPos.z = 
				m_pFrameList[i].m_pVtxList[m_pIndexList[j].m_nB].m_fY;
			
			model_vertex.m_vTex.x = m_pIndexList[j].m_fUB;
			model_vertex.m_vTex.y = m_pIndexList[j].m_fVB;
			
			model_vertex.m_dwDiffuse = diffuse_color;

			m_Meshes[i].m_Vertices.push_back( model_vertex );

			//----------------------------------------------------

			//存储三角形顶点C
			model_vertex.m_vPos.x = 
				m_pFrameList[i].m_pVtxList[m_pIndexList[j].m_nC].m_fX;
			model_vertex.m_vPos.y = 
				m_pFrameList[i].m_pVtxList[m_pIndexList[j].m_nC].m_fZ;
			model_vertex.m_vPos.z = 
				m_pFrameList[i].m_pVtxList[m_pIndexList[j].m_nC].m_fY;
			
			model_vertex.m_vTex.x = m_pIndexList[j].m_fUC;
			model_vertex.m_vTex.y = m_pIndexList[j].m_fVC;
			
			model_vertex.m_dwDiffuse = diffuse_color;

			m_Meshes[i].m_Vertices.push_back( model_vertex );
		}
	}

	return 1;
}

/**
 * @name  XKMD2Model::ReturnCurrentAnim
 * @brief     得到当前要播放的动画编号
 * @return    int 
 * @param     int nCurrent
 * @remark    
 **/
int XKMD2Model::ReturnCurrentAnim( int nCurrent )
{
	//遍历所有动画的名字
	for (int i = 0 ; i < m_Animation.size() ; ++i )
	{
		if (m_Animation[i].m_nIndex == nCurrent )
			return i;
	}
	
	return -1;	
}

/**
 * @name  XKMD2Model::ReturnCurrentTime
 * @brief     得到当前的时刻
 * @return    float 
 * @param     int nSelect
 * @param     int nNextFrame
 * @remark    
 **/
float XKMD2Model::ReturnCurrentTime(int nSelect,int nNextFrame)
{
	static float elapsed_time		= 0.0f;
	static float last_time	= 0.0f;
	
	//获取当前的时刻
	float time = GetTickCount();
	
	elapsed_time = time - last_time;
	
	float t = elapsed_time / (1000.0f / MD2_ANIMATION_SPEED );
	
	//到了换帧的时间
	if (elapsed_time >= (1000.0f / MD2_ANIMATION_SPEED) )
	{
		//切换到下一帧
		m_Animation[nSelect].m_nCurrent = nNextFrame;
		last_time = time;
	}
	
	return t;
}

/**
 * @name  XKMD2Model::Render
 * @brief     渲染模型
 * @return    bool 
 * @param     int nCurrent
 * @remark    
 **/
int XKMD2Model::Render( int nCurrent )
{
	int select = ReturnCurrentAnim( nCurrent );
	
	if( select == -1 ) 
		return select;
	
	int frame = m_Animation[select].m_nCurrent;
	int next_frame = (m_Animation[select].m_nCurrent + 1) %
					 m_Animation[select].m_nEnd;
	
	if( next_frame == 0 ) 
		next_frame =  m_Animation[select].m_nStart;
	
	float t = ReturnCurrentTime( select , next_frame );
	
	int max_point = GetTriangleCount()*3;
	
	MD2_ModelVertex p1,p2;
	
	g_pD3DDevice->SetVertexShader( FVF_MD2_MODELVERTEX );
	
	for(int j = 0; j < max_point; j++)
	{
		p1 = m_Meshes[frame].m_Vertices[j];
		p2 = m_Meshes[next_frame].m_Vertices[j];
		
		p1.m_vPos += (p2.m_vPos - p1.m_vPos) * t;
		m_VertexBuffer[j] = p1;
	}
	
	//指定顶点
	HRESULT hr = g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
											GetTriangleCount(),		 
											(BYTE**)&m_VertexBuffer[0],
											sizeof(MD2_ModelVertex));
		
	return (SUCCEEDED(hr));
}
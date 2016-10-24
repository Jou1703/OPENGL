#ifndef _3DS_H
#define _3DS_H

#include <math.h>
#include <vector>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "draw_utils.h"

#define PRIMARY 0x4D4D

#define OBJECTINFO 0x3D3D       
#define VERSION 0x0002        
#define EDITKEYFRAME 0xB000       

#define MATERIAL   0xAFFF      
#define OBJECT     0x4000      


#define MATNAME 0xA000       
#define MATDIFFUSE 0xA020        
#define MATMAP 0xA200        
#define MATMAPFILE 0xA300        

#define OBJECT_MESH 0x4100        

#define OBJECT_VERTICES 0x4110      
#define OBJECT_FACES    0x4120      
#define OBJECT_MATERIAL    0x4130     
#define OBJECT_UV      0x4140     
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))


#define MAX_TEXTURES 100               



using namespace std;
class NBVector3
{
public:
	NBVector3() {}
	NBVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}
	inline NBVector3 operator+(NBVector3 vVector)
	{
		return NBVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}
	inline NBVector3 operator-(NBVector3 vVector)
	{
		return NBVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	inline NBVector3 operator-()
	{
		return NBVector3(-x, -y, -z);
	}
	inline NBVector3 operator*(float num)
	{
		return NBVector3(x * num, y * num, z * num);
	}
	inline NBVector3 operator/(float num)
	{
		return NBVector3(x / num, y / num, z / num);
	}

	inline NBVector3 operator^(const NBVector3 &rhs) const
	{
		return NBVector3(y * rhs.z - rhs.y * z, rhs.x * z - x * rhs.z, x * rhs.y - rhs.x * y);
	}

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float v[3];
	};				
};


class CVector2 
{
public:
	float x, y;
};


struct tFace
{
	int vertIndex[3];      // ¶¥µãË÷Òý
	int coordIndex[3];      // ÎÆÀí×ø±êË÷Òý
};


struct tMaterialInfo
{
	char strName[255];      // ÎÆÀíÃû³Æ
	char strFile[255];      // Èç¹ûŽæÔÚÎÆÀíÓ³Éä£¬Ôò±íÊŸÎÆÀíÎÄŒþÃû³Æ
	GLubyte color[3];        // ¶ÔÏóµÄRGBÑÕÉ«
	int texureId;        // ÎÆÀíID
	float uTile;        // u ÖØžŽ
	float vTile;        // v ÖØžŽ
	float uOffset;       // u ÎÆÀíÆ«ÒÆ
	float vOffset;        // v ÎÆÀíÆ«ÒÆ
} ;


struct t3DObject 
{
	int numOfVerts;      // Ä£ÐÍÖÐ¶¥µãµÄÊýÄ¿
	int numOfFaces;      // Ä£ÐÍÖÐÃæµÄÊýÄ¿
	int numTexVertex;      // Ä£ÐÍÖÐÎÆÀí×ø±êµÄÊýÄ¿
	int materialID;      // ÎÆÀíID
	bool bHasTexture;      // ÊÇ·ñŸßÓÐÎÆÀíÓ³Éä
	char strName[255];      // ¶ÔÏóµÄÃû³Æ
	NBVector3 *pVerts;      // ¶ÔÏóµÄ¶¥µã
	NBVector3 *pNormals;    // ¶ÔÏóµÄ·šÏòÁ¿
	CVector2 *pTexVerts;    // ÎÆÀíUV×ø±ê
	tFace *pFaces;        // ¶ÔÏóµÄÃæÐÅÏ¢
};


struct t3DModel 
{
	GLuint texture[MAX_TEXTURES];
	int numOfObjects;          // Ä£ÐÍÖÐ¶ÔÏóµÄÊýÄ¿
	int numOfMaterials;          // Ä£ÐÍÖÐ²ÄÖÊµÄÊýÄ¿
	vector<tMaterialInfo> pMaterials;  // ²ÄÖÊÁŽ±íÐÅÏ¢
	vector<t3DObject> pObject;      // Ä£ÐÍÖÐ¶ÔÏóÁŽ±íÐÅÏ¢
};





struct tIndices 
{              
	unsigned short a, b, c, bVisible;  
};


struct tChunk
{
	unsigned short int ID;          // ¿éµÄID    
	unsigned int length;          // ¿éµÄ³€¶È
	unsigned int bytesRead;          // ÐèÒª¶ÁµÄ¿éÊýŸÝµÄ×ÖœÚÊý
};




typedef struct tagBoundingBoxStruct
{
	NBVector3  BoxPosMaxVertex;
	NBVector3  BoxNegMaxVertex;
} BoundingBoxVertex2;



NBVector3 Vector(NBVector3 vPoint1, NBVector3 vPoint2);

NBVector3 AddVector(NBVector3 vVector1, NBVector3 vVector2);


NBVector3 DivideVectorByScaler(NBVector3 vVector1, float Scaler);

NBVector3 Cross(NBVector3 vVector1, NBVector3 vVector2);


NBVector3 Normalize(NBVector3 vNormal);

void DrawModel(t3DModel& Model,bool touming=false);


//////////////////////////////////////////////////////////////////////////
#define FRAND   (((float)rand()-(float)rand())/RAND_MAX)
#define Clamp(x, min, max)  x = (x<min  ? min : x<max ? x : max);

#define SQUARE(x)  (x)*(x)
struct vector3_t
{
	vector3_t(float x, float y, float z) : x(x), y(y), z(z) {}
	vector3_t(const vector3_t &v) : x(v.x), y(v.y), z(v.z) {}
	vector3_t() : x(0.0f), y(0.0f), z(0.0f) {}

	vector3_t& operator=(const vector3_t &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	// vector add
	vector3_t operator+(const vector3_t &rhs) const
	{
		return vector3_t(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	// vector subtract
	vector3_t operator-(const vector3_t &rhs) const
	{
		return vector3_t(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	// scalar multiplication
	vector3_t operator*(const float scalar) const
	{
		return vector3_t(x * scalar, y * scalar, z * scalar);
	}

	// dot product
	float operator*(const vector3_t &rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	// cross product
	vector3_t operator^(const vector3_t &rhs) const
	{
		return vector3_t(y * rhs.z - rhs.y * z, rhs.x * z - x * rhs.z, x * rhs.y - rhs.x * y);
	}

	float& operator[](int index)
	{
		return v[index];
	}

	float Length()
	{
		float length = (float)sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));
		return (length != 0.0f) ? length : 1.0f;
	}

	/*****************************************************************************
	Normalize()

	Helper function to normalize vectors
	*****************************************************************************/
	vector3_t Normalize()
	{
		*this = *this * (1.0f/Length());
		return *this;
	}

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float v[3];
	};
};

// CLoad3DSÀàŽŠÀíËùÓÐµÄ×°ÈëŽúÂë
class CLoad3DS
{
public:
	CLoad3DS();                // ³õÊŒ»¯ÊýŸÝ³ÉÔ±
	// ×°Èë3dsÎÄŒþµœÄ£ÐÍœá¹¹ÖÐ
	bool Import3DS(t3DModel *pModel, const char *strFileName);

private:
	// ¶ÁÈëÒ»žöÎÆÀí
	int BuildTexture(char *szPathName, GLuint &texid);
	// ¶ÁÒ»žö×Ö·ûŽ®
	int GetString(char *);
	// ¶ÁÏÂÒ»žö¿é
	void ReadChunk(tChunk *);
	// ¶ÁÏÂÒ»žö¿é
	void ProcessNextChunk(t3DModel *pModel, tChunk *);
	// ¶ÁÏÂÒ»žö¶ÔÏó¿é
	void ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *);
	// ¶ÁÏÂÒ»žö²ÄÖÊ¿é
	void ProcessNextMaterialChunk(t3DModel *pModel, tChunk *);
	// ¶Á¶ÔÏóÑÕÉ«µÄRGBÖµ
	void ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk);
	// ¶Á¶ÔÏóµÄ¶¥µã
	void ReadVertices(t3DObject *pObject, tChunk *);
	// ¶Á¶ÔÏóµÄÃæÐÅÏ¢
	void ReadVertexIndices(t3DObject *pObject, tChunk *);
	// ¶Á¶ÔÏóµÄÎÆÀí×ø±ê
	void ReadUVCoordinates(t3DObject *pObject, tChunk *);
	// ¶Áž³Óè¶ÔÏóµÄ²ÄÖÊÃû³Æ
	void ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk);
	// ŒÆËã¶ÔÏó¶¥µãµÄ·šÏòÁ¿
	void ComputeNormals(t3DModel *pModel);
	// ¹Ø±ÕÎÄŒþ£¬ÊÍ·ÅÄÚŽæ¿ÕŒä
	void CleanUp();
	// ÎÄŒþÖžÕë
	FILE *m_FilePointer;

	tChunk *m_CurrentChunk;
	tChunk *m_TempChunk;
};
void changeObject(float trans[10]);
void drawModel(t3DModel Model,bool touming,bool outTex);

void Show3ds(int num);
void load3ds();
#endif

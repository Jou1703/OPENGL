#include <string.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "load3ds.h"
#include "all_path.h"

NBVector3 Vector(NBVector3 vPoint1, NBVector3 vPoint2)
{
	NBVector3 vVector;              
	
	vVector.x = vPoint1.x - vPoint2.x;      
	vVector.y = vPoint1.y - vPoint2.y;      
	vVector.z = vPoint1.z - vPoint2.z;      
	
	return vVector;                
}


NBVector3 AddVector(NBVector3 vVector1, NBVector3 vVector2)
{
	NBVector3 vResult;              
	
	vResult.x = vVector2.x + vVector1.x;    
	vResult.y = vVector2.y + vVector1.y;    
	vResult.z = vVector2.z + vVector1.z;    
	
	return vResult;                
}


NBVector3 DivideVectorByScaler(NBVector3 vVector1, float Scaler)
{
	NBVector3 vResult;              
	
	vResult.x = vVector1.x / Scaler;      
	vResult.y = vVector1.y / Scaler;      
	vResult.z = vVector1.z / Scaler;      
	
	return vResult;                
}


NBVector3 Cross(NBVector3 vVector1, NBVector3 vVector2)
{
	NBVector3 vCross;                
	
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));
	
	return vCross;                
}


NBVector3 Normalize(NBVector3 vNormal)
{
	double Magnitude;              
                           
}

int CLoad3DS::BuildTexture(char *filename,GLuint &texid)			
{    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	
	GLubyte		TGAcompare[12];								
	GLubyte		header[6];									
	GLuint		bytesPerPixel;								
	GLuint		imageSize;									
	GLuint		temp;										
	GLuint		type=GL_RGBA;								
	TextureImage texture;
	char path[50] = IMAGE_PATH_HOME;
	strcat(path,filename);

	FILE *file = fopen(path, "rb");						

	if(	file==NULL ||										
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	
		fread(header,1,sizeof(header),file)!=sizeof(header))				
	{
		if (file == NULL)									
			return false;									
		else
		{
			fclose(file);									// If Anything Failed, Close The File
			return false;									// Return False
		}
	}

	texture.width  = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	texture.height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)

	
	if(	texture.width	<=0	||								// Is The Width Less Than Or Equal To Zero
		texture.height	<=0	||								// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);										// If Anything Failed, Close The File
		return false;										// Return False
	}

	texture.bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture.bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texture.width*texture.height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture.imageData=(GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

	if(	texture.imageData==NULL ||							// Does The Storage Memory Exist?
		fread(texture.imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(texture.imageData!=NULL)						// Was Image Data Loaded
			free(texture.imageData);						// If So, Release The Image Data

		fclose(file);										// Close The File
		return false;										// Return False
	}

	
	GLuint i = 0;
	for(i=0; i<(imageSize); i+=bytesPerPixel)		// Loop Through The Image Data
	{														// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=texture.imageData[i];							// Temporarily Store The Value At Image Data 'i'
		texture.imageData[i] = texture.imageData[i + 2];	// Set The 1st Byte To The Value Of The 3rd Byte
		texture.imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);											// Close The File

	
	glGenTextures(1, &texid);					// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texid);			// Bind Our Texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered

	if (texture.bpp==24)									// Was The TGA 24 Bits
	{
		type=GL_RGB;										// If So Set The 'type' To GL_RGB
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture.width, texture.height, 0, type, GL_UNSIGNED_BYTE, texture.imageData);
	return true;											// Texture Building Went Ok, Return True
}

CLoad3DS::CLoad3DS()
{
	m_CurrentChunk = new tChunk;        
	m_TempChunk = new tChunk;          
}



bool CLoad3DS::Import3DS(t3DModel *pModel, const char *strFileName)
{
	
	
	m_FilePointer = fopen(strFileName, "rb");


	if(!m_FilePointer) 
	{
		printf("Unable to find the file: %s!", strFileName);
		
		return false;
	}
	
	ReadChunk(m_CurrentChunk);
	
	if (m_CurrentChunk->ID != PRIMARY)
	{
		printf("Unable to load PRIMARY chuck from file: %s!", strFileName);
		
		return false;
	}
	ProcessNextChunk(pModel, m_CurrentChunk);
	ComputeNormals(pModel);
	CleanUp();

	return true;
}

void CLoad3DS::CleanUp()
{

	fclose(m_FilePointer);            
	delete m_CurrentChunk;            
	delete m_TempChunk;              
}

void CLoad3DS::ProcessNextChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	t3DObject newObject = {0};          
	tMaterialInfo newTexture = {0};        
	unsigned int version = 0;          
	int buffer[50000] = {0};          

	m_CurrentChunk = new tChunk;     
	
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{

		ReadChunk(m_CurrentChunk);
		switch (m_CurrentChunk->ID)
		{
		case VERSION:
			m_CurrentChunk->bytesRead += fread(&version, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);

			break;

		case OBJECTINFO:           
			ReadChunk(m_TempChunk);
			m_TempChunk->bytesRead += fread(&version, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);
			m_CurrentChunk->bytesRead += m_TempChunk->bytesRead;
			ProcessNextChunk(pModel, m_CurrentChunk);
			break;

		case MATERIAL:              
			pModel->numOfMaterials++;
			pModel->pMaterials.push_back(newTexture);
			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case OBJECT:              
			pModel->numOfObjects++;
			pModel->pObject.push_back(newObject);
			memset(&(pModel->pObject[pModel->numOfObjects - 1]), 0, sizeof(t3DObject));
			m_CurrentChunk->bytesRead += GetString(pModel->pObject[pModel->numOfObjects - 1].strName);
			ProcessNextObjectChunk(pModel, &(pModel->pObject[pModel->numOfObjects - 1]), m_CurrentChunk);
			break;

		case EDITKEYFRAME:
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		default: 
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}		
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}


void CLoad3DS::ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	int buffer[50000] = {0};         
	m_CurrentChunk = new tChunk;
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		
		ReadChunk(m_CurrentChunk);
		switch (m_CurrentChunk->ID)
		{
		case OBJECT_MESH:          

			ProcessNextObjectChunk(pModel, pObject, m_CurrentChunk);
			break;

		case OBJECT_VERTICES:        
			ReadVertices(pObject, m_CurrentChunk);
			break;

		case OBJECT_FACES:          
			ReadVertexIndices(pObject, m_CurrentChunk);
			break;

		case OBJECT_MATERIAL:        

			ReadObjectMaterial(pModel, pObject, m_CurrentChunk);      
			break;

		case OBJECT_UV:            

			ReadUVCoordinates(pObject, m_CurrentChunk);
			break;

		default: 

			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}

void CLoad3DS::ProcessNextMaterialChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	int buffer[50000] = {0};          
	m_CurrentChunk = new tChunk;

	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		ReadChunk(m_CurrentChunk);

		switch (m_CurrentChunk->ID)
		{
		case MATNAME: 
			m_CurrentChunk->bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strName, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		case MATDIFFUSE:            
			ReadColorChunk(&(pModel->pMaterials[pModel->numOfMaterials - 1]), m_CurrentChunk);
			break;

		case MATMAP:  
			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case MATMAPFILE:       
			m_CurrentChunk->bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strFile, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		default: 
			m_CurrentChunk->bytesRead += fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}


		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}

void CLoad3DS::ReadChunk(tChunk *pChunk)
{
	pChunk->bytesRead = fread(&pChunk->ID, 1, 2, m_FilePointer);
	pChunk->bytesRead += fread(&pChunk->length, 1, 4, m_FilePointer);
}
int CLoad3DS::GetString(char *pBuffer)
{
	int index = 0;
	fread(pBuffer, 1, 1, m_FilePointer);

	while (*(pBuffer + index++) != 0) {
		fread(pBuffer + index, 1, 1, m_FilePointer);
	}
	return strlen(pBuffer) + 1;
}

void CLoad3DS::ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk)
{

	ReadChunk(m_TempChunk);
	m_TempChunk->bytesRead += fread(pMaterial->color, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);
	pChunk->bytesRead += m_TempChunk->bytesRead;
}

void CLoad3DS::ReadVertexIndices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	unsigned short index = 0;      
	pPreviousChunk->bytesRead += fread(&pObject->numOfFaces, 1, 2, m_FilePointer);

	pObject->pFaces = new tFace [pObject->numOfFaces];
	memset(pObject->pFaces, 0, sizeof(tFace) * pObject->numOfFaces);

	for(int i = 0; i < pObject->numOfFaces; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			pPreviousChunk->bytesRead += fread(&index, 1, sizeof(index), m_FilePointer);

			if(j < 3)
			{
				pObject->pFaces[i].vertIndex[j] = index;
			}
		}
	}
}

void CLoad3DS::ReadUVCoordinates(t3DObject *pObject, tChunk *pPreviousChunk)
{
	pPreviousChunk->bytesRead += fread(&pObject->numTexVertex, 1, 2, m_FilePointer);
	pObject->pTexVerts = new CVector2 [pObject->numTexVertex];
	pPreviousChunk->bytesRead += fread(pObject->pTexVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}

void CLoad3DS::ReadVertices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	pPreviousChunk->bytesRead += fread(&(pObject->numOfVerts), 1, 2, m_FilePointer);

	pObject->pVerts = new NBVector3 [pObject->numOfVerts];
	memset(pObject->pVerts, 0, sizeof(NBVector3) * pObject->numOfVerts);
	pPreviousChunk->bytesRead += fread(pObject->pVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);

	for(int i = 0; i < pObject->numOfVerts; i++)
	{
		float fTempY = pObject->pVerts[i].y;
		pObject->pVerts[i].y = pObject->pVerts[i].z;

		pObject->pVerts[i].z = -fTempY;
	}
}

void CLoad3DS::ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	char strMaterial[255] = {0};      
	int buffer[50000] = {0};        
	pPreviousChunk->bytesRead += GetString(strMaterial);
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		if(strcmp(strMaterial, pModel->pMaterials[i].strName) == 0)
		{
			pObject->materialID = i;
			if(strlen(pModel->pMaterials[i].strFile) > 0) {
				
				BuildTexture(pModel->pMaterials[i].strFile, pModel->texture[pObject->materialID]);
				pObject->bHasTexture = true;
			}  
			break;
		}
		else
		{
			pObject->materialID = -1;
		}
		
	}

	pPreviousChunk->bytesRead += fread(buffer, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}      

void CLoad3DS::ComputeNormals(t3DModel *pModel)
{
	NBVector3 vVector1, vVector2, vNormal, vPoly[3];
	if(pModel->numOfObjects <= 0)
		return;
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		t3DObject *pObject = &(pModel->pObject[index]);
		NBVector3 *pNormals    = new NBVector3 [pObject->numOfFaces];
		NBVector3 *pTempNormals  = new NBVector3 [pObject->numOfFaces];
		pObject->pNormals    = new NBVector3 [pObject->numOfVerts];
		
		int i=0;
		for(i=0; i < pObject->numOfFaces; i++)
		{                        
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			vVector1 = Vector(vPoly[0], vPoly[2]);    
			vVector2 = Vector(vPoly[2], vPoly[1]);    

			vNormal = Cross(vVector1, vVector2);    
			pTempNormals[i] = vNormal;          
			vNormal = Normalize(vNormal);        

			pNormals[i] = vNormal;            
		}
		
		NBVector3 vSum (0.0, 0.0, 0.0);
		NBVector3 vZero = vSum;
		int shared=0;
		
		for (i = 0; i < pObject->numOfVerts; i++)      
		{
			for (int j = 0; j < pObject->numOfFaces; j++)  
			{                        
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);
					shared++;                
				}
			} 

			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);  

			vSum = vZero;                
			shared = 0;                    
		}
		delete [] pTempNormals;
		delete [] pNormals;
	}
}
void changeObject(float trans[10])
{
	glTranslatef(trans[0],trans[1],trans[2]);
	glScalef(trans[3],trans[4],trans[5]);
	glRotatef(trans[6],trans[7],trans[8],trans[9]);
}
void drawModel(t3DModel Model,bool touming,bool outTex)
{
	
	if( touming ){
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1,1,1,0.5);
	}
	
	
	for(int i = 0; i < Model.numOfObjects; i++)
	{
		
		t3DObject *pObject = &Model.pObject[i];
		
		if(!outTex) {
			//printf("bHasTexture = %d\n",pObject->bHasTexture);
			if(pObject->bHasTexture) 
			{
				glEnable(GL_TEXTURE_2D);
				
				glBindTexture(GL_TEXTURE_2D, Model.texture[pObject->materialID]);
			} else {
				
				
				glDisable(GL_TEXTURE_2D);
				glColor3ub(255, 255, 255);
			}
		} 
		
		glBegin(GL_TRIANGLES);          
		
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			
			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				
				int index = pObject->pFaces[j].vertIndex[whichVertex];
				
				glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
				//printf("x = %f y = %f z = %f \n",pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
				
				if(pObject->bHasTexture) 
				{
					
					
					if(pObject->pTexVerts) { 
						glColor3f(1.0,1.0,1.0);
						glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
					}
				} 
				else
				{
					if(Model.pMaterials.size() && pObject->materialID >= 0) 
					{
						
						GLubyte *pColor = Model.pMaterials[pObject->materialID].color;
						glColor3ub(pColor[0], pColor[1], pColor[2]);
						
					}
				}
				glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
				//printf("x = %f y = %f z = %f \n",pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
			}
			
		}
		
		glEnd();                
	}
	if( touming )
		glDisable(GL_BLEND);
	
}
GLuint text1;
GLuint load_texture(const char* file_name)
{
     GLuint width, height, total_bytes;
     GLubyte* pixels = 0;
     GLuint last_texture_ID=0;
		GLuint texture_ID = 0;
     FILE* pFile = fopen(file_name, "rb");
    if( pFile == 0 )
        return 0;
        
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, 54, SEEK_SET);

     {
         GLint line_bytes = width * 3;
        while( line_bytes % 4 != 0 )
             ++line_bytes;
         total_bytes = line_bytes * height;
     }

     pixels = (GLubyte*)malloc(total_bytes);
    if( pixels == 0 )
     {
        fclose(pFile);
        exit(0);
     }

    if( fread(pixels, total_bytes, 1, pFile) <= 0 )
     {
        free(pixels);
        fclose(pFile);
        exit(0);
     }

     glGenTextures(1, &texture_ID);

    if( texture_ID == 0 )
     {
        free(pixels);
        fclose(pFile);
        exit(0);
     }

     
     glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&last_texture_ID);
     glBindTexture(GL_TEXTURE_2D, texture_ID);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
         GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
     glBindTexture(GL_TEXTURE_2D, last_texture_ID);

    free(pixels);
    return texture_ID;
}
static float leng = 1.0f;
GLfloat  xrot;   
GLfloat  yrot;   
GLfloat  zrot;   

CLoad3DS *gothicLoader=new(CLoad3DS);
t3DModel gothicModel;
CLoad3DS *gothicLoader1=new(CLoad3DS);
t3DModel gothicModel1;
float gothicTrans[10] = 
{
	0,-300,0,
	1.0,1.0,2.0,
	0.0f,0,1,0
};


static float gScale[7]=
{
	2.0,4.0,3.0,1.0,1.5,3.6,5.0
};


static float gPos[7][3]=
{
	{-960.0f-200,440+300.0f,0},
	{-760.0f-200,340.0f+300.0f,0},
	{-660.0f-200,240.0f+300.0f,0},
	{-560.0f-200,140.0f+300.0f,0},
	{-560.0f-200,240.0f+300.0f,0},
	{-660.0f-200,400.0f+300.0f,0},
	{-760.0f-200,100.0f+300.0f,0},
};

typedef struct _Particle_
{
	GLuint active;
	float life;
	GLfloat x;
	GLfloat y;
	GLfloat z;

	float xi;
	float yi;
	float scale;
}TParticle;

TParticle dspri[1000];

GLfloat LightPosition[]= { 0.0f, 0.0f, 1000.0f, 1.0f };
GLfloat LightAmbient[]= { 0.5f,0.5f, 0.5f, 0.5f };
GLfloat LightDiffuse[]= { 0.5f, 0.5f, 0.5f, 0.5f };
GLfloat LightSpecular[]= { 1.0f, 1.0f, 1.0f, 1.0f  };

	
GLfloat MatAmbient[]= { 248.0f/255.0f , 78.0f/255.0f, 22.0f/255.0f, 1.0f };
GLfloat MatDiffuse[]= {248.0f/255.0f , 78.0f/255.0f, 22.0f/255.0f, 1.0f};
GLfloat MatSpecular[]= {248.0f/255.0f , 78.0f/255.0f, 22.0f/255.0f, 1.0f};
GLfloat MatEmission[]= {0.0f, 0.0f, 0.0f, 1.0f};
GLuint priloop;
void ParticleInit()
{
	for(priloop=0; priloop<1000; ++priloop)
	{		
		dspri[priloop].xi=(float)((rand()%50)-26.0f)*10.0f;		
		dspri[priloop].yi=(float)((rand()%50)-25.0f)*10.0f;		
		
		dspri[priloop].x=0.0f;
		dspri[priloop].y=0.0f;
		if (priloop<25)
		{		
			dspri[priloop].y = -priloop*100.0f;
		}
		else 
		{
			dspri[priloop].y = priloop*100.0f;
		}
	}
}
void changeObj(float trans[])
{	
	//glScalef(trans[3],trans[4],trans[5]);
	//glTranslatef(trans[0],trans[1],trans[2]);
	//glRotatef(trans[6],trans[7],trans[8],trans[9]);		
}
float npos;
void load3ds()
{
	char path[128] = IMAGE_PATH_HOME;
	strcat(path,"1.3ds");
	gothicLoader->Import3DS(&gothicModel, path);
	ParticleInit();
}
void Show3ds(int num)
{
	gothicTrans[6]+=1.0f;


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); 
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, MatDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, MatSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	
	glEnable(GL_CULL_FACE);
	if(num>1000)
		num=1000;
	else if(num<10)
		num=10; 
	
	for(int i=0; i<num;++i)
	{
		
		GLfloat x=dspri[i].x;
		GLfloat y=dspri[i].y;
		GLfloat z=dspri[i].z;
		glPushMatrix();  
		glTranslatef(x,y,z);
	
		glScalef(gScale[i%7],gScale[i%7],gScale[i%7]);
		if (i%2==0)
			glRotatef(gothicTrans[6],0,1,1);
		else
			glRotatef(gothicTrans[6],0,-1,0);
		drawModel(gothicModel,1,0);
		glPopMatrix();

		dspri[i].x+=dspri[i].xi/(2*100);
		dspri[i].y+=dspri[i].yi/(2*100);
		if (dspri[i].x>1060 || dspri[i].x<-1060 || dspri[i].y>640 || dspri[i].y<-640)
		{
			dspri[i].x=0;
			dspri[i].y=0;
		}
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	
}


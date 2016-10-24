
#include "includes.h"
#include "draw_utils.h"
#include <ctype.h>
#include "mtf.h"
#include "all_path.h"
#include "fun.h"
#include <jpeglib.h>
#include <jconfig.h>
#include <png.h>

#define false 0
#define true 1
#define bool int 

#define GL_PI 3.1415926
float depth;

using namespace freetype;

MFont_Data our_font40;
MFont_Data our_font30;
MFont_Data our_font25;
MFont_Data our_font15;
MFont_Data our_font20;


TextureImage gray_bit,black_bit,white_num,yellow_num,wait;

tImageJPG *LoadJPG(const char *filename);

void FontInit()
{
	our_font40.init(MSYH_PATH,40);
	our_font30.init(MSYH_PATH,30);
	our_font25.init(MSYH_PATH,25);
	our_font15.init(MSYH_PATH,15);
	our_font20.init(MSYH_PATH,20);
}
void FontDraw(float x,float y,const char *string,int size)
{
	if(size == 40)
		our_font40.drawtext(x,y,string, 0);
	else if(size == 30)
		our_font30.drawtext(x,y,string, 0);
	else if(size == 25)
		our_font25.drawtext(x,y,string, 0);
	else if(size == 15)
		our_font15.drawtext(x,y,string, 0);
	else 
		our_font20.drawtext(x,y,string, 0);
	
	//our_font.drawonetext(x,y,string, 0);
	
}

void BitLoad()
{
	LoadTgaTex(&wait,get_path("wait.tga"));
}


char path[50];
char *get_path(const char *p)
{
	sprintf(path,"%s%s",IMAGE_PATH_HOME, p);
	return path;
}


GLuint CreateTextureFromPng(const char* filename)
{
	unsigned char header[8];
	int k;   
	GLuint textureID;
	int width, height; 
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr; 
	png_infop info_ptr; 
	int number_of_passes; 
	png_bytep *row_pointers;
	int row,col,pos;  
	GLubyte *rgba;

	FILE *fp=fopen(filename,"rb");
	if(!fp)
	{
		fclose(fp);
		return 0;
	}

	fread(header, 1, 8, fp);
	if(png_sig_cmp(header,0,8))
	{
		fclose(fp);
		return 0;
	}

	png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL); 
	if(!png_ptr)
	{ 
		fclose(fp);
		return 0;
	}
	info_ptr=png_create_info_struct(png_ptr);

	if(!info_ptr)
	{
		png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
		fclose(fp);
		return 0;
	}
	if (setjmp(png_jmpbuf(png_ptr)))
	{
	 png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
	 fclose(fp);
	 return 0;
	}
	png_init_io(png_ptr,fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	
	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
	{
		png_set_swap_alpha(png_ptr);
	}
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(fp);
			return 0;
	}
	rgba=(GLubyte*)malloc(width * height * 4);
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (k = 0; k < height; k++)
		row_pointers[k] = NULL;
      
	for (k=0; k<height; k++)
	{
		row_pointers[k] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr,info_ptr));
	}

    png_read_image(png_ptr, row_pointers);

	pos = (width * height * 4) - (4 * width);
	for( row = 0; row < height; row++)
	{
		for( col = 0; col < (4 * width); col += 4)
		{
			rgba[pos++] = row_pointers[row][col+1];        // red
			rgba[pos++] = row_pointers[row][col + 2];    // green
			rgba[pos++] = row_pointers[row][col + 3];    // blue
			rgba[pos++] = row_pointers[row][col ];    // alpha
		}
		pos=(pos - (width * 4)*2);
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1,&textureID);
	glBindTexture(GL_TEXTURE_2D,textureID); //将纹理绑定到名字

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
 
	free(row_pointers); 
	fclose(fp);
	return textureID;
}

void DecodeJPG(jpeg_decompress_struct* cinfo, tImageJPG *pImageData)
{
	jpeg_read_header(cinfo, TRUE);
	jpeg_start_decompress(cinfo);
	pImageData->rowSpan = cinfo->image_width * cinfo->num_components;
	pImageData->sizeX = cinfo->image_width;
	pImageData->sizeY = cinfo->image_height;
	pImageData->data = new unsigned char[pImageData->rowSpan * pImageData->sizeY];
	
	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];
	for (int i = 0; i < pImageData->sizeY; i++)

	rowPtr[i] = &(pImageData->data[i*pImageData->rowSpan]);
	
	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height)
	{
		rowsRead+=jpeg_read_scanlines(cinfo,&rowPtr[rowsRead],
		cinfo->output_height-rowsRead);
	}
	delete [] rowPtr;
	jpeg_finish_decompress(cinfo);
}

tImageJPG *LoadJPG(const char *filename)
{
	struct jpeg_decompress_struct cinfo;
	tImageJPG *pImageData = NULL; 
	FILE *pFile;
	if((pFile = fopen(filename, "rb")) == NULL)
	{
		return NULL;
	}
	jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, pFile);
	pImageData = (tImageJPG*)malloc(sizeof(tImageJPG));
	DecodeJPG(&cinfo, pImageData);
	jpeg_destroy_decompress(&cinfo);
	fclose(pFile);
	return pImageData;

}

GLuint CreateTexture(const char* strTextureFileName)
{

	GLuint tex; //纹理的标识
	tImageJPG *pJpg = 0; //存放JPG纹理像素数据

	if(!strTextureFileName) //如果文件名为空则返回
	{
		return FALSE;
	}

	if(strstr(strTextureFileName, ".jpg") ||strstr(strTextureFileName, ".jpeg")) 
	{
		pJpg = LoadJPG(strTextureFileName);
		printf("pJpg->sizeX = %d,pJpg->sizeY = %d\n",pJpg->sizeX, pJpg->sizeY);
		if(!pJpg) 
			return FALSE;
	}
	else
		return FALSE;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);					
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pJpg->sizeX, pJpg->sizeY,	0, GL_RGB, GL_UNSIGNED_BYTE, pJpg->data);

	if(pJpg) //pJpg->data已经被释放了
		free(pJpg);
	return tex; //返回生成纹理的标识
}

int LoadTgaTex(TextureImage *texture, char *file_name)			// Loads A TGA File Into Memory
{    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
	GLubyte		TGAcompare[12];								// Used To Compare TGA Header
	GLubyte		header[6];									// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;										// Temporary Variable
	GLuint		type=GL_RGBA;								// Set The Default GL Mode To RBGA (32 BPP)

	FILE *file = fopen(file_name, "rb");						// Open The TGA File

	if(	file==NULL ||										// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
	{
		if (file == NULL)									// Did The File Even Exist? *Added Jim Strong*		
{
		
        	
			return false;}									// Return False
		else
		{
			fclose(file);									// If Anything Failed, Close The File
			return false;									// Return False
		}
	}
	

	texture->width  = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)

	if(	texture->width	<=0	||								// Is The Width Less Than Or Equal To Zero
		texture->height	<=0	||								// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);										// If Anything Failed, Close The File
		return false;										// Return False
	}

	texture->bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture->bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texture->width*texture->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture->imageData=(GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

	if(	texture->imageData==NULL ||							// Does The Storage Memory Exist?
		fread(texture->imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(texture->imageData!=NULL)						// Was Image Data Loaded
			free(texture->imageData);						// If So, Release The Image Data

		fclose(file);										// Close The File
		return false;										// Return False
	}

	for(GLuint i=0; i<(imageSize); i+=bytesPerPixel)		
	{														
		temp=texture->imageData[i];							
		texture->imageData[i] = texture->imageData[i + 2];	
		texture->imageData[i + 2] = temp;					
	}
	fclose(file);											
	
	glGenTextures(1, &texture[0].texID);			
	glBindTexture(GL_TEXTURE_2D, texture[0].texID);			
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	if (texture[0].bpp==24)									
	{
		type=GL_RGB;										
	}
	glTexImage2D(GL_TEXTURE_2D, 0, type, texture[0].width, texture[0].height, 0, type, GL_UNSIGNED_BYTE, texture->imageData);
	free(texture->imageData);
    
	return true;											
}

GLuint LoadBmpTex(TextureImage *texture ,char* file_name)
{
     GLint width, height, total_bytes;
     //GLubyte* pixels = 0;
   
     FILE* pFile = fopen(file_name, "rb");
    if( pFile == 0 )
	{
			LOG_ERR("LoadBmpTex %s fopen err",file_name);
        	return 0;
	}
	
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, 54, SEEK_SET);

	texture->width = width;
	texture->height = height;
	
     {
         GLint line_bytes = width * 3;
        while( line_bytes % 4 != 0 )
             ++line_bytes;
         total_bytes = line_bytes * height;
     }

     texture->imageData = (GLubyte*)malloc(total_bytes);
    if( texture->imageData == NULL )
     {
        fclose(pFile);
        exit(0);
     }

    if( fread(texture->imageData, total_bytes, 1, pFile) <= 0 )
     {
        free(texture->imageData);
        fclose(pFile);
        exit(0);
     }
	
    glGenTextures(1, &texture[0].texID);
    if( texture[0].texID == 0 )
     {
        free(texture->imageData);
        fclose(pFile);
        exit(0);
     }
	fclose(pFile);
     glBindTexture(GL_TEXTURE_2D, texture[0].texID);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, texture->imageData);
	printf("LoadBmp:%s\n",file_name);
    free(texture->imageData);
    
    return 1;
}

GLuint DelTexID(TextureImage *texture)
{
		glDeleteTextures(1, &texture[0].texID);
}
GLboolean IsTexUsing(GLuint texture)
{
	return glIsTexture (texture);
}
GLuint load_texture(char* file_name)
{
     GLint width, height, total_bytes;
     GLubyte* pixels = 0;
     GLint last_texture_ID;
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

     // 读取像素数据
    if( fread(pixels, total_bytes, 1, pFile) <= 0 )
     {
        free(pixels);
        fclose(pFile);
        exit(0);
     }
	
     // 分配一个新的纹理编号
    glGenTextures(1, &texture_ID);
    if( texture_ID == 0 )
     {
        free(pixels);
        fclose(pFile);
        exit(0);
     }
     
     glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
     glBindTexture(GL_TEXTURE_2D, texture_ID);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

     glBindTexture(GL_TEXTURE_2D, last_texture_ID);

    free(pixels);
    return texture_ID;
}
void ShowJpg(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat heigth, GLuint texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
    glBegin( GL_QUADS );  
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y, z); // 左下
	glTexCoord2f(0.0f, 0.0f); glVertex3f( x, y+heigth, z);     // 左上
	glTexCoord2f(1.0f, 0.0f); glVertex3f( x+width, y+heigth, z);     // 右上
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y, z);     // 右下 
    glEnd(); 
}

void ShowPic(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat heigth, GLuint texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
    glBegin( GL_QUADS );  
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z); // 左下
	glTexCoord2f(0.0f, 1.0f); glVertex3f( x, y+heigth, z);     // 左上
	glTexCoord2f(1.0f, 1.0f); glVertex3f( x+width, y+heigth, z);     // 右上
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y, z);     // 右下 
    glEnd(); 
}

void ShowPartPic(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2,GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat heigth, GLuint texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
    glBegin( GL_QUADS );  
	glTexCoord2f(x1, y1); glVertex3f(x, y, z); // 左下
	glTexCoord2f(x1, y2); glVertex3f( x, y+heigth, z);     // 左上
	glTexCoord2f(x2, y2); glVertex3f( x+width, y+heigth, z);     // 右上
	glTexCoord2f(x2, y1); glVertex3f(x+width, y, z);     // 右下 
    glEnd(); 
}

void dis_bit(GLfloat des_x1, GLfloat des_x2, GLfloat des_y1, GLfloat des_y2,GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat heigth, GLuint texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
    glBegin( GL_QUADS );  
	glTexCoord2f(des_x1, des_y1); glVertex3f(x, y, z); // 左下
	glTexCoord2f(des_x1, des_y2); glVertex3f( x, y+heigth, z);     // 左上
	glTexCoord2f(des_x2, des_y2); glVertex3f( x+width, y+heigth, z);     // 右上
	glTexCoord2f(des_x2, des_y1); glVertex3f(x+width, y, z);     // 右下 
    glEnd(); 
}

void Drawtext(char *string,double x,double y)
{
	char *c;
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
}

void DrawWait(float x, float y)
{
	static int s=0;
	s+=5;
	if(s>360)
		s=0;
	
	glPushMatrix();  
	glLoadIdentity();
	glTranslatef(x,y,0);
	glRotatef(s,0,0,-1.0f);
	ShowPic(-(wait.width/2.0f), -(wait.height/2.0f), 0, wait.width, wait.height, wait.texID);
	glPopMatrix();
}

void DrawRect(float x, float y,float w, float h,int flag)
{
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D); 
	glBegin(GL_QUADS);
	if (flag)
		glColor3f(0.0,0.0,0.0);
	else
		glColor3f(0.1,0.1,0.1);
	glVertex3f(x,y+h,0.0);

	if (flag)
		glColor3f(0.5,0.5,0.5);
	else
		glColor3f(0.5,0.5,0.5);
	
	glVertex3f(x+w,y+h,0.0);
	glVertex3f(x+w,y,0.0);

	if (flag)
		glColor3f(0.0,0.0,0.0);
	else
		glColor3f(0.1,0.1,0.1);
	glVertex3f(x,y,0.0);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}
void DrawYeRect(float x, float y,float w, float h)
{

	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D); 
	glColor3ub(231,111,1);
	glBegin(GL_QUADS);	
	glVertex3f(x,y+h,0.0);
	glVertex3f(x+w,y+h,0.0);
	glVertex3f(x+w,y,0.0);
	glVertex3f(x,y,0.0);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}
void DrawWhRect(float x, float y,float w, float h)
{
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D); 
	glColor3ub(255,255,255);
	glBegin(GL_QUADS);	
	glVertex3f(x,y+h,0.0);
	glVertex3f(x+w,y+h,0.0);
	glVertex3f(x+w,y,0.0);
	glVertex3f(x,y,0.0);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void DrawBlWave(float x, float y,float w, float h)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	GLint circle_points = 100;
	int i = 0;
	float zdep = 0.0f;
	float z=0,r=12.0f;
	float width = 13.0f;
	
	glDisable(GL_TEXTURE_2D); 

	glBegin(GL_QUADS);	
	//glColor3f(1.0,0.6,0.0);
	glColor3ub(26,76,125);
	glVertex3f(x, y, z+depth);
	glVertex3f(x, y+h, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x-width, y+h, z-zdep+depth);
	glVertex3f(x-width, y, z-zdep+depth);
	glEnd();

	glBegin(GL_QUADS);	
	glColor3ub(26,76,125);
	glVertex3f(x, y+h, z+depth);
	glVertex3f(x+w, y+h, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x+w, y+h+width, z-zdep+depth);
	glVertex3f(x, y+h+width, z-zdep+depth);
	glEnd();

	glBegin(GL_QUADS);	
	glColor3ub(26,76,125);
	glVertex3f(x+w, y+h, z+depth);
	glVertex3f(x+w, y, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x+width+w, y, z-zdep+depth);
	glVertex3f(x+width+w, y+h, z-zdep+depth);
	glEnd();

	glBegin(GL_QUADS);	
	glColor3ub(26,76,125);
	glVertex3f(x+w, y, z+depth);
	glVertex3f(x, y, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x, y-width, z-zdep+depth);
	glVertex3f(x+w, y-width, z-zdep+depth);
	glEnd();
	
	
    glBegin(GL_TRIANGLE_FAN);
	glColor3ub(26,76,125);
	glVertex3f(x, y, 0.0f+depth);
    for(i = circle_points*2/4; i <= circle_points*3/4; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
		
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x, r*sin(angle)+y, -3.0f+depth);
    }
	
    glEnd();


	glBegin(GL_TRIANGLE_FAN);
	glColor3ub(26,76,125);
	glVertex3f(x, y+h, 0.0f+depth);
	for(i = circle_points*1/4; i <= circle_points*2/4; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
			
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x, r*sin(angle)+y+h, -3.0f+depth);
    }
    glEnd();	

	glBegin(GL_TRIANGLE_FAN);
	glColor3ub(26,76,125);
	glVertex3f(x+w, y+h, 0.0f+depth);
	for(i = 0; i <= circle_points*1/4; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
		
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x+w, r*sin(angle)+y+h, -3.0f+depth);
    }
    glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3ub(26,76,125);
	glVertex3f(x+w, y, 0.0f+depth);
	for(i = circle_points*3/4; i <= circle_points; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
		
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x+w, r*sin(angle)+y,-3.0f+depth);
    }
    glEnd();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
void DrawGrWave(float x, float y,float w, float h)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	GLint circle_points = 100;
	int i = 0;
	float zdep = 0.0f;
	float z=0,r=12.0f;
	float width = 13.0f;
	
	glDisable(GL_TEXTURE_2D); 

	glBegin(GL_QUADS);	
	//glColor3f(1.0,0.6,0.0);
	glColor3ub(127,197,57);
	glVertex3f(x, y, z+depth);
	glVertex3f(x, y+h, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x-width, y+h, z-zdep+depth);
	glVertex3f(x-width, y, z-zdep+depth);
	glEnd();

	glBegin(GL_QUADS);	
	glColor3ub(127,197,57);
	glVertex3f(x, y+h, z+depth);
	glVertex3f(x+w, y+h, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x+w, y+h+width, z-zdep+depth);
	glVertex3f(x, y+h+width, z-zdep+depth);
	glEnd();

	glBegin(GL_QUADS);	
	glColor3ub(127,197,57);
	glVertex3f(x+w, y+h, z+depth);
	glVertex3f(x+w, y, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x+width+w, y, z-zdep+depth);
	glVertex3f(x+width+w, y+h, z-zdep+depth);
	glEnd();

	glBegin(GL_QUADS);	
	glColor3ub(127,197,57);
	glVertex3f(x+w, y, z+depth);
	glVertex3f(x, y, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x, y-width, z-zdep+depth);
	glVertex3f(x+w, y-width, z-zdep+depth);
	glEnd();
	
    glBegin(GL_TRIANGLE_FAN);
	glColor3ub(127,197,57);
	glVertex3f(x, y, 0.0f+depth);
    for(i = circle_points*2/4; i <= circle_points*3/4; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
		
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x, r*sin(angle)+y, -3.0f+depth);
    }
	
    glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3ub(127,197,57);
	glVertex3f(x, y+h, 0.0f+depth);
	for(i = circle_points*1/4; i <= circle_points*2/4; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
			
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x, r*sin(angle)+y+h, -3.0f+depth);
    }
    glEnd();	

	glBegin(GL_TRIANGLE_FAN);
	glColor3ub(127,197,57);
	glVertex3f(x+w, y+h, 0.0f+depth);
	for(i = 0; i <= circle_points*1/4; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
		
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x+w, r*sin(angle)+y+h, -3.0f+depth);
    }
    glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3ub(127,197,57);
	glVertex3f(x+w, y, 0.0f+depth);
	for(i = circle_points*3/4; i <= circle_points; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
		
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x+w, r*sin(angle)+y,-3.0f+depth);
    }
    glEnd();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
void DrawRectangle(float x, float y,float w, float h)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	GLint circle_points = 100;
	int i = 0;
	float zdep = 0.0f;
	float z=0,r=12.0f;
	float width = 13.0f;
	
	glDisable(GL_TEXTURE_2D); 

	glBegin(GL_QUADS);	
	glColor3f(1.0,0.6,0.0);
	glVertex3f(x, y, z+depth);
	glVertex3f(x, y+h, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x-width, y+h, z-zdep+depth);
	glVertex3f(x-width, y, z-zdep+depth);
	glEnd();

	glBegin(GL_QUADS);	
	glColor3f(1.0,0.6,0.0);
	glVertex3f(x, y+h, z+depth);
	glVertex3f(x+w, y+h, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x+w, y+h+width, z-zdep+depth);
	glVertex3f(x, y+h+width, z-zdep+depth);
	glEnd();

	glBegin(GL_QUADS);	
	glColor3f(1.0,0.6,0.0);
	glVertex3f(x+w, y+h, z+depth);
	glVertex3f(x+w, y, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x+width+w, y, z-zdep+depth);
	glVertex3f(x+width+w, y+h, z-zdep+depth);
	glEnd();

	glBegin(GL_QUADS);	
	glColor3f(1.0,0.6,0.0);
	glVertex3f(x+w, y, z+depth);
	glVertex3f(x, y, z+depth);

	glColor4f(0.3,0.3,0.3,0.0f);
	glVertex3f(x, y-width, z-zdep+depth);
	glVertex3f(x+w, y-width, z-zdep+depth);
	glEnd();
	
	
    glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0,0.6,0.0);
	glVertex3f(x, y, 0.0f+depth);
    for(i = circle_points*2/4; i <= circle_points*3/4; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
		
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x, r*sin(angle)+y, -3.0f+depth);
    }
    glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0,0.6,0.0);
	glVertex3f(x, y+h, 0.0f+depth);
	for(i = circle_points*1/4; i <= circle_points*2/4; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
			
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x, r*sin(angle)+y+h, -3.0f+depth);
    }
    glEnd();	

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0,0.6,0.0);
	glVertex3f(x+w, y+h, 0.0f+depth);
	for(i = 0; i <= circle_points*1/4; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
		
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x+w, r*sin(angle)+y+h, -3.0f+depth);
    }
    glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0,0.6,0.0);
	glVertex3f(x+w, y, 0.0f+depth);
	for(i = circle_points*3/4; i <= circle_points; i++ )
    {
        float angle = 2*GL_PI*i/circle_points;
		
			glColor4f(0.3,0.3,0.3,0.0);
        glVertex3f(r*cos(angle)+x+w, r*sin(angle)+y,-3.0f+depth);
    }
    glEnd();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

int CreateImgTex(TexImg *tex,const char* name)
{
	if(!name)
		return false;
	if(strstr(name, ".jpg") ||strstr(name, ".jpeg")) 
	{
		GLuint texID; //纹理的标识
		tImageJPG *pJpg = 0; //存放JPG纹理像素数据

		if(!name) //如果文件名为空则返回
		{
			return -1;
		}
		pJpg = LoadJPG(name);
		
		if(!pJpg) 
			return -1;

		glGenTextures(1, &tex[0].texID);
		glBindTexture(GL_TEXTURE_2D, tex[0].texID);					
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pJpg->sizeX, pJpg->sizeY,	0, GL_RGB, GL_UNSIGNED_BYTE, pJpg->data);

		tex->width = pJpg->sizeX;
		tex->height = pJpg->sizeY;
		
		if(pJpg) 
			free(pJpg);
		
		return 0;
	}
	else if(strstr(name, ".png"))
	{
		unsigned char header[8];
		int k;   
		GLuint texID;
		int width, height; 
		png_byte color_type;
		png_byte bit_depth;

		png_structp png_ptr; 
		png_infop info_ptr; 
		int number_of_passes; 
		png_bytep *row_pointers;
		int row,col,pos;  
		GLubyte *rgba;

		FILE *fp=fopen(name,"rb");
		if(!fp)
		{
			fclose(fp);
			return -1;
		}

		fread(header, 1, 8, fp);
		if(png_sig_cmp(header,0,8))
		{
			fclose(fp);
			return -1;
		}

		png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL); 
		if(!png_ptr)
		{ 
			fclose(fp);
			return -1;
		}
		info_ptr=png_create_info_struct(png_ptr);

		if(!info_ptr)
		{
			png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
			fclose(fp);
			return -1;
		}
		if (setjmp(png_jmpbuf(png_ptr)))
		{
		 png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
		 fclose(fp);
		 return -1;
		}
		png_init_io(png_ptr,fp);
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);
		width = png_get_image_width(png_ptr, info_ptr);
		height = png_get_image_height(png_ptr, info_ptr);
		color_type = png_get_color_type(png_ptr, info_ptr);
		
		tex->width = width;
		tex->height = height;
		if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		{
			png_set_swap_alpha(png_ptr);
		}
		bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		number_of_passes = png_set_interlace_handling(png_ptr);
		png_read_update_info(png_ptr, info_ptr);

		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fclose(fp);
			return -1;
		}
		rgba=(GLubyte*)malloc(width * height * 4);
		row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
		for (k = 0; k < height; k++)
			row_pointers[k] = NULL;
		  
		for (k=0; k<height; k++)
		{
			row_pointers[k] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr,info_ptr));
		}

		png_read_image(png_ptr, row_pointers);

		pos = (width * height * 4) - (4 * width);
		for( row = 0; row < height; row++)
		{
			for( col = 0; col < (4 * width); col += 4)
			{
				rgba[pos++] = row_pointers[row][col+1];        // red
				rgba[pos++] = row_pointers[row][col + 2];    // green
				rgba[pos++] = row_pointers[row][col + 3];    // blue
				rgba[pos++] = row_pointers[row][col ];    // alpha
			}
			pos=(pos - (width * 4)*2);
		}
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1,&tex[0].texID);
		glBindTexture(GL_TEXTURE_2D,tex[0].texID); //将纹理绑定到名字

		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	 
		free(row_pointers); 
		fclose(fp);	
	}
	else if(strstr(name, ".bmp"))
	{
		GLint width, height, total_bytes;	   
		FILE* pFile = fopen(name, "rb");
		GLubyte* imageData = NULL; 
		if( pFile == NULL )
		{
			return -1;
		}

		fseek(pFile, 0x0012, SEEK_SET);
		fread(&width, 4, 1, pFile);
		fread(&height, 4, 1, pFile);
		fseek(pFile, 54, SEEK_SET);

		tex->width = width;
		tex->height = height;

		{
			GLint line_bytes = width * 3;
			while( line_bytes % 4 != 0 )
			 ++line_bytes;
			total_bytes = line_bytes * height;
		}

		imageData = (GLubyte*)malloc(total_bytes);
		if(imageData == NULL )
		{
			fclose(pFile);
			return -1;
		}

		if( fread(imageData, total_bytes, 1, pFile) <= 0 )
		{
			free(imageData);
			fclose(pFile);
			return -1;
		}

		glGenTextures(1, &tex[0].texID);
		if( tex[0].texID == 0 )
		{
			free(imageData);
			fclose(pFile);
			return -1;
		}
		fclose(pFile);
		glBindTexture(GL_TEXTURE_2D, tex[0].texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, imageData);
		free(imageData);
	}
	else if(strstr(name, ".tga"))
	{
		GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
		GLubyte		TGAcompare[12];								// Used To Compare TGA Header
		GLubyte		header[6];									// First 6 Useful Bytes From The Header
		GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
		GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
		GLuint		temp;										// Temporary Variable
		GLuint		type=GL_RGBA;								// Set The Default GL Mode To RBGA (32 BPP)
		GLuint      bpp;
		GLubyte* 	imageData = NULL; 

		FILE *file = fopen(name, "rb");						// Open The TGA File

		if(	file==NULL ||										// Does File Even Exist?
			fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
			memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
			fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
		{
			if (file == NULL)									// Did The File Even Exist? *Added Jim Strong*		
			{	
				return -1;
			}									// Return False
		}
		tex->width  = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
		tex->height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)

		//OpenGL中纹理只能使用24位或者32位的TGA图像
		if(	tex->width	<=0	||								// Is The Width Less Than Or Equal To Zero
			tex->height	<=0	||								// Is The Height Less Than Or Equal To Zero
			(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
		{
			fclose(file);										// If Anything Failed, Close The File
			return -1;										// Return False
		}

		bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
		bytesPerPixel	= bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
		imageSize		= tex->width*tex->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

		imageData=(GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

		if(	imageData==NULL ||							// Does The Storage Memory Exist?
			fread(imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
		{
			if(imageData!=NULL)						// Was Image Data Loaded
				free(imageData);						// If So, Release The Image Data

			fclose(file);										// Close The File
			return -1;										// Return False
		}

		for(GLuint i=0; i<(imageSize); i+=bytesPerPixel)		
		{														
			temp=imageData[i];							
			imageData[i] = imageData[i + 2];	
			imageData[i + 2] = temp;					
		}
		fclose(file);											
		
		glGenTextures(1, &tex[0].texID);			
		glBindTexture(GL_TEXTURE_2D, tex[0].texID);			
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

		if (bpp==24)									
		{
			type=GL_RGB;										
		}
		glTexImage2D(GL_TEXTURE_2D, 0, type, tex[0].width, tex[0].height, 0, type, GL_UNSIGNED_BYTE, imageData);
		free(imageData);
	}
	printf("LoadImg: %s %dx%d\n",name,tex->width, tex->height);
	return 0;
	
}

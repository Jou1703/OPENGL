#ifndef __DRAW_UTILS_H__
#define __DRAW_UTILS_H__

typedef struct  
{  
    GLubyte *imageData; 
    GLuint bpp; 
    GLuint width; 
    GLuint height;
    GLuint texID;
}TextureImage; 

typedef struct  
{  
    GLuint width; 
    GLuint height;
    GLuint texID;
}TexImg; 

struct tImageJPG
{
	int rowSpan;
	int sizeX;
	int sizeY;
	unsigned char *data;
};
int CreateImgTex(TexImg *tex,const char* name);
GLuint CreateTextureFromPng(const char* filename);
GLuint CreateTexture(const char* strTextureFileName);
void ShowJpg(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat heigth, GLuint texture);

GLuint load_texture(char* file_name);
GLuint LoadBmpTex(TextureImage *texture ,char* file_name);
int LoadTgaTex(TextureImage *texture, char *filename);

GLuint DelTexID(TextureImage *texture);
void ShowPic(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat heigth, GLuint texture);
void ShowPartPic(GLfloat des_x1, GLfloat des_x2, GLfloat des_y1, GLfloat des_y2,GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat heigth, GLuint texture);

void DrawBlWave(float x, float y,float w, float h);
void DrawGrWave(float x, float y,float w, float h);
void DrawRectangle(float x, float y,float w, float h);
void DrawRect(float x, float y,float w, float h,int flag);
void DrawYeRect(float x, float y,float w, float h);
void DrawWhRect(float x, float y,float w, float h);

void Drawtext(char *string,double x,double y);

void dis_bit(GLfloat des_x1, GLfloat des_x2, GLfloat des_y1, GLfloat des_y2,GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat heigth, GLuint texture);

char *get_path(const char *p);
void dis_num(float x, float y, float z,GLuint n);
void dis_text(float x, float y, float z,const char *string,int style);
void dis_char(float x, float y, float z,char c,int style);
void BitLoad();
void DrawNum(float x, float y, float z,const char* string,GLuint color);
void FontDraw(float x,float y,const char *string,int size);
void FontInit();
void DrawWait(float x, float y);

#endif //__DRAW_UTILS_H__

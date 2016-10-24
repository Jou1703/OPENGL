#include "includes.h"
#include "particle.h"
#include "init_ui.h"
#include "draw_utils.h"
#include "option.h"

GLuint bubble_image;

extern char leftmove, rightmove;
extern float x_drift;
extern UI_PAR shimmerUI_par;
float slowdown = 2.0f;
float xspeed;
float yspeed;
float zoom;

GLuint loop;
GLuint col;
GLuint delay;

typedef struct PARTICLES{
	GLuint active;
	float life;
	float fade;
	float r;
	float g;
	float b;
	GLfloat x;
	GLfloat y;
	GLfloat z;
	float xi;
	float yi;
	float zi;	
}TParticles;
/*
static GLfloat colors[12][3]=				// 彩虹颜色
{
	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},

	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},

	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};
*/
const GLfloat Pi=3.1415926536f;
TParticles particle[1000];

GLuint Load_transparentpic(const char* file_name, int alpha)
{
     GLint width, height, total_bytes;
     GLubyte* pixels = 0;
     GLint last_texture_ID;
	 GLuint texture_ID = 0;

     FILE* pFile = fopen(file_name, "rb");
    if( pFile == 0 )
        return 0;

     // 读取文件中图象的宽度和高度
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

	GLubyte* new_pixels = 0;
	int i;

	new_pixels = (GLubyte*)malloc((total_bytes/3)*4);
	if( pixels == 0 )
    {
        fclose(pFile);
        exit(0);
    }

	for (i=0; i<width*height;++i)
	{
		new_pixels[i*4]=pixels[i*3];	
		new_pixels[i*4+1]=pixels[i*3+1];	
		new_pixels[i*4+2]=pixels[i*3+2];
		if ((255-pixels[i*3]) >= alpha && 
			(255-pixels[i*3+1]) >= alpha &&
			(255-pixels[i*3+2]) >= alpha)	
			new_pixels[i*4+3]=0;    
		else
			new_pixels[i*4+3]=255;	
	}
     glGenTextures(1, &texture_ID);
    if( texture_ID == 0 )
     {
        free(pixels);
        fclose(pFile);
        exit(0);
     }

     // 绑定新的纹理，载入纹理并设置纹理参数
     // 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
     glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
     glBindTexture(GL_TEXTURE_2D, texture_ID);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	 gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height,GL_BGRA_EXT, GL_UNSIGNED_BYTE, new_pixels);

     glBindTexture(GL_TEXTURE_2D, last_texture_ID);
	 printf("width = %d height = %d\n", width, height);

     // 之前为pixels分配的内存可在使用glTexImage2D以后释放
     // 因为此时像素数据已经被OpenGL另行保存了一份（可能被保存到专门的图形硬件中）
    free(pixels);
    return texture_ID;
}

void Particles_init(void)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //指定混合函数
	glEnable(GL_BLEND);

	bubble_image = Load_transparentpic("/home/ma/Shimmer/src/image/bubble.bmp",10);
	
	for(loop=0; loop<50; ++loop)
	{
		particle[loop].active = 1;
		particle[loop].life=1.0f;
		particle[loop].fade=0.002f;
		particle[loop].xi=(float)((rand()%50)-26.0f)*10.0f;		// 随机生成X轴方向速度
		particle[loop].yi=(float)((rand()%50)-25.0f)*10.0f;		// 随机生成Y轴方向速度
		if(loop<25)
		{
			particle[loop].x=960.0f;
			particle[loop].y=-500.0f;
		}
		else 
		{
			particle[loop].x=-960.0f;
			particle[loop].y=500.0f;
		}
	}
}


void ParticlesRender(void)
{	
	for(loop=0; loop<50; ++loop)
	{
		if (particle[loop].active)					
		{
			GLfloat x;				
			GLfloat y=particle[loop].y;				
		/*	if(leftmove)
				x=particle[loop].x-(200*(4-shimmerUI_par.position) + x_drift);
			else if(rightmove)
				x=particle[loop].x-(200*(6-shimmerUI_par.position) - x_drift);
			else
		*/	
			x=particle[loop].x;
			ShowPic(x, y, -3.0f, BUBBLE_WIDTH, BUBBLE_HEIGTH, bubble_image);
			particle[loop].x+=particle[loop].xi/(slowdown*100);
			particle[loop].y+=particle[loop].yi/(slowdown*100);

			particle[loop].life -= particle[loop].fade;
			if(particle[loop].life <= 0.0f)	
			{	
				//particle[loop].active = 0;
				particle[loop].life = 1.0f;
				particle[loop].xi=(float)((rand()%50)-26.0f)*10.0f;		
				particle[loop].yi=(float)((rand()%50)-25.0f)*10.0f;		
			}
		}
	}
}

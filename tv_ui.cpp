#include "includes.h"

#include "init_ui.h"
#include "particle.h"
#include "draw_utils.h"
#include "option.h"
#include "tv_ui.h"

extern float xcoord, ycoord, width, heigth;
extern TextureImage bg_tex;
TextureImage mgtv_title,tvtu,mangguo;
void MgtvPicLoad(void)
{
	LoadTgaTex(&tvtu ,get_path("mei.tga"));
	LoadTgaTex(&mangguo ,get_path("mangguo.tga"));
}

void MangoRender(void)
{
	
	ShowPic(xcoord, ycoord, 0.0f, width,heigth,bg_tex.texID);
	
	if (width<1902)
		return;
	TipNote();
	TimeDisplay();
	glEnable(GL_BLEND);
	//ShowPic(-880,320.0f, 0.0f, tvtu.width,tvtu.height,tvtu.texID);
	ShowPic(-850,290.0f, 0.0f, 170,150,mangguo.texID);
	glColor3ub(231,111,1);
	//FontDraw(-330, -20.0f, "中国融合媒体平台暂未上线,敬请期待!",30);
	FontDraw(-250, -20.0f, "芒果TV暂未上线,敬请期待!",30);
	glDisable(GL_BLEND);

}


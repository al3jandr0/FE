/******************************************************************************
* Copyright (C) 2011 by Jonathan Appavoo, Boston University
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h> /* for exit() */
#include <pthread.h>
#include <assert.h>
#include "types.h"
#include "uistandalone.h"
#include "../lib/maze.h"
#include "../lib/maze.c"
#include "SDL_rotozoom.h"
#include "SDL_rotozoom.c"


/* A lot of this code comes from http://www.libsdl.org/cgi/docwiki.cgi */

/* Forward declaration of some dummy player code */
static void dummyPlayer_init(UI *ui);
static void dummyPlayer_paint(UI *ui);

#define SPRITE_H 32
#define SPRITE_W 32

#define UI_FLOOR_BMP "floor.bmp"
#define UI_REDWALL_BMP "redwall.bmp"
#define UI_GREENWALL_BMP "greenwall.bmp"
#define UI_TEAMA_BMP "teama.bmp"
#define UI_TEAMB_BMP "teamb.bmp"
#define UI_LOGO_BMP "logo.bmp"
#define UI_REDFLAG_BMP "redflag.bmp"
#define UI_GREENFLAG_BMP "greenflag.bmp"
#define UI_JACKHAMMER_BMP "shovel.bmp"

typedef enum {UI_SDLEVENT_UPDATE, UI_SDLEVENT_QUIT} UI_SDL_Event;

struct UI_Player_Struct {
  SDL_Surface *img;
  uval base_clip_x;
  SDL_Rect clip;
};
typedef struct UI_Player_Struct UI_Player;


char mazem[40000];



char *loadMapArray()
{
  //  char maze[40000];
  FILE *mapFile;
  if((mapFile = fopen("./../daGame.map", "r"))==NULL) {
    printf("Cannot open file.\n");
    return mazem;
  }
  int c;

  
  rewind(mapFile);    
    
  int currentIndex = 0;
  while ((c = fgetc(mapFile)) != EOF) {
    if(currentIndex == 198) {
      fprintf(stderr, "here");
    }
    if (currentIndex%200!= 0) {
      // newCell.C_Type = c;
      // newCell.Cell_Pos.x = currentIndex/200;
      int team = (currentIndex % 200);
      team =  team %200;

      mazem[currentIndex] = ' ';

      if (team<100){
	// newCell.Cell_Team = Team1;
	if (c=='#'){
	  mazem[currentIndex] = 'w';
	}
      } else {
	// newCell.Cell_Team = Team2;
	if (c=='#'){
	  mazem[currentIndex] = 'W';
	}
      }
      
      
      //  fprintf(stderr, "\ncurrentIndex ==%d\ncurVariable==%d\n\n", currentIndex, mazem[currentIndex]);
      currentIndex++;
      
    } else{
      //     if ((c = fgetc(mapFile)) == EOF) break;
      c = fgetc(mapFile);
      mazem[currentIndex] = 'w';
      currentIndex++;
      //    if (c=='#') mazem[currentIndex] = 'w';
    }
  }
  
  fclose(mapFile);
  
  return mazem;
}
    
  
void
dumpInfo(UI *ui)
{
  
  fprintf(stderr, "\n\nui->screen INFO\n  w--%d\n  h--%d\n  x--%d\n  y--%d\n",   ui->screen->w, ui->screen->h, ui->screen->clip_rect.x, ui->screen->clip_rect.y);    // %d%d%d%d%d%d%d%d%d%d"
    
 fprintf(stderr, "ui->scaledMap INFO\n  w--%d\n  h--%d\n  x--%d\n  y--%d\n", ui->scaledMap->w, ui->scaledMap->h, ui->scaledMap->clip_rect.x, ui->scaledMap->clip_rect.y);

 fprintf(stderr, "ui->fullMap INFO\n  w--%d\n  h--%d\n  x--%d\n  y--%d\n", ui->fullMap->w, ui->fullMap->h, ui->fullMap->clip_rect.x, ui->fullMap->clip_rect.y);  

 fprintf(stderr, "ui->mapFrame INFO\n  w--%d\n  h--%d\n  x--%d\n  y--%d\n", ui->mapFrame.w, ui->mapFrame.h, ui->mapFrame.x, ui->mapFrame.y);  

 fprintf(stderr, "ui window factor -- %d\nui currscale -- %d\n\n\n",(int) &ui->windowFactor,(int) ui->currScale);

}

SDL_Surface*
createSurface(int width,int height,const SDL_Surface* display)
{
  
  SDL_PixelFormat fmt = *(display->format);
  return SDL_CreateRGBSurface(display->flags,width,height,
			      fmt.BitsPerPixel,
			      fmt.Rmask,fmt.Gmask,fmt.Bmask,fmt.Amask );
}

void
scale_map_frame(UI *ui)
{
  /*
  fprintf(stderr, "INSIDE  ZOOMFIT");
  //put checks in zoom for windowFac
  
  double scale_x = (double) (scaled->w) / (double) (unscaled->w);
  double scale_y = (double) (scaled->h) / (double) (unscaled->h);
  if (scale_y < scale_x) {
    ui->windowFactor = scale_y;
  } else {
    ui->windowFactor = scale_x;
  }
  SDL_Surface *result = zoomSurface(unscaled, ui->windowFactor,  ui->windowFactor, SMOOTHING_ON);
  if (result == (SDL_Surface *) (NULL))
    {
      fprintf(stderr, "\n Error from zoomSurface()\n\n");
      ui_quit(ui);
    }
  
  scaled = result;
  */


  ui->screenFrame.x = 0;
  ui->screenFrame.y = 0;
  // ui->mapFrame.h = ui->fullMap->h;
  //  ui->mapFrame.w = ;

  double scale_x = (double) (ui->screen->w) / (double) (ui->mapFrame.w);
  double scale_y = (double) (ui->screen->h) / (double) (ui->mapFrame.h);
  if (scale_y < scale_x) {
    ui->windowFactor = scale_y;
  } else {
    ui->windowFactor = scale_x;
  }
  
  SDL_Surface* screenTEMP =  createSurface(ui->mapFrame.w,ui->mapFrame.h,ui->screen);


 //SDL_ConvertSurface(ui->screen, ui->screen->format, ui->screen->flags);
  /*
  int x,y,w,h;
  x = ui->mapFrame.x;
  y = ui->mapFrame.y;
  w = ui->mapFrame.w;
  h = ui->mapFrame.h;
  */
  dumpInfo(ui);
  SDL_BlitSurface(ui->fullMap,&ui->mapFrame, screenTEMP, &ui->screenFrame);
  
/*
  ui->mapFrame.x=x;
  ui->mapFrame.y=y;
  ui->mapFrame.w=w;
  ui->mapFrame.h=h;
  */
  // SDL_Surface *tmp = createSurface(ui->screen->w,ui->screen->h,ui->screen);
   SDL_Surface *tmp = zoomSurface(screenTEMP, ui->windowFactor,  ui->windowFactor, SMOOTHING_ON);
  if (tmp == (SDL_Surface *) (NULL))
    {
      fprintf(stderr, "\n Error from zoomSurface()\n\n");
      ui_quit(ui);
    }
  
  ui->scaledMap = tmp;
}


static inline SDL_Surface *
ui_player_img(UI *ui, int team)
{  
  return (team == 0) ? ui->sprites[TEAMA_S].img 
    : ui->sprites[TEAMB_S].img;
}

static inline sval 
pxSpriteOffSet(int team, int state)
{
  if (state == 1)
    return (team==0) ? SPRITE_W*1 : SPRITE_W*2;
  if (state == 2) 
    return (team==0) ? SPRITE_W*2 : SPRITE_W*1;
  if (state == 3) return SPRITE_W*3;
  return 0;
}

static sval
ui_uip_init(UI *ui, UI_Player **p, int id, int team)
{
  UI_Player *ui_p;
  
  ui_p = (UI_Player *)malloc(sizeof(UI_Player));
  if (!ui_p) return 0;

  ui_p->img = ui_player_img(ui, team);
  ui_p->clip.w = SPRITE_W; ui_p->clip.h = SPRITE_H; ui_p->clip.y = 0;
  ui_p->base_clip_x = id * SPRITE_W * 4;

  *p = ui_p;

  return 1;
}

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
static uint32_t 
ui_getpixel(SDL_Surface *surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  uint8_t *p = (uint8_t *)surface->pixels + y * surface->pitch + x * bpp;
  
  switch (bpp) {
  case 1:
    return *p;
  case 2:
    return *(uint16_t *)p;
  case 3:
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;
  case 4:
    return *(uint32_t *)p;
  default:
    return 0;       /* shouldn't happen, but avoids warnings */
  } // switch
}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
static void 
ui_putpixel(SDL_Surface *surface, int x, int y, uint32_t pixel)
 {
   int bpp = surface->format->BytesPerPixel;
   /* Here p is the address to the pixel we want to set */
   uint8_t *p = (uint8_t *)surface->pixels + y * surface->pitch + x * bpp;

   switch (bpp) {
   case 1:
	*p = pixel;
	break;
   case 2:
     *(uint16_t *)p = pixel;
     break;     
   case 3:
     if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
       p[0] = (pixel >> 16) & 0xff;
       p[1] = (pixel >> 8) & 0xff;
       p[2] = pixel & 0xff;
     }
     else {
       p[0] = pixel & 0xff;
       p[1] = (pixel >> 8) & 0xff;
       p[2] = (pixel >> 16) & 0xff;
     }
     break;
 
   case 4:
     *(uint32_t *)p = pixel;
     break;
 
   default:
     break;           /* shouldn't happen, but avoids warnings */
   } // switch
 }

static 
sval splash(UI *ui)
{
  SDL_Rect r;
  SDL_Surface *temp;


  temp = SDL_LoadBMP(UI_LOGO_BMP);
  
  if (temp != NULL) {
    ui->sprites[LOGO_S].img = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    r.h = ui->sprites[LOGO_S].img->h;
    r.w = ui->sprites[LOGO_S].img->w;
    r.x = ui->screen->w/2 - r.w/2;
    r.y = ui->screen->h/2 - r.h/2;
    //    printf("r.h=%d r.w=%d r.x=%d r.y=%d\n", r.h, r.w, r.x, r.y);
    SDL_BlitSurface(ui->sprites[LOGO_S].img, NULL, ui->screen, &r);
  } else {
    /* Map the color yellow to this display (R=0xff, G=0xFF, B=0x00)
       Note:  If the display is palettized, you must set the palette first.
    */
    r.h = 40;
    r.w = 80;
    r.x = ui->screen->w/2 - r.w/2;
    r.y = ui->screen->h/2 - r.h/2;
 
    /* Lock the screen for direct access to the pixels */
    if ( SDL_MUSTLOCK(ui->screen) ) {
      if ( SDL_LockSurface(ui->screen) < 0 ) {
	fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
	return -1;
      }
    }
    SDL_FillRect(ui->screen, &r, ui->yellow_c);

    if ( SDL_MUSTLOCK(ui->screen) ) {
      SDL_UnlockSurface(ui->screen);
    }
  }
  /* Update just the part of the display that we've changed */
  SDL_UpdateRect(ui->screen, r.x, r.y, r.w, r.h);

  SDL_Delay(1000);
  return 1;
}


static sval
load_sprites(UI *ui) 
{
  SDL_Surface *temp;
  sval colorkey;

  /* setup sprite colorkey and turn on RLE */
  // FIXME:  Don't know why colorkey = purple_c; does not work here???
  colorkey = SDL_MapRGB(ui->screen->format, 255, 0, 255);
  
  temp = SDL_LoadBMP(UI_TEAMA_BMP);
  if (temp == NULL) { 
    fprintf(stderr, "ERROR: loading teama.bmp: %s", SDL_GetError()); 
    return -1;
  }
  ui->sprites[TEAMA_S].img = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  SDL_SetColorKey(ui->sprites[TEAMA_S].img, SDL_SRCCOLORKEY | SDL_RLEACCEL, 
		  colorkey);

  temp = SDL_LoadBMP(UI_TEAMB_BMP);
  if (temp == NULL) { 
    fprintf(stderr, "ERROR: loading teamb.bmp: %s\n", SDL_GetError()); 
    return -1;
  }
  ui->sprites[TEAMB_S].img = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

  SDL_SetColorKey(ui->sprites[TEAMB_S].img, SDL_SRCCOLORKEY | SDL_RLEACCEL, 
		  colorkey);
  temp = SDL_LoadBMP(UI_FLOOR_BMP);
  if (temp == NULL) {
    fprintf(stderr, "ERROR: loading floor.bmp %s\n", SDL_GetError()); 
    return -1;
  }
  ui->sprites[FLOOR_S].img = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  SDL_SetColorKey(ui->sprites[FLOOR_S].img, SDL_SRCCOLORKEY | SDL_RLEACCEL, 
		  colorkey);

  temp = SDL_LoadBMP(UI_REDWALL_BMP);
  if (temp == NULL) { 
    fprintf(stderr, "ERROR: loading redwall.bmp: %s\n", SDL_GetError());
    return -1;
  }
  ui->sprites[REDWALL_S].img = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  SDL_SetColorKey(ui->sprites[REDWALL_S].img, SDL_SRCCOLORKEY | SDL_RLEACCEL, 
		  colorkey);

  temp = SDL_LoadBMP(UI_GREENWALL_BMP);
  if (temp == NULL) {
    fprintf(stderr, "ERROR: loading greenwall.bmp: %s", SDL_GetError()); 
    return -1;
  }
  ui->sprites[GREENWALL_S].img = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  SDL_SetColorKey(ui->sprites[GREENWALL_S].img, SDL_SRCCOLORKEY | SDL_RLEACCEL,
		  colorkey);

  temp = SDL_LoadBMP(UI_REDFLAG_BMP);
  if (temp == NULL) {
    fprintf(stderr, "ERROR: loading redflag.bmp: %s", SDL_GetError()); 
    return -1;
  }
  ui->sprites[REDFLAG_S].img = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  SDL_SetColorKey(ui->sprites[REDFLAG_S].img, SDL_SRCCOLORKEY | SDL_RLEACCEL,
		  colorkey);

  temp = SDL_LoadBMP(UI_GREENFLAG_BMP);
  if (temp == NULL) {
    fprintf(stderr, "ERROR: loading redflag.bmp: %s", SDL_GetError()); 
    return -1;
  }
  ui->sprites[GREENFLAG_S].img = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  SDL_SetColorKey(ui->sprites[GREENFLAG_S].img, SDL_SRCCOLORKEY | SDL_RLEACCEL,
		  colorkey);

  temp = SDL_LoadBMP(UI_JACKHAMMER_BMP);
  if (temp == NULL) {
    fprintf(stderr, "ERROR: loading %s: %s", UI_JACKHAMMER_BMP, SDL_GetError()); 
    return -1;
  }
  ui->sprites[JACKHAMMER_S].img = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  SDL_SetColorKey(ui->sprites[JACKHAMMER_S].img, SDL_SRCCOLORKEY | SDL_RLEACCEL,
		  colorkey);
  
  return 1;
}


inline static void
draw_cell(UI *ui, SPRITE_INDEX si, SDL_Rect *t, SDL_Surface *s)
{
  SDL_Surface *ts=NULL;
  uint32_t tc;

  ts = ui->sprites[si].img;

  if ( ts && t->h == SPRITE_H && t->w == SPRITE_W) 
    SDL_BlitSurface(ts, NULL, s, t);
}

static SDL_Rect
ui_fill_sdl_surface(UI *ui, SDL_Surface *s, char *c)
{

  SDL_Rect t;
  int i, j;
  i=0;
  t.y = 0; t.x = 0; t.h = ui->tile_h; t.w = ui->tile_w; 
  
  for (t.y=0; t.y<s->h; t.y+=t.h) {
    for (t.x=0; t.x<s->w; t.x+=t.w) {
      // printf("\nThis is i=%d \nThis is char= %c\n X--%d || Y--%d  || H--%d   || W--%d\n", i,map[i],t.x, t.y, ui->screen->h, t.w);// ui->screen->w );

      char currentChar = c[i];
      i++;
      //      if (i == 100) i =0;
 
      if (currentChar == ' ') {
	draw_cell(ui, FLOOR_S, &t, s);
      
      } else if (currentChar == 'w'){
	draw_cell(ui, REDWALL_S, &t, s);
        
      } else if (currentChar == 'W'){
	draw_cell(ui,  GREENWALL_S, &t, s);
     
      } else if (currentChar == 'f'){
	draw_cell(ui, FLOOR_S, &t, s);
	draw_cell(ui, REDFLAG_S, &t, s);
      
      } else if (currentChar == 'F'){
	draw_cell(ui, FLOOR_S, &t, s);
	draw_cell(ui, GREENFLAG_S, &t, s);
      
      } else if (currentChar == 'p'){
	draw_cell(ui, FLOOR_S, &t, s);
	draw_cell(ui, JACKHAMMER_S, &t, s);
	//	} else if (currentChar == ''){
	//	  draw_cell(ui, FLOOR_S, &t, ui->screen);
      } else {
	draw_cell(ui, FLOOR_S, &t, s);
	//   printf("something wrong");

      }
    }
  }
  return t;
}

static sval
ui_paintmap(UI *ui) 
{

  //  loadMap();
  char map[] ={'W','W','W','W','W','w','w','w','w','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W','W','W','W','W','w','w','w','w','w'};	   

 
  SDL_Rect t =  ui_fill_sdl_surface(ui, ui->screen, map);
 fprintf(stderr, "BEFORE \nt.x--%d   \nt.y--%d   \nt.w--%d \nt.h--%d\n", t.x, t.y, t.w, t.h);
 //dummyPlayer_paint(ui);
   fprintf(stderr, "AFTER \nt.x--%d   \nt.y--%d   \nt.w--%d \nt.h--%d\n", t.x, t.y, t.w, t.h);

  SDL_UpdateRect(ui->screen, 0, 0, ui->screen->w, ui->screen->h);
  return 1;
}

static sval
ui_buff_screen(SDL_Surface *src,SDL_Surface *dst, SDL_Rect *origin, SDL_Rect *size) 
{

  // ui_buff_screen(ui, ui)
  SDL_BlitSurface(src, size, dst, origin);
      
  // SDL_BlitSurface(ui->scaledMap, NULL, ui->screen, ui->screenFrame);
}
/*
static sval
ui_paint_curr_pan(UI *ui) 
{
  
  if (ui->currScale==1) {
    
    //scale_map_frame(SDL_Surface *unscaled, SDL_Surface *scaled);// should workd same
    
    zoomFit(ui);
    // SDL_Surface *result = zoomSurface(ui->fullMap, ui->windowFactor,  ui->windowFactor, SMOOTHING_ON);
    // if (result == (SDL_Surface *) (NULL))
    // {
    //	fprintf(stderr, "\n Error from zoomSurface()\n\n");
    //	ui_quit(ui);
    // }
    
    // ui->scaledMap = result;
  } else {
    
    SDL_Surface *temp = createSurface(ui->mapFrame.w, ui->mapFrame.h, ui->screen);
    ui_buff_screen(ui->fullMap, temp, &ui->mapFrame, &ui->mapFrame);
    
    scale_map_frame(ui, temp , ui->scaledMap);

  }
  

  ui_buff_screen(ui->scaledMap, ui->screen, &ui->screenFrame, &ui->screenFrame);
  

  SDL_UpdateRect(ui->screen, 0, 0, ui->screen->w, ui->screen->h);


  //  ui_buff_screen(ui, ui->scaledMap, ui->screen, &ui->screenFrame);  
  
  
  //image =fullmap;
  //  if (ui->fullMap->w < ui->screen->w && ui->fullMap->h < ui->screen->h) {
  //  scaledImage = zoom100(image, &picturePortion, &scale);
  // } else {
  // ui->scaledMap =
  // }
  // ui->screen;
  // SDL_Rect t= ui->screenf 
  // FIXMEEE AADD PLAYERS TO PRINT

  // 
  //  createSurface(Uint32 flags,int width,int height,const SDL_Surface* display);
  
  
  }
*/

static sval
ui_init_sdl(UI *ui, int32_t h, int32_t w, int32_t d)
{

  fprintf(stderr, "UI_init: Initializing SDL.\n");

  /* Initialize defaults, Video and Audio subsystems */
  if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)==-1)) { 
    fprintf(stderr, "Could not initialize SDL: %s.\n", SDL_GetError());
    return -1;
  }

  atexit(SDL_Quit);

  fprintf(stderr, "ui_init: h=%d w=%d d=%d\n", h, w, d);

  ui->depth = d;
  ui->screen = SDL_SetVideoMode(w, h, ui->depth, SDL_SWSURFACE);
  

  
  ui->scaledMap = createSurface(ui->screen->w, ui->screen->h,ui->screen);

  if ( ui->screen == NULL) {// || ui->screenFrame == NULL) {
    fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n", w, h, ui->depth, 
	    SDL_GetError());
    return -1;
  }

  fprintf(stderr, "UI_init: SDL initialized.\n");


  if (load_sprites(ui)<=0) return -1;

  ui->black_c      = SDL_MapRGB(ui->screen->format, 0x00, 0x00, 0x00);
  ui->white_c      = SDL_MapRGB(ui->screen->format, 0xff, 0xff, 0xff);
  ui->red_c        = SDL_MapRGB(ui->screen->format, 0xff, 0x00, 0x00);
  ui->green_c      = SDL_MapRGB(ui->screen->format, 0x00, 0xff, 0x00);
  ui->yellow_c     = SDL_MapRGB(ui->screen->format, 0xff, 0xff, 0x00);
  ui->purple_c     = SDL_MapRGB(ui->screen->format, 0xff, 0x00, 0xff);

  ui->isle_c         = ui->black_c;
  ui->wall_teama_c   = ui->red_c;
  ui->wall_teamb_c   = ui->green_c;
  ui->player_teama_c = ui->red_c;
  ui->player_teamb_c = ui->green_c;
  ui->flag_teama_c   = ui->white_c;
  ui->flag_teamb_c   = ui->white_c;
  ui->jackhammer_c   = ui->yellow_c;
  
  // ui->screenFrame.w = w;
  // ui->screenFrame.h = h;

  ui->screenFrame.x=0;
  ui->screenFrame.y=0;
  ui->screenFrame.h=  ui->screen->h;
  ui->screenFrame.w=  ui->screen->w;
  

  ui->currScale = 1.0;

  /* set keyboard repeat */
  SDL_EnableKeyRepeat(70, 70);  

  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);

  splash(ui);

  return 1;
}


sval
ui_load_map_sdl(UI *ui)
{
  
  //char *map  =
  loadMapArray();
  /*={'W','W','W','W','W','W','W','W','W','W','w','w','w','w','w','w','w','w','w','w',
    'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
    'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
    'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
    'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
    'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','w',
	       'W','W','W','W','W','W','W','W','W','W','w','w','w','w','w','w','w','w','w','w'};*/
  
  
  sval Dx=200;
  sval Dy=200;
  sval width = (int)(Dx*ui->tile_w);
  sval height = (int)(Dy*ui->tile_h);
  
  
  ui->fullMap = createSurface(width, height,ui->screen);
  //= SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,32,0,0,0,0); SDL_Surface;

  
  SDL_Rect t = ui_fill_sdl_surface(ui, ui->fullMap,  mazem);
  
  // ui->currScale = 1.0;
  // dumpInfo(ui);
  // ui_paint_curr_pan(ui); 
  
}


static void
ui_shutdown_sdl(void)
{
  fprintf(stderr, "UI_shutdown: Quitting SDL.\n");
  SDL_Quit();
}

static sval
ui_userevent(UI *ui, SDL_UserEvent *e) 
{
  if (e->code == UI_SDLEVENT_UPDATE) return 2;
  if (e->code == UI_SDLEVENT_QUIT) return -1;
  return 0;
}
/*
static sval
ui_process(UI *ui)
{
  SDL_Event e;
  sval rc = 1;

  while(SDL_WaitEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      return -1;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      rc = ui_keypress(ui, &(e.key));
      break;
    case SDL_ACTIVEEVENT:
      break;
    case SDL_USEREVENT:
      rc = ui_userevent(ui, &(e.user));
      break;
    default:
      fprintf(stderr, "%s: e.type=%d NOT Handled\n", __func__, e.type);
    }
    if (rc==2) { 
      zoomFit(ui);
      ui_paint_curr_pan(UI *ui) ;
  
//    ui_paintmap(ui); }
    if (rc<0) break;
  }
  return rc;
}
*/
void
pan(UI *ui, int dx, int dy)
{

  if (ui->currScale!=1) {
    if (ui->fullMap->w > ui->mapFrame.w) {
      ui->mapFrame.x += dx;
      if (ui->mapFrame.x < 0) {
	ui->mapFrame.x = 0;
      }
      if ( ui->mapFrame.x >= ui->fullMap->w - ui->mapFrame.w) {
	ui->mapFrame.x = (ui->fullMap->w - ui->mapFrame.w);
      }
    } else {
      ui->mapFrame.x = 0;
    }
    if (ui->fullMap->h > ui->mapFrame.h ) {
      ui->mapFrame.y += dy;
      if (ui->mapFrame.y < 0) {
	ui->mapFrame.y = 0;
      }
      if (ui->mapFrame.y >= ui->fullMap->h - ui->mapFrame.h) {
	ui->mapFrame.y = (ui->fullMap->h - ui->mapFrame.h);

	//	ui->mapFrame.y = (Sint16) (ui->fullMap->h - ui->mapFrame.h);
      }
    } else {
      ui->mapFrame.y = 0;
    }
  
    //   = createSurface(ui->screen->flags, width, height,ui->screen);
    
    
    
  /*SDL_Surface *result
    
    SDL_Surface *result = zoomSurface(ui->fullMap, ui->windowFactor,  ui->windowFactor, SMOOTHING_ON);
    if (result == (SDL_Surface *) (NULL))
    {
    fprintf(stderr, "\n Error from zoomSurface()\n\n");
    ui_quit(ui);
    }
    
    ui->scaledMap = result;
    dumpInfo(ui);
  */
  }
  
}

void
zoomFit(UI *ui)
{

    ui->currScale= 1;

  ui->screenFrame.x = 0;
  ui->screenFrame.y = 0;
  // ui->mapFrame.h = ui->fullMap->h;
  //  ui->mapFrame.w = ;

  double scale_x = (double) (ui->screen->w) / (double) (ui->fullMap->w);
  double scale_y = (double) (ui->screen->h) / (double) (ui->fullMap->h);
  if (scale_y < scale_x) {
    ui->windowFactor = scale_y;
  } else {
    ui->windowFactor = scale_x;
  }
  
  SDL_Surface *result = zoomSurface(ui->fullMap, ui->windowFactor,  ui->windowFactor, SMOOTHING_ON);
  if (result == (SDL_Surface *) (NULL))
    {
      fprintf(stderr, "\n Error from zoomSurface()\n\n");
      ui_quit(ui);
    }
  
  ui->scaledMap = result;
}

void
zoom_in(UI *ui)
{
  
  ui->currScale *= ui->zoomFactor;
  
  ui->mapFrame.h = ui->fullMap->h;
  ui->mapFrame.w = ui->fullMap->w;
  ui->mapFrame.h *= (1/ui->currScale);
  ui->mapFrame.w *= (1/ui->currScale);
  
  
  // scale_map_frame(SDL_Surface *unscaled, SDL_Surface *scaled, )

}

void
zoom_out(UI *ui)
{
  if (ui->currScale == 1){
    
  }else if( ui->currScale == 2){
    zoomFit(ui);
  } else {
    
  ui->currScale /= ui->zoomFactor;
  
  ui->mapFrame.h = ui->fullMap->h;
  ui->mapFrame.w = ui->fullMap->w;
  ui->mapFrame.h /= (ui->currScale);
  ui->mapFrame.w /= (ui->currScale);
  
  }
  // scale_map_frame(SDL_Surface *unscaled, SDL_Surface *scaled, )

}

/*

void
zoomFit(UI *ui)
{

  fprintf(stderr, "INSIDE  ZOOMFIT");
  ui->screenFrame.x = 0;
  ui->screenFrame.y = 0;
  // ui->mapFrame.h = ui->fullMap->h;
  //  ui->mapFrame.w = ;
  double scale_x = (double) (ui->screen->w) / (double) (ui->fullMap->w);
  double scale_y = (double) (ui->screen->h) / (double) (ui->fullMap->h);
  if (scale_y < scale_x) {
    ui->windowFactor = scale_y;
  } else {
    ui->windowFactor = scale_x;
  }
  
  // ui->mapFrame.x=0;
  // ui-mapFrame.y=0;
  //  ui->mapFrame.h = ui->fullMap->h;
  //  ui->mapFrame.w = ui->fullMap->w;

}
*/



void
get_scaled(UI *ui)
{
  //  ui->screenFrame.x = 0;
  // ui->screenFrame.y = 0;
  // ui->mapFrame.h = ui->fullMap->h;
  //  ui->mapFrame.w = ;
  double scale_x = (double) (ui->screen->w) / (double) (ui->mapFrame.w);
  double scale_y = (double) (ui->screen->h) / (double) (ui->mapFrame.h);
  if (scale_y < scale_x) {
    ui->windowFactor = scale_y;
  } else {
    ui->windowFactor = scale_x;
  }
  
  SDL_Surface *result = zoomSurface(ui->fullMap, ui->windowFactor,  ui->windowFactor, SMOOTHING_ON);
  if (result == (SDL_Surface *) (NULL))
    {
      fprintf(stderr, "\n Error from zoomSurface()\n\n");
      ui_quit(ui);
    }
  
  ui->scaledMap = result;
}



static sval
ui_process(UI *ui)
{
  SDL_Event e;
  sval rc = 1;

  while(SDL_WaitEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      return -1;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      rc = ui_keypress(ui, &(e.key));
      break;
    case SDL_ACTIVEEVENT:
      break;
    case SDL_USEREVENT:
      rc = ui_userevent(ui, &(e.user));
      break;
    default:
      fprintf(stderr, "%s: e.type=%d NOT Handled\n", __func__, e.type);
    }
    if (rc==2) { 
      //  zoomFit(ui);
      ui_paint_curr_pan(ui); }
    if (rc<0) break;
  }
  return rc;
}

extern sval
ui_zoom(UI *ui, sval fac)
{
  
  if (fac==1){
    zoom_in(ui);
  } else if (fac == -1){
     zoom_out(ui);
    // zoomFit(ui); 
  }
  
  fprintf(stderr, "%s:\n", __func__);
  return 2;
}

extern sval
ui_pan(UI *ui, sval xdir, sval ydir)
{
  if (xdir==1){
    pan(ui, (ui->panOffset), 0);
  } else if (xdir == -1){
    pan(ui, -(ui->panOffset), 0);
  } else if (ydir==1){
    pan(ui, 0 , (ui->panOffset));
  } else if (ydir == -1){
    pan(ui, 0 , -(ui->panOffset));
  }
  
  fprintf(stderr, "%s:\n", __func__);
  return 2;
}
 
extern sval
ui_move(UI *ui, sval xdir, sval ydir)
{
  fprintf(stderr, "%s:\n", __func__);
  return 1;
}


extern void
ui_update(UI *ui)
{
  SDL_Event event;
  
  event.type      = SDL_USEREVENT;
  event.user.code = UI_SDLEVENT_UPDATE;
  SDL_PushEvent(&event);

}


extern void
ui_quit(UI *ui)
{  
  SDL_Event event;
  fprintf(stderr, "ui_quit: stopping ui...\n");
  event.type      = SDL_USEREVENT;
  event.user.code = UI_SDLEVENT_QUIT;
  SDL_PushEvent(&event);
}

extern void
ui_main_loop(UI *ui, uval h, uval w)
{
  sval rc;
  
  assert(ui);

  ui_init_sdl(ui, h, w, 32);

  ui_load_map_sdl(ui);

  //dumpInfo(ui);
  dummyPlayer_init(ui);

  zoomFit(ui);
  
  //dumpInfo(ui);

  ui_paint_curr_pan(ui);
   
  
  while (1) {
    if (ui_process(ui)<0) break;
  }

  ui_shutdown_sdl();
}


extern void
ui_init(UI **ui)
{
  *ui = (UI *)malloc(sizeof(UI));
  if (ui==NULL) return;

  bzero(*ui, sizeof(UI));
  
  (*ui)->tile_h = SPRITE_H;
  (*ui)->tile_w = SPRITE_W;

  (*ui)->currScale = 1.0;
  (*ui)->zoomFactor = 2.0;
  (*ui)->panOffset = 200.0;

  //=(*ui)->tile_h = SPRITE_H;
}

// Kludgy dummy player for testing purposes
struct DummyPlayerDesc {
  UI_Player *uip;
  int id;
  int x, y;
  int team;
  int state;
} dummyPlayer;

static void 
dummyPlayer_init(UI *ui) 
{
  dummyPlayer.id = 0;
  dummyPlayer.x = 0; dummyPlayer.y = 0; dummyPlayer.team = 0; dummyPlayer.state = 0;
  ui_uip_init(ui, &dummyPlayer.uip, dummyPlayer.id, dummyPlayer.team); 
}

static void 
dummyPlayer_paint(UI *ui)
{
  SDL_Rect t = {dummyPlayer.x * ui->tile_w, dummyPlayer.y * ui->tile_h, 32, 32};
  // t->y = dummyPlayer.y * ui->tile_h; t->x = dummyPlayer.x * ui->tile_w;
  dummyPlayer.uip->clip.x = dummyPlayer.uip->base_clip_x +
    pxSpriteOffSet(dummyPlayer.team, dummyPlayer.state);
  //dumpInfo(ui);
  // fprintf(stderr, "\n\nt.x %d\n\n", dummyPlayer.uip->clip.h);

  SDL_BlitSurface(dummyPlayer.uip->img, &(dummyPlayer.uip->clip), ui->fullMap, &t);
}

int
ui_dummy_left(UI *ui)
{
  dummyPlayer.x--;
  return 2;
}

int
ui_dummy_right(UI *ui)
{
  dummyPlayer.x++;
  return 2;
}

int
ui_dummy_down(UI *ui)
{
  dummyPlayer.y++;
  return 2;
}

int
ui_dummy_up(UI *ui)
{
  dummyPlayer.y--;
  return 2;
}

int
ui_dummy_normal(UI *ui)
{
  dummyPlayer.state = 0;
  return 2;
}

int
ui_dummy_pickup_red(UI *ui)
{
  dummyPlayer.state = 1;
  return 2;
}

int
ui_dummy_pickup_green(UI *ui)
{
  dummyPlayer.state = 2;
  return 2;
}


int
ui_dummy_jail(UI *ui)
{
  dummyPlayer.state = 3;
  return 2;
}

int
ui_dummy_toggle_team(UI *ui)
{
  if (dummyPlayer.uip) free(dummyPlayer.uip);
  dummyPlayer.team = (dummyPlayer.team) ? 0 : 1;
  ui_uip_init(ui, &dummyPlayer.uip, dummyPlayer.id, dummyPlayer.team);
  return 2;
}

int
ui_dummy_inc_id(UI *ui)
{
  if (dummyPlayer.uip) free(dummyPlayer.uip);
  dummyPlayer.id++;
  if (dummyPlayer.id>=100) dummyPlayer.id = 0;
  ui_uip_init(ui, &dummyPlayer.uip, dummyPlayer.id, dummyPlayer.team);
  return 2;
}

void
ui_paint_curr_pan(UI *ui) 
{
  // dumpInfo(ui);
  ui_fill_sdl_surface(ui, ui->fullMap,  mazem);
  // dumpInfo(ui);
  // dummyPlayer_paint(ui);
  // dumpInfo(ui);

  if (ui->currScale==1) {
    
    zoomFit(ui);
    
  } else {

    //    SDL_Surface *temp = createSurface(ui->mapFrame.w, ui->mapFrame.h, ui->screen);
    // ui_buff_screen(ui->fullMap, temp, &ui->mapFrame, &ui->mapFrame);
    
    //    scale_map_frame(ui, temp , ui->scaledMap);
    scale_map_frame(ui);
    // dumpInfo(ui);

  }
  

  ui_buff_screen(ui->scaledMap, ui->screen, &ui->screenFrame, &ui->screenFrame);
  

  SDL_UpdateRect(ui->screen, 0, 0, ui->screen->w, ui->screen->h);

}


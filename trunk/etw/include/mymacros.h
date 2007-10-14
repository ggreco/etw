#define TextShadow(x,y,text,len) drawtext(text,len,x+1,y+1,Pens[P_NERO]);drawtext(text,len,x,y,Pens[P_BIANCO]);

#define ColorTextShadow(x,y,text,len,c) drawtext(text,len,x+1,y+1,Pens[P_NERO]);drawtext(text,len,x,y,c);
/*

#define TextShadow(rp,xp,yp,txt,len)    SetAPen(rp,Pens[P_NERO]); \
            Move(rp,xp+1,yp+1); Text(rp,txt,len); \
            SetAPen(rp,Pens[P_BIANCO]); \
            Move(rp,xp,yp);Text(rp,txt,len);

#define ColorTextShadow(rp,xp,yp,txt,len,color)    SetAPen(rp,Pens[P_NERO]); \
            Move(rp,xp+1,yp+1); Text(rp,txt,len); \
            SetAPen(rp,Pens[color]); \
            Move(rp,xp,yp);Text(rp,txt,len);
*/

#define DeleteAudio2Fast()     D(bug("DeleteAudio2Fast: DA FARE!"))

// Execute("c:delete >NIL: t:cwd t:speak QUIET ALL"/*-*/,NULL,NULL)

#define os_delay(x) SDL_Delay(x*20)

#define ScaledY(x) (((x)*WINDOW_HEIGHT)/oldheight)
#define ScaledX(x) (((x)*WINDOW_WIDTH)/oldwidth)
#define FixedScaledY(x) (((x)*WINDOW_HEIGHT)/256)
#define FixedScaledX(x) (((x)*WINDOW_WIDTH)/320)

#define RangeRand(v) (rand()%v)


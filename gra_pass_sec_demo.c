/******************************************************************/
/***  Security Demo for a Graphical Password                    ***/
/***   -- Security on Challenge-Response Type                   ***/
/***                     Direct Human Identification Scheme --- ***/
/***------------------------------------------------------------***/
/*** WE PROVIDE ABSOLUTELY NO WARRANTY.                         ***/
/*** ----                                                       ***/
/*** Copyright (C) 1994-2017 Kazukuni Kobara                    ***/
/******************************************************************/

/*
See also related papers:

[1] K. Kobara and H. Imai "Security against peeping attacks of challenge-response type direct human identification schemes using uniform mapping" Trans. of Electronics and Communications in Japan (Part III: Fundamental Electronic Science), Vol.82, No.1, pp. 79–86, DOI: 10.1002/(SICI)1520-6440(199901)82:1<79::AID-ECJC9>3.0.CO;2-R, 1999.01

[2] K. Kobara and H. Imai "Limiting the Visible Space Visual Secret Sharing Schemes and Their Application to Human Identification" Proc. of ASIACRYPT 1996, LNCS 1163, pp. 185-195, 1996
*/


#include <stdio.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* added later */
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <X11/Xos.h>


#define BELL 1 /* 1: 音を鳴らす */
#define EXT 0 /* 1: exit を付ける */
#define BEL 7
#define SEC 3000000 /* 押すと黒の時間 */

/* the root window of tool */
#define WSX 900
#define WSY 890
#define CHAS 30 /* 文字位置 */
#define SEP 10

/* Head */
#define WHY 60
#define WSWX 100
#define WSWY 40

/* work */
#define WWX WSX-SEP*2-8
#define WWY WSY-WHY-SEP*3-4*4

/* wterm */
#define WTX 370
#define WTY 550

/* whelp */
#define WHLY 130

/* wattack */
#define WAX 420
#define WAY WTY+WHLY+20

/* the menu window */
#define WMSX (int)WSX*3/4  



  /********** 井島用 ***********/
#define S_SIZE 8   /*** 暗証番号の size ***/
#define ALP_S 36   /*** The whole alphabet size ***/
#define QI_SIZE 4  /*** 応答文字の範囲 １ー ***/

int seed;

/******************************/
/***	    Human identification		***/
/******************************/
unsigned long MyColor (display,color)
Display *display;
char    *color;
{
        Colormap cmap;
        XColor   c0,c1;

        cmap = DefaultColormap (display,0);

        XAllocNamedColor (display, cmap, color, &c1, &c0);

        return (c1.pixel);

}
main()
{	
	Display 	         *dis;
	Window			 r, w, head, sw2, term;
	GC			 gc;
	XEvent					e;
	Font					f1, f2;
	XSetWindowAttributes	a;
	int						i, x, y, mm;
	int						x0, y0, x1, y1;

	/* For a practical use, replace insecure random functions
	   with cryptographically secure ones. */
	void ssrand();  
	char alp[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int menu();

	
	static int spot[] = {3, 5, 7, 10, 13, 15, 17, 20, 23, 25, 27, 
														30, 33, 40, 50};
	unsigned long color[10];
	
	dis = XOpenDisplay(NULL);
	r = RootWindow(dis, 0);
	gc = XCreateGC(dis, r, 0, 0);

	/* Choose installed fonts shown by 
	   $ xlsfonts 
	*/
	f1 = XLoadFont(dis, "*utopia-bold-r-normal*--15-*");
	f2 = XLoadFont(dis, "*utopia-bold-r-normal*--19-*");

	/*
	f1 = XLoadFont(dis, "*");
	f2 = XLoadFont(dis, "*");

	f1 = XLoadFont(dis, "*helvetica-bold-r-normal*--24-*");
	f2 = XLoadFont(dis, "*helvetica-bold-r-normal*--24-*");
	f1 = XLoadFont(dis, "*helvetica*--18-*");
	f2 = XLoadFont(dis, "*helvetica*--18-*");
	f1 = XLoadFont(dis, "*courier*--18-*");
	f2 = XLoadFont(dis, "*courier*--18-*");
	f1 = XLoadFont(dis, "*serif*");
	f2 = XLoadFont(dis, "*sans*");
	f1 = XLoadFont(dis, "fixed");
	f2 = XLoadFont(dis, "rk24");
	*/

	XSetFont(dis, gc, f2);

	color[0] = MyColor(dis,"white");
	color[1] = MyColor(dis,"black");
	color[3] = MyColor(dis,"green");
	color[4] = MyColor(dis,"yellow");
	color[2] = MyColor(dis,"skyblue");
	color[5] = MyColor(dis,"red");
	color[6] = MyColor(dis,"brown");
	color[7] = MyColor(dis,"blue");
	color[8] = MyColor(dis,"grey");

	
	w = XCreateSimpleWindow(dis, r, 100, 100, WSX, WSY, 2, 1,color[8] );
	a.backing_store = Always;
	XChangeWindowAttributes(dis, w, CWBackingStore, &a);
	head = XCreateSimpleWindow(dis, w, SEP, SEP, 870, WHY, 5, 1,color[1] );
#if EXT==1 
	sw2 = XCreateSimpleWindow(dis, head, 750, 5, WSWX, WSWY, 4, 1, color[8]);
#endif
	XSelectInput(dis, w, ExposureMask | ButtonPressMask | ButtonMotionMask);
	XSelectInput(dis, head, ExposureMask);
/*	XSelectInput(dis, term, ExposureMask);*/
#if EXT==1
	XSelectInput(dis, sw2, ExposureMask | ButtonPressMask);
#endif

	XMapWindow(dis, w);
	XMapSubwindows(dis, head);
	XMapSubwindows(dis, w);
	XStoreName(dis, w, "Human Identification Tool");
	
	XSetLineAttributes(dis, gc, spot[0], LineSolid, CapRound, JoinRound);

	/* For a practical use, replace insecure random functions
	   with cryptographically secure ones. */
	srandom((int)time(NULL));

	while(1)
	{	



/*     メインメニュー                           */

		  mm= menu(dis,gc,f1,f2,w,head,sw2,color);

		switch(mm)
		  {case 1:
		     /***   パスワードタイプ ***/
		     menu1(dis,gc,f1,f2,w,head,sw2,color,alp);
		     break;
		   case 2:
		     /***   質問応答個人認証 ***/
		     menu2(dis,gc,f1,f2,w,head,sw2,color,alp);
		     break;
		   }


	}
}

/***********************************/
/*       ヘッドの表示              */
/***********************************/
drawhead(dis,gc,f1,f2,w,head,sw2,color)
     Display 	         *dis;
     GC                 gc;
     Font               f1, f2;     
     Window		head,sw2,w;
     unsigned long color[];
{
  XSetForeground(dis, gc, color[5]);
  XDrawString(dis, head, gc, 280, 40,"HUMAN IDENTIFICATION TOOL", 25);

#if EXT==1
  XSetForeground(dis, gc, color[4]);
  XDrawString(dis, sw2, gc, 25, CHAS, "Exit", 4);
  XSetForeground(dis, gc, color[1]);
#endif
}


/***********************************/
/*       help の表示              */
/***********************************/
drawhelp(dis,gc,f1,f2,w,whelp,sw2,color,comma,comma2,yourpass)
     Display 	         *dis;
     GC                 gc;
     Font               f1, f2;     
     Window		whelp,sw2,w;
     unsigned long color[];
     char yourpass[],comma[],comma2[];
{
		 XDrawString(dis, whelp, gc, 20, CHAS,"Suppose", 7);
		 XDrawString(dis, whelp, gc, 20, CHAS*2,"this user's password is", 23);
		 XDrawString(dis, whelp, gc, 20, CHAS*3, comma, 2);
		 XDrawString(dis, whelp, gc, 20+12*2, CHAS*3,yourpass, S_SIZE);
		 /* Depend on the font width */
		 /*
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+1), CHAS*3,comma2, 2);
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+3), CHAS*3,".", 1);
		 */
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+2), CHAS*3,comma2, 2);
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+4), CHAS*3,".", 1);

		 XDrawString(dis, whelp, gc, 20, CHAS*4,"And now, you are this user.", 27);
}

/***********************************/
/*       help の表示 2 (you are the attacker)             */
/***********************************/
drawhelp2(dis,gc,f1,f2,w,whelp,sw2,color,comma,comma2,yourpass)
     Display 	         *dis;
     GC                 gc;
     Font               f1, f2;     
     Window		whelp,sw2,w;
     unsigned long color[];
     char yourpass[],comma[],comma2[];
{
		 XDrawString(dis, whelp, gc, 20, CHAS,"Suppose", 7);
		 XDrawString(dis, whelp, gc, 20, CHAS*2,"this user's password is", 23);
		 XDrawString(dis, whelp, gc, 20, CHAS*3, comma, 2);
		 XDrawString(dis, whelp, gc, 20+12*2, CHAS*3,yourpass, S_SIZE);
		 /* Depend on the font width */
		 /*
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+1), CHAS*3,comma2, 2);
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+3), CHAS*3,".", 1);
		 */
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+2), CHAS*3,comma2, 2);
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+4), CHAS*3,".", 1);




		 XDrawString(dis, whelp, gc, 10, CHAS*4,"And now, you are the attacker.", 30);
}
/************************************************/
/*     メインメニュー                           */
/************************************************/
int menu(dis,gc,f1,f2,w,head,sw2,color)
     Display 	         *dis;
     GC                 gc;
     Font               f1, f2;     
     Window		head,sw2,w;
     unsigned long color[];
{
  Window	  wmenur, wmenu1, wmenu2;
  XEvent                                  e;

  wmenur = XCreateSimpleWindow(dis, w,(int)WSX/8, 200, WMSX, 400, 4, 1, color[6]);
  
  wmenu1 = XCreateSimpleWindow(dis, wmenur, 10, 150, WMSX-30 , WSWY, 1, 1, color[2]);
  wmenu2 = XCreateSimpleWindow(dis, wmenur, 10, 250, WMSX-30, WSWY, 1, 1, color[2]);

  XSelectInput(dis, wmenu1, ExposureMask | ButtonPressMask);
  XSelectInput(dis, wmenu2, ExposureMask | ButtonPressMask);

  XMapSubwindows(dis, w);
  XMapSubwindows(dis, wmenur);
  while(1)
    {
      XNextEvent(dis,&e);
      switch (e.type)
	{       case Expose:

		  drawhead(dis,gc,f1,f2,w,head,sw2,color);

		   XSetForeground(dis,gc,color[3]);
		  XDrawString(dis, wmenur, gc, (int)WMSX/2-CHAS*2, CHAS ,"MENU", 4);

XDrawString(dis, wmenur, gc, (int)WMSX/2-CHAS*4, CHAS*3 ,"Please select.", 14);
		  XSetForeground(dis,gc,color[1]);
		  XDrawString(dis, wmenu1, gc, 20, CHAS,"1 : Password Type Human Identification", 38);
		  XDrawString(dis, wmenu2, gc, 20, CHAS,"2 : Challenge-Response Type Human Identification", 48);
		  break;
		  
		case ButtonPress:
/*
		  switch((int)e.xany.window)
		    {case (int)sw2:
		       exit(1);
		     case (int)wmenu1:
		       XDestroyWindow(dis,wmenur);
		       return(1);
		       break;
		     case (int)wmenu2:
		       XDestroyWindow(dis,wmenur);
		       return(2);
		       break;
		     }    
*/

#if EXT==1
		  if ( e.xany.window == sw2 ){
		    exit (1);
		  }
#endif
		  if( e.xany.window == wmenu1 ){
		    XDestroyWindow(dis,wmenur);

		    return(1);
		  }
		  if( e.xany.window == wmenu2 ){
		    XDestroyWindow(dis,wmenur);
		    
		    return(2);
		  }

		  break;
		}
    }
}

/******************/
/* パスワード方式 */
/******************/
menu1(dis,gc,f1,f2,w,head,sw2,color,alp)
     Display 	         *dis;
     GC                 gc;
     Font               f1, f2;     
     Window		head,sw2,w;
     unsigned long color[];
     char alp[];
{
  Window	  wwork,wterm,whelp,wattack,press;
  Window          ws[6][6];
  XEvent                 e;

  int hantei();
  int i,j,k,lamcou=0,han,hrej,hacc;
  long int sec;
  char alpc[2],pass[S_SIZE+1],yourpass[S_SIZE+1];
  char comma[] = "``", comma2[] = "''";

  alpc[1]=0;  pass[S_SIZE]=0; yourpass[S_SIZE]=0; 

  wwork = XCreateSimpleWindow(dis, w, SEP,WHY+2*SEP+8, WWX, WWY, 4, 1, color[6]);


  /*  wterm = XCreateSimpleWindow(dis, wwork, 20, CHAS+2*SEP+WHLY+20, WTX, WTY, 4, 1, 0); */
  /* For cygwin */
  wterm = XCreateSimpleWindow(dis, wwork, 20, CHAS+2*SEP+WHLY+20, WTX, WTY, 4, 1, color[0]);
  whelp = XCreateSimpleWindow(dis, wwork, 20, CHAS+2*SEP , WTX, WHLY, 4, 1, color[0]);
  wattack = XCreateSimpleWindow(dis, wwork, 420, CHAS+2*SEP , WAX, WAY, 4, 1, color[0]);

  XSelectInput(dis, wterm, ExposureMask );
  XSelectInput(dis, wwork, ExposureMask );

  genepass(yourpass,alp);


  XMapSubwindows(dis, w);
  XMapSubwindows(dis, wwork);


 situmononaji:

  for(i=0; i<6; i++){
    for(j=0; j<6; j++){
      ws[i][j]=XCreateSimpleWindow(dis, wterm, 50+40*i, CHAS*3+40*j, 40, 40, 1, 1, color[2]);
        XSelectInput(dis, ws[i][j], ExposureMask | ButtonPressMask);
    }
  }


  XMapSubwindows(dis, wterm);

  while(1)
    {

      XNextEvent(dis,&e);
      switch (e.type)
	{      case Expose:
		 drawhelp(dis,gc,f1,f2,w,whelp,sw2,color,comma,comma2,yourpass);

/*
		 XDrawString(dis, whelp, gc, 20, CHAS,"Suppose", 7);
		 XDrawString(dis, whelp, gc, 20, CHAS*2,"this user's password is", 23);
		 XDrawString(dis, whelp, gc, 20, CHAS*3, comma, 2);
		 XDrawString(dis, whelp, gc, 20+12*2, CHAS*4,yourpass, S_SIZE);
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+2), CHAS*3,comma2, 2);
		 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+4), CHAS*3,".", 1);

		 XDrawString(dis, whelp, gc, 20, CHAS*4,"And now, you are this user.", 27);
*/

		 XDrawString(dis, wattack, gc, 20, CHAS,"I'm an Attacker.", 16);
		 XDrawString(dis, wattack, gc, 20, CHAS*2,"I'll guess this user's password.", 32);
		 drawhead(dis,gc,f1,f2,w,head,sw2,color);

		 XSetForeground(dis,gc,color[3]);
		 XDrawString(dis, wwork, gc, 200, CHAS,"Password Type Human Identification", 34);
		 XSetForeground(dis,gc,color[1]);
XDrawString(dis, wterm, gc, SEP*2, CHAS,"Please Input Your Password.", 27);

		  for(i=0; i<6; i++){
		    for(j=0; j<6; j++){
		      alpc[0]=alp[i+j*6];
		  XDrawString(dis, ws[i][j], gc, 15, CHAS,alpc, 1);
		     
		    }
		  }

		  break;
	
		case ButtonPress:
#if EXT==1		  
		  if ( e.xany.window == sw2 ){
		    exit (1);
		  }
		  else{
#endif
		    for(i=0; i<6; i++){
		      for(j=0; j<6; j++){
			if(lamcou<S_SIZE){
			  /*-------------  入力  ----------------*/
			  if( e.xany.window == ws[i][j] ){
/*---- 押すと黒く --------*/
			    press=XCreateSimpleWindow(dis, wterm, 50+40*i, CHAS*3+40*j, 40, 40, 1, 1, color[1]);			    
			    XMapWindow(dis, press);
			    alpc[0]=alp[i+j*6];
			    XSetForeground(dis,gc,color[0]);
			    XDrawString(dis, press, gc, 15, CHAS,alpc, 1);
			    XFlush(dis);
			    XSetForeground(dis,gc,color[1]);
			    for(sec=0; sec<SEC; sec++){}
			    XDestroyWindow(dis,press);
/*=========================*/	

/* printf("%c\n",alp[i+j*6]);*/
			    pass[lamcou]=alp[i+j*6]; 
			    alpc[0]=alp[i+j*6]; 
			    XFillArc(dis, wterm,gc,50+32*lamcou,CHAS*4+40*6,30,30,0,360*64);
#if BELL==1			    
			    printf("%c\n",BEL);
#endif

			    XDrawString(dis, wattack, gc, 20+15*lamcou, CHAS*3,alpc, 1);  




			    lamcou++;
			   XFlush(dis);
			    if(lamcou>=S_SIZE){
			      XDestroySubwindows(dis,wterm);
			      XClearWindow(dis,wterm);
			      han=hantei(pass,yourpass);
			      if(han==1){
XDrawString(dis, wattack, gc, 20, CHAS*4,"was accepted.", 13);

XDrawString(dis, wattack, gc, 20, CHAS*6,"So this user's password must be", 31);


XDrawString(dis, wattack, gc, 20, CHAS*7, comma, 2);
XDrawString(dis, wattack, gc, 20+12*2, CHAS*7,yourpass, S_SIZE);
                 XDrawString(dis, wattack, gc, 20+12*(S_SIZE+2), CHAS*7,comma2, 2);
                 XDrawString(dis, wattack, gc, 20+12*(S_SIZE+4), CHAS*7,".", 1);

XDrawString(dis, wattack, gc, 20, CHAS*9,"I can learn this user's", 23);
/*XDrawString(dis, wattack, gc, 20, CHAS*10,"password by only a one-time", 27);
XDrawString(dis, wattack, gc, 20, CHAS*11,"illegal observation.", 24);*/
XDrawString(dis, wattack, gc, 20, CHAS*10,"password by only", 16);
XDrawString(dis, wattack, gc, 20, CHAS*11,"a single glance.", 16);


				hacc=accept(dis,wterm,whelp,gc,f1,f2,w,head,sw2,color);
/* Go to MENU */
if(hacc==1){
  XDestroySubwindows(dis,wwork);
  XDestroyWindow(dis,wwork);
				return;
            }else{
                          if(hacc==2){
                              /**** もう一度入力 ****/
                              lamcou=0;
                              XClearWindow(dis,wterm);
                              XClearWindow(dis,wattack);
                              XClearWindow(dis,whelp);
                              goto situmononaji;
                          }       
            }
			      }else{
XDrawString(dis, wattack, gc, 20, CHAS*4,"was not accepted.", 17); 
				hrej=reject(dis,wterm,whelp,gc,f1,f2,w,head,sw2,color);
				/* Go to menu */

				if(hrej==0){
				  XDestroySubwindows(dis,wwork);
				  XDestroyWindow(dis,wwork);
				  return;
				}else{if(hrej==3){
				  /**** もう一度入力 ****/
				  lamcou=0;
				  XClearWindow(dis,wterm);
				  XClearWindow(dis,wattack);
                  goto situmononaji;
				  /*========================*/
				}
				}
#if EXT==1
			      }
#endif
			    }
			  }
			}
		      }
		    }
/*
		    if( e.xany.window == wmenu2 ){
		      XDestroyWindow(dis,wterm);

		      return(2);
		    }
*/
		  }		  
		  break;

		}

    }

/*  printf("menu1\n");*/
}

/****************************************/
/*             質問応答型               */
/****************************************/
menu2(dis,gc,f1,f2,w,head,sw2,color,alp)
     Display 	         *dis;
     GC                 gc;
     Font               f1, f2;     
     Window		head,sw2,w;
     unsigned long color[];
     char alp[];
{
  Window	  wterm,whelp,wattack,wwarn,wwaok,press,wwork;
  Window          ws[6][6],wa[4];
  
  Window wk[S_SIZE][9];
  Window wyorn,wyes,wno;
  XEvent                 e;

  int hantei();
  int i,j,k,lamcou=0,han,hrej,hacc,look=1,tmpc;
  long int sec;
  /* char alpc[2],pass[S_SIZE+1],yourhen_c[S_SIZE+1],yourpass[S_SIZE+1],comma[3],comma2[3]; */
  char alpc[2],pass[S_SIZE+1],yourhen_c[S_SIZE+1],yourpass[S_SIZE+1];
  char comma[] = "``", comma2[] = "''";

  int yourhen[S_SIZE],hentou[ALP_S], kouho[S_SIZE][ALP_S/4],kouho0[S_SIZE][ALP_S/4], kouhosuu[S_SIZE];

  for(i=0; i<S_SIZE; i++){
    kouhosuu[i]=9;
  }

  alpc[1]=0;  pass[S_SIZE]=0; yourpass[S_SIZE]=0; yourhen_c[S_SIZE]=0;

  wwork = XCreateSimpleWindow(dis, w, SEP,WHY+2*SEP+8, WWX, WWY, 4, 1, color[6])
;

  /* For Cygwin */
  wterm = XCreateSimpleWindow(dis, wwork, 20, CHAS+2*SEP+WHLY+20, WTX, WTY, 4, 1, color[0])
;
  whelp = XCreateSimpleWindow(dis, wwork, 20, CHAS+2*SEP , WTX, WHLY, 4, 1, color[0]);
  wattack = XCreateSimpleWindow(dis, wwork, 420, CHAS+2*SEP , WAX, WAY, 4, 1
, color[0]);

  XSelectInput(dis, wterm, ExposureMask );
/******* 各秘密情報に返答を割り当てる。 ****/
  genepass(yourpass,alp);

 situmontigau:

  situmon(alp,hentou);
  genehen(yourpass,yourhen,hentou,alp);
  
  for(i=0; i<S_SIZE; i++){
    yourhen_c[i]=(int)(yourhen[i]+48); /* koko */
  }

 situmononeji:

  for(i=0; i<6; i++){
    for(j=0; j<6; j++){
 /*     ws[i][j]=XCreateSimpleWindow(dis, wterm, 10+30*i, 50+30*j, 30, 30, 1, 1, color[2+hentou[i+j*6]]);
*/
      ws[i][j]=XCreateSimpleWindow(dis, wterm, 50+40*i, CHAS*3+40*j, 40, 40, 1,1, color[2+hentou[i+j*6]]);
        XSelectInput(dis, ws[i][j], ExposureMask | ButtonPressMask);
    }
  }

  for(i=0; i<4; i++){
      wa[i]=XCreateSimpleWindow(dis, wterm, 50+40*(i+1) , CHAS*3+40*6+20, 40, 40, 1, 1,color[2+i] );
      XSelectInput(dis, wa[i], ExposureMask | ButtonPressMask);
  }

  XMapSubwindows(dis, w);
  XMapSubwindows(dis, wwork);
  XMapSubwindows(dis, wterm);
 

  while(1)
  {
  hajime:

      XNextEvent(dis,&e);
      switch (e.type)
      { 
      case Expose:
	drawhelp(dis,gc,f1,f2,w,whelp,sw2,color,comma,comma2,yourpass);
/*
                XDrawString(dis, whelp, gc, 20, CHAS,"This user's password is", 23);
                 XDrawString(dis, whelp, gc, 20, CHAS*2, comma, 2);
                 XDrawString(dis, whelp, gc, 20+12*2, CHAS*2,yourpass, S_SIZE);
                 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+2), CHAS*2,comma2, 2)
;
                 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+4), CHAS*2,".", 1);
		 XDrawString(dis, whelp, gc, 20, CHAS*3,"And now, you are this user.", 27);
*/

                 XDrawString(dis, wattack, gc, 20, CHAS,"I'm an Attacker.", 16);
                 XDrawString(dis, wattack, gc, 20, CHAS*2,"I'll guess this user's password.", 32);


		  drawhead(dis,gc,f1,f2,w,head,sw2,color);

                 XSetForeground(dis,gc,color[3]);
		 XDrawString(dis, wwork, gc, 180, CHAS,"Challenge-Response Type Human Identification", 44);

                 XSetForeground(dis,gc,color[1]);
XDrawString(dis, wterm, gc, SEP*2, CHAS,"Please Input Proper Colors.", 27);


		  for(i=0; i<6; i++){
		    for(j=0; j<6; j++){
		      alpc[0]=alp[i+j*6];
		  XDrawString(dis, ws[i][j], gc, 15, CHAS,alpc, 1);
		     
		    }
		  }

		  break;
	
		case ButtonPress:
#if EXT==1		  
		  if ( e.xany.window == sw2 ){
		    exit (1);
		  }
#endif
/* 秘密情報のパネルを押した場合 */
         for(i=0; i<6; i++){
             for(j=0; j<6; j++){
                 if ( e.xany.window == ws[i][j] ){
                     wwarn=XCreateSimpleWindow(dis, wterm, 70, 150, 200, 150, 2, 1, color[0]);
                     wwaok=XCreateSimpleWindow(dis, wwarn, 60, 100, 60, 40, 2, 1, color[8]);
       
                     XSelectInput(dis, wwarn, ExposureMask );
                     XSelectInput(dis, wwaok, ExposureMask | ButtonPressMask);

                     XMapWindow(dis, wwarn);
                     XMapSubwindows(dis, wwarn);


                     while(1){
                         XNextEvent(dis,&e);
                         switch (e.type)
                         { 
                         case Expose:
/*                            XDrawString(dis, wwarn, gc, 20, CHAS,"Don't press these panels!", 25);*/


XDrawString(dis, wwarn, gc, 20, CHAS,"Don't press", 11);
XDrawString(dis, wwarn, gc, 40, CHAS*2,"these panels!", 14);

XSetForeground(dis, gc, color[4]);
                             XDrawString(dis, wwaok, gc, 20, CHAS,"OK", 2);
XSetForeground(dis, gc, color[1]);

                             break;
                         case ButtonPress:
#if EXT==1
                             if ( e.xany.window == sw2 ){
                                 exit (1);
                             }
#endif
                             if ( e.xany.window == wwaok ){
                                 XDestroySubwindows(dis,wwarn);
                                 XDestroyWindow(dis,wwarn);
                                 goto hajime;
                             }
                             break;
                         }
                     }
                 }
             }

         }

/* 返答のパネルを押した場合 */
          for(i=0; i<4; i++){
              if(lamcou<S_SIZE){
                  /*-------------  入力  ----------------*/
                  if( e.xany.window == wa[i] ){

/*---- 押すと黒く --------*/
                            press=XCreateSimpleWindow(dis, wterm, 50+40*(i+1), CHAS*3+40*6+20, 40, 40, 1, 1, color[1]);                            
                            XMapWindow(dis, press);
			    XFlush(dis);
                            for(sec=0; sec<SEC; sec++){}
                            XDestroyWindow(dis,press);
/*=========================*/   


/*                      printf("%d\n",i);*/
                      pass[lamcou]=(int)(i+48); /*koko*/
                      alpc[0]=i; 
                      XFillArc(dis, wterm,gc,50+32*lamcou,CHAS*4+40*7+20,30,30,0,360*64);

#if BELL==1                         
                            printf("%c\n",BEL);
#endif

                      XSetForeground(dis,gc,color[1]);
                XFillRectangle(dis, wattack,gc,19+22*lamcou,CHAS*3-18,20,20);
                XSetForeground(dis,gc,color[i+2]);
                XFillRectangle(dis, wattack,gc,20+22*lamcou,CHAS*3+1-18,18,18);
                XSetForeground(dis,gc,color[1]);              

/*
			    XDrawString(dis, wattack, gc, 20, 80+10*lamcou,alpc, 1);  
*/


			    lamcou++;
			   XFlush(dis);
			    if(lamcou>=S_SIZE){
/*			      XDestroySubwindows(dis,wterm);
			      XClearWindow(dis,wterm);
*/
			      /* 判定 */
/*			      for(j=0; j<S_SIZE+1; j++){
				printf("%d  %d\n",pass[j],yourhen_c[j]);
				}
printf("%d %d %d\n",strcmp(pass,yourhen_c),strlen(pass),strlen(yourhen_c));
*/
			      han=hantei(pass,yourhen_c);

/*			      printf("%d\n%",han);*/
			      if(han==1){
/*---------------------------- accept -------------------------------*/
                      XDrawString(dis, wattack, gc, 20, CHAS*4,"was accepted.", 13); 


XDrawString(dis, wattack, gc, 20, CHAS*5,"So this user's password must be", 31); 

     /*-----------------         推定      ---------------------*/
		      /*----- S1 を求める ---*/		     
		      suitei0(S_SIZE,ALP_S,yourhen,hentou,kouho0);
		      /*---- １回目の覗き見 ---*/
		      if(look==1){
			for(j=0; j<S_SIZE; j++){
			  for(k=0; k<9; k++){
			    kouho[j][k]=kouho0[j][k];
			  }
			}
		      }else{
/*			prn_kouho(S_SIZE,ALP_S,kouho,alp,kouhosuu);*/
			/*----- Sn (n>1) を求める ---*/
			for(j=0; j<S_SIZE; j++){
			  tmpc=kouhosuu[j];
			  suitei_siboru(kouho0[j],kouho[j],&tmpc);
			  kouhosuu[j]=tmpc;
			}
		      }
			/*------- window に表示 --------*/
/* prn_kouho(S_SIZE,ALP_S,kouho,alp);*/
XDrawString(dis, wattack, gc, 10, CHAS*7,"digit           candidates", 26); 
for(k=0; k<8; k++){
  alpc[0]=alp[k+1];
    XDrawString(dis, wattack, gc, 20, CHAS*(8+k),alpc, 1); 
}
  alpc[0]='.';
    XDrawString(dis, wattack, gc, 20, CHAS*16,alpc, 1); 

 for(k=0; k<8; k++){
    for(j=0; j<kouhosuu[k]; j++){
      wk[k][j]=XCreateSimpleWindow(dis, wattack, 50+40*j, CHAS*8+30*k-22, 20, 20, 1,1, color[yourhen[k]+2]);
        XSelectInput(dis, wk[k][j], ExposureMask );
      

    }
  }
		      /* 覗き見するかしないか？ */
		      wyorn=XCreateSimpleWindow(dis, wattack, 50, CHAS*17,WAX-100 , CHAS*5, 2,1, color[0]);
		      wyes=XCreateSimpleWindow(dis, wyorn, 30, CHAS*2+SEP*2, WSWX , WSWY, 1,1, color[8]);
		      wno=XCreateSimpleWindow(dis, wyorn, WAX-100-WSWX-30, CHAS*2+SEP*2, WSWX , WSWY, 1,1, color[8]);

		      XSelectInput(dis, wyorn, ExposureMask);
  XSelectInput(dis, wyes, ExposureMask | ButtonPressMask);
  XSelectInput(dis, wno, ExposureMask | ButtonPressMask);

 
		      XMapSubwindows(dis, wattack);
		      XMapSubwindows(dis, wyorn);


 while(1)
    {
      XNextEvent(dis,&e);
      switch (e.type)
        {      case Expose:

/*		 XDrawString(dis, wyorn, gc, 20, CHAS,"Do you do one more", 18);
		 XDrawString(dis, wyorn, gc, 40, CHAS*2,"illegal observation?", 20); */

		 XDrawString(dis, wyorn, gc, 10, CHAS,"Peep at another challenge", 25);
		 XDrawString(dis, wyorn, gc, 40, CHAS*2,"and its responce.", 17);

		 XSetForeground(dis, gc, color[4]);
		 XDrawString(dis, wyes, gc, 35, CHAS,"OK", 2);
		 XDrawString(dis, wno, gc, 25, CHAS,"QUIT", 4);
		 XSetForeground(dis, gc, color[1]);

		 /* 候補の表示 */
		  for(k=0; k<8; k++){
		    for(j=0; j<kouhosuu[k]; j++){
/*		      printf("%d\n",kouho[k][j]);*/
		      alpc[0]=alp[kouho[k][j]];
		  XDrawString(dis, wk[k][j], gc, 5, (int)CHAS*2/3,alpc, 1);
		     
		    }
		  }
		 /*===========*/

		 break;

              case ButtonPress:
#if EXT==1                  
                 if ( e.xany.window == sw2 ){
                   exit (1);
                 }
#endif
/* Go to MENU */
         if ( e.xany.window == wno ){
                     XDestroySubwindows(dis,wterm);
		     XDestroySubwindows(dis,wattack);
                     goto gtmenu;
         }
/* illegal observation */
         if ( e.xany.window == wyes ){
                     nozokimi(S_SIZE,ALP_S,dis,gc,f1,f2,w,head,sw2,wterm,whelp,wattack,wwork,color,alp,yourpass,look,kouho,kouhosuu);
		     goto gtmenu;
         }
		 break;
	       }
    }


       /*==========================================================*/


/*------------------ いらない -----------*/
/*		      XDestroySubwindows(dis,wterm);
		      XClearWindow(dis,wterm);
                      hacc=accept_cr(dis,wterm,whelp,gc,f1,f2,w,head,sw2,color);

                      * Go to menu *
                      if(hacc==1){
*/
/*======================== いらない ===========================*/
		      gtmenu:
			XDestroySubwindows(dis,wwork);
			XDestroyWindow(dis,wwork);

                          return;

/*------------------ いらない2 -----------*/
/*  
                    }else{
                          if(hacc==2){
                              **** もう一度入力 ****
                              lamcou=0;
                              XClearWindow(dis,wterm);
                              XClearWindow(dis,wattack);
                              XClearWindow(dis,whelp);
                              goto situmontigau;
                          }       
                      }
*/
/*======================== いらない2 ===========================*/
/*======================== accept =============================*/
                  }else{

/*------------------------ reject ------------------------------*/
		      XDestroySubwindows(dis,wterm);
		      XClearWindow(dis,wterm);


                      XDrawString(dis, wattack, gc, 20, CHAS*4,"was not accepted.", 17); 
                      hrej=reject(dis,wterm,whelp,gc,f1,f2,w,head,sw2,color);
                      /* Go to menu */

                      if(hrej==0){
			XDestroySubwindows(dis,wwork);
			XDestroyWindow(dis,wwork);
/*                          XDestroyWindow(dis,wterm);
                          XDestroyWindow(dis,whelp);
                          XDestroyWindow(dis,wattack);
  */
                        return;
                      }else{
                          if(hrej==3){
                              /**** もう一度入力 ****/
                              lamcou=0;
                              XClearWindow(dis,wterm);
                              XClearWindow(dis,wattack);
                              goto situmononeji;
                          }
                      }

/*====================== reject =================================*/
                  }
              }
                  }
                  
			  }
          }
      break;
      }
  }         
/*  printf("menu1\n");*/
}


/***************************/
/*      覗き見             */
/***************************/
nozokimi(s_size,alp_s,dis,gc,f1,f2,w,head,sw2,wterm,whelp,wattack,wwork,color,alp,yourpass,look,kouho,kouhosuu)
     int s_size,alp_s;
     Display 	         *dis;
     GC                 gc;
     Font               f1, f2;     
     Window		head,sw2,w,wterm,whelp,wattack,wwork;
     unsigned long color[];
     char alp[];
     char yourpass[];
     int look;
     int kouho[s_size][alp_s/4];
     int kouhosuu[];
{
  Window	 press;
  Window          ws[6][6],wa[4];
  
  Window wk[S_SIZE][9];
  Window wyorn,wyes,wno,wok;
  XEvent                 e;

  int hantei();
  int i,j,k,han,hrej,hacc,tmpc;
  long int sec;
  char alpc[2],yourhen_c[S_SIZE+1];
  char comma[] = "``", comma2[] = "''";

  int yourhen[S_SIZE],hentou[ALP_S], kouho0[S_SIZE][ALP_S/4];

  alpc[1]=0;  yourhen_c[S_SIZE+1]=0;


/******* 各秘密情報に返答を割り当てる。 ****/
 situmontigau:
  look++;

  /* window の初期化 */
                     XDestroySubwindows(dis,wterm);
  XDestroySubwindows(dis,wattack);
		     XClearWindow(dis,wattack);
		     XClearWindow(dis,wterm);
  XClearWindow(dis,whelp);

  situmon(alp,hentou);
  genehen(yourpass,yourhen,hentou,alp);
  
  for(i=0; i<S_SIZE; i++){
    yourhen_c[i]=(char)yourhen[i];
  }


/*------ 質問応答パネル表示 ----*/
  for(i=0; i<6; i++){
    for(j=0; j<6; j++){
      ws[i][j]=XCreateSimpleWindow(dis, wterm, 50+40*i, CHAS*3+40*j, 40, 40, 1,1, color[2+hentou[i+j*6]]);
        XSelectInput(dis, ws[i][j], ExposureMask | ButtonPressMask);
    }
  }

  for(i=0; i<4; i++){
      wa[i]=XCreateSimpleWindow(dis, wterm, 50+40*(i+1) , CHAS*3+40*6+20, 40, 40, 1, 1,color[2+i] );
      XSelectInput(dis, wa[i], ExposureMask | ButtonPressMask);
  }

  XMapSubwindows(dis, w);
  XMapSubwindows(dis, wwork);
  XMapSubwindows(dis, wterm);

/*===============================*/


  /*---------- 返答の候補表示 (候補の表示) --------------*/
  for(k=0; k<8; k++){
    for(j=0; j<kouhosuu[k]; j++){
      wk[k][j]=XCreateSimpleWindow(dis, wattack, 50+40*j, CHAS*8+30*k-22, 20, 20, 1,1, color[hentou[kouho[k][j]]+2]);	   
    }
  }

  wok = XCreateSimpleWindow(dis, wattack, 50, CHAS*17-22, WSWX, WSWY, 2, 1, color[8]);
  XSelectInput(dis, wok, ExposureMask | ButtonPressMask);

  XMapSubwindows(dis, wattack);

  for(k=0; k<8; k++){
    for(j=0; j<kouhosuu[k]; j++){
      alpc[0]=alp[kouho[k][j]];
      XDrawString(dis, wk[k][j], gc, 5, (int)CHAS*2/3,alpc, 1);
    }
  }

  while(1)
    {
      XNextEvent(dis,&e);
      switch (e.type)
        {      case Expose:

/*- wattack -*/
		 XSetForeground(dis, gc, color[4]);
  XDrawString(dis, wok, gc, 25, CHAS,"NEXT", 4);
  XSetForeground(dis, gc, color[1]);

/*  XDrawString(dis, wattack, gc, 20, CHAS*5,"Acceptable responses are", 24); */
		 XDrawString(dis, wattack, gc, 20, CHAS*2,"By the new challenge pattern,", 29);
		 XDrawString(dis, wattack, gc, 20, CHAS*3,"the candidates were assigned to", 31);
		 XDrawString(dis, wattack, gc, 20, CHAS*4,"following responses (collors).", 29);


XDrawString(dis, wattack, gc, 10, CHAS*7,"digit           candidates", 26); 
for(k=0; k<8; k++){
  alpc[0]=alp[k+1];
    XDrawString(dis, wattack, gc, 20, CHAS*(8+k),alpc, 1); 
}
/*  alpc[0]='.';
  XDrawString(dis, wattack, gc, 20, CHAS*16,alpc, 1); 
*/
/*==========*/


/* その他 */
	drawhelp2(dis,gc,f1,f2,w,whelp,sw2,color,comma,comma2,yourpass);
/*
  XDrawString(dis, whelp, gc, 20, CHAS,"This user's password is", 23);
  XDrawString(dis, whelp, gc, 20, CHAS*2, comma, 2);
  XDrawString(dis, whelp, gc, 20+12*2, CHAS*2,yourpass, S_SIZE);
  XDrawString(dis, whelp, gc, 20+12*(S_SIZE+2), CHAS*2,comma2, 2);

                 XDrawString(dis, whelp, gc, 20+12*(S_SIZE+4), CHAS*2,".", 1);
XDrawString(dis, whelp, gc, 20, CHAS*3,"And now, you are the attacker.", 30);
*/

		  for(i=0; i<6; i++){
		    for(j=0; j<6; j++){
		      alpc[0]=alp[i+j*6];
		  XDrawString(dis, ws[i][j], gc, 15, CHAS,alpc, 1);
		     
		    }
		  }
		  drawhead(dis,gc,f1,f2,w,head,sw2,color);

                 XSetForeground(dis,gc,color[3]);
		 XDrawString(dis, wwork, gc, 180, CHAS,"Challenge-Response Type Human Identification", 44);

                 XSetForeground(dis,gc,color[1]);
XDrawString(dis, wterm, gc, SEP*2, CHAS,"Please Input Proper Colors.", 27);
/*========*/

		 break;

	       case ButtonPress:
#if EXT==1
		 if ( e.xany.window == sw2 ){
                   exit (1);
                 }	 
#endif
		 if ( e.xany.window == wok ){
/*		   XDestroyWindow(dis,wok);*/
		   XClearWindow(dis,wattack);
		   goto out;
		 }
	       }
    }
 out:
  /*========= 返答の候補表示 (候補の表示) ================*/

  
  XDrawString(dis, wattack, gc, 20, CHAS,"At", 2);
  alpc[0]=alp[look]; /* printf("%d\n",look);*/
  XDrawString(dis, wattack, gc, 20+12*3, CHAS,alpc, 1);

switch(look)
  {
  case 2:
    XDrawString(dis, wattack, gc, 20+12*4,CHAS,"nd time illegal observations,", 29);
    break;
    
  case 3:
   XDrawString(dis, wattack, gc, 20+12*4,CHAS,"rd time illegal observations,", 29);
    break;

  default:
    XDrawString(dis, wattack, gc, 20+12*4,CHAS,"th time illegal observations,", 29);
  }

  XDrawString(dis, wattack, gc, 20, CHAS*2,"the following response", 22);


/* 返答のパネルを押した場合 */
  for(i=0; i<S_SIZE; i++){
                  /*-------------  自動入力  ----------------*/
/*---- 押すと黒く --------*/
    press=XCreateSimpleWindow(dis, wterm, 50+40*(yourhen[i]+1), CHAS*3+40*6+20, 40, 40, 1, 1, color[1]);                            
    XMapWindow(dis, press);
    XFlush(dis);
    for(sec=0; sec<SEC; sec++){}
    XDestroyWindow(dis,press);
/*=========================*/   

    alpc[0]=yourhen[i]; 
    XFillArc(dis, wterm,gc,50+32*i,CHAS*4+40*7+20,30,30,0,360*64);

#if BELL==1                         
    printf("%c\n",BEL);
#endif
    XSetForeground(dis,gc,color[1]);
    XFillRectangle(dis, wattack,gc,19+22*i,CHAS*3-18,20,20);
    XSetForeground(dis,gc,color[yourhen[i]+2]);
    XFillRectangle(dis, wattack,gc,20+22*i,CHAS*3+1-18,18,18);
    XSetForeground(dis,gc,color[1]);              
			    
  }


/*---------------------------- accept -------------------------------*/


  XMapSubwindows(dis, wattack);


  while(1)
    {
      XNextEvent(dis,&e);
      switch (e.type)
        {      case Expose:
		 XDrawString(dis, wattack, gc, 20, CHAS*4,"was accepted.", 13); 
		 XDrawString(dis, wattack, gc, 10, CHAS*7,"digit           candidates", 26); 
		 for(k=0; k<8; k++){
		   alpc[0]=alp[k+1];
		   XDrawString(dis, wattack, gc, 20, CHAS*(8+k),alpc, 1); 
		 }
		 alpc[0]='.';
		 XDrawString(dis, wattack, gc, 20, CHAS*16,alpc, 1); 

		 break;
		 
	       case ButtonPress:
#if EXT==1
		 if ( e.xany.window == sw2 ){
                   exit (1);
                 }	 
#endif
		 if ( e.xany.window == wok ){
		   XDestroyWindow(dis,wok);
		   XClearWindow(dis,wattack);
		   goto out2;
		 }
	       }
    }
 out2:



  XDrawString(dis, wattack, gc, 20, CHAS*5,"So this user's password must be", 31); 

  /*-----------------         推定      ---------------------*/
  /*----- S1 を求める ---*/		     
  suitei0(S_SIZE,ALP_S,yourhen,hentou,kouho0);

/*  prn_kouho(S_SIZE,ALP_S,kouho,alp,kouhosuu);*/
  /*----- Sn (n>1) を求める ---*/
  for(j=0; j<S_SIZE; j++){
    tmpc=kouhosuu[j];
    suitei_siboru(kouho0[j],kouho[j],&tmpc);
    kouhosuu[j]=tmpc;
  }

  /*------- window に表示 --------*/

XDrawString(dis, wattack, gc, 10, CHAS*7,"digit           candidates", 26); 
for(k=0; k<8; k++){
  alpc[0]=alp[k+1];
    XDrawString(dis, wattack, gc, 20, CHAS*(8+k),alpc, 1); 
}
  alpc[0]='.';
  XDrawString(dis, wattack, gc, 20, CHAS*16,alpc, 1); 

  XDestroySubwindows(dis,wattack);

  for(k=0; k<8; k++){
    for(j=0; j<kouhosuu[k]; j++){
      wk[k][j]=XCreateSimpleWindow(dis, wattack, 50+40*j, CHAS*8+30*k-22, 20, 20, 1,1, color[yourhen[k]+2]);
        XSelectInput(dis, wk[k][j], ExposureMask );
      
    }
  }
		      /* 覗き見するかしないか？ */
		      wyorn=XCreateSimpleWindow(dis, wattack, 50, CHAS*17,WAX-100 , CHAS*5, 2,1, color[0]);
		      wyes=XCreateSimpleWindow(dis, wyorn, 30, CHAS*2+SEP*2, WSWX , WSWY, 1,1, color[8]);
		      wno=XCreateSimpleWindow(dis, wyorn, WAX-100-WSWX-30, CHAS*2+SEP*2, WSWX , WSWY, 1,1, color[8]);

		      XSelectInput(dis, wyorn, ExposureMask);
  XSelectInput(dis, wyes, ExposureMask | ButtonPressMask);
  XSelectInput(dis, wno, ExposureMask | ButtonPressMask);

 
  XMapSubwindows(dis, wattack);
  XMapSubwindows(dis, wyorn);


  while(1)
    {
      XNextEvent(dis,&e);
      switch (e.type)
        {      case Expose:
/*		 XDrawString(dis, wyorn, gc, 20, CHAS,"Do you do one more", 18);
		 XDrawString(dis, wyorn, gc, 40, CHAS*2,"illegal observation?", 20);*/

		 XDrawString(dis, wyorn, gc, 10, CHAS,"Peep at another challenge", 25);
		 XDrawString(dis, wyorn, gc, 40, CHAS*2,"and its responce", 16);

		 XSetForeground(dis, gc, color[4]);
		 XDrawString(dis, wyes, gc, 35, CHAS,"OK", 2);
		 XDrawString(dis, wno, gc, 25, CHAS,"QUIT", 4);
		 XSetForeground(dis, gc, color[1]);

		 /* 候補の表示 */
		 for(k=0; k<8; k++){
		   for(j=0; j<kouhosuu[k]; j++){
/*		      printf("%d\n",kouho[k][j]);*/
		     alpc[0]=alp[kouho[k][j]];
		     XDrawString(dis, wk[k][j], gc, 5, (int)CHAS*2/3,alpc, 1);
		     
		   }
		 }
		 /*===========*/

		 break;

	       case ButtonPress:
#if EXT==1                  
                 if ( e.xany.window == sw2 ){
                   exit (1);
                 }
#endif
/* Go to MENU */
		 if ( e.xany.window == wno ){
		   XDestroySubwindows(dis,wterm);
		   XDestroySubwindows(dis,wattack);
		   return;
		 }
/* illegal observation */
		 if ( e.xany.window == wyes ){
		   XDestroySubwindows(dis,wterm);
		   goto situmontigau;
		 }
		 break;
	       }
    }
}




/*****************************/
/* Sn  を求める              */
/*****************************/
suitei_siboru(kouho0,kouho,kouhosuu)
     int kouho0[],kouho[];
     int *kouhosuu;
{
    int i,ii,iii;
  
    int tmp[9];

    for(i=0;i<*kouhosuu;i++){
      tmp[i]=kouho[i];
      kouho[i]=-1;
    }
    
    iii=0;
    for(i=0;i<*kouhosuu;i++){
      for(ii=0;ii<9;ii++)
        {
          if( tmp[i] == kouho0[ii])
            {
              kouho[iii]=tmp[i];
              iii++;
	      break;
            }
        }
    }
    *kouhosuu=iii;
}


genehen(yourpass,yourhen,hentou,alp)
     char yourpass[];
     int yourhen[], hentou[];
     char alp[];
{
    int i,j;

    for(i=0; i<S_SIZE; i++){
        for(j=0; j<ALP_S; j++){
            if(yourpass[i]==alp[j]){
               yourhen[i]=hentou[j];
            }
        }
    }
}

prn_kouho(size,alp_s,kouho,alp,kouhosuu)
     int size,alp_s;
     int kouho[size][9];
     char alp[];
     int kouhosuu[];
{
  int i,ii;

    for(i=0;i<S_SIZE;i++)
    {
      for(ii=0;ii<kouhosuu[i];ii++)
        {
	  printf("%c",alp[(int)kouho[i][ii]]);
	}
      printf("\n");
    }
}





/***************************************************************/
/*     質問と答から暗証文字を推測                              */
/*=============================================================*/
/* Included by                                                 */
/*                                                     */
/***************************************************************/
suitei0(size,alp_s,yourhen,hentou,kouho)
     int size,alp_s;
     int yourhen[],hentou[],kouho[size][9];
{
  int i,ii,iii;
  

  
  for(i=0;i<S_SIZE;i++)
    {
      iii=0;
      for(ii=0;ii<ALP_S;ii++)
        {
          if( yourhen[i] == hentou[ii])
            {
              kouho[i][iii]=ii;
              iii++;
            }
        }
    }
}


/******* 各秘密情報に返答を割り当てる。 ****/
situmon(alp,hentou)
char alp[];
int hentou[];
{
  unsigned long int shyo;
  int i,j,tmpc;
  char tmp[ALP_S];

  for(i=0; i<ALP_S; i++){
      tmp[i]=i%4;
  }

  for(i=0; i<ALP_S-1; i++){
/*    printf("tmp=");
      for(j=0; j<ALP_S; j++){  
          printf("%d ",tmp[j]);
      }
    printf("\n");*/
    /* For a practical use, replace insecure random functions
       with cryptographically secure ones. */
    shyo=random();
    tmpc=shyo%(ALP_S-i); /* printf("tmpc=%d\n",tmpc);*/
    hentou[i]=tmp[tmpc];
    for(j=tmpc+1; j<ALP_S-i; j++){
        tmp[j-1]=tmp[j];
    }
  }
  hentou[ALP_S-1]=tmp[0];
/*    printf("hentou=");
      for(j=0; j<ALP_S; j++){  
          printf("%d",hentou[j]);
      }
    printf("\n");*/
}


/******************************/
/**  判定                */
/******************************/
int hantei(pass,yourpass)
     char pass[],yourpass[];
{
/*
printf("%s##%s#####\n",pass,yourpass);
printf("%d %d %d\n",strcmp(pass,yourpass),strlen(pass),strlen(yourpass));
*/

  if(strcmp(pass,yourpass)==0){
    return(1);  /* accept */
  }
  else{
    return(2);  /* reject */
  }
}

/******************************/
/**  accept                */
/******************************/
accept(dis,wterm,whelp,gc,f1,f2,w,head,sw2,color)
	Display 	         *dis;
	Window			 wterm,whelp,sw2;
	GC			 gc;
     Font					f1, f2;
     unsigned long color[];
{
  Window        wok;/*,wret;*/
  XEvent	   e;


  wok = XCreateSimpleWindow(dis, wterm, SEP*3, WTY-WSWY-SEP*3, WSWX, WSWY, 2, 1, color[8]);
/*
 wret = XCreateSimpleWindow(dis, wterm, WTX-WSWX-SEP*3, WTY-WSWY-SEP*3, WSWX, WSWY, 2, 1, color[8]);
*/

  XSelectInput(dis, wterm, ExposureMask);
  XSelectInput(dis, wok, ExposureMask | ButtonPressMask);
/*  XSelectInput(dis, wret, ExposureMask | ButtonPressMask);*/

  XMapSubwindows(dis, wterm);


  while(1)
    {
      XNextEvent(dis,&e);
      switch (e.type)
	{      case Expose:
		  drawhead(dis,gc,f1,f2,w,head,sw2,color);

		 XDrawString(dis, wterm, gc, 20, 50,"You are accepted!!", 18);

		  XSetForeground(dis, gc, color[4]);
/*		  XDrawString(dis, wok, gc, 20, CHAS,"GO TO MENU", 10);*/
		  XDrawString(dis, wok, gc, CHAS, CHAS,"OK", 4);
/*		  XDrawString(dis, wret, gc, 20, CHAS,"RETRY", 5);*/
		  XSetForeground(dis, gc, color[1]);
/*		  XSetFont(dis, gc, f2);*/
		 break;
	
	       case ButtonPress:
#if EXT==1		  
		 if ( e.xany.window == sw2 ){
		   exit (1);
		 }
#endif
/* Go to MENU */
         if ( e.xany.window == wok ){
		     XDestroySubwindows(dis,wterm);
		     return(1);
         }
/* Retry 
         if ( e.xany.window == wret ){
		     XDestroySubwindows(dis,wterm);
		     return(2);
         
		   }
*/
         break;
     }
    }
}


/******************************/
/**  accept                */
/******************************/
accept_cr(dis,wterm,whelp,gc,f1,f2,w,head,sw2,color)
	Display 	         *dis;
	Window			 wterm,whelp,sw2;
	GC			 gc;
     Font					f1, f2;
     unsigned long color[];
{
  Window        wok;/*,wret;*/
  XEvent	   e;


  wok = XCreateSimpleWindow(dis, wterm, SEP*3, WTY-WSWY-SEP*3, WSWX, WSWY, 2, 1, color[8]);
/* wret = XCreateSimpleWindow(dis, wterm, WTX-WSWX-SEP*3, WTY-WSWY-SEP*3, WSWX, WSWY, 2, 1, color[8]);*/

  XSelectInput(dis, wterm, ExposureMask);
  XSelectInput(dis, wok, ExposureMask | ButtonPressMask);
/*  XSelectInput(dis, wret, ExposureMask | ButtonPressMask);*/

  XMapSubwindows(dis, wterm);


  while(1)
    {
      XNextEvent(dis,&e);
      switch (e.type)
	{      case Expose:
		  drawhead(dis,gc,f1,f2,w,head,sw2,color);

		 XDrawString(dis, wterm, gc, 20, 50,"You are accepted!!", 18);


		  XSetForeground(dis, gc, color[4]);
/*		  XDrawString(dis, wok, gc, 20, CHAS,"GO TO MENU", 10);*/
		  XDrawString(dis, wok, gc, 25, CHAS,"MENU", 4);
/*		  XDrawString(dis, wret, gc, 20, CHAS,"RETRY", 5);*/
		  XSetForeground(dis, gc, color[1]);
/*		  XSetFont(dis, gc, f2);*/
		 break;
	
	       case ButtonPress:
#if EXT==1		  
		 if ( e.xany.window == sw2 ){
		   exit (1);
		 }
#endif
/* Go to MENU */
         if ( e.xany.window == wok ){
		     XDestroySubwindows(dis,wterm);
		     return(1);
         }
/* Retry 
         if ( e.xany.window == wret ){
		     XDestroySubwindows(dis,wterm);
		     return(2);
         }
*/         break;

     }
    }
}

/******************************/
/**  reject                */
/******************************/
reject(dis,wterm,whelp,gc,f1,f2,w,head,sw2,color)
	Display 	         *dis;
	Window			 wterm,whelp,sw2;
	GC			 gc;
	Font					f1, f2;
    unsigned long color[];
{
  Window        wok,wret;
  XEvent					e;


  wok = XCreateSimpleWindow(dis, wterm, SEP*3, WTY-WSWY-SEP*3, WSWX, WSWY, 2, 1, color[8]);
  wret = XCreateSimpleWindow(dis, wterm, WTX-WSWX-SEP*3, WTY-WSWY-SEP*3, WSWX, WSWY, 2, 1, color[8]);

  XSelectInput(dis, wterm, ExposureMask);
  XSelectInput(dis, wok, ExposureMask | ButtonPressMask);
  XSelectInput(dis, wret, ExposureMask | ButtonPressMask);

  XMapSubwindows(dis, wterm);


  while(1)
    {
      XNextEvent(dis,&e);
      switch (e.type)
	{      case Expose:
		 drawhead(dis,gc,f1,f2,w,head,sw2,color);


		 XDrawString(dis, wterm, gc, 20, 50,"You are rejected!!", 18);

                  XSetForeground(dis, gc, color[4]);
                  XDrawString(dis, wok, gc, 25, CHAS,"MENU", 4);
                  XDrawString(dis, wret, gc, 20, CHAS,"RETRY", 5);
                  XSetForeground(dis, gc, color[1]);


		 break;
	
	       case ButtonPress:
#if EXT==1		  
		 if ( e.xany.window == sw2 ){
		   exit (1);
		 }
#endif
         /* Go to menu */
         if ( e.xany.window == wok ){
		     XDestroySubwindows(dis,wterm);
		     return(0);
         }else{
		     /* retry */
		     if( e.xany.window == wret){
                 XDestroySubwindows(dis,wterm);
                 
                 return(3);
		     }
         }

	       }
    }
}



/********************************************/
/*           パスワードの設定               */
/********************************************/
genepass(yourpass,alp)
     char yourpass[],alp[];
{
  unsigned long int shyo;
  int i;

  for(i=0; i<S_SIZE; i++){
    /* For a practical use, replace insecure random functions
       with cryptographically secure ones. */
    shyo=random();
    yourpass[i]=alp[shyo%ALP_S];
  }

/*  printf("yourpass = %s\n",yourpass);*/
}



/*******************************************************
 **             LaserTank ver 5.0                     **
 **               By Jack Powell                      **
 **        Originally by Jim Kindley                  **
 **               (c) 2001-2016                       **
 **       See LICENSE in the top level directory      **
 *******************************************************
 **       Release version 2002 by Yves Maingoy        **
 **               ymaingoy@free.fr                    **
 *******************************************************
 **       Release version 2015 by Jack Powell         **
 **               jack9267@yahoo.com                  **
 *******************************************************/

#include "LaserTank.h"

using namespace Galactic3D;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

const int CLTank::GetNextBMArray[MaxObjects+1] = {0,1,2,3,4,5,6,8,9,10,7,12,13,14,11,16,17,18,15,19,21,22,23,20,24};
const int CLTank::OpeningBMA[16] = {4,6,1,9,56,57,33,19,16,13,14,45,15,55,21,47};

const int CLTank::GetOBMArray[MaxObjects+1] = {1,2,6,9,13,14,15,16,36,39,42,20,21,22,23,24,27,30,33,45,47,48,49,50,56,57,55};
const int CLTank::CheckArray[MaxObjects] = {1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1};
// Pad the beggining with junk array was [1..MaxBitMaps]
const int CLTank::BMSTA[MaxBitMaps+1] = {0,0,1,1,1,1,0,0,0,0,0,0,1,0,1,0,1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0};
const GCOLOUR CLTank::ColorList[8] =  {GCOLOUR_RED,GCOLOUR_LIME,GCOLOUR_BLUE,GCOLOUR_AQUA,GCOLOUR_YELLOW,
							GCOLOUR_FUCHSIA,GCOLOUR_WHITE,0xFF808080};

#ifdef GALACTIC_PLATFORM_IOS
GALACTIC3D_EXTERN_C void FirstLevelAchievement(void);
GALACTIC3D_EXTERN_C float LTank_GetStatusBarHeight(void);
#endif

CLTank::CLTank(void)
{
	AmbColour = GCOLOUR_WHITE;

	EditorOn   	= 0;		// true when in editor mode
	QHELP		= 0;		// True when Quick Help is On

	Rendering       = 0;            // do any rendering
	GFXError      	= 0;            // error used for load
	GFXOn			= 0;		// True when Graphics are loaded
	TankDirty		= 0;		// if true then we need to repaint the tank
	NoLevel     	= 1;         // if true Main Paint will show Openning
	Game_On     	= 0;        // true when game is running
	Ani_On      	= 1;         // true when Animation is On
	RLL         	= 1;         // remember last level
	ConvMoving   	= 0;        // true when moving on the conveyor belts
	OKtoHS			= 1;			// true if OK to Set HighScore
	OKtoSave		= 0;		// true if OK to Set HighScore
	Recording		= 0;		// true if Recording
	PlayBack		= 0;		// true if PlayBack is recording
	PBOpen			= 0;		// true when Playback window is open
	ARecord			= 0;		// AutoRecord is On/Off
	SkipCL			= 0;		// true if Skip Complete Level is on
	DWarn			= 0;		// Disable Warning
	CurLevel       	= 0;            // Used to Figure out the Current Level
	AniLevel        = 0;            // Used for Animation Position
	AniCount        = 0;            // counter for animation
	CurSelBM_L     	= 3;            // current selected bm in editor
	CurSelBM_R     	= 0;            // current selected bm in editor
	SpBm_Width		= 32;			// Width of Sprite
	SpBm_Height		= 32;			// Height of Sprite
	LaserOffset		= 10;			// Offset of Laser Size
	ContXPos		= 540;			// Position of Control Side
	ContYPos        = 245;          // Position of Control Side
	EditBMWidth		= 5;			// # of bitmaps across edit select area
	Speed			= 1;			// Playback speed
	SlowPB			= 1;
	RecBufSize		= 10000;		// Size of recording buffer
	UndoBufSize		= 3200;			// Size of Undo Buffer ( * sizeof(TGAMEREC))
	Difficulty		= 0;			// Difficulty Enable ( use Bits )
	GraphM			= 0;			// Graphics Mode 0=int; 1=ext; 2=ltg
	FindTank		= 0;		// True when First starting a level
	BlackHole		= 0;		// True if we TunnleTranslae to a Black Hole
	AltOp           = 0;            // Alternate operation

	XOffset = 16;             	// Game Board Offset from Left
	YOffset = 16;             	// Game Board Offset from top

	HeightOffset = 0;

	LastSFWord = 0;
	SFXError = 0;
	Sound_On = 1;				// true when sound is on

	RecBuffer = NULL;
	UndoBuffer = NULL;

	BS_SP	= 0;

	m_RenderingYOffset = 0;

	memset(m_pImages,0,sizeof(m_pImages));
	m_SpritesCount = 0;
	m_prgpSprites = NULL;
}

CLTank::~CLTank(void)
{
	KillBuffers();
	GFXKill();
}

void CLTank::Initialise(void)
{
	GStrCpy(FileName,GStr(LevelData));    // set up default file name
	PBSRec.Author[0] = (char)0;
	LaserColorG = GCOLOUR_LIME;
	LaserColorR = GCOLOUR_RED;
	InitBuffers();
	PBHold = 0;							// used by playback to hold charecters
	VHSOn = 0;
	RLL = 0;
	AssignHSFile();
	SFxInit();
}

void CLTank::ShutDown(void)
{
	KillBuffers();
	GFXKill();
}

void CLTank::Process(void)
{
	//SetGameSize();
	
	if (Game_On)
	{
		if (FindTank)
			{
				FindTank--;
				//PutLevel();
				//SetTimer(MainH,1,GameDelay,NULL);
			}
		if (Ani_On) AniCount++;
		if (AniCount == ani_delay) Animate(); 	// Do Animation
		if (Game.Tank.Firing)
			MoveLaser();   	// Move laser if one was fired

		if (PBOpen)
		{
			if (Speed == 2)
			{
				SlowPB++;
				if (SlowPB == SlowPBSet) SlowPB = 1;
			}
			if (PlayBack && (!( ConvMoving || SlideO.s || SlideT.s))
				&& ((Speed != 2) || ((Speed == 2) && (SlowPB == 1))))
			{
				PBHold = 0;
				//itoa(Game.RecP,temps,10);
				//SendMessage(PBCountH,WM_SETTEXT,0,(long)(temps));
				//if (Speed == 3) SendMessage(PlayH,WM_COMMAND,ID_PLAYBOX_02,0);
			}
			else PBHold = 1;
		}
		// Check Key Press }

		if ((Game.RecP < RB_TOS) && // (speedBug) &&
			(!(Game.Tank.Firing || ConvMoving || SlideO.s || SlideT.s || PBHold)))
		{
			switch (RecBuffer[Game.RecP])
			{
			case LC_Up:
				MoveTank(1); // Move tank Up one
				break;
			case LC_Right:
				MoveTank(2);
				break;
			case LC_Down:
				MoveTank(3);
				break;
			case LC_Left:
				MoveTank(4);
				break;
			case LC_Fire:
				{
					UpdateUndo();
					Game.ScoreShot++;  // do here Not in FireLaser
					FireLaser(Game.Tank.X,Game.Tank.Y,Game.Tank.Dir,S_Fire); // Bang
				}
				break;
			case LC_Undo:
				break;
			default:
				break;
			}
			Game.RecP++;	// Point to next charecter
			AntiTank();  // give the Anti-Tanks a turn to play
		}

		if (SlideO.s) IceMoveO();
		if (SlideT.s) IceMoveT();
		if (TankDirty) UpDateTank();	// I know we do this again later.
		ConvMoving = 0;    // used to disable Laser on the conveyor

		switch (Game.PF[Game.Tank.X][Game.Tank.Y])
		{
		case 2:
			if (Game_On)                   // Reached the Flag
			{
#ifdef GALACTIC_PLATFORM_IOS
				//if (CurLevel <= 1)
					FirstLevelAchievement();
#endif
				GameOn(0);
				VHSOn= 0;
				SoundPlay(S_EndLev);
				if (!PBOpen)
				{
					//if (Recording) SendMessage( Window,WM_SaveRec,0,0);
					CheckHighScore();
					LoadNextLevel(0,0);
				}
			}
			break;
		case 3:
			Dead();  // Water
			break;
		case 15:
			if (CheckLoc(Game.Tank.X,Game.Tank.Y-1)) // Conveyor Up
				ConvMoveTank(0,-1,1);
			break;
		case 16:
			if (CheckLoc(Game.Tank.X+1,Game.Tank.Y))
				ConvMoveTank(1,0,1);
			break;
		case 17:
			if (CheckLoc(Game.Tank.X,Game.Tank.Y+1))
				ConvMoveTank(0,1,1);
			break;
		case 18:
			if (CheckLoc(Game.Tank.X-1,Game.Tank.Y))
				ConvMoveTank(-1,0,1);
		}

		// Check the mouse Buffer
		if ((Game.RecP == RB_TOS) && (MB_TOS != MB_SP) &&
			(!(Game.Tank.Firing || ConvMoving || SlideO.s || SlideT.s)))
		{
			if (MouseOperation(MB_SP))				// Turn Mouse Operation into KB chars
			{
				MB_SP++;
				if (MB_SP == MaxMBuffer) MB_SP = 0;
			} else {
				MB_SP = MB_TOS;						// error so clear the rest
#ifdef WIN32
				MessageBeep(0);
#endif
			}
		}
		if (TankDirty) UpDateTank();
	}
}

void CLTank::Render(Galactic3D::Renderer* pRenderer, Galactic3D::Renderers::TwoDimensional* pTwoD)
{
	GCOLOUR ob;
	int i,j,x,y,LastLevel;

	Rendering = 1;
	
	//ContXPos = m_pSubscriber->m_pApp->GetRenderer()->m_nWidth-182;
	//ContXPos = 5;
	//ContYPos = (YOffset+(SpBm_Height*16)+16)+20;

	// draw 3D frames }
	//JK3dFrame(XOffset-1,YOffset-1,(SpBm_Width*16)+XOffset,(SpBm_Height*16)+YOffset,0);
	//JK3dFrame(XOffset-2,YOffset-2,(SpBm_Width*16)+XOffset+1,(SpBm_Height*16)+YOffset+1,0);
	//JK3dFrame(1,1,ContXPos-5,m_pSubscriber->m_pApp->GetRenderer()->m_nHeight-2,1);
	//JK3dFrame(ContXPos-1,1,ContXPos+181,m_pSubscriber->m_pApp->GetRenderer()->m_nHeight-2,1);
	//if (!EditorOn) JK3dFrame(ContXPos+10,250,ContXPos+165,m_pSubscriber->m_pApp->GetRenderer()->m_nHeight-10,0); // was 405

	ob = SelectColour(GCOLOUR_WHITE);
	//DrawImage(IMAGE_CONTROL,ContXPos,2,180,245,0,0,180,245);
	
	if ((CurLevel == 0) || QHELP )
	{
		int Image = IMAGE_OPENINGPC;
		if (PLATFORM_CURRENT == PLATFORM_IOS || PLATFORM_CURRENT == PLATFORM_ANDROID)
			Image = IMAGE_OPENING;
		DrawImage(pTwoD,Image,XOffset,YOffset,SpBm_Width*16,SpBm_Height*16,0,0,384,384);

		x = XOffset+3;
		y = YOffset + (SpBm_Height*8);
		j = 1;
		for (i=0;i<16;i++)
		{
			PutSprite(pTwoD,OpeningBMA[i],x,y);
   			x += (SpBm_Width*4);
			j++;
			if (j > 4)
			{
				x = XOffset+3; y += (SpBm_Height*2); j = 1;
  			}
		}
		// desactive  2004/05/09 - mgy
	}
	else {
		//ob = SelectColour(GCOLOUR_RED);
		//int ystart = YOffset+(SpBm_Height*16)+16;
		//DrawImage(-1,XOffset,ystart,SpBm_Width*16,m_pSubscriber->m_pApp->GetRenderer()->m_nHeight-ystart-16,0,0,0,0);
		//SelectColour(ob);

		// Lable Game Grid
		x = SpBm_Width / 2;
		y = (SpBm_Height-15) /2;

		/*for (i=1; i<17; i++)
		{
			//TextOut(pdc,8,YOffset+y+((i-1) * SpBm_Height),itoa(i,temps,10),strlen(temps));
			if ( i<10 )
			{
				//TextOut(pdc,8+XOffset+(16*SpBm_Width) ,YOffset+y+((i-1) * SpBm_Height),itoa(i,temps,10),strlen(temps));
			}
			else
			{
				//strcpy(temps, "1 ");
				//TextOut(pdc,-1+8+XOffset+(16*SpBm_Width) ,YOffset+y+((i-1) * SpBm_Height),temps, strlen(temps));
				//itoa(i-10,temps,10);
				//TextOut(pdc,3+8+XOffset+(16*SpBm_Width) ,YOffset+y+((i-1) * SpBm_Height),temps,strlen(temps));
			}


			//strcpy(temps,"@"); temps[0] = temps[0] + i;
			//TextOut(pdc,XOffset+x+((i-1) * SpBm_Width),1,temps,strlen(temps));
			//TextOut(pdc,XOffset+x+((i-1) * SpBm_Width),YOffset+1+(16 * SpBm_Height),temps,strlen(temps));

		}*/

		PutLevel(pTwoD);
		if (EditorOn)
		{
			PutSelectors(pTwoD);
			ShowTunnelID();
		}
		/*else {
			//SetTextColor(pdc,DifCList[0]);
			//itoa(CurLevel,temps,10);
			switch (CurRecData.SDiff)
			{
			case 1:
				//strcat(temps,txt023);
				//SetTextColor(pdc,DifCList[1]);
				break;
			case 2:
				//strcat(temps,txt024);
				//SetTextColor(pdc,DifCList[2]);
				break;
			case 4:
				//strcat(temps,txt025);
				//SetTextColor(pdc,DifCList[3]);
				break;
			case 8:
				//strcat(temps,txt026);
				//SetTextColor(pdc,DifCList[4]);
				break;
			case 16:
				//strcat(temps,txt027);
				//SetTextColor(pdc,DifCList[5]);
				break;
			}
			//TextOut(pdc,ContXPos+91,43,temps,strlen(temps));
			//TextOut(pdc,ContXPos+91,100,CurRecData.LName,strlen(CurRecData.LName));
			//TextOut(pdc,ContXPos+91,150,CurRecData.Author,strlen(CurRecData.Author));
			//SetTextColor(pdc,0x0000FF00);
			//TextOut(pdc,ContXPos+48,207,itoa(Game.ScoreMove,temps,10),strlen(temps));
			//TextOut(pdc,ContXPos+134,207,itoa(Game.ScoreShot,temps,10),strlen(temps));
		}*/
	}

	if (laser.Firing)
		UpDateLaser(pTwoD);

	laser.Firing = 0;

	Rendering = 0;
}

void CLTank::SoundPlay ( int sound )
{
}

void CLTank::SFxInit( void)
{
}

// ---------------------------------------------------
// Move   a Sliding Object FROM stack
// ---------------------------------------------------
void CLTank::Mem_to_SlideO( int iSlideObj ) // MGY
{
	if (iSlideObj <= SlideMem.count ) {
	    SlideO.x  = SlideMem.Objects[iSlideObj].x;
		SlideO.y  = SlideMem.Objects[iSlideObj].y;
		SlideO.dx = SlideMem.Objects[iSlideObj].dx;
		SlideO.dy = SlideMem.Objects[iSlideObj].dy;
		SlideO.s  = SlideMem.Objects[iSlideObj].s;
	}
}

// ---------------------------------------------------
// Move a Sliding Object ON TO stack
// Update SlideMem with an object SlideO
// ---------------------------------------------------
void CLTank::SlideO_to_Mem( int iSlideObj ) // MGY
{
 	if (iSlideObj <= SlideMem.count ) {
	    SlideMem.Objects[iSlideObj].x  = SlideO.x ;
		SlideMem.Objects[iSlideObj].y  = SlideO.y ;
		SlideMem.Objects[iSlideObj].dx = SlideO.dx;
		SlideMem.Objects[iSlideObj].dy = SlideO.dy;
		SlideMem.Objects[iSlideObj].s  = SlideO.s ;
	}
}

// ---------------------------------------------------
// Add an object in the stack for slidings objects
// But, if this object is already in this stack,
// just change dir and don't increase the counter.
// ---------------------------------------------------
void CLTank::add_SlideO_to_Mem() // MGY
{
	int iSlideObj;

	if (SlideMem.count < MAX_TICEMEM-1) {
	    for ( iSlideObj = 1 ; iSlideObj <= SlideMem.count; iSlideObj++) {
			if ( (SlideMem.Objects[iSlideObj].x  == SlideO.x) &&
				 (SlideMem.Objects[iSlideObj].y  == SlideO.y) )
			{
				SlideO_to_Mem( iSlideObj ); // Update the stack
				return; // don't inc the counter
			}
		}
		// Add this object to the stack
		SlideMem.count++;
		SlideO_to_Mem(SlideMem.count);
		SlideO.s = (SlideMem.count>0);
	}
}

// ---------------------------------------------------
// Delete a Sliding Object from stack
// ---------------------------------------------------
void CLTank::sub_SlideO_from_Mem( int iSlideObj ) // MGY
{
	int i;
	for (i = iSlideObj ; i < SlideMem.count; i++) {
		Mem_to_SlideO( i+1 );
	    SlideO_to_Mem( i );
	}
	SlideMem.count--;
	SlideO.s = (SlideMem.count>0);
}

// ---------------------------------------------------
// If an object is sliding and is hit by a laser,
// delete it from stack.
// ---------------------------------------------------
void CLTank::del_SlideO_from_Mem( int x, int y ) // MGY
{
	int iSlideObj;
    for ( iSlideObj = SlideMem.count; iSlideObj >=1 ; iSlideObj--) {
		if ( (SlideMem.Objects[iSlideObj].x  == x) &&
			 (SlideMem.Objects[iSlideObj].y  == y) )
		{
			// remove this object
		    sub_SlideO_from_Mem( iSlideObj );

			return;
		}
	}
	SlideO.s = (SlideMem.count>0);
}

// ---------------------------------------------------
// Used to handle a bug :  the speed bug
// MGY - 22-nov-2002
// Return True if the tank is on Convoyor.
// ---------------------------------------------------
int CLTank::TestIfConvCanMoveTank()
{
	switch (Game.PF[Game.Tank.X][Game.Tank.Y])
	{
	case 15:
		if (CheckLoc(Game.Tank.X,Game.Tank.Y-1)) // Conveyor Up
			return(1);
		break;
	case 16:
		if (CheckLoc(Game.Tank.X+1,Game.Tank.Y))
			return(1);
		break;
	case 17:
		if (CheckLoc(Game.Tank.X,Game.Tank.Y+1))
			return(1);
		break;
	case 18:
		if (CheckLoc(Game.Tank.X-1,Game.Tank.Y))
			return(1);
		break;
	}
	return( 0 );
}

void CLTank::SetButtons( int ButtonX)
{
	//EnableWindow(BT1,(ButtonX & 1) == 1);
	//EnableWindow(BT2,(ButtonX & 2) == 2);
	//EnableWindow(BT3,(ButtonX & 4) == 4);
	//EnableWindow(BT4,(ButtonX & 8) == 8);
	//EnableWindow(BT5,(ButtonX & 16) == 16);
	//EnableWindow(BT6,(ButtonX & 32) == 32);
	//EnableWindow(BT7,(ButtonX & 64) == 64);
	//EnableWindow(BT8,(ButtonX & 128) == 128);
}

void CLTank::FileError()
{
	//LPVOID lpMsgBuf;

	//FormatMessage(
	//	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	//	NULL,
	//	GetLastError(),
	//	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	//	(LPTSTR) &lpMsgBuf,
	//	0, NULL );
	//// Display the string.
	//MessageBox( MainH, lpMsgBuf, "System Error", MB_OK|MB_ICONINFORMATION );
	//// Free the buffer.
	//LocalFree( lpMsgBuf );
}

void CLTank::AddKBuff( char zz)
{
	int i;
	if (!Game_On) return;
	RecBuffer[RB_TOS] = zz;
	RB_TOS++;
	if (RB_TOS >= RecBufSize)
	{

		i = RecBufSize + RecBufStep;
		RecBuffer = (char*)realloc(RecBuffer,i);
		if (RecBuffer == NULL)
		{			// Recorder Buffer Overflow
			FileError();
			//if (Recording) SendMessage(MainH,WM_SaveRec,0,0);
			RB_TOS = 0;
		}
		else RecBufSize = i;
	}
}

/* find the shortest path to the target via a fill search algorithm */
void CLTank::FindTarget(int px, int py, int pathlen)
{
	if ((px<0) || (px>15) || (py<0) || (py>15)) return; // outer edges
	// if we hit something AND we are not at the tank then return
	if((Game.PF[px][py] != 0) && (!((Game.Tank.X == px) && (Game.Tank.Y == py))))
		return;		// we hit something - ouch


	if(findmap[px][py] <= pathlen) return;

	findmap[px][py] = pathlen++;

	if((px == Game.Tank.X) && (py == Game.Tank.Y)) return; // speed's us up

	FindTarget(px - 1, py, pathlen);
	FindTarget(px + 1, py, pathlen);
	FindTarget(px, py - 1, pathlen);
	FindTarget(px, py + 1, pathlen);
}

int CLTank::MouseOperation (int sp)
{
	int dx,dy,XBigger,cx,cy,ltdir;

	dx = MBuffer[sp].X - Game.Tank.X;
	dy = MBuffer[sp].Y - Game.Tank.Y;
	XBigger = abs(dx) > abs(dy);		// true if x is bigger than y

	if (MBuffer[sp].Z == 1)
	{
		// Mouse Move
		/* Fill the trace map */
		for(cx = 0; cx < 16; cx++)
			for (cy = 0; cy < 16; cy++)
				findmap[cx][cy] = BADMOVE;
		// We will test the destination manually
		dx = Game.PF[MBuffer[sp].X][MBuffer[sp].Y];		// temp store in dx
		if (!( (dx < 3) || ((dx > 14) && (dx < 19)) ||
			    (dx > 23) || ((Obj_Tunnel & dx) == Obj_Tunnel)))
			return(0);
		findmap[MBuffer[sp].X][MBuffer[sp].Y] = 0;		// destination
		/* flood fill search to find a shortest path to the push point. */
		FindTarget(MBuffer[sp].X - 1, MBuffer[sp].Y, 1);
		FindTarget(MBuffer[sp].X + 1, MBuffer[sp].Y, 1);
		FindTarget(MBuffer[sp].X, MBuffer[sp].Y - 1, 1);
		FindTarget(MBuffer[sp].X, MBuffer[sp].Y + 1, 1);

		/* if we didn't make it back to the players position, there is no valid path
		* to that place.
		*/
		if(findmap[Game.Tank.X][Game.Tank.Y] == BADMOVE) {
			return(0);
		} else {
			/* we made it back, so let's walk the path we just built up */
			cx = Game.Tank.X;
			cy = Game.Tank.Y;
			ltdir = Game.Tank.Dir;			// we need to keep track of direction
			while(findmap[cx][cy]) {
				if((cx > 0) && (findmap[cx - 1][cy] == (findmap[cx][cy] - 1))) {
					if (ltdir != 4) AddKBuff(LC_Left);
					AddKBuff(LC_Left);
					cx--;
					ltdir = 4;
				} else if((cx < 15) && (findmap[cx + 1][cy] == (findmap[cx][cy] - 1))) {
					if (ltdir != 2) AddKBuff(LC_Right);
					AddKBuff(LC_Right);
					cx++;
					ltdir = 2;
				} else if((cy > 0) && (findmap[cx][cy - 1] == (findmap[cx][cy] - 1))) {
					if (ltdir != 1) AddKBuff(LC_Up);
					AddKBuff(LC_Up);
					cy--;
					ltdir = 1;
				} else if((cy < 15) && (findmap[cx][cy + 1] == (findmap[cx][cy] - 1))) {
					if (ltdir != 3) AddKBuff(LC_Down);
					AddKBuff(LC_Down);
					cy++;
					ltdir = 3;
				} else {
					/* if we get here, something is SERIOUSLY wrong, so we should abort */
					abort();
				}
			}
		}

	}
	else {
		// Mouse Shot
		if (XBigger)
		{
			if (dx > 0)
			{
				if (Game.Tank.Dir != 2) AddKBuff(LC_Right);		// Turn Right
			}
			else if (Game.Tank.Dir != 4) AddKBuff(LC_Left);		// Turn Left
		}
		else {
			if (dy > 0)
			{
				if (Game.Tank.Dir != 3) AddKBuff(LC_Down);		// Turn Down
			}
			else if (Game.Tank.Dir != 1) AddKBuff(LC_Up);		// Turn Up
		}
		AddKBuff(LC_Fire);
	}
	return(1);
}

void CLTank::InitBuffers()
{
	int x;

	UndoP = 0;
	UndoBuffer = (PGAMEREC)realloc(UndoBuffer,UndoBufSize * sizeof(TGAMEREC));// Undo Buffer
	RecBuffer = (char*)realloc(RecBuffer,RecBufSize);						// Recording Buffer
	Backspace[BS_SP] = 0;												// Clear Backspace stack
	MB_TOS = MB_SP = 0;
	SlideT.s = 0;														// nothing sliding
	SlideO.s = 0;
	SlideMem.count = 0; // MGY

	UndoRollOver = UndoMax;
}

void CLTank::ResetUndoBuffer()
// Reset Undo Buffer to 1 block only; all variable = 0
{
	UndoBuffer = (PGAMEREC)realloc(UndoBuffer,UndoBufStep * sizeof(TGAMEREC));
	if (UndoBuffer == NULL)
	{			// Undo Buffer Allocation Error
		FileError();
	}
	UndoBufSize = UndoBufStep;
	UndoP = 0;
	UndoBuffer->Tank.Dir = 0;
	// Lets also init the Mouse Buffer
	MB_TOS = MB_SP = 0;
}

void CLTank::KillBuffers()
{
	if (UndoBuffer != NULL)
	{
		free(UndoBuffer);
		UndoBuffer = NULL;
	}
	if (RecBuffer != NULL)
	{
		free(RecBuffer);
		RecBuffer = NULL;
	}
}

void CLTank::UpdateUndo()		// Come here whenever we move or shoot
{
	int i;
	PGAMEREC tmpUndoArray;

	UndoP++;
	if (UndoP >= UndoBufSize)
	{
		if (UndoP >= UndoMax)
		{
			UndoRollOver = (UndoP - 1);			// Save Where we rolled Over
			UndoP = 0;
		}
		else {
			i = UndoBufSize + UndoBufStep;
			tmpUndoArray = (PGAMEREC)realloc(UndoBuffer,i * sizeof(TGAMEREC));
			if (tmpUndoArray == NULL)
			{			// Undo Buffer Allocation Error
				//MessageBox(MainH,txt019,txt007,MB_OK | MB_ICONERROR);
				UndoRollOver = (UndoP - 1);			// Save Where we rolled Over
				UndoP = 0;
			} else 	{
				UndoBufSize = i;
				UndoBuffer = tmpUndoArray;
			}
		}
	}
	UndoBuffer[UndoP] = Game;
	//EnableMenuItem(MMenu,110,MF_BYCOMMAND | MF_ENABLED); // enable Undo Command
	//EnableWindow(BT1,SW_SHOWNA);
}

void CLTank::UndoStep()
{
	if (UndoBuffer[UndoP].Tank.Dir == 0)	 return;	// out of data
	Game = UndoBuffer[UndoP];
	RB_TOS = Game.RecP;						// clear all keys not processed
	SlideT.s = 0;							// stop any sliding
	SlideO.s = 0;
	SlideMem.count = 0; // MGY
	UndoBuffer[UndoP].Tank.Dir = 0;
	UndoP--;
	if (UndoP < 0)
	{
		UndoP = UndoRollOver;
	}
	MB_TOS = MB_SP = 0;						// we need to cancle all mouse inputs
}

void CLTank::DrawSprite(Galactic3D::Renderers::TwoDimensional* pTwoD, char bmn, int x,int y)
// paint sprite ( bitmap ) at screen location x,y
// add grass behind if transparent
{
	GCOLOUR ob;
	if (Rendering)
	{
		ob = SelectColour(AmbColour);
		//DrawImage(IMAGE_GAME,x-1,y-1,SpBm_Width+1,SpBm_Height+1,BMA[bmn].X,BMA[bmn].Y,32,32);
		DrawImage(pTwoD,bmn+100,x,y,SpBm_Width,SpBm_Height,0,0,0,0);
		SelectColour(ob);
	}
}

void CLTank::PutSprite(Galactic3D::Renderers::TwoDimensional* pTwoD, char bmn, int x,int y)
// paint sprite ( bitmap ) at screen location x,y
// add grass behind if transparent
{
	if (BMSTA[bmn] == 1)
	{
		DrawSprite(pTwoD,1,x,y);
	}
	DrawSprite(pTwoD,bmn,x,y);
}

void CLTank::PutSprite(char bmn, int x,int y)
{
}

void CLTank::UpDateSprite(Galactic3D::Renderers::TwoDimensional* pTwoD, int x, int y)
{
	int bmn,bmn2,x1,y1;
	GCOLOUR Bpen,penO;

	bmn = Game.BMF[x][y];
	//x1 = XOffset + (x * SpBm_Width);
	//y1 = YOffset + (y * SpBm_Height);
	x1 = XOffset + (x * (SpBm_Width));
	y1 = YOffset + (y * (SpBm_Height));

	//if (BMSTA[bmn] == 1 )
	//{
	//	bmn2 = Game.BMF2[x][y];
	//	DrawSprite(bmn2,x1,y1);
	//	DrawSprite(bmn,x1,y1);
	//}
	//else
	//{
	//	DrawSprite(bmn,x1,y1);
	//}

	if (bmn == 55)
	{
		Bpen = ColorList[GetTunnelID(x,y)];
		penO = SelectColour(Bpen);
		Rectangle(pTwoD,x1,y1,x1+SpBm_Width,y1+SpBm_Height);
		Rectangle(pTwoD,x1,y1,x1+SpBm_Width,y1+SpBm_Height);
		//SetTextColor(gDC,RGB(0,0,0));
		//SetBkColor(gDC,RGB(255,255,255));
		DrawSprite(pTwoD,55,x1,y1);
		SelectColour(penO);
	}
	else
	    {
		if (BMSTA[bmn] == 1 )
		{
			bmn2 = Game.BMF2[x][y];
			if (bmn2 == 55)
			{
				Bpen = ColorList[((Game.PF2[x][y] & 0x0F)  >> 1)];
				penO = SelectColour(Bpen);
				Rectangle(pTwoD,x1,y1,x1+SpBm_Width,y1+SpBm_Height);
				//SetTextColor(gDC,RGB(0,0,0));
				//SetBkColor(gDC,RGB(255,255,255));
				DrawSprite(pTwoD,55,x1,y1);
				SelectColour(penO);
			}
			else
			{
				DrawSprite(pTwoD,bmn2,x1,y1);
			}
			//SetTextColor(gDC,RGB(0,0,0));
			//SetBkColor(gDC,RGB(255,255,255));
			//if (bmn != 1)
				DrawSprite(pTwoD,bmn,x1,y1);
		}
		else{
			//if (bmn != 1)
				DrawSprite(pTwoD,bmn,x1,y1);
		}
	}
}

void CLTank::UpDateSprite(int x, int y)
{
}

void CLTank::UpDateTank(Galactic3D::Renderers::TwoDimensional* pTwoD)
// paint mask then tank sprite, we mask the cell so the back ground will show
{
	TankDirty = 0;
	DrawSprite(pTwoD,1 + Game.Tank.Dir,XOffset +(Game.Tank.X*SpBm_Width),YOffset +(Game.Tank.Y*SpBm_Height));
	//SetTextColor(gDC,RGB(0,0,0));
	//SetBkColor(gDC,RGB(255,255,255));
	//BitBlt (gDC,XOffset +(Game.Tank.X*SpBm_Width),YOffset +(Game.Tank.Y*SpBm_Height),
	//	SpBm_Width,SpBm_Height,MaskDC,BMA[1 + Game.Tank.Dir].X,0,SRCAND);
	//BitBlt (gDC,XOffset +(Game.Tank.X*SpBm_Width),YOffset +(Game.Tank.Y*SpBm_Height),
	//	SpBm_Width,SpBm_Height,BuffDC,BMA[1 + Game.Tank.Dir].X,0,SRCPAINT);
}

void CLTank::UpDateTank(void)
{
}

void CLTank::UpDateLaser(Galactic3D::Renderers::TwoDimensional* pTwoD)
// paint laser
{
	GCOLOUR ob;
	int   x,y;

	ob = SelectColour(LaserColor);
	x = XOffset + (laser.X*SpBm_Width);
	y = YOffset + (laser.Y*SpBm_Height);
	if ((laser.Dir & 1) == 1) Rectangle(pTwoD,x+LaserOffset,y,x+SpBm_Width-LaserOffset,y+SpBm_Height);
	else  Rectangle(pTwoD,x,y+LaserOffset,x+SpBm_Width,y+SpBm_Height-LaserOffset);
	//if ((laser.Dir & 1) == 1) m_pRenderer->m_pTwoD->Draw2DImage(NULL, Math::Vector2D(x+LaserOffset,y+m_pRenderer->m_RenderingYOffset), Math::Vector2D(SpBm_Width-LaserOffset*2,SpBm_Height), LaserColor);
	//else  m_pRenderer->m_pTwoD->Draw2DImage(NULL, Math::Vector2D(x,y+LaserOffset+m_pRenderer->m_RenderingYOffset), Math::Vector2D(SpBm_Width,SpBm_Height-LaserOffset*2), LaserColor);
	SelectColour(ob);
}

void CLTank::UpDateLaser(void)
{
}

void CLTank::UpDateLaserBounce(int a, int b)
// paint laser bouncing off of a mirror }
{
	int   x,y,h;
	int iSlideObj; // MGY

	//ob = SelectObject(gDC,LaserColor);
	x = XOffset + (laser.X*SpBm_Width);
	y = YOffset + (laser.Y*SpBm_Height);
	h = SpBm_Width / 2;

	// we need to stop advance the LaserShot if sliding on ice & hit
	for ( iSlideObj=1; iSlideObj<= SlideMem.count; iSlideObj++) // MGY
		// MGY
		if (SlideMem.Objects[iSlideObj].s
	        && (SlideMem.Objects[iSlideObj].x == laser.X)
			&& (SlideMem.Objects[iSlideObj].y == laser.Y)) LaserBounceOnIce = 1;
		//if (SlideO.s && (SlideO.x == laser.X) && (SlideO.y == laser.Y)) LaserBounceOnIce = TRUE;

	//switch (a)
	//{
	//case 1: Rectangle(gDC,x+LaserOffset,y+h,x+SpBm_Width-LaserOffset,y+SpBm_Height);
	//		break;
	//case 2: Rectangle(gDC,x,y+LaserOffset,x+h,y+SpBm_Height-LaserOffset);
	//		break;
	//case 3:	Rectangle(gDC,x+LaserOffset,y,x+SpBm_Width-LaserOffset,y+h);
	//		break;
	//case 4: Rectangle(gDC,x+h,y+LaserOffset,x+SpBm_Width,y+SpBm_Height-LaserOffset);
	//}
	//switch (b)
	//{
	//case 1: Rectangle(gDC,x+LaserOffset,y,x+SpBm_Width-LaserOffset,y+h);
	//	break;
	//case 2: Rectangle(gDC,x+h,y+LaserOffset,x+SpBm_Width,y+SpBm_Height-LaserOffset);
	//	break;
	//case 3: Rectangle(gDC,x+LaserOffset,y+h,x+SpBm_Width-LaserOffset,y+SpBm_Height);
	//	break;
	//case 4: Rectangle(gDC,x,y+LaserOffset,x+h,y+SpBm_Height-LaserOffset);
	//}
}

void CLTank::PutLevel(Galactic3D::Renderers::TwoDimensional* pTwoD)
// paint all game cell's and the tank
{
	GCOLOUR ob;
	int x,y;

	/*{
		if (Rendering)
		{
			ob = SelectColour(AmbColour);
			//DrawImage(IMAGE_GAME,x-1,y-1,SpBm_Width+1,SpBm_Height+1,BMA[bmn].X,BMA[bmn].Y,32,32);
			DrawImage(1+100,XOffset,YOffset,SpBm_Width*16,SpBm_Height*16,0,0,0,0);
			SelectColour(ob);
		}
	}*/
	for (y=0; y<16; y++) for (x=0;x<16;x++) UpDateSprite(pTwoD,x,y);
	UpDateTank(pTwoD);
	TankDirty = 0;
	if (FindTank)
	{
		ob = SelectColour(LaserColorR);
		//SelectObject(gDC,GetStockObject(NULL_PEN));
		x = XOffset + (Game.Tank.X*SpBm_Width);
		y = YOffset + (Game.Tank.Y*SpBm_Height);
		Rectangle(pTwoD,x+LaserOffset,YOffset+2,x+SpBm_Width-LaserOffset,YOffset+(16*SpBm_Height));
		Rectangle(pTwoD,XOffset+2,y+LaserOffset,XOffset+(16*SpBm_Width),y+SpBm_Height-LaserOffset);
		SelectColour(ob);
	}
}

void CLTank::GFXInit()
{
	int x,y,i;
	x = 0; y = 0;
	for (i = 1; i <= MaxBitMaps; i++)
	{
		BMA[i].X = x;
		BMA[i].Y = y;
		x += 32;
		if ( x >= (32 * 10))
		{
			x = 0; y += 32;
		}
	}
	GFXOn = 1;
}

void CLTank::GFXKill()
{
	if (m_prgpSprites != NULL)
	{
		for (size_t i=0; i<m_SpritesCount; i++)
			m_prgpSprites[i]->Release();

		delete[] m_prgpSprites;
		m_prgpSprites = NULL;
	}
	for (size_t i=0; i<IMAGE_COUNT; i++)
	{
		if (m_pImages[i] != NULL)
		{
			m_pImages[i]->Release();
			m_pImages[i] = NULL;
		}
	}
	GFXOn = 0;
}

void CLTank::ChangeGO(int x, int y, int CurSelBM)  // Change Game Object
{
	int i;
	if (CurSelBM == 1)  // Tank
	{
		UpDateSprite(Game.Tank.X,Game.Tank.Y); // repaint under old tank
		Game.BMF[x][y] = 1;
		Game.PF[x][y] = 0;
		Game.Tank.X = x; Game.Tank.Y = y;
		UpDateSprite(Game.Tank.X,Game.Tank.Y); // paint under new tank
		UpDateTank();
	}
	else
	    {
		if (CurSelBM == MaxObjects)
		{
			// Tunnel
			i = 0;
			//i = DialogBox(hInst,"LoadTID",MainH,(DLGPROC)LoadTID) << 1;
			Game.PF[x][y] = i + 0x40;
			Game.BMF[x][y] = 55;
			UpDateSprite(x,y);
			ShowTunnelID();
		}
		else {
			Game.PF[x][y] = CurSelBM;
			Game.BMF[x][y] = GetOBM(CurSelBM);
			UpDateSprite(x,y);
		}

	}
}

void CLTank::BuildBMField()
{
	int x,y,i,j;
	unsigned char pt; // mgy 18-05-2003

	Game.Tank.X = 7; Game.Tank.Y = 15;
	Game.Tank.Dir = 1; Game.Tank.Firing = 0;
	for (x = 0; x<16; x++) for (y = 0; y<16; y++)
	{
		// --- mgy 18-05-2003 only legal pieces ---
		pt = Game.PF[x][y];
		if(pt>0x19)
		{
			pt = GetTunnelID(x,y);
			Game.PF[x][y] = (pt<<1) +0x40;
		}
		// --- end of 18-05-2003 ---

		if (Game.PF[x][y] == 1)
		{
			i = 1;
			Game.Tank.X = x;
			Game.Tank.Y = y;
			Game.PF[x][y] = 0;
		}
		else {
			if (Game.PF[x][y] < 64 ) i = GetOBM(Game.PF[x][y]);
			else if (ISTunnel(x,y)) i = 55;
		}
		Game.BMF[x][y] = i;
		Game.BMF2[x][y] = 1;
		Game.PF2[x][y] = 0;
	}
	// this is a good place to reset the score counters }
	Game.ScoreMove = 0;
	Game.ScoreShot = 0;
}

void CLTank::GameOn(int b)
{
	//if (b) SetTimer(MainH,1,GameDelay,NULL);
	//else  KillTimer(MainH,1);
	Game_On = b;
//#ifdef DEBUG
//	if (b) DEBUG_Time = timeGetTime();
//	DEBUG_Frames = 0;
//#endif
}

void CLTank::JK3dFrame(int x, int y, int x2, int y2, int up)
{
}

void CLTank::JKSelFrame(int x, int y, int x2, int y2, int pnum)
{
}

char CLTank::GetOBM ( char ob )
{
	if ((ob > -1) && (ob <= MaxObjects)) return (GetOBMArray[ob]);
	else return(1);
}

void CLTank::LoadLastLevel()
{
	char temps[11];
	THSREC TempHSData;

	if (CurLevel < 2) return;
	GameOn(0);
	Stream* pLevelStream = GFileSystem->Open(FileName, false);
	if (pLevelStream == NULL)
	{
		//MessageBox(MainH,txt001,txt007,MB_OK | MB_ICONERROR);
		//PostMessage(MainH,WM_COMMAND,108,0);
		return;
	}
#ifdef LASERTANK_HIGHSCORES
	Stream* pHighscoresStream = GFileSystem->Open(HFileName, false);
#endif
	CurLevel--;
	do {
		CurLevel--;
		pLevelStream->Seek(CurLevel * sizeof(TLEVEL),SEEK_SET);
		BytesMoved = pLevelStream->Read(&CurRecData, sizeof(TLEVEL), 1)*sizeof(TLEVEL);
		if (BytesMoved < sizeof(TLEVEL))
		{
			pLevelStream->Release();
#ifdef LASERTANK_HIGHSCORES
			if (pHighscoresStream != NULL)
				pHighscoresStream->Release();
#endif
			//SendMessage(MainH,WM_GameOver,0,0);
			return;
		}
#ifdef LASERTANK_HIGHSCORES
		if ( SkipCL && (pHighscoresStream != NULL))
		{
			pHighscoresStream->Seek(CurLevel * sizeof(THSREC),SEEK_SET);
			BytesMoved = pHighscoresStream->Read(&TempHSData,sizeof(THSREC),1)*sizeof(THSREC);
			if (BytesMoved < sizeof(THSREC)) TempHSData.moves = 0;
			if (TempHSData.moves > 0) CurRecData.SDiff = 128; // Error SDiff
		}
#endif
	} while ((CurLevel > 0) && (CurRecData.SDiff > 0) && ((Difficulty & CurRecData.SDiff) == 0));
	pLevelStream->Release();
#ifdef LASERTANK_HIGHSCORES
	if (pHighscoresStream != NULL)
		pHighscoresStream->Release();
#endif
	// Load For Real
	LoadNextLevel(1,0);
}

bool CLTank::LoadNextLevel(int DirectLoad, int Scanning)
// Directload is true if we shouldn't use difficulties & Completed Level check
// Scanning is true if we are searching and dont want any errors displayed
{
	char temps[11];
	THSREC TempHSData;
	int SavedLevelNum;

	//if (GameInProg)
	//{
	//	SavedLevelNum = MessageBox(MainH,txt039,txt038,MB_YESNOCANCEL | MB_ICONQUESTION);
	//	if (SavedLevelNum == IDCANCEL) return(false);
	//	if (SavedLevelNum == IDYES)	SendMessage(MainH,WM_SaveRec,0,0);
	//}
	GameOn(0);
	SavedLevelNum = CurLevel;
	if ( Difficulty == 0) Difficulty = 31;
	//if ( Difficulty == 0) SendMessage(MainH,WM_COMMAND,225,0);	// Set Difficulty Window
	//if ( Recording ) SendMessage(MainH,WM_COMMAND,123,0);		// Turn Off Recording if on
	Stream* pLevelStream = GFileSystem->Open(FileName, false);
	if (pLevelStream == NULL)
	{
		//MessageBox(MainH,txt001,txt007,MB_OK | MB_ICONERROR);
		//PostMessage(MainH,WM_COMMAND,108,0);
		return false;
	}
#ifdef LASERTANK_HIGHSCORES
	Stream* pHighscoresStream = GFileSystem->Open(HFileName, false);
#endif
	do {
		pLevelStream->Seek(CurLevel * sizeof(TLEVEL), SEEK_SET);
		BytesMoved = pLevelStream->Read(&CurRecData, sizeof(TLEVEL), 1)*sizeof(TLEVEL);
		if (BytesMoved < sizeof(TLEVEL))
		{
			pLevelStream->Release();
#ifdef LASERTANK_HIGHSCORES
			if (pHighscoresStream != NULL)
				pHighscoresStream->Release();
#endif
			CurLevel = SavedLevelNum;				// If eof use last Level Number
			//if (!Scanning) SendMessage(MainH,WM_GameOver,0,0);
			return false;
		}
#ifdef LASERTANK_HIGHSCORES
		if ( SkipCL && (pHighscoresStream != NULL)  && (!DirectLoad) )
		{
			pHighscoresStream->Seek(CurLevel * sizeof(THSREC), SEEK_SET);
			BytesMoved = pHighscoresStream->Read(&TempHSData,sizeof(THSREC), 1)*sizeof(THSREC);
			if (BytesMoved < sizeof(THSREC)) TempHSData.moves = 0;
			if (TempHSData.moves > 0) CurRecData.SDiff = 128; // Error SDiff
		}
#endif
		CurLevel++;
	} while ((!DirectLoad) && (CurRecData.SDiff > 0) && ((Difficulty & CurRecData.SDiff) == 0));
	pLevelStream->Release();
#ifdef LASERTANK_HIGHSCORES
	if (pHighscoresStream != NULL)
		pHighscoresStream->Release();
#endif
	//Game.PF = CurRecData.PF;
	memcpy(Game.PF,CurRecData.PF,sizeof(TPLAYFIELD));
	BuildBMField();
	GameOn(1);
	OKtoHS = 1;
	OKtoSave = 1;
	//EnableMenuItem(MMenu,110,MF_BYCOMMAND | MF_GRAYED); // disable Undo
	//EnableMenuItem(MMenu,112,MF_BYCOMMAND | MF_GRAYED); // disable Restore
	SetButtons(0xFA);
	FindTank = 5;
	//SetTimer(MainH,1,700,NULL);
	//InvalidateRect(MainH,NULL,TRUE);
	ResetUndoBuffer();
	if (RLL)
	{
		//WritePrivateProfileString("DATA",psRLLN,FileName,INIFile);
		//WritePrivateProfileString("DATA",psRLLL,itoa(CurLevel,temps,10),INIFile);
	}
	Game.RecP = 0;
	RB_TOS = 0;
	SlideT.s = 0;							// Just in case
	SlideO.s = 0;
	SlideMem.count = 0; // MGY
	//if (ARecord && ( !PBOpen)) SendMessage(MainH,WM_COMMAND,123,0);
	if (Backspace[BS_SP] != CurLevel)		// if CurLevel != LastLevel
	{
		//if (Backspace[BS_SP]) EnableMenuItem(MMenu,118,MF_BYCOMMAND ); // Enable Menu Item
		BS_SP++;
		if (BS_SP > 9) BS_SP = 0;
		Backspace[BS_SP] = CurLevel;		// Add CurLevel to stack
	}
	return(true);
}

void CLTank::AssignHSFile()
{
	GChar *P;

	GStrCpy(GHFileName,GStr("/User/"));
	GStrCat(GHFileName,FileName);
	GStrCpy(GStrRChr(GHFileName,GStr('.')),GStr(".ghs"));
	GStrCpy(HFileName,GStr("/User/"));
	GStrCat(HFileName,FileName);
	GStrCpy(GStrRChr(HFileName,'.'),GStr(".hs"));

	// Set up Record Default file name
	GStrCpy(PBFileName,FileName);
	P = GStrRChr(PBFileName,'.');				// Set Up to truncate file name
	if (P)  P[0] = 0;						// Set end of string
	GStrCat(PBFileName,GStr("_0000.lpb"));			// add to name
}

void CLTank::CheckHighScore()
{
#ifdef LASERTANK_HIGHSCORES
	int x, i;

	if (!OKtoHS) return;
	//F2 = CreateFile(HFileName,GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,NULL,
	//	OPEN_ALWAYS, FILE_FLAG_RANDOM_ACCESS,NULL);
	F2 = GFileSystem->Open(HFileName,true);
	if (F2 == NULL)
		F2 = GFileSystem->Create(HFileName);

	if (F2 == NULL)	FileError();
	else
	{
		// Check if All lower scores are filled in, if not then set levels
		if ((CurLevel * sizeof(THSREC)) > (F2->GetLength()))
		{
			HS.moves = 0;			// Sets field to blank
			for (x=(i / sizeof(THSREC)); x < CurLevel-1; x++)
				BytesMoved = F2->Write(&HS,sizeof(THSREC),1);
		}
		F2->Seek((CurLevel-1) * sizeof(THSREC),SEEK_SET);
		BytesMoved = F2->Read(&HS,sizeof(THSREC),1)*sizeof(THSREC);
		if ((HS.moves == 0) || (Game.ScoreMove < HS.moves) || ((Game.ScoreMove == HS.moves) && (Game.ScoreShot < HS.shots)))
		{
			//SendMessage(MainH,WM_NewHS,0,0);
			F2->Seek((CurLevel - 1) * sizeof(THSREC),SEEK_SET);
			BytesMoved = F2->Write(&HS,sizeof(THSREC),1);
		}
		SAFE_RELEASE(F2);
	}
#endif
}

void CLTank::Animate()
{
	int x,y;

	AniLevel++;
	AniCount = 0;
	if (AniLevel == 3) AniLevel = 0;
	for (x = 0; x<16; x++) for (y = 0; y<16; y++)
	{
		// Animate Conveyor Belts & Flag if under something
		switch (Game.PF2[x][y])
		{
		case 2:		Game.BMF2[x][y] = 6 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 15:	Game.BMF2[x][y] = 24 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 16:	Game.BMF2[x][y] = 27 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 17:	Game.BMF2[x][y] = 30 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 18:	Game.BMF2[x][y] = 33 + AniLevel;
					UpDateSprite(x,y);
					break;
		}
		// Now animate all top sprites
		switch (Game.PF[x][y])
		{
		case 2:		Game.BMF[x][y] = 6 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 3:		Game.BMF[x][y] = 9 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 7:		Game.BMF[x][y] = 16 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 8:		Game.BMF[x][y] = 36 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 9:		Game.BMF[x][y] = 39 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 10:	Game.BMF[x][y] = 42 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 15:	Game.BMF[x][y] = 24 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 16:	Game.BMF[x][y] = 27 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 17:	Game.BMF[x][y] = 30 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 18:	Game.BMF[x][y] = 33 + AniLevel;
					UpDateSprite(x,y);
					break;
		}
	}
	TankDirty = 1;
}

void CLTank::TranslateTunnel( int *x, int *y )
{
	int cx,cy,i;
	char bb;

	bb = Game.PF[*x][*y];		// bb is ID #
	WaitToTrans = 0;
	BlackHole = 0;
	for (cy=0; cy < 16; cy ++) for (cx = 0; cx < 16; cx ++)
		if ((Game.PF[cx][cy] == bb) && (!((*x == cx) && (*y == cy))))
		{
			*x = cx;			// We found an exit YEA !!!
			*y = cy;
			return;
		}
	// check for blocked hole - something is over the exit
	// Scan the 2nd layer any matches are blocked holes
	for (cy=0; cy < 16; cy ++) for (cx = 0; cx < 16; cx ++)
		if ((Game.PF2[cx][cy] == bb) && (!((*x == cx) && (*y == cy))))
		{
			// We found one so we will set the flag
			WaitToTrans = 1;
			return;						// Blocked so no warp
		}
	// There is no match, so it is a black hole
	BlackHole = 1;
}

void CLTank::ConvMoveTank(int x, int y, int check)
{
	UpDateSprite(Game.Tank.X,Game.Tank.Y);
	Game.Tank.Y += y;
	Game.Tank.X += x;
	if (ISTunnel(Game.Tank.X,Game.Tank.Y))
	{
		TranslateTunnel(&Game.Tank.X,&Game.Tank.Y);		// We moved into a tunnel
		if (BlackHole) Dead();	// The tunnel was a black hole
	}
	if (WaitToTrans) Game.Tank.Good = 1;
	TankDirty = 1;
	ConvMoving = 1;
	if (wasIce && check)
	{
		SlideT.x = Game.Tank.X;
		SlideT.y = Game.Tank.Y;
		SlideT.s = 1;
		SlideT.dx = x;
		SlideT.dy = y;
	}
	AntiTank();
}

void CLTank::UpDateTankPos(int x, int y)
{
	char temps[50];

	SoundPlay(S_Move);
	UpdateUndo();
	Game.ScoreMove++;
	//SetTextAlign(gDC,TA_CENTER);
	//SetTextColor(gDC,0x0000FF00);
	//SetBkColor(gDC,0);
	//TextOut(gDC,ContXPos+48,207,itoa(Game.ScoreMove,temps,10),strlen(temps));
	UpDateSprite(Game.Tank.X,Game.Tank.Y);
	Game.Tank.Y += y;
	Game.Tank.X += x;
	Game.Tank.Good = 0;				// we need it somewhere if we move off a tunnel
	if (ISTunnel(Game.Tank.X,Game.Tank.Y))
	{
		TranslateTunnel(&Game.Tank.X,&Game.Tank.Y);		// We moved into a tunnel
		if (BlackHole) Dead();	// The tunnel was a black hole
	}
	if (WaitToTrans) Game.Tank.Good = 1;
	TankDirty = 1;
}

void CLTank::MoveTank(int d)
{
	if (Game.Tank.Dir != d)				// The Tank is Turning
	{
		Game.Tank.Dir = d;
		//UpDateSprite(Game.Tank.X,Game.Tank.Y);
		TankDirty = 1;
		SoundPlay(S_Turn);
		return;
	}
	switch (d)
	{
	case 1:	if (CheckLoc(Game.Tank.X,Game.Tank.Y-1)) UpDateTankPos(0,-1);
			else SoundPlay(S_Head);		// Ouch we are hitting something hard
			SlideT.dy = -1;	SlideT.dx = 0;
			break;
	case 2:	if (CheckLoc(Game.Tank.X+1,Game.Tank.Y)) UpDateTankPos(1,0);
			else SoundPlay(S_Head);
			SlideT.dy = 0; SlideT.dx = 1;
			break;
	case 3:	if (CheckLoc(Game.Tank.X,Game.Tank.Y+1)) UpDateTankPos(0,1);
			else SoundPlay(S_Head);
			SlideT.dy = 1; SlideT.dx = 0;
			break;
	case 4:	if (CheckLoc(Game.Tank.X-1,Game.Tank.Y)) UpDateTankPos(-1,0);
			else SoundPlay(S_Head);
			SlideT.dy = 0; SlideT.dx = -1;
			break;
	}
	if (wasIce)
	{
		SlideT.x = Game.Tank.X;
		SlideT.y = Game.Tank.Y;
		SlideT.s = TRUE;
	}
}

int CLTank::CheckLoc(int x, int y)
{
	// Check if Tank can move
	if ((x<0) || (x>15) || (y<0) || (y>15)) return(0);
	wasIce = ((Game.PF[x][y] == Obj_Ice) || (Game.PF[x][y] == Obj_ThinIce));
	if ((Game.PF[x][y] & Obj_Tunnel) == Obj_Tunnel) return(TRUE);
	return(CheckArray[Game.PF[x][y]]);
}

void CLTank::MoveObj(int x, int y, int dx, int dy, int sf)
// used by CheckLLoc
{
	int obt, bm, i, bb, cx, cy, ok;

	obt = Game.PF[x][y];								// Get Object type
	bm = Game.BMF[x][y];
	if ((Game.PF2[x][y] & Obj_Tunnel) == Obj_Tunnel)	// Check if Tunnel
	{
		bb = Game.PF2[x][y] | 1;						// bb is ID # w/ 1 set
		ok = FALSE;
		for (cy=0; cy < 16; cy ++) for (cx = 0; cx < 16; cx ++)
			if ((Game.PF2[cx][cy] == bb) && (!((x == cx) && (y == cy))))
			{
				ok = TRUE;
				goto MoveObj1;
				// Ok if something wants to move here; cx & cy set to orig
			}
MoveObj1:
		if (ok)										// We are Moving an Object
		{
			Game.PF[x][y] = Game.PF[cx][cy];		// Transfer Blocked Object
			Game.BMF[x][y] = Game.BMF[cx][cy];
			Game.PF[cx][cy] = Game.PF2[cx][cy] & 0xFE;	// Return Saved State
			Game.PF2[cx][cy] = 0;
			Game.BMF[cx][cy] = Game.BMF2[cx][cy];
			UpDateSprite(cx,cy);
		}
		else {										// Not Blocked Anymore
			Game.PF[x][y] = Game.PF2[x][y] & 0xFE;	// Return Saved State strip
			Game.PF2[x][y] = 0;
			Game.BMF[x][y] = Game.BMF2[x][y];
			// We didn't find a match so maybe the tank is it
			if ((Game.PF[Game.Tank.X][Game.Tank.Y] == (bb & 0xFE)) && Game.Tank.Good)
			{
				Game.ScoreMove--; // MGY - 2003/05/18 - v408b15 -  Bartok Bug.lvl
				UpDateTankPos(0,0);
				UndoP--;
			}
		}
	}
	else {			// If not a tunnel
		Game.PF[x][y] = Game.PF2[x][y];				 	// Return Saved State
		Game.PF2[x][y] = 0;
		Game.BMF[x][y] = Game.BMF2[x][y];
	}
	UpDateSprite(x,y);
	x += dx;
	y += dy;
	if (ISTunnel(x,y))
	{
		TranslateTunnel(&x,&y);		// We moved into a tunnel
		if (BlackHole) return;		// The tunnel was a black hole
	}
	else WaitToTrans = FALSE;

	Game.PF2[x][y] = Game.PF[x][y];  				// Save Return State
	if (WaitToTrans) Game.PF2[x][y] |= 1;			// Set bit 1 if we are waiting to transport
	Game.BMF2[x][y] = Game.BMF[x][y];
	if (Game.PF[x][y] != 3)
	{
		Game.PF[x][y] = obt;
		Game.BMF[x][y] = bm;
	}
	else
	{
		sf = S_Sink;
		if (obt == 5)
		{
			Game.PF[x][y] = 0;
			Game.PF2[x][y] = 0;  // Pushing Block into Water }
			Game.BMF[x][y] = 19;
			Game.BMF2[x][y] = 19;
		}
	}
	UpDateSprite(x,y);
	if ((x == Game.Tank.X) && (y == Game.Tank.Y)) TankDirty = TRUE;
	SoundPlay(sf);
}

void CLTank::IceMoveT()					// Move the tank on the Ice
{
	int savei;


	if (Game.PF[SlideT.x][SlideT.y] == Obj_ThinIce)
	{
		Game.BMF[SlideT.x][SlideT.y] = 9;
		Game.PF[SlideT.x][SlideT.y] = Obj_Water;	// Ice to Water
		UpDateSprite(SlideT.x,SlideT.y);
	}

	if (CheckLoc(SlideT.x + SlideT.dx, SlideT.y + SlideT.dy))
	{
		savei = wasIce;
		ConvMoveTank(SlideT.dx,SlideT.dy,FALSE);		// use this insted of UpDateTank
	}
	else {
		SlideT.s = FALSE;
		return;
	}

	SlideT.x += SlideT.dx;					// Update Position
	SlideT.y += SlideT.dy;					// Update Position
	if (!savei) SlideT.s = FALSE;			// The ride is over
}

void CLTank::IceMoveO()					// Move an Object on the Ice
{
	int savei;
    int iSlideObj; // MGY

	for ( iSlideObj = SlideMem.count; iSlideObj>= 1; iSlideObj--) // MGY
    {
		Mem_to_SlideO( iSlideObj ); // Get from memory

		if ( iSlideObj <= SlideMem.count ) // just in case ... MGY
			{

			if (Game.PF2[SlideO.x][SlideO.y] == Obj_ThinIce)
			{
				Game.BMF2[SlideO.x][SlideO.y] = 9;
				Game.PF2[SlideO.x][SlideO.y] = Obj_Water;	// Ice to Water
			}

			if (CheckLoc(SlideO.x + SlideO.dx, SlideO.y + SlideO.dy) &&
			    (!((SlideO.x + SlideO.dx == Game.Tank.X) && (SlideO.y + SlideO.dy == Game.Tank.Y))))
			{
				savei = wasIce;
				MoveObj(SlideO.x,SlideO.y,SlideO.dx,SlideO.dy,S_Push2);
				AntiTank();

				SlideO.x += SlideO.dx;					// Update Position
				SlideO.y += SlideO.dy;					// Update Position
				if (!savei) {
			   		SlideO.s = FALSE;			// The ride is over
					SlideO_to_Mem( iSlideObj ); // update memory
					sub_SlideO_from_Mem( iSlideObj );
				}
				else {
					SlideO_to_Mem( iSlideObj ); // update memory
				}
			}
			else {

				if (Game.PF2[SlideO.x][SlideO.y] == Obj_Water)
					MoveObj(SlideO.x,SlideO.y,0,0,0);		// Drop Object in the water (was thin ice)
				SlideO.s = FALSE;
				SlideO_to_Mem( iSlideObj ); // update memory
				sub_SlideO_from_Mem( iSlideObj );
				AntiTank();										// incase an anti-tank is behind a block
				//return; // MGY
			}
		}


	}

	Mem_to_SlideO( SlideMem.count ); // Get from memory the last object of the list
	SlideO.s = ( SlideMem.count > 0 );
}

void CLTank::KillAtank( int x, int y, char bm)
// used by CheckLLoc
{
	Game.PF[x][y] = 4; // Solid Object}
	Game.BMF[x][y] = bm; // Junk Bitmap}
	//UpDateSprite(x,y);
	SoundPlay(S_Anti1);
}

int CLTank::CheckLLoc(int x, int y, int dx, int dy)
// this is were the laser does it's damage
// returns true if laser didn't hit anything
{
	if ((x<0) || (x>15) || (y<0) || (y>15))
	{
		return(FALSE);
	}
	if ((x == Game.Tank.X) && (y == Game.Tank.Y))
	{
		Dead();
		return(FALSE);
	}
	wasIce = FALSE;
	switch (Game.PF[x][y])
	{
	case 0:
	case 2:
	case 3:
	case 15:
	case 16:
	case 17:
	case 18:	return(TRUE);
	case 4:		SoundPlay(S_LaserHit);
				break;
	case 5:	if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push1);
			else SoundPlay(S_LaserHit);
			break;
	case 6:	Game.PF[x][y] = 0;
			Game.BMF[x][y] = 1;
			//UpDateSprite(x,y);
			SoundPlay(S_Bricks);
			break;
	case 7:	if (dy == 1) { KillAtank(x,y,54); return(FALSE); }
			else if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push3);
			else SoundPlay(S_LaserHit);
			break;
	case 8:	if (dx == -1) { KillAtank(x,y,52);return(FALSE); }
			else if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push3);
			else SoundPlay(S_LaserHit);
			break;
	case 9:	if (dy == -1) { KillAtank(x,y,12);return(FALSE); }
			else if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push3);
			else SoundPlay(S_LaserHit);
			break;
	case 10: if (dx == 1) { KillAtank(x,y,53);return(FALSE); }
			else if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push3);
			else SoundPlay(S_LaserHit);
			break;
	case 11: if ((laser.Dir == 2) || (laser.Dir == 3)) return(TRUE);
			if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push2);
			else SoundPlay(S_LaserHit);
			break;
	case 12: if ((laser.Dir == 3) || (laser.Dir == 4)) return(TRUE);
			if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push2);
			else SoundPlay(S_LaserHit);
			break;
	case 13: if ((laser.Dir == 1) || (laser.Dir == 4)) return(TRUE);
			if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push2);
			else SoundPlay(S_LaserHit);
			break;
	case 14: if ((laser.Dir == 1) || (laser.Dir == 2)) return(TRUE);
			if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push2);
			else SoundPlay(S_LaserHit);
			break;
	case 19: if (laser.Good) PutSprite(46,XOffset+(x*SpBm_Width),YOffset + (y*SpBm_Height));
			else PutSprite(51,XOffset+(x*SpBm_Width),YOffset + (y*SpBm_Height));
			return(TRUE);
	case 20: if ((laser.Dir == 2) || (laser.Dir == 3)) return(TRUE);
			Game.PF[x][y] = 21;	Game.BMF[x][y] = 48;
			UpDateSprite(x,y);
			SoundPlay(S_Rotate);
			break;
	case 21: if ((laser.Dir == 3) || (laser.Dir == 4)) return(TRUE);
			Game.PF[x][y] = 22;	Game.BMF[x][y] = 49;
			UpDateSprite(x,y);
			SoundPlay(S_Rotate);
			break;
	case 22: if ((laser.Dir == 1) || (laser.Dir == 4)) return(TRUE);
			Game.PF[x][y] = 23;	Game.BMF[x][y] = 50;
			UpDateSprite(x,y);
			SoundPlay(S_Rotate);
			break;
	case 23: if ((laser.Dir == 1) || (laser.Dir == 2)) return(TRUE);
			Game.PF[x][y] = 20;	Game.BMF[x][y] = 47;
			UpDateSprite(x,y);
			SoundPlay(S_Rotate);
			break;
	case 24:	// Ice
	case 25:	// thin Ice
			return(TRUE);
	default: if (ISTunnel(x,y)) return(TRUE);
	}
	if (wasIce)
	{
		// If is already sliding, del it !
		del_SlideO_from_Mem( x, y);
		// and add a new slide in a new dirrection
		SlideO.x = x+dx;
		SlideO.y = y+dy;
		SlideO.s = TRUE;
		SlideO.dx = dx;
		SlideO.dy = dy;
		add_SlideO_to_Mem();
	}
	// MGY
	else {
	    // SlideO.s = FALSE;		// in case we side hit off of the ice
		del_SlideO_from_Mem( x, y);
	}
	return (FALSE);
}

void CLTank::MoveLaser()
{
	int x,y,oDir;

LaserMoveJump:
	LaserBounceOnIce = 0;
	x = 0;
	y = 0;
	switch (laser.Dir)
	{
	case 1:	y = -1;
		break;
	case 2:	x = +1;
		break;
	case 3:	y = +1;
		break;
	case 4:	x = -1;
	}
	if (CheckLLoc(laser.X+x,laser.Y+y,x,y))
	{
		if (laser.Firing) UpDateSprite(laser.X,laser.Y);
		laser.Y += y;
		laser.X += x;
		if (((Game.PF[laser.X][laser.Y] > 10) && (Game.PF[laser.X][laser.Y] < 15))
			    || ((Game.PF[laser.X][laser.Y] > 19) && (Game.PF[laser.X][laser.Y] < 24)))
		{
			oDir = laser.Dir;
			switch (Game.PF[laser.X][laser.Y])
			{
			case 11:
			case 20:	if (laser.Dir == 2) laser.Dir = 1;
						else laser.Dir = 4;
						break;
			case 12:
			case 21:	if (laser.Dir == 3) laser.Dir = 2;
						else laser.Dir = 1;
						break;
			case 13:
			case 22:	if (laser.Dir == 1) laser.Dir = 2;
						else laser.Dir = 3;
						break;
			case 14:
			case 23:	if (laser.Dir == 1) laser.Dir = 4;
						else laser.Dir = 3;
			}
			UpDateLaserBounce(oDir,laser.Dir);
			SoundPlay(S_Deflb);
		}
		else UpDateLaser();
		laser.Firing = TRUE;
	} else {
		Game.Tank.Firing = FALSE;
		if (laser.Firing) UpDateSprite(laser.X,laser.Y);
		if (Game_On || VHSOn) AntiTank();

		// SpeedBug - MGY - 22-11-2002
		if ( TestIfConvCanMoveTank() )
			ConvMoving = TRUE;
	}
	if (LaserBounceOnIce) goto LaserMoveJump;
}

void CLTank::FireLaser(int x, int y, int d, int sf)
{
	char temps[30];

	Game.Tank.Firing = TRUE;
	laser.Dir = d;
	laser.X = x;
	laser.Y = y;
	laser.Firing = FALSE; // true if laser has been moved
	SoundPlay(sf);
	//SetTextAlign(gDC,TA_CENTER);
	//SetTextColor(gDC,0x0000FF00);
	//SetBkColor(gDC,0);
	//itoa(Game.ScoreShot,temps,10);   // we incremented it in wm_timer
	//TextOut(gDC,ContXPos+134,207,temps,strlen(temps));
	if (sf == 2) LaserColor = LaserColorG;
	else LaserColor = LaserColorR;
	laser.Good = ( sf == 2);
	MoveLaser();
}

void CLTank::AntiTank()
{
	int x,y;
	// Program Anti tank seek }

	if (Game.Tank.Firing) return;

	x = Game.Tank.X;	// Look to the right
	while (CheckLoc(x,Game.Tank.Y)) x++;
	if ((x<16) && (Game.PF[x][Game.Tank.Y] == 10) && (Game.Tank.X != x))
	{
		FireLaser(x,Game.Tank.Y,4,S_Anti2);
		return;
	}
	x = Game.Tank.X;	// Look to the left
	while (CheckLoc(x,Game.Tank.Y)) x--;
	if ((x>=0) && (Game.PF[x][Game.Tank.Y] == 8) && (Game.Tank.X != x))
	{
		FireLaser(x,Game.Tank.Y,2,S_Anti2);
		return;
	}
	y = Game.Tank.Y;	// Look Down
	while (CheckLoc(Game.Tank.X,y)) y++;
	if ((y<16) && (Game.PF[Game.Tank.X][y] == 7) && (Game.Tank.Y != y))
	{
		FireLaser(Game.Tank.X,y,1,S_Anti2);
		return;
	}
	y = Game.Tank.Y;	// Look Up
	while (CheckLoc(Game.Tank.X,y)) y--;
	if ((y>=0) && (Game.PF[Game.Tank.X][y] == 9) && (Game.Tank.Y != y))
	{
		FireLaser(Game.Tank.X,y,3,S_Anti2);
		return;
	}
}

void CLTank::PutSelectors(Galactic3D::Renderers::TwoDimensional* pTwoD)
{
	int x,y,i,j;

	/*x = ContXPos + 5; y = 260; j = 1;
	for (i=0; i <= MaxObjects; i++)
	{
		JK3dFrame(x-1,y-1,x+SpBm_Width,y+SpBm_Height,TRUE);
		if (i == CurSelBM_L) JKSelFrame(x-1,y-1,x+SpBm_Width,y+SpBm_Height,1);
		if (i == CurSelBM_R) JKSelFrame(x-1,y-1,x+SpBm_Width,y+SpBm_Height,2);
		PutSprite(GetOBM(i),x,y);
		x += SpBm_Width+4;
		j++;
		if (j > EditBMWidth)
		{
			x = ContXPos + 5; y +=SpBm_Height+4; j = 1;
		}
	}*/

	// I fixed an error in LaserTank here where it originally adds 15 rather than 5.
	// ContYPos + 15
	// Doing this change makes the selectors work pixel perfect.

	x = ContXPos + 5; y = ContYPos + 5; j = 1;
	for (i=0; i <= MaxObjects; i++)
	{
		PutSprite(pTwoD,GetOBM(i),x,y);
		x += SpBm_Width+4;
		j++;
		if (j > EditBMWidth)
		{
			x = ContXPos + 5; y +=SpBm_Height+4; j = 1;
		}
	}

	pTwoD->Flush();

	x = ContXPos + 5; y = ContYPos + 5; j = 1;
	for (i=0; i <= MaxObjects; i++)
	{
		JK3dFrame(x-1,y-1,x+SpBm_Width,y+SpBm_Height,TRUE);
		if (i == CurSelBM_L) JKSelFrame(x-1,y-1,x+SpBm_Width,y+SpBm_Height,1);
		if (i == CurSelBM_R) JKSelFrame(x-1,y-1,x+SpBm_Width,y+SpBm_Height,2);
		x += SpBm_Width+4;
		j++;
		if (j > EditBMWidth)
		{
			x = ContXPos + 5; y +=SpBm_Height+4; j = 1;
		}
	}
}

void CLTank::ShowTunnelID()
{
	int x,y;
	GChar temps[20];

	// scan and add Tunnel ID #s
	//SetBkMode(gDC,OPAQUE);
	//SetTextAlign(gDC,TA_LEFT);
	//SetTextColor(gDC,0);
	for (x=0; x < 16; x++) for (y=0; y < 16; y++)
		if (ISTunnel(x,y))
		{
			GSPrintF(temps,GStr("(%1d)"),GetTunnelOldID(x,y));
			//TextOut(gDC,22 + (x * SpBm_Width),20 + (y * SpBm_Height),temps,strlen(temps));
		}
}

void CLTank::SetGameSize(int nWidth, int nHeight, int StatusBarHeight)
{
	//char temps[11];
	//CheckMenuItem(MMenu,120,0);
	//CheckMenuItem(MMenu,121,0);
	//CheckMenuItem(MMenu,122,0);
	//CheckMenuItem(MMenu,119+i,MF_CHECKED);
	//itoa(i,temps,10);
	//WritePrivateProfileString("SCREEN",psSize,temps,INIFile);
	//if (GFXOn) GFXKill();
	int iPadding = 8;
	int iPaddingLeft = iPadding;
	int iPaddingTop = iPadding;
	int iPaddingRight = iPadding;
	int iPaddingBottom = iPadding;
	
	bool bYCentre = true;

#ifdef GALACTIC_PLATFORM_IOS
	bYCentre = false;
#endif
	
	int iMaxGridWidth = nWidth-iPaddingLeft-iPaddingRight;
	int iMaxGridHeight = nHeight-StatusBarHeight-iPaddingTop-iPaddingBottom;
	int iMaxGridSize = GMATHUTIL_SELECT_MIN(iMaxGridWidth,iMaxGridHeight);
	int iTileSize = iMaxGridSize/16;
	int iGridSize = iTileSize*16;
	XOffset = iPaddingLeft+iMaxGridWidth/2-iGridSize/2;
	if (bYCentre)
		YOffset = iPaddingTop+iMaxGridHeight/2-iGridSize/2;
	else
		YOffset = iPaddingTop;
	SpBm_Width = iTileSize;
	SpBm_Height = iTileSize;
	ContXPos = iPaddingLeft;
	ContYPos = iPaddingTop+iGridSize+iPadding;
	ControlsY = ContYPos;
	ControlsHeight = nHeight-StatusBarHeight-HeightOffset-iPaddingBottom-ControlsY;
	ButtonsWidth = 110;
	ControlsWidth = nWidth-iPaddingRight-iPaddingLeft-ButtonsWidth-iPadding;
	ControlsX = nWidth-iPaddingBottom-ControlsWidth;
	EditBMWidth = 6;//TODO: Fix this; it's wrong
	LaserOffset = 17*SpBm_Width/40;
	ButtonsX = ContXPos;
	ButtonsY = ContYPos;
	ButtonsHeight = ControlsHeight/3*2;
}

void CLTank::SetGameSize(Galactic3D::App* pApp, int size)
{
	switch (size)
	{
	case 1:
		pApp->SetSize(GDPIX(535),GDPIY(543));
		break;

	case 2:
		pApp->SetSize(GDPIX(669),GDPIY(679));
		break;

	case 3:
		pApp->SetSize(GDPIX(803),GDPIY(815));
		break;

	default:
		break;
	}
}

void CLTank::SavePBFile()
{
	/*Stream* Book;
	char temps[60];
	char SaveAuthor[31];

	// Get Hs.name
	//GetPrivateProfileString("DATA",psUser,"",temps,5,INIFile);
    //if (stricmp(temps,HS.name) != 0)
	//{
    //    strcpy(HS.name,temps);
    //    WritePrivateProfileString("DATA",psUser,HS.name,INIFile);
    //}

	if ( temps[0] != '\0' )
	{
		// Fill the name with spaces
		strcat( temps,"      ");
		temps[4] = '-';
		temps[5] = '\0';
	}
	// Add The Author's name
	strcat(temps,PBSRec.Author);
	// Limit the size
	temps[30] = '\0';


	// Save	PBSRec
    strcpy(SaveAuthor,PBSRec.Author);
	strcpy(PBSRec.Author,temps);

	// Write file
	//Book = CreateFile (PBFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);// Create New PlayBack File
	BytesMoved = Book->Write(&PBSRec,sizeof(PBSRec),1);				 // Save Header Info
	BytesMoved = Book->Write(RecBuffer,PBSRec.Size,1);					 // Data
	//SAFE_RELEASE(Book);

	// restore 	PBSRec
    strcpy(PBSRec.Author,SaveAuthor);*/
}

void CLTank::Dead(void)
{
	//AmbColour = GCOLOUR_RED;
	//Game_On = 0;
	Game.RecP = 0;
	RB_TOS = 0;
	MB_TOS = MB_SP = 0;						// we need to cancle all mouse inputs
	GameOn(0);
	if (VHSOn)
	{
		RB_TOS = Game.RecP;
		return;
	}
	SoundPlay(S_Die);
}

void CLTank::MouseOp(int xPos, int yPos)
{
	int i,j,x,y;
	
	if (EditorOn)
	{
		//SetFocus(Window);				// We need to get the focus off of the Edits
		//if (xPos > ContXPos)
		if (xPos > ContXPos && yPos > YOffset+(SpBm_Height*16)+16)
		{
			// we are on the selector window area
			y = (yPos - (ContYPos+5)) / (SpBm_Height + 4);
			x = (xPos - (ContXPos+5)) / (SpBm_Width + 4);
			i = x + (y*EditBMWidth);
			if ((i > MaxObjects+1) || (i < 0)) return;
			CurSelBM_L = i;
			//PutSelectors();
		}
		else {
			// we are in the Game window area - Edit Mode
			// JP: Fixed XOffset and YOffset being wrong way round
			y = ((yPos - YOffset) / SpBm_Height);
			x = ((xPos - XOffset) / SpBm_Width);
			if ((x<0) || (x>15) || (y<0) || (y>15)) return;
			if (AltOp) ChangeGO(x,y,GetNextBMArray[Game.PF[x][y]]); // Rotate Object
			else {
				if (Game.PF[x][y] != CurSelBM_L) ChangeGO(x,y,CurSelBM_L);
			}
			Modified = TRUE;
		}
		return;
	}
	else {
			// JP: Fixed XOffset and YOffset being wrong way round
			y = ((yPos - YOffset) / SpBm_Height);
			x = ((xPos - XOffset) / SpBm_Width);
			if ((x<0) || (x>15) || (y<0) || (y>15)) return;
			MBuffer[MB_TOS].X = x;
			MBuffer[MB_TOS].Y = y;
			MBuffer[MB_TOS].Z = AltOp^1;	// 1 = mouse clicked
			MB_TOS++;
			if (MB_TOS == MaxMBuffer) MB_TOS = 0;
	}
}

void CLTank::NewGame(void)
{
	int LastLevel;

	AmbColour = GCOLOUR_WHITE;

	LastLevel = CurLevel; CurLevel = 0;
	if (RLL)   // Remember Last Level
		{
		//GetPrivateProfileString("DATA",psRLLL,"1",temps,5,INIFile);
		//CurLevel = atoi(temps);
		CurLevel --;
	}
	if (!LoadNextLevel(TRUE,FALSE)) CurLevel = LastLevel;
}

void CLTank::LoadLevel(int Lvl)
{
	int LastLevel;
	
	AmbColour = GCOLOUR_WHITE;
	
	LastLevel = CurLevel; CurLevel = Lvl;
	if (RLL)   // Remember Last Level
	{
		//GetPrivateProfileString("DATA",psRLLL,"1",temps,5,INIFile);
		//CurLevel = atoi(temps);
		CurLevel --;
	}
	if (!LoadNextLevel(TRUE,FALSE)) CurLevel = LastLevel;
}

bool CLTank::GetOption(const GChar* psText, bool bDefault)
{
	bool bValue;
	if (GSettings->Read(GStr("OPT"),psText,&bValue))
	{
		return bValue;
	}
	GSettings->Write(GStr("OPT"),psText,bDefault); // fill in the default value
	return bDefault;
}

void CLTank::ReStart(void)
{
	if (CurLevel > 0)		// ReStart
	{
		if (UndoP > 0 ) UpdateUndo();		// Without this we loose the last move
		memcpy(Game.PF,CurRecData.PF,sizeof(TPLAYFIELD)); // Game.PF = CurRecData.PF;
		BuildBMField();
		GameOn(1);
		Game.RecP = 0;
		RB_TOS = 0;
		SlideO.s = 0;						// stop any sliding
		SlideMem.count = 0;					// MGY --- stop any sliding
		SlideT.s = 0;
		// Lets also init the Mouse Buffer
		MB_TOS = MB_SP = 0;
	}
}

GCOLOUR CLTank::SelectColour(GCOLOUR Colour)
{
	GCOLOUR O = m_Colour;
	m_Colour = Colour;
	return O;
}

void CLTank::Rectangle(Galactic3D::Renderers::TwoDimensional* pTwoD, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect)
{
	GCOLOUR ob = SelectColour(GCOLOUR_BLACK);
	DrawImage(pTwoD,-1,nLeftRect,nTopRect,nRightRect-nLeftRect,nBottomRect-nTopRect,0,0,0,0);
	SelectColour(ob);
	DrawImage(pTwoD,-1,nLeftRect+1,nTopRect+1,nRightRect-nLeftRect-2,nBottomRect-nTopRect-2,0,0,0,0);
}

void CLTank::DrawImage(Galactic3D::Renderers::TwoDimensional* pTwoD, int Image, int x, int y, int w, int h, int sx, int sy, int sw, int sh)
{
	pTwoD->GetRenderer()->SetTextureWrap(TEXTUREWRAP_CLAMP,TEXTUREWRAP_CLAMP);
	Image::Texture* pImage;
	if (Image == -1)
		pImage = NULL;
	else if (Image < 100)
		pImage = m_pImages[Image];
	else
		pImage = m_prgpSprites[Image-101];
	if (sw == 0 || sh == 0)
	{
//#ifdef GALACTIC_PLATFORM_IOS
//		pTwoD->Draw2DImage(pImage,Math::Vector2D((float)x,(float)y+m_RenderingYOffset),Math::Vector2D((float)w,(float)h),m_Colour,0.0f,Math::Vector2D(0.0f,0.0f),false,false,false);
//#else
		pTwoD->DrawRectangle(pImage,Math::Vector2D((float)x,(float)y+(float)m_RenderingYOffset),Math::Vector2D((float)w,(float)h),m_Colour,m_Colour,m_Colour,m_Colour);
//#endif
	}
	else
	{
//#ifdef GALACTIC_PLATFORM_IOS
		pTwoD->Draw2DImage(pImage,Math::Vector2D((float)x,(float)y+m_RenderingYOffset),Math::Vector2D((float)w,(float)h),m_Colour,0.0f,Math::Vector2D(0.0f,0.0f),false,false,false,Math::Vector2D(((float)sx)/(float)pImage->GetWidth(),((float)sy)/(float)pImage->GetHeight()),Math::Vector2D(((float)sw)/(float)pImage->GetWidth(),((float)sh)/(float)pImage->GetHeight()));
//#else
//		pTwoD->DrawRectangle(pImage,Math::Vector2D((float)x,(float)y+(float)m_RenderingYOffset),Math::Vector2D((float)w,(float)h),m_Colour,m_Colour,m_Colour,m_Colour,0.0f,Math::Vector2D(0.0f,0.0f),Math::Vector2D(((float)sx)/(float)pImage->GetWidth(),((float)sy)/(float)pImage->GetHeight()),Math::Vector2D(((float)sw)/(float)pImage->GetWidth(),((float)sh)/(float)pImage->GetHeight()));
//#endif
	}
}

void CLTank::LoadImages(Stream* pStream)
{
	HBF::RootNode Root;
	Root.Read(pStream);
	memset(m_pImages,0,sizeof(m_pImages));
	for (size_t i=0; i<Root.m_SubNodes; i++)
	{
		m_pImages[i] = Image::Texture::CreateFromNode(&Root.children[i],false);
	}
}

void CLTank::LoadSprites(Stream* pStream)
{
	HBF::RootNode Root;
	Root.Read(pStream);
	m_prgpSprites = new Image::Texture*[Root.m_SubNodes];
	m_SpritesCount = Root.m_SubNodes;
	for (size_t i=0; i<Root.m_SubNodes; i++)
	{
		m_prgpSprites[i] = Image::Texture::CreateFromNode(&Root.children[i],false);
	}
}

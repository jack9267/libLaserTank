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

/*

Obj			BitM	Description
---     	----    -----------

0 			1		Dirt
1 			2		Tank Up ( Primary )
-			3		Tank Right
-			4		Tank Down
-			5		Tank left
2			6		Base ( Primary)
-			7		Base Alt ( animation )
-			8		Base Alt2
3			9		Water ( Primary )
-			10		Water Alt
-			11		Water Alt 2
-       	12  	Blown Up Anti Tank (down)
4			13		Solid Block NOT movable
5			14		Movable Block
6			15		Bricks
7			16		Anti-Tank Gun Up
-			17		Anti-Tank Alt
-			18		Anti-Tank Alt 2
-			19	    Movable block after pushing into water
11			20		Mirror Up-Lt
12			21		Mirror Up-Rt
13			22		Mirror Dn-Rt
14			23		Mirror Dn-Lt
15			24		One Way Up (Primary)
-			25		One Way Up Alt
-			26		One Way Up Alt2
16			27		One Way Rt (Primary)
-			28		One Way Rt Alt
-			29		One Way Rt Alt2
17			30		One Way Dn (Primary)
-			31		One Way Dn Alt
-			32		One Way Dn Alt2
18			33		One Way Lt (Primary)
-			34		One Way Lt Alt
-			35		One Way Lt Alt2
8 	      	36      Anti-Tank facing right
- 	      	37
- 	      	38
9       	39      Anti-Tank facing down
-       	40
-       	41
10      	42      Anti-Tank facing left
-       	43
-       	44
19      	45      Crystal Block
-       	46      Crystal when hit by tank
20			47		Roto Mirror Up-Lt
21			48		Roto Mirror Up-Rt
22			49		Roto Mirror Dn-Rt
23			50		Roto Mirror Dn-Lt
-       	51      Crystal when hit by Anti-tank
-       	52      Blown Up Anti Tank (right)
-       	53      Blown Up Anti Tank (left)
-       	54      Blown Up Anti Tank (up)
24			56		Ice
25			57		Thin Ice
01dddddx	55		Worm Hole / Tunnel

*/

#ifndef __LTANK__H__
#define __LTANK__H__

// Language Section for Lasertank
// Import this from its own file

#include "lt32l_us.h"

// Game Defaults
#define LevelData       "LaserTank.lvl"    // Default Level Data File
#define INIFileName     "LaserTank.ini"
// MAX text size for a lang_text
#define MAX_LANG_SIZE    300



#define MaxObjects	    26              // Maximum number of game objects used
#define MaxBitMaps      58             	// the maximum number of BitMaps ( + 1 )
//#define XOffset         17             	// Game Board Offset from Left
//#define YOffset         17             	// Game Board Offset from top
#define ani_delay       4              	// animation Delay in GameDelay units
#define GameDelay       50             	// Main Operation Loop
#define WM_GameOver     WM_USER +1     	// Send Message if there are no more levels
#define WM_Dead         WM_USER +2     	// Send Message when you die
#define WM_NewHS	    WM_USER +3      // Send to Display New High Score Dialog
#define WM_SaveRec      WM_USER +4      // Send to Save Recording
#define App_Class		"LaserTC2"		// The Class Name
#define UndoBufStep		200				// Groth Amount of Undo Buffer
#define UndoMax			10000			// Max Amount of Undo Buffer
#define RecBufStep		10000			// Groth amount of Rec Buffer
#define RecMax			65500			// Max recording steps saved to file
#define SlowPBSet		5				// Delay of Slow Playback setting
#define MaxMBuffer		20				// Size of Mouse Buffer
#define Obj_Water		3				// Object Number 3 is water
#define Obj_Ice			24				// Ice Object
#define Obj_ThinIce		25				// Object Number of Thin Ice
#define Obj_Tunnel		0x40			// Object 01dddddX = Tunnel
#define LTG_ID			"LTG1"			// ID field of LTG file
#define psRLLOn			"RLL"          // Profile String for remember last level
#define psRLLN			"RLLFilename"  // remember last level file name
#define psRLLL			"RLLLevel"     // Level number
#define psAni			"Animation"
#define psSound			"Sound"
#define psSize			"Size"			// 1 = small, 3 = large
#define psXpos			"PosX"
#define psYpos			"PosY"
#define psUser 			"Player"
#define psPBA 			"Record Author"
#define psARec			"Auto_Record"
#define psSCL			"SkipComLev"
#define psDiff			"Diff_Setting"
#define psGM			"Graphics_Mode"
#define psGFN			"Graphics_File"
#define psGDN			"Graphics_Dir"
#define psDW			"DisableWarnings"
#define psYes			"Yes"
#define psNET			"NETWORK_INI"	// 1 = Use INI file from C:\Windows
#define BADMOVE 		256

typedef char TPLAYFIELD [16][16]; 		// Matrix of G.O. types

typedef struct tLevel               // Level Data from File
{
  TPLAYFIELD PF;               // Object Grid
  char LName[31];  			// Level Name
  char Hint[256];  			// Hint for this level
  char Author[31]; 			// the Author of the Level
  unsigned short SDiff; 				// Score Difficulty
}TLEVEL;

typedef struct tRecordRec		// Recording Header
{
	char LName[31];				// Level Name
	char Author[31];			// Author of the recording
	unsigned short Level;					// Level Number
	unsigned short Size;					// Size of Data -- Data to fallow
}TRECORDREC;

typedef struct tSearch			// Search Record used in Level Load
{
	int mode,					// 1=title, 2 = author
		SkipComp;				// True = Skip Completed
	unsigned short Diff;					// Difficulty charecter
	char data[61];				// Search String
}TSEARCH;

typedef struct tTankRec              // Store the Tank & Laser information
{
  int X, Y, Dir, Firing, Good;		// Good is used for Tunnel Wait in Game.Tank
}TTANKREC;

typedef struct tGameRec
{
  TPLAYFIELD	PF,               // Store Game Objects
  				PF2,              // Store Objects Under Stuff ( Ground, conveyor)
  				BMF,              // Bitmaps for Objects
  				BMF2;             // Bitmaps for Under Stuff ( Bridges )
  unsigned short 			ScoreMove,        // Move Counter
  				ScoreShot;        // Shot Counter
  unsigned int			RecP;			  // Recording Pointer
  TTANKREC		Tank;             // Tank Data
}TGAMEREC, *PGAMEREC;

typedef struct tXYRec			// Use in BMA
{
	int X,Y;					// X & Y Location in Big Butmap
}TXYREC;

typedef struct tXYZRec			// Use for Mouse Buffer
{
	int X,Y,Z;
}TXYZREC;

typedef struct tHSRec					// High Score Record
{
	unsigned short moves, shots;
	char name[6];						// Initials
}THSREC;

typedef struct tLTGRec			// Record for LTG graphics
{
	char Name[40];		// Name of Graphic Set
	char Author[30];	// Author of Graphics set
	char Info[245];		// 3 line Description of Graphics Set
	char ID[5];			// LTG ID = "LTG1"+0
	unsigned int MaskOffset;	// Offset from the biggining of file to Mask Bitmap
}TLTGREC;

typedef struct tIceRec	// Record used for sliding on the Ice
{
	int x,y,			// Last XY position of object to move
		dx,dy,			// Direction to move in Delta Cords
		s;				// True if Sliding

}TICEREC;

/// ------------ MGY -----------
#define MAX_TICEMEM 16
typedef struct tIceMem	// Record used for sliding objects on the Ice
{
	TICEREC Objects[MAX_TICEMEM];  // MGY - mem up 16 sliding objects
	int count;          // number of current sliding objects
}TICEMEM;
/// ------------ MGY -----------

#define	 	MaxSounds	20

typedef enum 	LT_Sound_Types {	S_Bricks = 1, S_Fire, S_Move, S_Head, S_Turn,
					S_EndLev, S_Die, S_Anti1, S_Anti2, S_Deflb,
					S_LaserHit, S_Push2, S_Push1, S_Rotate, S_Push3,
					S_Sink };

typedef enum 	LT_Control_Types {	LC_Up = 1, LC_Down, LC_Left, LC_Right, LC_Fire, LC_Undo };

enum eImages
{
	//IMAGE_CONTROL,
	//IMAGE_GAME,
	IMAGE_OPENING,
	IMAGE_OPENINGPC,
	IMAGE_COUNT,
};

class CLTank
{
public:
	CLTank(void);
	virtual ~CLTank(void);

private:
	GCOLOUR m_Colour;

public:
	GCOLOUR AmbColour;

	static const int GetNextBMArray[MaxObjects+1];
	static const int OpeningBMA[16];
	int EditorOn;		// true when in editor mode
	int QHELP;		// True when Quick Help is On
	int FileHand,RB_TOS,PBHold,VHSOn;
	TPLAYFIELD ShiftPF,ShiftBMF;
	char PrintJobName[100];
	char HelpFile[GPathMax];

	int Rendering;      // do any rendering
	int GFXError;            // error used for load
	int GFXOn;		// True when Graphics are loaded
	int TankDirty;		// if true then we need to repaint the tank
	int NoLevel;         // if true Main Paint will show Openning
	int Game_On;        // true when game is running
	int Ani_On;         // true when Animation is On
	int RLL;         // remember last level
	int ConvMoving;        // true when moving on the conveyor belts
	int OKtoHS;			// true if OK to Set HighScore
	int OKtoSave;		// true if OK to Set HighScore
	int Recording;		// true if Recording
	int PlayBack;		// true if PlayBack is recording
	int PBOpen;		// true when Playback window is open
	int ARecord;		// AutoRecord is On/Off
	int SkipCL;		// true if Skip Complete Level is on
	int DWarn;		// Disable Warning
	int CurLevel;            // Used to Figure out the Current Level
	int AniLevel;            // Used for Animation Position
	int AniCount;            // counter for animation
	int CurSelBM_L;            // current selected bm in editor
	int CurSelBM_R;            // current selected bm in editor
	int SpBm_Width;			// Width of Sprite
	int SpBm_Height;			// Height of Sprite
	int LaserOffset;			// Offset of Laser Size
	int ContXPos;			// Position of Control Side
	int ContYPos;			// Position of Control Side
	int EditBMWidth;			// # of bitmaps across edit select area
	int Speed;			// Playback speed
	int SlowPB;
	int RecBufSize;		// Size of recording buffer
	int UndoBufSize;			// Size of Undo Buffer ( * sizeof(TGAMEREC))
	int Difficulty;			// Difficulty Enable ( use Bits )
	int GraphM;			// Graphics Mode 0=int; 1=ext; 2=ltg
	int FindTank;		// True when First starting a level
	int BlackHole;		// True if we TunnleTranslae to a Black Hole
	int AltOp;         // Alternate operation

	int XOffset;             	// Game Board Offset from Left
	int YOffset;             	// Game Board Offset from top
	
	int HeightOffset;//minus from game height
	
	int ControlsX;
	int ControlsY;
	int ControlsWidth;
	int ControlsHeight;

	int ButtonsX;
	int ButtonsY;
	int ButtonsWidth;
	int ButtonsHeight;

	int LastSFWord;
	int SFXError;
	int Sound_On;				// true when sound is on

	TGAMEREC Game,SaveGame;		  // The Level Data
	TLEVEL CurRecData;
	GCOLOUR LaserColor,LaserColorR, LaserColorG;
	GChar FileName[GPathMax], HFileName[GPathMax], GHFileName[GPathMax];
	GChar PBFileName[GPathMax], GraphFN[GPathMax], GraphDN[GPathMax],INIFile[GPathMax];
	int Modified;
	TICEREC SlideO,SlideT;
	TICEMEM SlideMem; // MGY - mem up MAX_TICEMEM sliding objects
	int wasIce;				// CheckLoc will set this to true if Ice
	int WaitToTrans;

	TXYREC BMA[MaxBitMaps+1]; 		// Bit Map Array
	TTANKREC laser;
	static const int GetOBMArray[MaxObjects+1];
	static const int CheckArray[MaxObjects];
	// Pad the beggining with junk array was [1..MaxBitMaps]
	static const int BMSTA[MaxBitMaps+1];
	static const GCOLOUR ColorList[8];

	int UndoP;
	TRECORDREC PBRec, PBSRec;
	char *RecBuffer;
	PGAMEREC UndoBuffer;
	THSREC HS;
	size_t BytesMoved;
	TXYZREC MBuffer[MaxMBuffer];
	int MB_TOS, MB_SP;			// TOS = Top of Stack ; SP = Stack Pointer
	int findmap[16][16];

	int Backspace[10];			// Backspace Buffer
	int BS_SP;			// StackPointer for Backspace
	int UndoRollOver;

	int LaserBounceOnIce;

	int m_RenderingYOffset;

	Galactic3D::Image::Texture* m_pImages[IMAGE_COUNT];
	size_t m_SpritesCount;
	Galactic3D::Image::Texture** m_prgpSprites;
	//Galactic3D::Renderers::TwoDimensional* m_pTwoD;

	void Initialise(void);
	void ShutDown(void);
	void Process(void);
	void Render(Galactic3D::Renderer* pRenderer, Galactic3D::Renderers::TwoDimensional* pTwoD);

	virtual void SoundPlay ( int );
	virtual void SFxInit( void);

	void Mem_to_SlideO( int iSlideObj );
	void SlideO_to_Mem( int iSlideObj );
	void add_SlideO_to_Mem();
	void sub_SlideO_from_Mem( int iSlideObj );
	void del_SlideO_from_Mem( int x, int y );
	int TestIfConvCanMoveTank();
	void SetButtons( int ButtonX);
	void FileError();
	void AddKBuff( char zz);
	void FindTarget(int px, int py, int pathlen);
	int MouseOperation (int sp);
	void InitBuffers();
	void ResetUndoBuffer();
	void KillBuffers();
	void UpdateUndo();
	void UndoStep();
	void DrawSprite(Galactic3D::Renderers::TwoDimensional* pTwoD, char bmn, int x,int y);
	void PutSprite(Galactic3D::Renderers::TwoDimensional* pTwoD, char bmn, int x,int y);
	void PutSprite(char bmn, int x,int y);
	void UpDateSprite(Galactic3D::Renderers::TwoDimensional* pTwoD, int x, int y);
	void UpDateSprite(int x, int y);
	void UpDateTank(Galactic3D::Renderers::TwoDimensional* pTwoD);
	void UpDateTank(void);
	void UpDateLaser(Galactic3D::Renderers::TwoDimensional* pTwoD);
	void UpDateLaser(void);
	void UpDateLaserBounce(int a, int b);
	void PutLevel(Galactic3D::Renderers::TwoDimensional* pTwoD);
	//int LoadLTG( HDC dc, HBITMAP *GH, HBITMAP *MH, char *FN);
	virtual void GFXInit();									// Load All the Bitmaps
	void GFXKill(void);								// Release All the Bitmaps
	void ChangeGO(int x, int y, int CurSelBM);
	void BuildBMField();
	void GameOn(int b);
	void JK3dFrame(int x, int y, int x2, int y2, int up);
	void JKSelFrame(int x, int y, int x2, int y2, int pnum);
	char GetOBM ( char ob );
	void LoadLastLevel();
	bool LoadNextLevel(int DirectLoad, int Scanning);
	void AssignHSFile();
	void CheckHighScore();
	void Animate();
	void TranslateTunnel( int *x, int *y );
	void ConvMoveTank(int x, int y, int check);
	void UpDateTankPos(int x, int y);
	void MoveTank(int d);
	int CheckLoc(int x, int y);
	void MoveObj(int x, int y, int dx, int dy, int sf);
	void IceMoveT();
	void IceMoveO();
	void KillAtank( int x, int y, char bm);
	int CheckLLoc(int x, int y, int dx, int dy);
	void MoveLaser();
	void FireLaser(int x, int y, int d, int sf);
	void AntiTank();
	void PutSelectors(Galactic3D::Renderers::TwoDimensional* pTwoD);
	void ShowTunnelID();
	virtual void SetGameSize(int nWidth, int nHeight, int StatusBarHeight);
	void SetGameSize(Galactic3D::App* pApp, int size);
	void SavePBFile();
	virtual void Dead(void);
	void MouseOp(int xPos, int yPos);
	void NewGame(void);
	void LoadLevel(int Lvl);
	bool GetOption(const GChar* psText, bool bDefault);
	void ReStart(void);

	GCOLOUR SelectColour(GCOLOUR Colour);
	void Rectangle(Galactic3D::Renderers::TwoDimensional* pTwoD, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect);

	void DrawImage(Galactic3D::Renderers::TwoDimensional* pTwoD, int Image, int x, int y, int w, int h, int sx, int sy, int sw, int sh);

	void LoadImages(Galactic3D::Stream* pStream);
	void LoadSprites(Galactic3D::Stream* pStream);
};

/* Lets try some Macros */
#define GetTunnelID(x,y) ((Game.PF[x][y] & 0x0F)  >> 1)			// 0 - 7

// mgy 18-05-2003 Tunnel is limited to 0-7.
#define GetTunnelOldID(x,y) ((Game.PF[x][y] & 0x0F)  >> 1)			// 0 - 7
//#define GetTunnelOldID(x,y) ((Game.PF[x][y] & 0x3F)  >> 1)		// 0 - 32

#define ISTunnel(x,y) ((Game.PF[x][y] & Obj_Tunnel) == Obj_Tunnel)
#define GameInProg Game.RecP && (Game.PF[Game.Tank.X][Game.Tank.Y] != 2) && (!DWarn)

#endif

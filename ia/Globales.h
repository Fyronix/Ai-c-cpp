/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                          MAIN                                               //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

void ReInitializeProgram( void );

HWND Hwnd = NULL,
	 HwndEditCreate = NULL,
	 HwndCom = NULL,
	 HwndTraining = NULL; 

LONGLONG LastClock,
		 NewClock,
		 Frequency,
		 TrainingClock;

static HINSTANCE g_hInst = NULL; 

int MaxIterations = 1000, 
	TotalIterations = 0,
	Modified = 0;

float ErrorPercentage = 0.5,
	  LastTrainingTime = 0, 
	  TotalTrainingTime = 0;

double MSecPerClock,
	   TrainingTime;

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                           Other                                             //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

POINT LastPos = { -1, -1 }, 
	  NewPos = { 0, 0 };

POINT MousePos( void );

void SetWindowTextToInteger( HWND, char* ),
	 SetWindowTextToReal( HWND, char*, int ),
	 ReInitializeProgram( void );

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                        Listview                                             //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

void AddColumn( HWND, int, char*, int ),
	 AddItem( HWND, char*, int );

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                       Requester                                             //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

char FileNam[ 1000 ] = "";

void SaveFile( void );

bool OpenFile( void ),
	 SaveFileAs( void );

char *FileToOpen( char*, char*, char* ),
	 *FileToSave( char*, char*, char* );

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                          Draw                                               //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

HWND HwndImgInit = NULL,
	 HwndImgReco = NULL;

WNDPROC ImgInitWndOrigProc,
		ImgRecoWndOrigProc;

int Zoom = 22;

struct ImageInfo
{
	HBITMAP H;
	int Width,
		Height;
	LPVOID DibBits;
	BITMAPINFO BMI;
};
struct ImageInfo Image[ 3 ];

HPEN HPen[ 2 ] = { NULL, NULL };

void NewBitmap( HWND, int, int, int ),
	 DrawBitmap( HWND, int ),
	 NewPen( int, int, COLORREF ),
	 PenDraw( int, int Mode = 0 );

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                          Neuron                                             //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

#define EntryNumber 35

int *EntriesState = NULL, 
	RecognizedNeuron = 0,
	ComparedStates[ EntryNumber ];

char *NeuronName = NULL;

unsigned char NbCar = 0;

double *Weights = NULL, 
	   *Sums = NULL, 
	   *Output = NULL, 
	   Cor = 0.1;

void GetStates( void ),
	 SetWeights( int ),
	 Correct( int, int ),
	 Correction( int ),
	 RealTimeRecognition( void ),
	 ShowRecognized( int );

bool NeuronNameExist( char[] ),
	 NeuronEntriesExist( void );
int RecognizeNeuron( void );

float GetErrorPercent( int* );

double Sum( int );

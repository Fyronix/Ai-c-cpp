#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "Globales.h"
#include "Other Functions.h"
#include "Listview Functions.h"
#include "Requester Functions.h"
#include "Draw Functions.h"
#include "Neuron Functions.h"

LRESULT CALLBACK MainProc( HWND HTDlg, UINT Msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ImgInitWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ImgRecoWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam );

int WINAPI WinMain( HINSTANCE HInst, HINSTANCE PrevhInst, LPSTR lpszArgument, int nFunsterStil )
{
	DialogBox( HInst, "IDD", NULL, (DLGPROC) MainProc );
	return 0;
}
LRESULT CALLBACK MainProc( HWND HTDlg, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	HWND HwndComEdit = NULL;
	HDC HDCBuffer = NULL, HDCImgInit = NULL, HDCImgReco = NULL;
	int i = 0, j = 0, NeuronToCorrect = 0, IterationsRealized = 0, NbCarRecognized = 0, MBReturnValue = 0;
	float NewCorrectionTempValue = 0, NewErrorPercentageTempValue = 0, CurrentErrorPercentage = 1, CurrentTrainingTime = 0;
	char NewNeuronName[ 16 ] = "", NewMaxIterationValue[ 10 ] = "", NewCorrectionValue[ 5 ] = "", NewErrorPercentageValue[ 5 ] = "", NeuronToCorrectName[ 16 ] = "", NewIterationValue[ 10 ] = "", NewRecognizedSymbolesValue[ 4 ] = "", NewCreatedSymbolesValue[ 4 ] = "", NewTrainingTime[ 15 ] = "", NewTotalTrainingTime[ 15 ] = "";

	switch( Msg ) 
	{
		case WM_INITDIALOG:
			InitCommonControls();

			QueryPerformanceFrequency( (LARGE_INTEGER*) &Frequency );
			MSecPerClock = 1000.0 / (double) Frequency;

			Hwnd = HTDlg;
			HwndCom = GetDlgItem( HTDlg, ComSymbolToCorrect );
			HwndImgInit = GetDlgItem( HTDlg, ImgInitialSymbol );
			HwndImgReco = GetDlgItem( HTDlg, ImgRecognizedSymbol );
			HwndTraining = GetDlgItem( HTDlg, ListTrainingDetail  );
			
			ImgInitWndOrigProc = (WNDPROC) SetWindowLong( HwndImgInit, GWL_WNDPROC, (LONG) ImgInitWndProc );
			ImgRecoWndOrigProc = (WNDPROC) SetWindowLong( HwndImgReco, GWL_WNDPROC, (LONG) ImgRecoWndProc );

			SendMessage( GetDlgItem( HTDlg, EditMaxIterations  ), EM_SETLIMITTEXT, (WPARAM)9, (LPARAM)NULL );	
			SendMessage( GetDlgItem( HTDlg, EditCorrectionValue  ), EM_SETLIMITTEXT, (WPARAM)4, (LPARAM)NULL );
			SendMessage( GetDlgItem( HTDlg, EditErrorPercentage  ), EM_SETLIMITTEXT, (WPARAM)4, (LPARAM)NULL );
			SendMessage( GetDlgItem( HTDlg, EditSymbolToCreate ), EM_SETLIMITTEXT, (WPARAM)15, (LPARAM)NULL );

			AddColumn( HwndTraining, 0, "Caractéristique", 185 );
			AddColumn( HwndTraining, 1, "Valeur", 186 );

			AddItem( HwndTraining, "itérations",0);
			AddItem( HwndTraining, "itérations total",1);
			AddItem( HwndTraining, "temps d'apprentissage",2);
			AddItem( HwndTraining, "temps d'apprentissage total",3);
			AddItem( HwndTraining, "Symboles reconnus",4);
			AddItem( HwndTraining, "Symboles créés",5);
			AddItem( HwndTraining, "pourcentage d'erreurs",6);

			ListView_SetExtendedListViewStyleEx( HwndTraining, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );

			ReInitializeProgram();
		break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			LastPos = MousePos();
		break;

		case WM_MOUSEMOVE:
			if( wParam == MK_LBUTTON )
				LastPos = MousePos();

			if( wParam == MK_RBUTTON )
				LastPos = MousePos();
		break;

		case WM_PAINT:
			DrawBitmap( HwndImgInit, 1 );
			DrawBitmap( HwndImgReco, 2 );
		break;

		case WM_COMMAND:
			if( LOWORD( wParam ) == ButCreateSymbol && HIWORD( wParam ) == BN_CLICKED )
			{
				GetWindowText( GetDlgItem( HTDlg, EditSymbolToCreate ) , NewNeuronName, 16 );
				if( NewNeuronName[ 0 ] != '\0' )
				{
					if( NeuronNameExist( NewNeuronName ) == 0 )
					{
						GetStates();
						if( NeuronEntriesExist() == 0 )
						{
							Modified = 1;
							SendMessage( HwndCom, CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPCTSTR)NewNeuronName );

							EntriesState = (int*) realloc( EntriesState, sizeof( int ) * ( NbCar + 1 ) * EntryNumber );
							Weights = (double*) realloc( Weights, sizeof( double ) * ( NbCar + 1 ) * EntryNumber );
							Sums = (double*) realloc( Sums, sizeof( double ) * ( NbCar + 1 ) );
							Output = (double*) realloc( Output, sizeof( double ) * ( NbCar + 1 ) );
							NeuronName = (char*) realloc( NeuronName, sizeof( char ) * ( NbCar + 1 ) * 16 );

							Sums[ NbCar ] = 0;

							for( i = 0; i < 16; i++ )
								NeuronName[ NbCar * 16 + i ] = NewNeuronName[ i ];

							for( i = 0; i < EntryNumber; i++ )
								EntriesState[ NbCar * EntryNumber + i ] = ComparedStates[ i ];

							SetWeights( NbCar );
							NbCar++;

							itoa( NbCar, NewCreatedSymbolesValue, 10 );
							ListView_SetItemText( HwndTraining, 5, 1, NewCreatedSymbolesValue );

							RealTimeRecognition();
						}
						else
						{
							MessageBox( HTDlg, "Forme deja enregistré", "Reconnaissance des formes", 0 );
						}
					}
					else
					{
						MessageBox( HTDlg, "Nom de symbole deja utilisé", "Reconnaissance des formes", 0 );
					}
				}
				else
				{
					MessageBox( HTDlg, "Aucun nom de symbole rentrer", "Reconnaissance des formes", 0 );
				}
			}
			if( LOWORD( wParam ) == ButCorrectSymbol && HIWORD( wParam ) == BN_CLICKED )
			{
				if( SendMessage( HwndCom, CB_GETCOUNT, (WPARAM)0, (LPARAM)0 ) > 1 )
				{
					NeuronToCorrect = SendMessage( HwndCom, CB_GETCURSEL, (WPARAM)0, (LPARAM)0 );
					if( NeuronToCorrect != CB_ERR )
					{
						Correction( NeuronToCorrect );
					}
					else
					{
						HwndComEdit = GetWindow( HwndCom, GW_CHILD );
						GetWindowText( HwndComEdit, NeuronToCorrectName, 16 );
						if( NeuronToCorrectName[ 0 ] != '\0' )
						{
							NeuronToCorrect = SendMessage( HwndCom, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPCSTR)NeuronToCorrectName );

							if( NeuronToCorrect != CB_ERR )
								Correction( NeuronToCorrect );
							else
								MessageBox( Hwnd, "Symbole inconnu", "Reconnaissance des formes", 0 );
						}
						else
						{
							MessageBox( HTDlg, "Aucun symbole a corriger", "Reconnaissance des formes", 0 );
						}
					}
				}
				else
				{
					MessageBox( HTDlg, "Aucun symbole a corriger", "Reconnaissance des formes", 0 );
				}
			}
			if( LOWORD( wParam ) == ButAutoLearn && HIWORD( wParam ) == BN_CLICKED )
			{
				if( NbCar > 1 )
				{
					CurrentErrorPercentage = GetErrorPercent( &NbCarRecognized );
					QueryPerformanceCounter( ( LARGE_INTEGER* ) &LastClock );
					while( IterationsRealized < MaxIterations && CurrentErrorPercentage > ErrorPercentage )
					{
						NbCarRecognized = 0;
						if( CurrentErrorPercentage > ErrorPercentage )
						{
							for( j = 0; j < NbCar; j++ )
							{
								for( i = 0; i < EntryNumber; i++ )
									ComparedStates[ i ] = EntriesState[ j * EntryNumber + i ];

								for( i = 0; i < NbCar; i++ )
									Output[ i ] = Sum( i );

								RecognizedNeuron = RecognizeNeuron();

								for( i = 0; i < NbCar; i++ )
									Sums[ i ] = 0;

								if( RecognizedNeuron != j )
								{
									Correct( j, 1 );

									IterationsRealized++;
									TotalIterations++;

									QueryPerformanceCounter( ( LARGE_INTEGER* ) &NewClock );
									TrainingClock = NewClock - LastClock;
									TrainingTime  = TrainingClock * MSecPerClock;
									CurrentTrainingTime = (float) TrainingTime;
									TotalTrainingTime = LastTrainingTime + (float) TrainingTime;

									sprintf( NewIterationValue, "%d", IterationsRealized );
									sprintf( NewMaxIterationValue, "%d", TotalIterations );
									sprintf( NewTrainingTime, "%.2f", CurrentTrainingTime );
									sprintf( NewTotalTrainingTime, "%.2f", TotalTrainingTime );

									ListView_SetItemText( HwndTraining, 0, 1, NewIterationValue );
									ListView_SetItemText( HwndTraining, 1, 1, NewMaxIterationValue );
									ListView_SetItemText( HwndTraining, 2, 1, NewTrainingTime );
									ListView_SetItemText( HwndTraining, 3, 1, NewTotalTrainingTime );

									if( IterationsRealized >= MaxIterations )
										break;
								}
								else
								{
									NbCarRecognized++;

									QueryPerformanceCounter( ( LARGE_INTEGER* ) &NewClock );
									TrainingClock = NewClock - LastClock;
									TrainingTime  = TrainingClock * MSecPerClock;
									CurrentTrainingTime = (float) TrainingTime;
									TotalTrainingTime = LastTrainingTime + (float) TrainingTime;

									sprintf( NewTrainingTime, "%.2f", CurrentTrainingTime );
									sprintf( NewTotalTrainingTime, "%.2f", TotalTrainingTime );

									ListView_SetItemText( HwndTraining, 2, 1, NewTrainingTime );
									ListView_SetItemText( HwndTraining, 3, 1, NewTotalTrainingTime );
								}
							}
							CurrentErrorPercentage = 1 - ( (float)NbCarRecognized / (float)NbCar );
						}
						if( CurrentErrorPercentage <= ErrorPercentage )
							CurrentErrorPercentage = GetErrorPercent( &NbCarRecognized );
					}
					if( IterationsRealized != 0 && NbCarRecognized != 0 )
					{
						CurrentErrorPercentage = GetErrorPercent( &NbCarRecognized );

						sprintf( NewRecognizedSymbolesValue, "%d", NbCarRecognized );
						sprintf( NewErrorPercentageValue, "%.2f", CurrentErrorPercentage );

						ListView_SetItemText( HwndTraining, 4, 1, NewRecognizedSymbolesValue );
						ListView_SetItemText( HwndTraining, 6, 1, NewErrorPercentageValue );
					}
					if( IterationsRealized == 0 )
					{
						sprintf( NewRecognizedSymbolesValue, "%d", NbCarRecognized );
						sprintf( NewErrorPercentageValue, "%.2f", CurrentErrorPercentage );

						ListView_SetItemText( HwndTraining, 0, 1, "0" );
						ListView_SetItemText( HwndTraining, 2, 1, "0.0" );
						ListView_SetItemText( HwndTraining, 4, 1, NewRecognizedSymbolesValue );
						ListView_SetItemText( HwndTraining, 6, 1, NewErrorPercentageValue );
					}
					else
					{
						Modified = 1;
					}
					if( NbCarRecognized == 0 )
					{
						ListView_SetItemText( HwndTraining, 4, 1, "0" );
						ListView_SetItemText( HwndTraining, 6, 1, "1.0" );
					}
					LastTrainingTime = TotalTrainingTime;

					RealTimeRecognition();
				}
				else
				{
					MessageBox( HTDlg, "Il faut au minimum deux symboles créés \npour utiliser l'apprentissage automatique", "Reconnaissance des formes", 0 );
				}
			}
			if( ( LOWORD( wParam ) == EditMaxIterations ) && ( HIWORD( wParam ) == EN_KILLFOCUS ) )
			{
				GetWindowText( GetDlgItem( HTDlg, EditMaxIterations ), NewMaxIterationValue, 10 );
				SetWindowTextToInteger( GetDlgItem( HTDlg, EditMaxIterations ), NewMaxIterationValue );
				GetWindowText( GetDlgItem( HTDlg, EditMaxIterations ), NewMaxIterationValue, 10 );

				if( NewMaxIterationValue[ 0 ] == '\0' )
				{
					sprintf( NewMaxIterationValue, "%d", MaxIterations );
					SetWindowText( GetDlgItem( HTDlg, EditMaxIterations ), NewMaxIterationValue );
				}
				else
				{
					Modified = 1;
					sscanf( NewMaxIterationValue, "%d", &MaxIterations );
				}
			}
			if( ( LOWORD( wParam ) == EditCorrectionValue ) && ( HIWORD( wParam ) == EN_KILLFOCUS ) )
			{
				GetWindowText( GetDlgItem( HTDlg, EditCorrectionValue ), NewCorrectionValue, 5 );
				SetWindowTextToReal( GetDlgItem( HTDlg, EditCorrectionValue ), NewCorrectionValue, 4 );
				GetWindowText( GetDlgItem( HTDlg, EditCorrectionValue ), NewCorrectionValue, 5 );

				if( NewCorrectionValue[ 0 ] == '\0' )
				{
					sprintf( NewCorrectionValue, "%g", Cor );
					SetWindowText( GetDlgItem( HTDlg, EditCorrectionValue ), NewCorrectionValue );
				}
				else
				{
					sscanf( NewCorrectionValue, "%f", &NewCorrectionTempValue );

					if( NewCorrectionTempValue < 0.0 || NewCorrectionTempValue > 3.0 )
					{
						MessageBox( HTDlg, "Entrez une valeur comprise entre 0 et 3", "Reconnaissance des formes", 0 );
					}
					else
					{
						Modified = 1;
						Cor = (double)NewCorrectionTempValue;
					}
				}
			}
			if( ( LOWORD( wParam ) == EditErrorPercentage ) && ( HIWORD( wParam ) == EN_KILLFOCUS ) )
			{
				GetWindowText( GetDlgItem( HTDlg, EditErrorPercentage ), NewErrorPercentageValue, 5 );
				SetWindowTextToReal( GetDlgItem( HTDlg, EditErrorPercentage ), NewErrorPercentageValue, 4 );
				GetWindowText( GetDlgItem( HTDlg, EditErrorPercentage ), NewErrorPercentageValue, 5 );

				if( NewErrorPercentageValue[ 0 ] == '\0' )
				{
					sprintf( NewErrorPercentageValue, "%g", ErrorPercentage );
					SetWindowText( GetDlgItem( HTDlg, EditErrorPercentage ), NewErrorPercentageValue );
				}
				else
				{
					sscanf( NewErrorPercentageValue, "%f", &NewErrorPercentageTempValue );

					if( NewErrorPercentageTempValue < 0.0 || NewErrorPercentageTempValue > 1.0 )
					{
						MessageBox( HTDlg, "Entrez une valeur comprise entre 0 et 1", "Reconnaissance des formes", 0 );
					}
					else
					{
						Modified = 1;
						ErrorPercentage = NewErrorPercentageTempValue;
					}
				}
			}
			if( HIWORD( wParam ) == 0 && lParam == NULL )
			{
				switch( LOWORD( wParam ) )
				{
					case NewNetwork:
						if( Modified == 1 )
						{
							MBReturnValue = MessageBox( Hwnd, "Enregistrer les modifications apportées au réseau?", "Reconnaissance des formes", MB_YESNOCANCEL | MB_ICONWARNING );
							if( MBReturnValue == IDYES )
							{
								if( SaveFileAs() == 1 )
									Modified = 0;
							}
							if( MBReturnValue == IDNO )
								Modified = 0;
						}
						if( Modified == 0 )
							ReInitializeProgram();
					break;

					case OpenNetwork:
						if( Modified == 1 )
						{
							MBReturnValue = MessageBox( Hwnd, "Enregistrer les modifications apportées au réseau?", "Reconnaissance des formes", MB_YESNOCANCEL | MB_ICONWARNING );
							if( MBReturnValue == IDYES )
							{
								if( SaveFileAs() == 1 )
									Modified = 0;
							}
							if( MBReturnValue == IDNO )
								Modified = 0;
						}
						if( Modified == 0 )
							OpenFile();
					break;

					case SaveNetwork:
						if( FileNam[ 0 ] != '\0' )
						{
							SaveFile();
							Modified = 0;
						}
						else
						{
							if( SaveFileAs() == 1 )
								Modified = 0;
						}
					break;

					case SaveNetworkAs:
						if( SaveFileAs() == 1 )
							Modified = 0;
					break;

					case Quit:
						if( Modified == 1 )
						{
							MBReturnValue = MessageBox( Hwnd, "Enregistrer les modifications apportées au réseau?", "Reconnaissance des formes", MB_YESNOCANCEL | MB_ICONWARNING );
							if( MBReturnValue == IDYES )
							{
								if( FileNam[ 0 ] != '\0' )
								{
									SaveFile();
									Modified = 0;
								}
								else
								{
									if( SaveFileAs() == 1 )
										Modified = 0;
								}
							}
							if( MBReturnValue == IDNO )
								Modified = 0;
						}
						if( Modified == 0 )
							EndDialog( HTDlg, (WPARAM)0 );
					break;
				}
			}
		break;

		case WM_CLOSE:
			if( Modified == 1 )
			{
				MBReturnValue = MessageBox( Hwnd, "Enregistrer les modifications apportées au réseau?", "Reconnaissance des formes", MB_YESNOCANCEL | MB_ICONWARNING );
				if( MBReturnValue == IDYES )
				{
					if( FileNam[ 0 ] != '\0' )
					{
						SaveFile();
						Modified = 0;
					}
					else
					{
						if( SaveFileAs() == 1 )
							Modified = 0;
					}
				}
				if( MBReturnValue == IDNO )
					Modified = 0;
			}
			if( Modified == 0 )
				EndDialog( HTDlg, (WPARAM)0 );
		break;

		case WM_DESTROY:
			SetWindowLong( HwndImgInit, GWL_WNDPROC, (LONG) ImgInitWndOrigProc );
			SetWindowLong( HwndImgReco, GWL_WNDPROC, (LONG) ImgRecoWndOrigProc );

			DeleteObject( Image[ 1 ].H );
			DeleteObject( Image[ 2 ].H );
			DeleteObject( HPen[ 0 ] );
			DeleteObject( HPen[ 1 ] );
			Image[ 1 ].H = NULL;
			Image[ 2 ].H = NULL;
			HPen[ 0 ] = NULL;
			HPen[ 1 ] = NULL;

			free( EntriesState );
			free( Weights );
			free( Sums );
			free( Output );
			free( NeuronName );
			EntriesState = NULL;
			Weights = NULL;
			Sums = NULL;
			Output = NULL;
			NeuronName = NULL;

			PostQuitMessage( 0 );
		break;
	}
	return 0;
}
LRESULT CALLBACK ImgInitWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	HDC HDCBuffer = NULL, HDCImgInit = NULL;

	switch( Message )
	{
		case WM_LBUTTONDOWN:
			PenDraw( 0 );
			RealTimeRecognition();
		break;

		case WM_RBUTTONDOWN:
			PenDraw( 1 );
			RealTimeRecognition();
		break;

		case WM_MOUSEMOVE:
			if( wParam == MK_LBUTTON )
			{
				PenDraw( 0, 1 );
				RealTimeRecognition();
			}
			if( wParam == MK_RBUTTON )
			{
				PenDraw( 1, 1 );
				RealTimeRecognition();
			}
		break;

		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_NCMOUSEMOVE:
			LastPos = MousePos();
		break;

		case WM_LBUTTONUP:
		case WM_NCLBUTTONUP:
		case WM_RBUTTONUP:
		case WM_NCRBUTTONUP:
			ReleaseCapture();
		break;

		default:
			return DefWindowProc( hwnd, Message, wParam, lParam );
	}
	return 0;
}
LRESULT CALLBACK ImgRecoWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch( Message )
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			LastPos = MousePos();
		break;

		case WM_MOUSEMOVE:
			if( wParam == MK_LBUTTON )
				LastPos = MousePos();

			if( wParam == MK_RBUTTON )
				LastPos = MousePos();
		break;

		default:
			return DefWindowProc( hwnd, Message, wParam, lParam );
	}
	return 0;
}

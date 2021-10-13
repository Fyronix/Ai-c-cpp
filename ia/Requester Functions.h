char* FileToOpen( char* Title, char* Filter, char* FileName )
{
	unsigned int i = 0;
	char *Filters = NULL;

	Filters = (char*) malloc( strlen( Filter ) + 2 );

	for( i = 0; i < strlen( Filter ); i++ )
	{
		if( Filter[ i ] == '|' )
			Filters[ i ] = '\0';
		else
			Filters[ i ] = Filter[ i ];
	}
	Filters[ strlen( Filter ) ] = '\0';
	Filters[ strlen( Filter ) + 1 ] = '\0';

	OPENFILENAME OpenFile;
	OpenFile.lStructSize = sizeof( OPENFILENAME );
	OpenFile.hwndOwner = Hwnd;
	OpenFile.hInstance = g_hInst;
	OpenFile.lpstrFilter = Filters;
	OpenFile.lpstrCustomFilter = NULL;
	OpenFile.nMaxCustFilter = NULL;
	OpenFile.lpstrFile = FileName;
	OpenFile.nMaxFile = 1000;
	OpenFile.lpstrFileTitle = NULL;
	OpenFile.lpstrInitialDir = NULL;
	OpenFile.lpstrTitle = Title;
	OpenFile.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON | OFN_HIDEREADONLY;
	OpenFile.lpstrDefExt = "rdn";

	if( GetOpenFileName( &OpenFile ) != 0 )
	{
		if( ( FileName[ strlen( FileName ) - 3 ] == 'R' && FileName[ strlen( FileName ) - 2 ] == 'D' && FileName[ strlen( FileName ) - 1 ] == 'N' ) == 1 || ( FileName[ strlen( FileName ) - 3 ] == 'r' && FileName[ strlen( FileName ) - 2 ] == 'd' && FileName[ strlen( FileName ) - 1 ] == 'n' ) == 1 )
		{
			if( FileName[ strlen( FileName ) - 3 ] == 'R' )
			{
				FileName[ strlen( FileName ) - 3 ] = 'r';
				FileName[ strlen( FileName ) - 2 ] = 'd';
				FileName[ strlen( FileName ) - 1 ] = 'n';
			}
			strcpy( FileNam, FileName );
			return FileName;
		}
		else
		{
			MessageBox(Hwnd, "Fichier invalide ou format inconnu du logiciel", "Reconnaissance des formes", 0 );
			FileNam[ 0 ] = '\0';
			return "";
		}
	}
	else
	{
		return "";
	}
}
char* FileToSave( char* Title, char* Filter, char* FileName )
{
	unsigned int i = 0;
	char *Filters = NULL;

	Filters = (char*) malloc( strlen( Filter ) + 2 );

	for( i = 0; i < strlen( Filter ); i++ )
	{
		if( Filter[ i ] == '|' )
			Filters[ i ] = '\0';
		else
			Filters[ i ] = Filter[ i ];
	}
	Filters[ strlen( Filter ) ] = '\0';
	Filters[ strlen( Filter ) + 1 ] = '\0';

	OPENFILENAME OpenFile;
	OpenFile.lStructSize = sizeof( OPENFILENAME );
	OpenFile.hwndOwner = Hwnd;
	OpenFile.hInstance = g_hInst;
	OpenFile.lpstrFilter = Filters;
	OpenFile.lpstrCustomFilter = NULL;
	OpenFile.nMaxCustFilter = NULL;
	OpenFile.lpstrFile = FileName;
	OpenFile.nMaxFile = 1000;
	OpenFile.lpstrFileTitle = NULL;
	OpenFile.lpstrInitialDir = NULL;
	OpenFile.lpstrTitle = Title;
	OpenFile.Flags = OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	OpenFile.lpstrDefExt = "rdn";

	if( GetSaveFileName( &OpenFile ) != 0 )
	{
		if( ( FileName[ strlen( FileName ) - 3 ] == 'R' && FileName[ strlen( FileName ) - 2 ] == 'D' && FileName[ strlen( FileName ) - 1 ] == 'N' ) == 1 || ( FileName[ strlen( FileName ) - 3 ] == 'r' && FileName[ strlen( FileName ) - 2 ] == 'd' && FileName[ strlen( FileName ) - 1 ] == 'n' ) == 1 )
		{
			if( FileName[ strlen( FileName ) - 3 ] == 'R' )
			{
				FileName[ strlen( FileName ) - 3 ] = 'r';
				FileName[ strlen( FileName ) - 2 ] = 'd';
				FileName[ strlen( FileName ) - 1 ] = 'n';
			}
			strcpy( FileNam, FileName );
			return FileName;
		}
		else
		{
			MessageBox(Hwnd, "Fichier invalide ou format inconnu du logiciel", "Reconnaissance des formes", 0 );
			FileNam[ 0 ] = '\0';
			return "";
		}
	}
	else
	{
		return "";
	}
}
bool OpenFile( void )
{
	FILE *OpenedFile = NULL;
	int i = 0, j = 0;
	char *TempFileName = NULL, TempString[ 15 ] = "", NewNeuronName[ 16 ] = "";

	TempFileName = FileToOpen( "Ouvrir", "Réseau de neurones (*.rdn)|*.rdn", FileNam );

	if( TempFileName[ 0 ] != '\0' )
	{
		ReInitializeProgram();

		OpenedFile = fopen( TempFileName,"rb" );
		fread( &NbCar, sizeof( char ), 1, OpenedFile );

		EntriesState = (int*) realloc( EntriesState, sizeof( int ) * NbCar * EntryNumber );
		Weights = (double*) realloc( Weights, sizeof( double ) * NbCar * EntryNumber );
		Sums = (double*) realloc( Sums, sizeof( double ) * NbCar );
		Output = (double*) realloc( Output, sizeof( double ) * NbCar );
		NeuronName = (char*) realloc( NeuronName, sizeof( char ) * NbCar * 16 );

		for( i = 0; i < NbCar; i++ )
		{
			Sums[ i ] = 0;
			fread( &NewNeuronName, sizeof( char ), 16, OpenedFile );

			for( j = 0; j < 16; j++ )
				NeuronName[ i * 16 + j ] = NewNeuronName[ j ];

			SendMessage( HwndCom, CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPCTSTR)NewNeuronName );
			for( j = 0; j < EntryNumber; j++ )
			{
				fread( &EntriesState[ i * EntryNumber + j ], sizeof( char ), 4, OpenedFile );
				fread( &Weights[ i * EntryNumber + j ], sizeof( char ), 8, OpenedFile );
			}
		}

		fread( &MaxIterations, sizeof( char ), 4, OpenedFile );
		sprintf( TempString, "%d", MaxIterations );
		SetWindowText( GetDlgItem( Hwnd, EditMaxIterations ), TempString );

		fread( &ErrorPercentage, sizeof( char ), 4, OpenedFile );
		sprintf( TempString, "%.2f", ErrorPercentage );
		SetWindowText( GetDlgItem( Hwnd, EditErrorPercentage ), TempString );

		fread( &Cor, sizeof( char ), 8, OpenedFile );
		sprintf( TempString, "%.2g", Cor );
		SetWindowText( GetDlgItem( Hwnd, EditCorrectionValue ), TempString );

		fread( TempString, sizeof( char ), 10, OpenedFile );
		ListView_SetItemText( HwndTraining, 0, 1, TempString );

		fread( TempString, sizeof( char ), 10, OpenedFile );
		TotalIterations = atoi( TempString );
		ListView_SetItemText( HwndTraining, 1, 1, TempString );

		fread( TempString, sizeof( char ), 15, OpenedFile );
		ListView_SetItemText( HwndTraining, 2, 1, TempString );

		fread( TempString, sizeof( char ), 15, OpenedFile );
		sscanf( TempString, "%f", &TotalTrainingTime );
		LastTrainingTime = TotalTrainingTime;
		ListView_SetItemText( HwndTraining, 3, 1, TempString );

		fread( TempString, sizeof( char ), 4, OpenedFile );
		ListView_SetItemText( HwndTraining, 4, 1, TempString );

		fread( TempString, sizeof( char ), 4, OpenedFile );
		ListView_SetItemText( HwndTraining, 5, 1, TempString );

		fread( TempString, sizeof( char ), 5, OpenedFile );
		ListView_SetItemText( HwndTraining, 6, 1, TempString );

		fclose( OpenedFile );
		return 1;
	}

	return 0;
}
void SaveFile( void )
{
	FILE *SavedFile = NULL;
	int i = 0, j = 0;
	char TempString[ 15 ] = "", NewNeuronName[ 16 ] = "";

	SavedFile = fopen( FileNam,"wb" );
	fwrite( &NbCar, sizeof( char ), 1, SavedFile );

	for( i = 0; i < NbCar; i++ )
	{
		for( j = 0; j < 16; j++ )
			NewNeuronName[ j ] = NeuronName[ i * 16 + j ];

		fwrite( &NewNeuronName, sizeof( char ), 16, SavedFile );
		for( j = 0; j < EntryNumber; j++ )
		{
			fwrite( &EntriesState[ i * EntryNumber + j ], sizeof( char ), 4, SavedFile );
			fwrite( &Weights[ i * EntryNumber + j ], sizeof( char ), 8, SavedFile );
		}
	}

	fwrite( &MaxIterations, sizeof( char ), 4, SavedFile );
	fwrite( &ErrorPercentage, sizeof( char ), 4, SavedFile );
	fwrite( &Cor, sizeof( char ), 8, SavedFile );

	ListView_GetItemText( HwndTraining, 0, 1, TempString, 10 );
	fwrite( TempString, sizeof( char ), 10, SavedFile );

	ListView_GetItemText( HwndTraining, 1, 1, TempString, 10 );
	fwrite( TempString, sizeof( char ), 10, SavedFile );

	ListView_GetItemText( HwndTraining, 2, 1, TempString, 15 );
	fwrite( TempString, sizeof( char ), 15, SavedFile );

	ListView_GetItemText( HwndTraining, 3, 1, TempString, 15 );
	fwrite( TempString, sizeof( char ), 15, SavedFile );

	ListView_GetItemText( HwndTraining, 4, 1, TempString, 4 );
	fwrite( TempString, sizeof( char ), 4, SavedFile );

	ListView_GetItemText( HwndTraining, 5, 1, TempString, 4 );
	fwrite( TempString, sizeof( char ), 4, SavedFile );

	ListView_GetItemText( HwndTraining, 6, 1, TempString, 5 );
	fwrite( TempString, sizeof( char ), 5, SavedFile );

	fclose( SavedFile );
}
bool SaveFileAs( void )
{
	char *TempFileName = NULL;

	TempFileName = FileToSave( "Enregistrer Sous", "Réseau de neurones (*.rdn)|*.rdn", FileNam );

	if( TempFileName[ 0 ] != '\0' )
	{
		SaveFile();
		return 1;
	}

	return 0;
}

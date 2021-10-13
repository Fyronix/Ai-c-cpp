POINT MousePos( void )
{
	POINT Pos;
	RECT rect;
	GetCursorPos( &Pos );
	GetWindowRect( Hwnd, &rect );
	Pos.x = Pos.x - rect.left - 20;
	Pos.y = Pos.y - rect.top - 82;
	return Pos;
}
void SetWindowTextToInteger( HWND Hwnd, char* TextToNum )
{
	int i = 0, j = 0, TextToNumLen = strlen( TextToNum );
	char* NewWindowText = NULL;

	NewWindowText = (char*) malloc( sizeof( char ) * ( TextToNumLen + 1 ) );

	for( i = 0; i < TextToNumLen + 1; i++ )
		NewWindowText[ i ] = '\0';

	for( i = 0; i < TextToNumLen; i++ )
	{
		if( TextToNum[ i ] > 47 && TextToNum[ i ] < 58 )
		{
			NewWindowText[ j ] = TextToNum[ i ];
			j++;
		}
	}

	if( NewWindowText[ 0 ] == '0' && strlen( NewWindowText ) > 1 )
	{
		for( i = 0; i < TextToNumLen; i++ )
		{
			if( NewWindowText[ i ] != '0' || i > (int) strlen( NewWindowText ) - 2 )
				break;
		}

		for( j = 0; j < TextToNumLen - i; j++ )
			TextToNum[ j ] = NewWindowText[ i + j ];

		TextToNum[ j ] = '\0';
		strcpy( NewWindowText, TextToNum );
	}

	SetWindowText( Hwnd, NewWindowText );
	free( NewWindowText );
	NewWindowText = NULL;
}
void SetWindowTextToReal( HWND Hwnd, char* TextToFloat, int MaxTextLen )
{
	int i = 0, j = 0, TextToFloatLen = strlen( TextToFloat );
	char* NewWindowText = NULL;

	NewWindowText = (char*) malloc( sizeof( char ) * ( MaxTextLen + 1 ) );

	for( i = 0; i < MaxTextLen + 1; i++ )
		NewWindowText[ i ] = '\0';

	for( i = 0; i < TextToFloatLen; i++ )
	{
		if( TextToFloat[ i ] == 46 || ( TextToFloat[ i ] > 47 && TextToFloat[ i ] < 58 ) )
		{
			NewWindowText[ j ] = TextToFloat[ i ];
			j++;
		}
	}

	if( NewWindowText[ 0 ] == '.' )
	{
		for( i = MaxTextLen - 1; i > 0; i-- )
			NewWindowText[ i ] = NewWindowText[ i - 1 ];

		NewWindowText[ 0 ] = '0';
	}

	SetWindowText( Hwnd, NewWindowText );
	free( NewWindowText );
	NewWindowText = NULL;
}
void ReInitializeProgram( void )
{
	NewPen( 0, 1, RGB( 255, 0, 0 ) );
	NewPen( 1, 1, RGB( 0, 255, 0 ) );
	NewBitmap( Hwnd, 1, 5, 7 );
	NewBitmap( Hwnd, 2, 5, 7 );
	DrawBitmap( HwndImgInit, 1 );
	DrawBitmap( HwndImgReco, 2 );

	SendMessage( HwndCom, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0 );

	MaxIterations = 1000;
	ErrorPercentage = 0.5;
	Cor = 0.1;
	LastTrainingTime = 0; 
	TotalIterations = 0;
	TotalTrainingTime = 0;
	NbCar = 0;

	ListView_SetItemText( HwndTraining, 0, 1, "0" );
	ListView_SetItemText( HwndTraining, 1, 1, "0" );
	ListView_SetItemText( HwndTraining, 2, 1, "0.0" );
	ListView_SetItemText( HwndTraining, 3, 1, "0.0" );
	ListView_SetItemText( HwndTraining, 4, 1, "0" );
	ListView_SetItemText( HwndTraining, 5, 1, "0" );
	ListView_SetItemText( HwndTraining, 6, 1, "0.0" );

	SetWindowText( GetDlgItem( Hwnd, EditSymbolToCreate ), "" );
	SetWindowText( GetDlgItem( Hwnd, EditMaxIterations ), "1000" );
	SetWindowText( GetDlgItem( Hwnd, EditCorrectionValue ), "0.1" );
	SetWindowText( GetDlgItem( Hwnd, EditErrorPercentage ), "0.5" );

	free( EntriesState );
	free( Weights );
	free( Sums );
	free( Output );
	free( NeuronName );
	EntriesState = (int*) malloc( sizeof( int ) * EntryNumber );
	Weights = (double*) malloc( sizeof( double ) * EntryNumber );
	Sums = (double*) malloc( sizeof( double ) );
	Output = (double*) malloc( sizeof( double ) );
	NeuronName = (char*) malloc( sizeof( char ) * 16 );
}
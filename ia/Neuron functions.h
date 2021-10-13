bool NeuronNameExist( char Name[ 16 ] )
{
	bool IsNameAlreadyExist = 0;
	int i = 0, j = 0;
	char NameToCompare[ 16 ] = "";

	for( i = 0; i < NbCar; i++ )
	{
		for( j = 0; j < 16; j++ )
			NameToCompare[ j ] = NeuronName[ i * 16 + j ];

		if( strcmp( Name, NameToCompare ) == 0 )
		{
			IsNameAlreadyExist = 1;
			break;
		}
	}

	return IsNameAlreadyExist;
}
bool NeuronEntriesExist( void )
{
	bool IsEntriesAlreadyExist = 0;
	int i = 0, j = 0, States = 0;

	for( i = 0; i < NbCar; i++ )
	{
		States = 0;
		for( j = 0; j < EntryNumber; j++ )
		{
			if( ComparedStates[ j ] == EntriesState[ i * EntryNumber + j ] )
				States++;
		}
		if( States == 35 )
		{
			IsEntriesAlreadyExist = 1;
			break;
		}
	}

	return IsEntriesAlreadyExist;
}
void GetStates( void )
{
	HDC HDCBuffer = NULL, HDCImgInit = NULL;
	COLORREF Red = RGB( 255, 0, 0 );
	int i = 0, j = 0;

	HDCImgInit = GetDC( HwndImgInit );
	HDCBuffer = CreateCompatibleDC( HDCImgInit );
	ReleaseDC( HwndImgInit, HDCImgInit );
	SelectObject( HDCBuffer, Image[ 1 ].H );

	for( i = 0; i < 7; i++ )
	{
		for( j = 0; j < 5; j++ )
		{
			if( GetPixel( HDCBuffer, j, i ) == Red )
				ComparedStates[ i * 5 + j ] = 1;
			else
				ComparedStates[ i * 5 + j ] = -1;
		}
	}

	DeleteDC( HDCBuffer );
}
void SetWeights( int Neuron )
{
	srand( (unsigned) time( NULL ) );
	int i = 0, Random = 0, Sign = 1;

	for( i = 0; i < EntryNumber; i++ )
	{
		Random = rand();
		Weights[ Neuron * EntryNumber + i ] = (float) Random / RAND_MAX * Sign;

		if( Weights[ Neuron * EntryNumber + i ] > 0.5 || Weights[ Neuron * EntryNumber + i ] < -0.5 )
			Sign = -1;
		else
			Sign = 1;
	}
}
double Sum( int Neuron )
{
	double OutputValue = 0.0;

	for( int i = 0; i < EntryNumber; i++ )
		Sums[ Neuron ] = Sums[ Neuron ] + ComparedStates[ i ] * Weights[ Neuron * EntryNumber + i ];	

	Sums[ Neuron ] = Sums[ Neuron ] / 10.0;
	OutputValue = 2.0 / ( 1.0 + exp( - Sums[ Neuron ] ) ) - 1.0;

	return OutputValue;
}
void Correct( int Neuron, int WishedOutput )
{	
	for( int i = 0; i < EntryNumber; i++ )
		Weights[ Neuron * EntryNumber + i ] = Weights[ Neuron * EntryNumber + i ] + Cor * ( ( WishedOutput - Output[ Neuron ] ) * ComparedStates[ i ] );
}
void Correction( int NeuronToCorrect )
{
	int i = 0, IterationsRealized = 0;
	float CurrentTrainingTime = 0;
	char NewMaxIterationValue[ 10 ] = "", NewIterationValue[ 10 ] = "", NewTrainingTime[ 15 ] = "", NewTotalTrainingTime[ 15 ] = "";
	GetStates();
	QueryPerformanceCounter( ( LARGE_INTEGER* ) &LastClock );
	while( NeuronToCorrect != RecognizedNeuron )
	{
		Correct( NeuronToCorrect, 1 );

		for( i = 0; i < NbCar; i++ )
			Output[ i ] = Sum( i );

		RecognizedNeuron = RecognizeNeuron();

		for( i = 0; i < NbCar; i++ )
			Sums[ i ] = 0;

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
	}
	LastTrainingTime = TotalTrainingTime;
	if( IterationsRealized == 0 )
	{
		ListView_SetItemText( HwndTraining, 0, 1, "0" );
		ListView_SetItemText( HwndTraining, 2, 1, "0.0" );
	}
	else
	{
		Modified = 1;
		RealTimeRecognition();
	}
}
float GetErrorPercent( int* RecognizedValue )
{
	int i = 0, j = 0, NbCarRecognized = 0;
	float NewErrorPercent = 0;

	for( j = 0; j < NbCar; j++ )
	{
		for( i = 0; i < EntryNumber; i++ )
			ComparedStates[ i ] = EntriesState[ j * EntryNumber + i ];	

		for( i = 0; i < NbCar; i++ )
			Output[ i ] = Sum( i );

		RecognizedNeuron = RecognizeNeuron();

		for( i = 0; i < NbCar; i++ )
			Sums[ i ] = 0;

		if( RecognizedNeuron == j )
			NbCarRecognized++;
	}

	*RecognizedValue = NbCarRecognized;
	NewErrorPercent = 1 - ( (float)NbCarRecognized / (float)NbCar );

	return NewErrorPercent;
}
int RecognizeNeuron( void )
{
	int i = 0, Recognized = 0;

	while( i < NbCar )
	{
		if( Recognized != i )
		{
			if( Sums[ i ] > Sums[ Recognized ] )
			{
				Recognized = i;
				i = -1;
			}
		}
		i++;
	}

	return Recognized;
}
void RealTimeRecognition( void )
{
	int i = 0;
	if( NbCar > 0 )
	{
		GetStates();

		for( i = 0; i < NbCar; i++ )
			Output[ i ] = Sum( i );

		RecognizedNeuron = RecognizeNeuron();
		ShowRecognized( RecognizedNeuron );

		for( i = 0; i < NbCar; i++ )
			Sums[ i ] = 0;
	}
}
void ShowRecognized( int Neuron )
{
	HDC HDCBuffer = NULL, HDCImgReco = NULL;
	COLORREF Red = RGB( 255, 0, 0 ), Green = RGB( 0, 255, 0 );
	int i = 0, j = 0;

	HDCImgReco = GetDC( HwndImgReco );
	HDCBuffer = CreateCompatibleDC( HDCImgReco );
	SelectObject( HDCBuffer, Image[ 2 ].H );

	for( i = 0; i < 7; i++ )
	{
		for( j = 0; j < 5; j++ )
		{
			if( EntriesState[ Neuron * EntryNumber + i * 5 + j ] == 1 )
				SetPixel( HDCBuffer, j, i, Red );
			else
				SetPixel( HDCBuffer, j, i, Green );
		}
	}

	DeleteDC( HDCBuffer );
	StretchDIBits( HDCImgReco, 0, 0, Image[ 2 ].Width*Zoom, Image[ 2 ].Height*Zoom, 0, 0, Image[ 2 ].Width, Image[ 2 ].Height, Image[ 2 ].DibBits, &Image[ 2 ].BMI, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC( HwndImgReco, HDCImgReco );
}
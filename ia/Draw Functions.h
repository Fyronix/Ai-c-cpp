void NewBitmap( HWND Hwnd, int ImageNum, int ImageWidth, int ImageHeight )
{
	if( Image[ ImageNum ].H )
	{
		DeleteObject( Image[ ImageNum ].H );
		Image[ ImageNum ].DibBits = NULL;
	}

	RECT rc={ 0, 0, ImageWidth, ImageHeight };

	BITMAPINFOHEADER BmInfoH;
	BmInfoH.biSize = sizeof( BITMAPINFOHEADER );
	BmInfoH.biWidth = ImageWidth;
	BmInfoH.biHeight = -ImageHeight;
	BmInfoH.biPlanes = 1;
	BmInfoH.biBitCount = 24;
	BmInfoH.biCompression = BI_RGB;
	BmInfoH.biSizeImage = 0;
	BmInfoH.biXPelsPerMeter = 0;
	BmInfoH.biYPelsPerMeter = 0;
	BmInfoH.biClrUsed = 0;
	BmInfoH.biClrImportant = 0;

	RGBQUAD RGBR[1]={ 0, 0, 0, 0 };

	BITMAPINFO BmInfo;
	BmInfo.bmiHeader = BmInfoH;
	*BmInfo.bmiColors = *RGBR;

	HDC HDCHwnd = NULL, CreateBitmap = NULL;
	HDCHwnd = GetDC( Hwnd );
	CreateBitmap = CreateCompatibleDC( HDCHwnd );
	ReleaseDC( Hwnd, HDCHwnd );

	Image[ ImageNum ].BMI = BmInfo;
	Image[ ImageNum ].Width = ImageWidth;
	Image[ ImageNum ].Height = ImageHeight;
	Image[ ImageNum ].H = CreateDIBSection( CreateBitmap, &BmInfo, DIB_RGB_COLORS, &Image[ ImageNum ].DibBits, NULL, NULL );

	HBRUSH brush = NULL;
	brush = CreateSolidBrush( RGB( 0, 255, 0 ) );

	SelectObject( CreateBitmap, brush );
	SelectObject( CreateBitmap, Image[ ImageNum ].H );
	FillRect( CreateBitmap, &rc, brush );
	DeleteDC( CreateBitmap );

	if( Image[ ImageNum ].H == NULL || Image[ ImageNum ].DibBits == NULL )
		MessageBox( Hwnd, "Cannot create bitmap", "pattern recognition", 0 );
}
void DrawBitmap( HWND Hwnd, int ImageNumber )
{
	HDC HDCHwnd = NULL;
	HDCHwnd = GetDC( Hwnd );
	StretchDIBits( HDCHwnd, 0, 0, Image[ ImageNumber ].Width*Zoom, Image[ ImageNumber ].Height*Zoom, 0, 0, Image[ ImageNumber ].Width, Image[ ImageNumber ].Height, Image[ ImageNumber ].DibBits, &Image[ ImageNumber ].BMI, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC( Hwnd, HDCHwnd );
}
void NewPen( int PenNumber, int PenSize, COLORREF PenColor )
{
	if( HPen[ PenNumber ] != NULL )
		DeleteObject( HPen[ PenNumber ] );
	HPen[ PenNumber ] = CreatePen( PS_SOLID, PenSize, PenColor );
}
void PenDraw( int PenNumber, int Mode )
{
	HDC HDCBuffer = NULL, HDCImgInit = NULL;

	SetCapture( HwndImgInit );
	NewPos = MousePos();

	HDCImgInit = GetDC( HwndImgInit );
	HDCBuffer = CreateCompatibleDC( HDCImgInit );
	SelectObject( HDCBuffer, HPen[ PenNumber ] );
	SelectObject( HDCBuffer, Image[ 1 ].H );

	if( NewPos.x > 0 && NewPos.y > 0 )
	{
		if( Mode == 1 )
		{
			MoveToEx( HDCBuffer, LastPos.x / Zoom, LastPos.y / Zoom, NULL );
			LineTo( HDCBuffer, NewPos.x / Zoom, NewPos.y / Zoom );
		}
		MoveToEx( HDCBuffer, NewPos.x / Zoom, NewPos.y / Zoom, NULL );
		LineTo( HDCBuffer, NewPos.x / Zoom + 1, NewPos.y / Zoom + 1 );
	}

	DeleteDC( HDCBuffer );
	StretchDIBits( HDCImgInit, 0, 0, Image[ 1 ].Width*Zoom, Image[ 1 ].Height*Zoom, 0, 0, Image[ 1 ].Width, Image[ 1 ].Height, Image[ 1 ].DibBits, &Image[ 1 ].BMI, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC( HwndImgInit, HDCImgInit );

	LastPos = NewPos;
}

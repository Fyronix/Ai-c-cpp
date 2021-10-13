void AddColumn( HWND HListeTemp, int ColumnNumber, char* ColumnName, int ColumnWidth ) 
{ 
	LVCOLUMN Lvc;
	Lvc.mask = LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT;
	Lvc.cx = ColumnWidth;
	Lvc.fmt = LVCFMT_LEFT;
	Lvc.pszText = ColumnName;
	ListView_InsertColumn( HListeTemp, ColumnNumber, &Lvc );
} 
void AddItem( HWND HListeTemp, char* ItemName, int LineNumber ) 
{ 
	LVITEM Lvi;
	Lvi.mask = LVIF_TEXT;
	Lvi.iItem = LineNumber;
	Lvi.iSubItem = 0;
	Lvi.pszText = ItemName;
	ListView_InsertItem( HListeTemp, &Lvi );
}

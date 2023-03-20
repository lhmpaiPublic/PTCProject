
#pragma once

void SetInsertMode( BOOL bInsert ) ;
BOOL GetInsertMode( void ) ;
int SetKeyLock( BYTE byteTargetKey, BOOL bState, BOOL *pbBackState = NULL ) ;
BOOL GetKeyLock( BYTE byteTargetKey ) ;
int IsMyMbcLegal( LPCTSTR lpszStr, int nStart = 0 ) ;
int CalclateExpressionString( CString& rstrExpression ) ;

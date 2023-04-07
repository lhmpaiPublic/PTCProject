﻿#if !defined(AFX_GRIDCELLSPINCTRL_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_GRIDCELLSPINCTRL_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "GridCell.h"


class CGridCellSpinCtrl : public CGridCell
{
    friend class CGridCtrl;
    DECLARE_DYNCREATE(CGridCellSpinCtrl)

public:
    CGridCellSpinCtrl();
#if 0 
// editing cells
public:
    virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual CWnd* GetEditWnd() const;
    //virtual void  EndEdit();
#endif

	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar) { return( TRUE ) ; }
	virtual void EndEdit(){}
	virtual BOOL GetTextRect( LPRECT pRect);
	virtual BOOL IsSpinIncrement( void ) ;

// Operations
public:
	virtual CSize GetCellExtent(CDC* pDC);
	virtual void OnClick( CPoint PointCellRelative);
    virtual void OnClickDown( CPoint PointCellRelative);

protected:
    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);

	BOOL m_bOnUp ;
	BOOL m_bOnDown ;
	CRect m_RectUp ;
	CRect m_RectDown ;
	CPoint m_ptLeftTop ;
	BOOL m_bIncrement ;

};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_GRIDCELLSPINCTRL_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)

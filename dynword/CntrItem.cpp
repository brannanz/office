
// CntrItem.cpp : implementation of the CWordCntrItem class
//

#include "pch.h"
#include "framework.h"
#include "dynword.h"

#include "WordDoc.h"
#include "WordView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWordCntrItem implementation

IMPLEMENT_SERIAL(CWordCntrItem, COleClientItem, 0)

CWordCntrItem::CWordCntrItem(REOBJECT* preo, CWordDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
}

CWordCntrItem::~CWordCntrItem()
{
	// TODO: add cleanup code here
}

void CWordCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	// When an item is being edited (either in-place or fully open)
	//  it sends OnChange notifications for changes in the state of the
	//  item or visual appearance of its content.

	// TODO: invalidate the item by calling UpdateAllViews
	//  (with hints appropriate to your application)

	GetDocument()->UpdateAllViews(nullptr);
		// for now just update ALL views/no hints
}

BOOL CWordCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	// During in-place activation CWordCntrItem::OnChangeItemPosition
	//  is called by the server to change the position of the in-place
	//  window.  Usually, this is a result of the data in the server
	//  document changing such that the extent has changed or as a result
	//  of in-place resizing.
	//
	// The default here is to call the base class, which will call
	//  COleClientItem::SetItemRects to move the item
	//  to the new position.

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	// TODO: update any cache you may have of the item's rectangle/extent

	return TRUE;
}

BOOL CWordCntrItem::OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow)
{
	CFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CFrameWndEx, pFrameWnd);

	if (pMainFrame != nullptr)
	{
		ASSERT_VALID(pMainFrame);
		return pMainFrame->OnShowPanes(bShow);
	}

	return FALSE;
}

void CWordCntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	// During in-place activation, CWordCntrItem::OnGetItemPosition
	//  will be called to determine the location of this item.  Usually, this 
	//  rectangle would reflect the current position of the item relative to the 
	//  view used for activation.  You can obtain the view by calling 
	//  CWordCntrItem::GetActiveView.

	// TODO: return correct rectangle (in pixels) in rPosition

	CSize size;
	rPosition.SetRectEmpty();
	if (GetExtent(&size, m_nDrawAspect))
	{
		CWordView* pView = GetActiveView();
		ASSERT_VALID(pView);
		if (!pView)
			return;
		CDC *pDC = pView->GetDC();
		ASSERT(pDC);
		if (!pDC)
			return;
		pDC->HIMETRICtoLP(&size);
		rPosition.SetRect(10, 10, size.cx + 10, size.cy + 10);
	}
	else
		rPosition.SetRect(10, 10, 210, 210);
}

void CWordCntrItem::OnActivate()
{
	// Allow only one inplace activate item per frame
	CWordView* pView = GetActiveView();
	ASSERT_VALID(pView);
	if (!pView)
		return;
	COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
	if (pItem != nullptr && pItem != this)
		pItem->Close();
	
	COleClientItem::OnActivate();
}

void CWordCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

	DWORD dwMisc = 0;
	m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
	if (dwMisc & OLEMISC_INSIDEOUT)
		DoVerb(OLEIVERB_HIDE, nullptr);
}

void CWordCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// Call base class first to read in COleClientItem data.
	// Since this sets up the m_pDocument pointer returned from
	//  CWordCntrItem::GetDocument, it is a good idea to call
	//  the base class Serialize first.
	COleClientItem::Serialize(ar);

	// now store/retrieve data specific to CWordCntrItem
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CWordCntrItem diagnostics

#ifdef _DEBUG
void CWordCntrItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CWordCntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif


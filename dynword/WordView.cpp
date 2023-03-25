
// WordView.cpp : implementation of the CWordView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "dynword.h"
#endif

#include "WordDoc.h"
#include "CntrItem.h"
#include "resource.h"
#include "WordView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWordView

IMPLEMENT_DYNCREATE(CWordView, CRichEditView)

BEGIN_MESSAGE_MAP(CWordView, CRichEditView)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_OLE_INSERT_NEW, &CWordView::OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, &CWordView::OnCancelEditCntr)
	ON_COMMAND(ID_FILE_PRINT, &CWordView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CWordView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CWordView construction/destruction

CWordView::CWordView() noexcept
{
	m_pSelection = nullptr;
	// TODO: add construction code here

}

CWordView::~CWordView()
{
}

BOOL CWordView::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bRes = CRichEditView::PreCreateWindow(cs);
	cs.style |= ES_SELECTIONBAR;
	return bRes;
}

// CWordView drawing

void CWordView::OnDraw(CDC* pDC)
{
	if (!pDC)
		return;

	CWordDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	// TODO: also draw all OLE items in the document

	// Draw the selection at an arbitrary position.  This code should be
	//  removed once your real drawing code is implemented.  This position
	//  corresponds exactly to the rectangle returned by CWordCntrItem,
	//  to give the effect of in-place editing.

	// TODO: remove this code when final draw code is complete.
	if (m_pSelection != nullptr)
	{
		CSize size;
		CRect rect(10, 10, 210, 210);

		if (m_pSelection->GetExtent(&size, m_pSelection->m_nDrawAspect))
		{
			pDC->HIMETRICtoLP(&size);
			rect.right = size.cx + 10;
			rect.bottom = size.cy + 10;
		}
		m_pSelection->Draw(pDC, rect);
	}
}

void CWordView::OnInitialUpdate()
{
	CView::OnInitialUpdate();


	// TODO: remove this code when final selection model code is written
	m_pSelection = nullptr;    // initialize selection

}


// CWordView printing


void CWordView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CWordView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWordView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWordView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CWordView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != nullptr && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == nullptr);
   }
   CView::OnDestroy();
}



// OLE Client support and commands

BOOL CWordView::IsSelected(const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only CWordCntrItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced

	// TODO: implement this function that tests for a selected OLE client item

	return pDocItem == m_pSelection;
}

void CWordView::OnInsertObject()
{
	// Invoke the standard Insert Object dialog box to obtain information
	//  for new CWordCntrItem object
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	BeginWaitCursor();

	CWordCntrItem* pItem = nullptr;
	TRY
	{
		// Create new item connected to this document
		CWordDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new CWordCntrItem(NULL, pDoc);
		ASSERT_VALID(pItem);

		// Initialize the item from the dialog data
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  // any exception will do
		ASSERT_VALID(pItem);

        if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);

		ASSERT_VALID(pItem);
		// As an arbitrary user interface design, this sets the selection
		//  to the last item inserted

		// TODO: reimplement selection as appropriate for your application
		m_pSelection = pItem;   // set selection to last inserted item
		pDoc->UpdateAllViews(nullptr);
	}
	CATCH(CException, e)
	{
		if (pItem != nullptr)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH

	EndWaitCursor();
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation
void CWordView::OnCancelEditCntr()
{
	// Close any in-place active item on this view.
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != nullptr)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == nullptr);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place
void CWordView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != nullptr &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// need to set focus to this item if it is in the same view
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != nullptr)
		{
			pWnd->SetFocus();   // don't call the base class
			return;
		}
	}

	CView::OnSetFocus(pOldWnd);
}

void CWordView::OnSize(UINT nType, int cx, int cy)
{
	CRichEditView::OnSize(nType, cx, cy);
	CRect rect(HORZ_TEXTOFFSET, VERT_TEXTOFFSET, cx, cy);
	GetRichEditCtrl().SetRect(rect);
}

void CWordView::OnFilePrint()
{
	//By default, we ask the Active document to print itself
	//using IOleCommandTarget. If you don't want this behavior
	//remove the call to COleDocObjectItem::DoDefaultPrinting.
	//If the call fails for some reason, we will try printing
	//the docobject using the IPrint interface.
	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != nullptr);
	if (S_OK == COleDocObjectItem::DoDefaultPrinting(this, &printInfo))
		return;

	CView::OnFilePrint();

}


void CWordView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CWordView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CWordView diagnostics

#ifdef _DEBUG
void CWordView::AssertValid() const
{
	CView::AssertValid();
}

void CWordView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWordDoc* CWordView::GetDocument() // non-debug version is inline
{
	return (CWordDoc*)m_pDocument;
}
#endif //_DEBUG


// CWordView message handlers

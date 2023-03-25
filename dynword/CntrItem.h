
// CntrItem.h : interface of the CWordCntrItem class
//

#pragma once

class CWordDoc;
class CWordView;

class CWordCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CWordCntrItem)

// Constructors
public:
	CWordCntrItem(REOBJECT* preo = NULL, CWordDoc* pContainer = NULL);
		// Note: pContainer is allowed to be null to enable IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-null document pointer

// Attributes
public:
	CWordDoc* GetDocument()
		{ return reinterpret_cast<CWordDoc*>(COleClientItem::GetDocument()); }
	CWordView* GetActiveView()
		{ return reinterpret_cast<CWordView*>(COleClientItem::GetActiveView()); }

public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();

protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow);

// Implementation
public:
	~CWordCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};


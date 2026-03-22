#include "stdafx.h"
#include "BtnEnhReadability.h"
#include "btnenh.h"
#include "surfaceColor.h"
#include "textDescriptor.h"
#include "cellsManager.h"

namespace {

static void FlattenSurfaceColor(CSurfaceColor& surf, COLORREF c)
{
	const unsigned long ul = static_cast<unsigned long>(c);
	surf.SetColor(ul);
	surf.SetColor2(ul);
	surf.SetGradientFactor(0);
	surf.SetGradientType(0);
	surf.SetRender3DType(0);
}

static void FlattenOffice2003(CBtnEnh& btn, COLORREF bg, COLORREF bgMo, COLORREF pressed)
{
	const unsigned long ub = static_cast<unsigned long>(bg);
	const unsigned long um = static_cast<unsigned long>(bgMo);
	const unsigned long up = static_cast<unsigned long>(pressed);
	btn.SetOffice2003ColorNorth(ub);
	btn.SetOffice2003ColorSouth(ub);
	btn.SetOffice2003ColorBorder(um);
	btn.SetOffice2003ColorNorthMouseOver(um);
	btn.SetOffice2003ColorSouthMouseOver(um);
	btn.SetOffice2003ColorBorderMouseOver(um);
	btn.SetOffice2003ColorNorthPressed(up);
	btn.SetOffice2003ColorSouthPressed(up);
	btn.SetOffice2003ColorBorderPressed(up);
}

static void FlattenOffice2007(CBtnEnh& btn, COLORREF bg, COLORREF bgMo, COLORREF pressed)
{
	const unsigned long ub = static_cast<unsigned long>(bg);
	const unsigned long um = static_cast<unsigned long>(bgMo);
	const unsigned long up = static_cast<unsigned long>(pressed);
	btn.SetOffice2007ColorNorthTop(ub);
	btn.SetOffice2007ColorNorthBottom(ub);
	btn.SetOffice2007ColorSouthTop(ub);
	btn.SetOffice2007ColorSouthBottom(ub);
	btn.SetOffice2007ColorInnerBorder(um);
	btn.SetOffice2007ColorOuterBorder(up);
	btn.SetOffice2007ColorNorthTopMouseOver(um);
	btn.SetOffice2007ColorNorthBottomMouseOver(um);
	btn.SetOffice2007ColorSouthTopMouseOver(um);
	btn.SetOffice2007ColorSouthBottomMouseOver(um);
	btn.SetOffice2007ColorInnerBorderMouseOver(um);
	btn.SetOffice2007ColorOuterBorderMouseOver(um);
	btn.SetOffice2007ColorNorthTopPressed(up);
	btn.SetOffice2007ColorNorthBottomPressed(up);
	btn.SetOffice2007ColorSouthTopPressed(up);
	btn.SetOffice2007ColorSouthBottomPressed(up);
	btn.SetOffice2007ColorInnerBorderPressed(up);
	btn.SetOffice2007ColorOuterBorderPressed(up);
}

// Checked / toggle / option-group buttons often still use these (default = near black)
static void FlattenVistaCheckedGlowAndBorders(CBtnEnh& btn, COLORREF bg, COLORREF bgMo, COLORREF pressed)
{
	const unsigned long ub = static_cast<unsigned long>(bg);
	const unsigned long um = static_cast<unsigned long>(bgMo);
	const unsigned long up = static_cast<unsigned long>(pressed);

	btn.SetVistaColorBackgroundChecked(ub);
	btn.SetVistaColorBackgroundCheckedMouseOver(um);
	btn.SetVistaColorGlowChecked(ub);
	btn.SetVistaColorGlowCheckedMouseOver(um);
	btn.SetVistaColorGlow(ub);
	btn.SetVistaColorGlowBack(ub);
	btn.SetVistaColorGlowBottom(ub);

	btn.SetVistaColorOuterBorder(um);
	btn.SetVistaColorInnerBorder(ub);
	btn.SetVistaColorMiddleBorder(um);
	btn.SetVistaColorInnerBorderPressed(up);
}

static void FlattenOneTextDescriptor(CTextDescriptor& td, COLORREF fg, COLORREF bg)
{
	const unsigned long uf = static_cast<unsigned long>(fg);
	const unsigned long ub = static_cast<unsigned long>(bg);

	td.SetColorNormal(uf);
	td.SetColorMouseOver(uf);
	td.SetColorPressed(uf);
	td.SetColorBorder(ub);
	td.SetSpecialEffect(0);
	td.SetSolidBack(TRUE);
	td.SetColorSolidBack(ub);
}

// Caption + 9-slot text (SetWindowText often maps to center / caption; headers may use other slots)
static void FlattenAllBtnEnhTextSlots(CBtnEnh& btn, COLORREF fg, COLORREF bg)
{
	FlattenOneTextDescriptor(btn.GetTextDescrCaption(), fg, bg);
	FlattenOneTextDescriptor(btn.GetTextDescrLT(), fg, bg);
	FlattenOneTextDescriptor(btn.GetTextDescrCT(), fg, bg);
	FlattenOneTextDescriptor(btn.GetTextDescrRT(), fg, bg);
	FlattenOneTextDescriptor(btn.GetTextDescrLM(), fg, bg);
	FlattenOneTextDescriptor(btn.GetTextDescrRM(), fg, bg);
	FlattenOneTextDescriptor(btn.GetTextDescrLB(), fg, bg);
	FlattenOneTextDescriptor(btn.GetTextDescrCB(), fg, bg);
	FlattenOneTextDescriptor(btn.GetTextDescrRB(), fg, bg);
}

static void FlattenBtnEnhCellsIfAny(CBtnEnh& btn, COLORREF bg, COLORREF bgMo)
{
	const unsigned long ub = static_cast<unsigned long>(bg);
	const unsigned long um = static_cast<unsigned long>(bgMo);

	CCellsManager cm = btn.GetCellsManager();
	const short n = cm.CellGetCount();
	if (n <= 0)
		return;

	for (short i = 0; i < n; ++i)
	{
		const short id = cm.CellGetUniqueID(i);
		cm.SetBackColor(id, ub);
		cm.SetBorderColor(id, um);
	}
}

static void ToneDownVistaGloss(CBtnEnh& btn, COLORREF bg)
{
	const unsigned long ub = static_cast<unsigned long>(bg);
	// Reduce glossy overlay (often reads as black band on top of BtnEnh)
	btn.SetVistaOpacityGlossyEffectUpper(0);
	btn.SetVistaOpacityGlossyEffectLower(0);
	btn.SetVistaOpacityGlossyEmphUpper(0);
	btn.SetVistaOpacityGlossyEmphLower(0);
	btn.SetVistaOpacityGlossyEmphUpperPressed(0);
	btn.SetVistaOpacityGlossyEmphLowerPressed(0);
	btn.SetVistaColorGlossyEffectUpper(ub);
	btn.SetVistaColorGlossyEffectLower(ub);
	btn.SetVistaColorGlossyEmphUpper(ub);
	btn.SetVistaColorGlossyEmphLower(ub);
	btn.SetVistaColorGlossyEmphUpperPressed(ub);
	btn.SetVistaColorGlossyEmphLowerPressed(ub);
}

} // namespace

void ApplyBtnEnhReadabilityStyle(CBtnEnh& btn)
{
	const COLORREF bg = INSPECT_GRID_BACK;
	const COLORREF bgMo = INSPECT_GRID_BACK_MO;
	const COLORREF fg = INSPECT_GRID_FORE;
	const COLORREF pressed = RGB(82, 82, 88);

	btn.SetBackColorInterior(bg);
	btn.SetBackColor(bg);
	btn.SetBackColorMouseOver(bgMo);
	btn.SetBackColorFocus(bgMo);
	btn.SetBackColorPressed(pressed);
	btn.SetForeColor(fg);
	btn.SetForeColorMouseOver(fg);
	btn.SetForeColorPressed(fg);
	btn.SetForeColorDisabled(RGB(180, 180, 185));
	btn.SetBackColorDisabled(RGB(70, 70, 75));
	btn.SetHighlightColor(RGB(150, 150, 158));
	btn.SetShadowColor(RGB(55, 55, 60));
	btn.SetBackColorContainer(bg);

	// Prefer flat / standard drawing when the OCX supports these dispids
	btn.SetSpecialEffect(0);
	btn.SetFrameEffect(0);
	btn.SetVistaLook(0);

	FlattenSurfaceColor(btn.GetColorSurfaceNormal(), bg);
	FlattenSurfaceColor(btn.GetColorSurfaceMO(), bgMo);
	FlattenSurfaceColor(btn.GetColorSurfacePressed(), pressed);
	FlattenSurfaceColor(btn.GetColorSurfaceInternal(), bg);
	FlattenSurfaceColor(btn.GetColorSurfaceFocus(), bgMo);
	FlattenSurfaceColor(btn.GetColorSurfaceDisabled(), RGB(70, 70, 75));

	FlattenOffice2003(btn, bg, bgMo, pressed);
	FlattenOffice2007(btn, bg, bgMo, pressed);
	ToneDownVistaGloss(btn, bg);
	FlattenVistaCheckedGlowAndBorders(btn, bg, bgMo, pressed);

	FlattenAllBtnEnhTextSlots(btn, fg, bg);
	FlattenBtnEnhCellsIfAny(btn, bg, bgMo);

	if (btn.GetSafeHwnd())
		btn.Refresh();
}

void ApplyBtnEnhReadabilityById(CWnd* pDlg, UINT nCtrlId)
{
	if (!pDlg || !pDlg->GetSafeHwnd())
		return;
	HWND h = ::GetDlgItem(pDlg->GetSafeHwnd(), static_cast<int>(nCtrlId));
	if (!h)
		return;
	CBtnEnh tmp;
	if (!tmp.Attach(h))
		return;
	ApplyBtnEnhReadabilityStyle(tmp);
	tmp.Detach();
}

void ApplyBtnEnhReadabilitySubtree(CWnd* pRoot)
{
	if (!pRoot || !pRoot->GetSafeHwnd())
		return;

	for (CWnd* pChild = pRoot->GetWindow(GW_CHILD); pChild != NULL; pChild = pChild->GetWindow(GW_HWNDNEXT))
	{
		ApplyBtnEnhReadabilitySubtree(pChild);

		HWND hChild = pChild->GetSafeHwnd();
		if (!hChild)
			continue;

		CWnd* pPerm = CWnd::FromHandlePermanent(hChild);
		if (!pPerm)
			continue;

		CBtnEnh* pBtn = dynamic_cast<CBtnEnh*>(pPerm);
		if (pBtn != nullptr)
			ApplyBtnEnhReadabilityStyle(*pBtn);
	}
}

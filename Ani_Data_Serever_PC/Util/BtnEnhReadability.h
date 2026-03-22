#pragma once

class CBtnEnh;
class CWnd;

// Flatten BtnEnh dark gradient and use INSPECT_GRID_* colors (Migration.h) for readable text.
void ApplyBtnEnhReadabilityStyle(CBtnEnh& btn);

// Style a BtnEnh on pDlg by control ID (uses temporary Attach; skips if HWND already wrapped).
void ApplyBtnEnhReadabilityById(CWnd* pDlg, UINT nCtrlId);

// Recurse child windows; apply style to every HWND that is a permanent CBtnEnh (DDX OCX host).
void ApplyBtnEnhReadabilitySubtree(CWnd* pRoot);

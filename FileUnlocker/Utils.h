#pragma once

#include "stdafx.h"

VOID PaintCustomCaption(HWND, HDC);
BOOL CALLBACK SetChildFont(HWND, LPARAM);
HRESULT ExtendIntoClientBottom(HWND);
HRESULT EnableBlurBehind(HWND);
HFONT GetMessageFont();
HFONT GetCaptionFont();
HFONT GetMenuFont();
HFONT GetSmCaptionFont();
HFONT GetStatusFont();

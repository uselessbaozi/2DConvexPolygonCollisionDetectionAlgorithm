#include "Control.h"

AppControl::UnknownControl::UnknownControl(RECT r, ControlType type, bool visible) :addr(r), type(type), visible(visible)
{
}

RECT AppControl::UnknownControl::GetAddr()
{
	return addr;
}

void AppControl::UnknownControl::SetAddr(const RECT& r)
{
	addr = r;
}

AppControl::ControlType AppControl::UnknownControl::GetControlType()
{
	return type;
}

bool AppControl::UnknownControl::GetVisible()
{
	return this->visible;
}

void AppControl::UnknownControl::SetVisible(const bool v)
{
	this->visible = v;
}

AppControl::BottomW::BottomW(RECT r, const wchar_t* text, const wchar_t* bitmapAddr, BottomWFun clickFun, BottomWFun unclickFun, bool visible)
	:UnknownControl(r, AppControl::BOTTOMCONTROL, visible), clickFun(clickFun), unclickFun(unclickFun)
{
	CreateIcon(text, bitmapAddr);
}

AppControl::BottomW::~BottomW()
{
	if (bitmap)
	{
		bitmap->Release();
		bitmap = nullptr;
	}
}

bool AppControl::BottomW::Click(float x, float y)
{
	if (x >= addr.left && x <= addr.right && y >= addr.top && y <= addr.bottom)
	{
		clicking = true;
		if (clickFun)
			clickFun();
		return true;
	}
	else
		return false;
}

bool AppControl::BottomW::UnClick()
{
	if(clicking)
	{
		if (unclickFun)
			unclickFun();
		clicking = false;
	}
	return true;
}

bool AppControl::BottomW::Move(float x, float y)
{
	if (clicking && !(x >= addr.left && x <= addr.right && y >= addr.top && y <= addr.bottom))
	{
		clicking = false;
		return true;
	}
	return false;
}

void AppControl::BottomW::Draw()
{
	App::GetApp()->DrawRect(addr);
	if (stringW != L"")
		App::GetApp()->DrawTextW(stringW, addr, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, DWRITE_TEXT_ALIGNMENT_LEADING);
	if (bitmap)
		App::GetApp()->DrawBitmap(bitmap, addr);
}

bool AppControl::BottomW::GetClicking()
{
	return clicking;
}

void AppControl::BottomW::CreateIcon(const wchar_t* text, const wchar_t* bitmapAddr)
{
	if (text == nullptr || text == L"")
	{
		this->stringW = L"";
	}
	else
	{
		this->stringW = text;
	}

	if (bitmapAddr == nullptr || bitmapAddr == L"")
	{
		this->bitmap = nullptr;
	}
	else
	{
		App::GetApp()->CreateBitmapW(bitmapAddr, &bitmap);
	}
}

std::wstring AppControl::BottomW::GetStringW() const
{
	return stringW;
}

ID2D1Bitmap* AppControl::BottomW::GetBitmap() const
{
	return bitmap;
}

AppControl::LabelW::LabelW(RECT r, const wchar_t* text, const wchar_t* bitmapAddr, bool visible) :UnknownControl(r, AppControl::LABELCONTROL, visible)
{
	CreateIcon(text, bitmapAddr);
}

AppControl::LabelW::~LabelW()
{
	if (bitmap)
	{
		bitmap->Release();
		bitmap = nullptr;
	}
}

void AppControl::LabelW::Draw()
{
	if (stringW != L"")
		App::GetApp()->DrawTextW(stringW, addr, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, DWRITE_TEXT_ALIGNMENT_LEADING);
	if (bitmap)
		App::GetApp()->DrawBitmap(bitmap, addr);
}

void AppControl::LabelW::CreateIcon(const wchar_t* text, const wchar_t* bitmapAddr)
{
	if (text == nullptr || text == L"")
	{
		this->stringW = L"";
	}
	else
	{
		this->stringW = text;
	}

	if (bitmapAddr == nullptr || bitmapAddr == L"")
	{
		this->bitmap = nullptr;
	}
	else
	{
		App::GetApp()->CreateBitmapW(bitmapAddr, &bitmap);
	}
}

std::wstring AppControl::LabelW::GetStringW() const
{
	return stringW;
}

ID2D1Bitmap* AppControl::LabelW::GetBitmap() const
{
	return bitmap;
}

AppControl::TextBoxW::TextBoxW(RECT r, const wchar_t* initalText = L"", const wchar_t* helpText = L"", TextBoxWFun getText, UINT32 textLimit, bool visible)
	:UnknownControl(r, AppControl::TEXTBOXCONTROL, visible)/*, text(initalText)*/, helpText(helpText), sendText(getText),
	clicking(false), active(false)
{
	text = initalText;
	showText = false;
	if (this->text.length() == 0 && this->helpText.length() > 0)
	{
		App::GetApp()->CreateTextLayoutW(helpText, r.bottom - r.top, r.right - r.left, showText0.GetAddressOf());
		showHelpText = true;
	}
	else
	{
		/*HRESULT hr(S_OK);
		DWRITE_HIT_TEST_METRICS hitTestMetrics = { 0 };
		float pointx, pointy;*/
		App::GetApp()->CreateTextLayoutW(this->text, r.bottom - r.top, r.right - r.left, showText0.GetAddressOf());
		/*hr = showText0->HitTestTextPosition(
			text.length(),
			true,
			&pointx,
			&pointy,
			&hitTestMetrics
		);
		this->cursorEx = { hitTestMetrics.left, hitTestMetrics.top, hitTestMetrics.left + 2.0f, hitTestMetrics.top + hitTestMetrics.height };
		cursor = this->text.length() + 1;*/
		showHelpText = false;
	}
	cursor = 0;
	this->cursorEx = { 0.0 };
}

AppControl::TextBoxW::~TextBoxW()
{
}

bool AppControl::TextBoxW::Click(float x, float y)
{
	if (x > this->addr.right || x<this->addr.left || y>this->addr.bottom || y < this->addr.top)
	{
		active = false;
		clicking = false;
		return false;
	}

	HRESULT hr(S_OK);
	if (active)
	{
		if (!showHelpText)
		{
			clicking = true;
			DWRITE_HIT_TEST_METRICS hitTestMetrics = { 0 };
			BOOL isTrailingHit, isInside;
			hr = GetShowText()->HitTestPoint(
				x - addr.left,
				y - addr.top,
				&isTrailingHit,
				&isInside,
				&hitTestMetrics
			);
			if (FAILED(hr))
				throw hr;

			if (isTrailingHit)
			{
				this->cursorEx = {
					hitTestMetrics.left + hitTestMetrics.width + addr.left,
					hitTestMetrics.top + addr.top,
					hitTestMetrics.left + hitTestMetrics.width + 2.0f + addr.left,
					hitTestMetrics.top + hitTestMetrics.height + addr.top
				};
				cursor = hitTestMetrics.textPosition + 1;
				if (cursor > text.length() + 1)
				{
					cursor -= 1;
				}
			}
			else
			{
				this->cursorEx = {
					hitTestMetrics.left + addr.left,
					hitTestMetrics.top + addr.top,
					hitTestMetrics.left + 2.0f + addr.left,
					hitTestMetrics.top + hitTestMetrics.height + addr.top
				};
				cursor = hitTestMetrics.textPosition;
			}
		}
	}
	else
	{
		active = true;
		DWRITE_HIT_TEST_METRICS hitTestMetrics = { 0 };
		float pointx, pointy;
		hr = GetShowText()->HitTestTextPosition(
			showHelpText ? helpText.length() : text.length(),
			true,
			&pointx,
			&pointy,
			&hitTestMetrics
		);
		if (FAILED(hr))
			throw hr;

		if (!showHelpText)
		{
			this->cursorEx = {
				hitTestMetrics.left + hitTestMetrics.width + addr.left,
				hitTestMetrics.top + addr.top,
				hitTestMetrics.left + hitTestMetrics.width + 2.0f + addr.left,
				hitTestMetrics.top + hitTestMetrics.height + addr.top
			};
			cursor = text.length();
		}
	}
	return true;
}

bool AppControl::TextBoxW::UnClick()
{
	return false;
}

bool AppControl::TextBoxW::Move(float x, float y)
{
	return false;
}

bool AppControl::TextBoxW::InputChar(wchar_t c)
{
	HRESULT hr(S_OK);
	if (active && !iscontrol(c))
	{
		text.insert(cursor, 1, c);
		cursor++;
		SwitchShowText();

		const auto nowTextLayout(GetShowText());
		DWRITE_HIT_TEST_METRICS hitTestMetrics = { 0 };
		float pointx, pointy;
		nowTextLayout->HitTestTextPosition(
			cursor,
			true,
			&pointx,
			&pointy,
			&hitTestMetrics
		);
		this->cursorEx = {
			hitTestMetrics.left + addr.left,
			hitTestMetrics.top + addr.top,
			hitTestMetrics.left + 2.0f + addr.left,
			hitTestMetrics.top + hitTestMetrics.height + addr.top
		};
		return true;
	}
	else
		return false;
}

bool AppControl::TextBoxW::KeyDown(wchar_t c)
{
	if (!active)
		return false;

	if (c == VK_BACK && cursor > 0)
	{
		text.erase(cursor - 1, 1);
		cursor--;
		SwitchShowText();
	}
	else if (c == VK_LEFT && cursor > 0)
	{
		cursor--;
	}
	else if (c == VK_RIGHT && cursor < text.length())
	{
		cursor++;
	}
	else if (c == VK_DELETE && cursor < text.length())
	{
		text.erase(cursor, 1);
		SwitchShowText();
	}
	else if (c == VK_RETURN)
		sendText(text);
	else
		return false;

	const auto nowTextLayout(GetShowText());
	DWRITE_HIT_TEST_METRICS hitTestMetrics = { 0 };
	float pointx, pointy;
	nowTextLayout->HitTestTextPosition(
		cursor,
		true,
		&pointx,
		&pointy,
		&hitTestMetrics
	);
	this->cursorEx = {
		hitTestMetrics.left  + addr.left,
		hitTestMetrics.top + addr.top,
		hitTestMetrics.left  + 2.0f + addr.left,
		hitTestMetrics.top + hitTestMetrics.height + addr.top
	};
	return true;
}

void AppControl::TextBoxW::Draw()
{
	App::GetApp()->DrawRect(addr);
	App::GetApp()->DrawTextLayout(GetShowText(), POINTF{ (float)addr.left,(float)addr.top });
	if (active)
		App::GetApp()->DrawFillRect(cursorEx);
}

D2D_RECT_F AppControl::TextBoxW::GetCursor(bool* show)
{
	*show = active;
	return cursorEx;
}

IDWriteTextLayout* AppControl::TextBoxW::GetShowText()const
{
	if (showText)
		return showText1.Get();
	else
		return showText0.Get();
}

void AppControl::TextBoxW::SwitchShowText()
{
	auto& nextTextLayout(showText ? showText0 : showText1);
	App::GetApp()->CreateTextLayoutW(text, addr.bottom - addr.top, addr.right - addr.left, nextTextLayout.ReleaseAndGetAddressOf());
	showText = !showText;
}

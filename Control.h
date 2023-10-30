#pragma once
#include "framework.h"
#include "App.h"

namespace AppControl
{
	enum ControlType
	{
		TEXTBOXCONTROL,
		BOTTOMCONTROL,
		LABELCONTROL
	};

	class UnknownControl
	{
	public:
		UnknownControl(RECT r, ControlType type, bool visible);

	public:
		virtual bool Click(float x, float y) = 0;
		virtual bool UnClick() = 0;
		virtual bool Move(float x, float y) = 0;
		virtual bool InputChar(wchar_t c) = 0;
		virtual bool KeyDown(wchar_t c) = 0;
		virtual void Draw() = 0;

	public:
		RECT GetAddr();
		void SetAddr(const RECT& r);
	protected:
		RECT addr;

	public:
		ControlType GetControlType();
	protected:
		ControlType type;

	public:
		bool GetVisible();
		void SetVisible(const bool v);
	protected:
		bool visible;
	};

	typedef void(*BottomWFun)();
	class BottomW :public UnknownControl
	{
	public:
		BottomW(RECT r,const wchar_t* text,const wchar_t* bitmapAddr, BottomWFun clickFun = nullptr, BottomWFun unclickFun = nullptr, bool visible = true);
		~BottomW();

	public:
		bool Click(float x, float y)override;
		bool UnClick()override;
		bool Move(float x, float y)override;
		bool InputChar(wchar_t c)override { return false; }
		bool KeyDown(wchar_t c)override { return false; }
		void Draw()override;
		bool GetClicking();
	protected:
		bool clicking;
		BottomWFun clickFun;
		BottomWFun unclickFun;

	public:
		void CreateIcon(const wchar_t* text, const wchar_t* bitmapAddr);
		std::wstring GetStringW()const;
		ID2D1Bitmap* GetBitmap()const;
	protected:
		ID2D1Bitmap* bitmap;
		std::wstring stringW;
	};

	class LabelW :public UnknownControl
	{
	public:
		LabelW(RECT r, const wchar_t* text, const wchar_t* bitmapAddr, bool visible = true);
		~LabelW();

	public:
		bool Click(float x, float y)override { return false; }
		bool UnClick()override { return false; }
		bool Move(float x, float y)override { return false; }
		bool InputChar(wchar_t c)override { return false; }
		bool KeyDown(wchar_t c)override { return false; }
		void Draw()override;

	public:
		void CreateIcon(const wchar_t* text, const wchar_t* bitmapAddr);
		std::wstring GetStringW()const;
		ID2D1Bitmap* GetBitmap()const;
	protected:
		ID2D1Bitmap* bitmap;
		std::wstring stringW;
	};

	constexpr UINT32 NUMBER = 0x3;
	constexpr UINT32 ASCII = 0x1;
	constexpr UINT32 INTEGER = 0x7;
	constexpr UINT32 NONNEGATIVE = 0xB;
	typedef void(*TextBoxWFun)(std::wstring);
	class TextBoxW :public UnknownControl
	{
	public:
		TextBoxW(RECT r, const wchar_t* initalText, const wchar_t* helpText, TextBoxWFun getText = nullptr, UINT32 textLimit = 0x0, bool visible = true);
		~TextBoxW();

	public:
		bool Click(float x, float y)override;
		bool UnClick()override;
		bool Move(float x, float y)override;
		bool InputChar(wchar_t c)override;
		bool KeyDown(wchar_t c)override;
		void Draw()override;
	protected:
		bool clicking;
		std::wstring text;
		bool showHelpText;
		std::wstring helpText;
		TextBoxWFun sendText;

	public:
		D2D_RECT_F GetCursor(bool* show);
	protected:
		bool active;
		int cursor;
		D2D_RECT_F cursorEx;

	public:
		IDWriteTextLayout* GetShowText()const;
		void SwitchShowText();
	protected:
		bool showText;
		CComPtr<IDWriteTextLayout> showText0;
		CComPtr<IDWriteTextLayout> showText1;
	};
}

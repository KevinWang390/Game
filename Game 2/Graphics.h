#pragma once
#include <cmath>
#include <string>
#include <vector>
#include <unordered_map>
#include <Windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

class Graphics {
public:
	//initialization step, run once
	static void init(HWND hWnd) {

		//create factory and render target
		HRESULT h = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
		if (h != S_OK) return;
		RECT rect;
		GetClientRect(hWnd, &rect);
		factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rect.right, rect.bottom)),
			&rtarget);

		//create text stuff (no text format)
		h = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&wfactory));
		rtarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);

		//create WICFactory (for bitmaps);
		h = CoInitialize(NULL);
		h = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicfactory);		
	}

	//begin and end
	static void begin() {
		rtarget->BeginDraw();
	}
	static void end() {
		rtarget->EndDraw();
	}

	//setters and getters for brushes and text formats, using unordered_map
	static void setSolidColorBrush(float r, float g, float b, float alpha, std::string name) {
		ID2D1SolidColorBrush* brush;
		rtarget->CreateSolidColorBrush(D2D1::ColorF(r, g, b, alpha), &brush);
		brushes.emplace(name, brush);
	}
	static ID2D1SolidColorBrush* getSolidColorBrush(std::string name) {
		return brushes[name];
	}
	static void setTextFormat(const wchar_t* font, float size, std::string name) {
		IDWriteTextFormat* txtformat;
		wfactory->CreateTextFormat(font, NULL, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"en-us", &txtformat);
		textFormats.emplace(name, txtformat);
	}
	static IDWriteTextFormat* getTextFormat(std::string name) {
		return textFormats[name];
	}

	//bitmap related stuff
	static ID2D1Bitmap* bitmapFromFilename(const wchar_t* f) {
		ID2D1Bitmap* bitmap;
		wicfactory->CreateDecoderFromFilename(f, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &(wicdecoder));
		wicdecoder->GetFrame(0, &wicframe);
		wicfactory->CreateFormatConverter(&wicconverter);
		wicconverter->Initialize(wicframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom);
		rtarget->CreateBitmapFromWicBitmap(wicconverter, &bitmap);
		wicdecoder->Release();
		wicframe->Release();
		wicconverter->Release();
		return bitmap;
	}

	//stuff for normal rendering
	static ID2D1Factory* factory;
	static ID2D1HwndRenderTarget* rtarget;
private:

	static std::unordered_map<std::string, ID2D1SolidColorBrush*> brushes;
	//crap for text
	static IDWriteFactory* wfactory;
	static std::unordered_map<std::string, IDWriteTextFormat*> textFormats;
	//junk for bitmaps
	static IWICImagingFactory* wicfactory;
	static IWICBitmapDecoder* wicdecoder;
	static IWICBitmapFrameDecode* wicframe;
	static IWICFormatConverter* wicconverter;
};

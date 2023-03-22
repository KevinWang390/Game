#include "Graphics.h"

ID2D1Factory* Graphics::factory;
ID2D1HwndRenderTarget* Graphics::rtarget;
std::unordered_map<std::string, ID2D1SolidColorBrush*> Graphics::brushes;
IDWriteFactory* Graphics::wfactory;
std::unordered_map<std::string, IDWriteTextFormat*> Graphics::textFormats;
IWICImagingFactory* Graphics::wicfactory;
IWICBitmapDecoder* Graphics::wicdecoder;
IWICBitmapFrameDecode* Graphics::wicframe;
IWICFormatConverter* Graphics::wicconverter;
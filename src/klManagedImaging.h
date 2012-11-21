// klManagedImaging.h

#pragma once

using namespace System;

using namespace System::Drawing;
using namespace System::Runtime::InteropServices;


namespace klManagedImaging {

	public ref class klImageTile
	{
	public:
		klImageTile(int x0,int y0,int w,int h, String^ id,array<Byte>^ bytes)
		{
			TileWidth = w;
			TileHeight = h;
			TileTop = y0;
			TileLeft = x0;
			Identifier = id;
			rgbValues = bytes;
		}

		int TileWidth;
        int TileHeight;
        int TileTop ;
        int TileLeft ;
        String^ Identifier;
        double TileScale;
		array<Byte>^ rgbValues;
	};

	public ref class klImageOp
	{
	public:
		void __clrcall Init(String^ filename, int w,int h);
		int __clrcall Operate(Bitmap^ img,String^ op);
		int __clrcall OperateTile(klImageTile^ tile,String^ op);

	};
}

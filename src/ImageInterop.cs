using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using klManagedImaging;
using System.Drawing;

namespace ImageInterop
{
    class TestManagedImageing
    {
        static void test(string[] args)
        {
            Bitmap img = new Bitmap("C:\\temp\\img.jpg");
            int x0=0;
            int y0=0;
            int w=img.Width;
            int h=img.Height;
            String id = "img";
            System.Drawing.Imaging.PixelFormat format= img.PixelFormat;
            Rectangle rect= new Rectangle(0, 0,w, h);
	        BitmapData bmd=img.LockBits(rect,System.Drawing.Imaging.ImageLockMode.ReadWrite, format);
	        IntPtr data = bmd.Scan0;
	        int bytes = img.Width * img.Height * 3;
		    byte[] _imageBuffer = new byte[bytes];
            IntPtr bmpdata = new IntPtr();
            bmpdata =bmd.Scan0;
		    System.Runtime.InteropServices.Marshal.Copy( bmpdata, _imageBuffer, 0, bytes );
		    img.Save("c:/temp/ippManaged_ProcessedImage.jpg",	System.Drawing.Imaging.ImageFormat.Jpeg);
		    img.UnlockBits( bmd);
	        klImageTile tile = new klImageTile(x0, y0, w, h, id, _imageBuffer);
            klImageOp kliop=new klImageOp();
            kliop.Init("c:\\temp\\out.rts",w,h);
            kliop.OperateTile(tile, "op");
            kliop.OperateTile(tile, "op");

        }
    }
}

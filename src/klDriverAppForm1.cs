using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using graphTheory;
using System.Runtime.InteropServices;
using RandomMatrixTheory;
using Bluebit.MatrixLibrary;
using System.IO;

using System.Runtime.Serialization;

using System.Runtime.Serialization.Formatters.Binary;

//using System.Runtime.Serialization.Formatters.

namespace klDriverApp
{
    public partial class Form1 : Form
    {
        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern int BitBlt(IntPtr hdcDst, int xDst, int yDst, int w, int h, IntPtr hdcSrc, int xSrc, int ySrc, int rop);
        static int SRCCOPY = 0x00CC0020;

        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern bool DeleteObject(IntPtr hObject);

        [System.Runtime.InteropServices.DllImport("user32.dll")]
        public static extern int InvalidateRect(IntPtr hwnd, IntPtr rect, int bErase);

        [System.Runtime.InteropServices.DllImport("user32.dll")]
        public static extern IntPtr GetDC(IntPtr hwnd);

        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern IntPtr CreateCompatibleDC(IntPtr hdc);

        [System.Runtime.InteropServices.DllImport("user32.dll")]
        public static extern int ReleaseDC(IntPtr hwnd, IntPtr hdc);

        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern int DeleteDC(IntPtr hdc);

        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern IntPtr SelectObject(IntPtr hdc, IntPtr hgdiobj);

        [System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
        public struct BITMAPINFO
        {
            public uint biSize;
            public int biWidth, biHeight;
            public short biPlanes, biBitCount;
            public uint biCompression, biSizeImage;
            public int biXPelsPerMeter, biYPelsPerMeter;
            public uint biClrUsed, biClrImportant;
            [System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.ByValArray, SizeConst = 256)]
            public uint[] cols;
        }

        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        static extern IntPtr CreateDIBSection(IntPtr hdc, ref BITMAPINFO bmi, uint Usage, out IntPtr bits, IntPtr hSection, uint dwOffset);
        static uint BI_RGB = 0;
        static uint DIB_RGB_COLORS = 0;
        static uint MAKERGB(int r, int g, int b)
        {
            return ((uint)(b & 255)) | ((uint)((r & 255) << 8)) | ((uint)((g & 255) << 16));
        }


        public Form1()
        {
            InitializeComponent();
        }

        private void KevinBaconGraph_Click(object sender, EventArgs e)
        {
            graphTheory.SmallWorldMatrix kevinBacon = new SmallWorldMatrix();
            String kevinBaconJpg = kevinBacon.KevinBacon();
            Bitmap img = new Bitmap(kevinBaconJpg);
            pictureBox1.Image = img;



        }

        private void GaussianRM_Click(object sender, EventArgs e)
        {
            int width = 512;
            int height = 1024;
            int bands = 1;

            RandomMatrixTheory.RandomMatrix rm = new RandomMatrix();
            
            Matrix mat = rm.Gaussian(height,width );
            
            FileStream fs= new FileStream("c:/temp/MatSerialized.txt",FileMode.Create);

            BinaryFormatter formatter = new BinaryFormatter();
            formatter.Serialize(fs, mat);

   //          FileStream^ fs = gcnew FileStream( "DataFile.dat",FileMode::Create );
   //try
   //{
   //   // Construct a BinaryFormatter and use it 
   //   // to serialize the data to the stream.
   //   BinaryFormatter^ formatter = gcnew BinaryFormatter;

   //   // Create an array with multiple elements refering to 
   //   // the one Singleton Object*.
   //   array<Singleton^>^a1 = {Singleton::GetSingleton(),Singleton::GetSingleton()};

   //   // This displays S"True".
   //   Console::WriteLine( "Do both array elements refer to the same Object? {0}", (a1[ 0 ] == a1[ 1 ]) );

   //   // Serialize the array elements.
   //   formatter->Serialize( fs, a1 );
   //     }






            Bitmap bmp = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format8bppIndexed); //       .Format16bppGrayScale);//   .Format32bppGrayScale);//   .Format24bppRgb);
            System.Drawing.Imaging.BitmapData bmpData = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), System.Drawing.Imaging.ImageLockMode.ReadWrite, bmp.PixelFormat);
            int stride = bmpData.Stride;
            byte[] rgbValues = new byte[stride* bmp.Height];
            //short[] rgbValues = new short[stride/sizeof(short) * bmp.Height];
            //fill rgbValues; make sure to jump (stride - width * 3) after each line
            Double[,] greyVal = mat.ToArray( ); 
            int rgbIndexer=0;
            //int strideJumpBytes = (stride / sizeof(short) - width);
            int strideJumpBytes = (stride  - width);
            for (int x = 0; x < greyVal.GetLength(0); x += 1)
            {
                Vector row = mat.RowVector(x);
                for (int y = 0; y < greyVal.GetLength(1); y += 1)
                {



                   rgbValues[rgbIndexer]=  (byte)row[y];
                   rgbIndexer += 1;
                  // rgbValues[rgbIndexer] = (byte)row[y];
                  //rgbIndexer += 1;
                 //  rgbValues[rgbIndexer] = (byte)row[y];
                 //  rgbIndexer += 1;
                   
                }
                rgbIndexer += strideJumpBytes;



            }     
            
            IntPtr ptr = bmpData.Scan0;
            System.Drawing.Imaging.ColorPalette myPal=bmp.Palette;
            int ncols = 256;
            Color[] colArray = new Color[ncols];
            //System.Runtime.InteropServices.Marshal.Copy(rgbValues, 0, ptr, stride/sizeof(short) * bmp.Height);
            for (int i = 0; i < ncols; i++) 
            {
                uint col=  MAKERGB(i, i, i);
                System.Drawing.Color myCol =Color.FromArgb( 0, (int)col, (int)col,(int) col );
                //colArray[i]=myCol;
                myPal.Entries[i] = myCol;
            
                  
            } 
            //colPal.Entries=colArray;
            bmp.Palette = myPal;

            System.Runtime.InteropServices.Marshal.Copy(rgbValues, 0, ptr, stride * bmp.Height);
            bmp.UnlockBits(bmpData);
            pictureBox1.Image = bmp;


            //ColorPalette pal = b.Palette;
            //for (int i = 0; i < 256; i++)
            //    pal.Entries[i] = Color.FromArgb(255, i, i, i);
            //b.Palette = pal;



            //Bitmap img = new Bitmap(width, height);//, System.Drawing.Imaging.PixelFormat.Format32bppPArgb);//   .Format32bppGrayScale);
            //System.Drawing.Imaging.PixelFormat format = img.PixelFormat;
            //System.Drawing.Rectangle rect = new Rectangle(0, 0, width, height);
            //System.Drawing.Imaging.BitmapData bmd = img.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite, format);
            //IntPtr data = bmd.Scan0;
            //Double[,] greyVal = mat.ToArray();
            //int bytes = img.Width * img.Height * bands;
            //Int32 pdata = data.ToInt32();
            //unsafe
            //{
            //    int* pint = (int*)data.ToPointer();
            //    for (int x = 0; x < greyVal.GetLength(0); x += 1)
            //    {
            //        for (int y = 0; y < greyVal.GetLength(1); y += 1)
            //        {



            //            *pint = (int)greyVal[x, y];
            //            pint += 1;
            //        }
            //        //  data.ToPointer();

            //        //data*(counter) = 0;// greyVal[counter];//   [counter];

            //    }
            //}//end unsafe
            //img.UnlockBits(bmd);




            bmp.Save("c:/temp/gaussianBitmap.jpg");
                //("c:/temp/gaussianBitmap.jpg",System.Drawing.Imaging.ImageFormat.Jpeg);
            
                    
            
            
            //pictureBox1.Image = img;




            //Bitmap b = img;
            //int bpp = 8;//&& bpp != 1
            //// Built into Windows GDI is the ability to convert bitmaps
            //// this job is actually done by the graphics hardware accelerator card
            //// and so is extremely fast. The rest of the time, the job is done by
            //// very fast native code.
            //int w = b.Width, h = b.Height;
            //IntPtr hbm = b.GetHbitmap();
            //BITMAPINFO bmi = new BITMAPINFO();
            //bmi.biSize = 40;
            //bmi.biWidth = w;
            //bmi.biHeight = h;
            //bmi.biPlanes = 1;
            //bmi.biBitCount = (short)bpp; // ie. 1bpp or 8bpp
            //bmi.biCompression = BI_RGB; // ie. the pixels in our RGBQUAD table are stored as RGBs, not palette indexes
            //bmi.biSizeImage = (uint)(((w + 7) & 0xFFFFFFF8) * h / 8);
            //bmi.biXPelsPerMeter = 1000000; // not really important
            //bmi.biYPelsPerMeter = 1000000; // not really important
            //// Now for the colour table.
            //uint ncols = (uint)1 << bpp; // 2 colours for 1bpp; 256 colours for 8bpp
            //bmi.biClrUsed = ncols;
            //bmi.biClrImportant = ncols;
            //bmi.cols = new uint[256];
            //IntPtr bits0;
            //IntPtr hbm0 = CreateDIBSection(IntPtr.Zero, ref bmi, DIB_RGB_COLORS, out bits0, IntPtr.Zero, 0);
            //IntPtr sdc = GetDC(IntPtr.Zero);
            //IntPtr hdc = CreateCompatibleDC(sdc); SelectObject(hdc, hbm);
            //IntPtr hdc0 = CreateCompatibleDC(sdc); SelectObject(hdc0, hbm0);

            //int xDst = 0;
            //int yDst = 0;

            //int xSrc = 0;
            //int ySrc = 0;
            //int rop = SRCCOPY;

            //int and = BitBlt(hdc0, xDst, yDst, w, h, hdc, xSrc, ySrc, rop);




            ////Copy(greyVal, data, 0, bytes);
        }

    }//end class
}//end namespace
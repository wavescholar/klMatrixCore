using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.Runtime.InteropServices;

//using WatermarkDllManaged;
using WatermarkClassLibrary;

using ippManagedDll;
using OpenCVWrapper;
//using OpenCVManaged;

namespace WindowsApplication1
{  
    public partial class Form1 : Form
    { 
        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
    public static extern int BitBlt(IntPtr hdcDst, int xDst, int yDst, int w, int h, IntPtr hdcSrc, int xSrc, int ySrc, int rop);

        //[DllImport("WatermarkDllUnmanaged.dll", EntryPoint = "ApplyWatermark")]
        //unsafe public static extern int ApplyWatermark(char* filename);
        //[DllImport("WatermarkDllUnmanaged.dll", EntryPoint = "ApplyWatermark2")]
        //unsafe public static extern void ApplyWatermark();
        //[DllImport("ippDLL.dll", EntryPoint = "ippMain")] unsafe public static extern int ippMain();
        //[DllImport("ippDLL.dll", EntryPoint = "ippMain")] unsafe public static extern int ippMain();
        //[DllImport("ippDLL.dll", EntryPoint = "openCVFaceDetect")]  unsafe public static extern int openCVFaceDetect();
        //[DllImport("ippManagedDLL.dll", EntryPoint = "ippWienerFilter")]        
        public Form1()
        {
            InitializeComponent();
        }
        public float[] RMS_ERROR;
        private void WaterMark_Click(object sender, EventArgs e)
        {



            Bitmap img=new Bitmap(openFileDialog1.FileName);
            Bitmap origImag = new Bitmap(img);
            ippWrapper myIPPWrapper=new ippWrapper();
            img.Save("c:/temp/WatermarkInputImage.jpg", System.Drawing.Imaging.ImageFormat.Jpeg);
            
            
            //Need a watermark implementation - this one was unwieldy
            //WatermarkClassLibrary.WatermarkManagedClass myWatermarkClass=new WatermarkManagedClass(img);
            //myWatermarkClass.ApplyWatermarkManaged(openFileDialog1.FileName);
			
            //myIPPWrapper.ippWienerFilter(img);
            //pictureBox3.Image= img;
            
            //SplitterPanel PANEL1 = splitContainer1.Panel1;
            //PANEL1.Show();
            //Graphics panel1Graphics = PANEL1.CreateGraphics();

            //panel1Graphics.DrawImage(origImag, 0, 0);

            //SplitterPanel PANEL2 = splitContainer1.Panel2;
            //PANEL2.Show();
            //Graphics panel2Graphics = PANEL1.CreateGraphics();

            //panel2Graphics.DrawImage(img,819, 0);
            //DrawImageUnscaledAndClipped(img);

            //= pictureBox1;
                //Contains(pictureBox1);
           // pictureBox2.Image = img;


           // OpenCV myOpenCVWrapper=new OpenCV();
           // myOpenCVWrapper.openCVFaceDetect(img);


            //WatermarkClassLibrary.WatermarkManagedClass myWatermarkClass=new WatermarkManagedClass(img);
	        //myWatermarkClass.ApplyWatermarkManaged(openFileDialog1.FileName);
			
            

            //unsafe
            //{
            //    char[] myFilename = new char[2048];// = new char(2048);// "test.jpg";
            //    //fixed(char* myFilename2 = new char(2048))

            //    fixed (char* myFilename2 = &myFilename[0])
            //    {
            //        //myFilename = 
            //        // openFileDialog1.FileName.CopyTo(1,myFilename,1,openFileDialog1.FileName.Length);
            //        myFilename = openFileDialog1.FileName.ToCharArray();
            //        //fixed (char* p = &myFilename)  // equivalent to p = &str[0]

            //        //This is the unmanaged call
            //        //ApplyWatermark(myFilename2);


            //        Bitmap img = new Bitmap(openFileDialog1.FileName);
            //  ;      //WatermarkManagedClass myManaged = new WatermarkManagedClass(img);
            //        //string file = openFileDialog1.FileName;
            //        //myManaged.ApplyWatermarkManaged(file);




            //    }
            //}
            ////ApplyWatermark2();
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {

        }

        private void OpenImage_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
            //Send to watermarking prog.
            Image img=new Bitmap(openFileDialog1.FileName);
            //splitContainer1.Panel1 = pictureBox1;
            
            pictureBox1.Image = img;    

            //SplitterPanel PANEL1 = splitContainer1.Panel1;
            //PANEL1.Show();
            //Graphics panel1Graphics = PANEL1.CreateGraphics();
            //panel1Graphics.DrawImage(img, 0, 0);
                
                //Contains(pictureBox1);
            unsafe
            {
                char[] myFilename=new char[2048];// = new char(2048);// "test.jpg";
                //fixed(char* myFilename2 = new char(2048))
                 
                fixed ( char* myFilename2 = &myFilename[0]) 
                {
                //myFilename = 
               // openFileDialog1.FileName.CopyTo(1,myFilename,1,openFileDialog1.FileName.Length);
                    myFilename=openFileDialog1.FileName.ToCharArray();
                //fixed (char* p = &myFilename)  // equivalent to p = &str[0]
                //ApplyWatermark(myFilename2);
                }
            }
            //ApplyWatermark2();
            
            CancelEventArgs ce = new CancelEventArgs();
            openFileDialog1_FileOk(sender, ce);

        }

        

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void IPP_Process_Click(object sender, EventArgs e)
        {
            Bitmap img = new Bitmap(openFileDialog1.FileName);
            Bitmap origImag = new Bitmap(img);
            ippWrapper myIPPWrapper = new ippWrapper();
            img.Save("c:/temp/ippInputImage.jpg", System.Drawing.Imaging.ImageFormat.Jpeg);
            myIPPWrapper.ippWienerFilter(img);
            pictureBox3.Image = img;  
        }

        private void OpenCV_Process_Click(object sender, EventArgs e)
        {
            Bitmap img = new Bitmap(openFileDialog1.FileName);
            Bitmap origImag = new Bitmap(img);
            OpenCVWrapper.OpenCV openCVWrapper = new OpenCV();
            img.Save("c:/temp/OpenCVInputImage.jpg", System.Drawing.Imaging.ImageFormat.Jpeg);
            int ans = openCVWrapper.openCVFaceDetect(img);
            pictureBox3.Image = img; 
            

        }

        private void ImageStats_Click(object sender, EventArgs e)
        {
            //Bitmap img = new Bitmap(openFileDialog1.FileName);
            //Bitmap origImag = new Bitmap(img);
            Bitmap img = (Bitmap)pictureBox3.Image;
            Bitmap img2 = (Bitmap) pictureBox1.Image;
            ippWrapper myIPPWrapper = new ippWrapper();
            //img.Save("c:/temp/ippInputImage.jpg", System.Drawing.Imaging.ImageFormat.Jpeg);
            RMS_ERROR = new float[3];
            myIPPWrapper.ippImageCompare(img,img2,RMS_ERROR);
            textBox1.AppendText(RMS_ERROR[0].ToString());
            textBox2.AppendText(RMS_ERROR[1].ToString());
            textBox3.AppendText(RMS_ERROR[2].ToString());

            //RMS_ERROR[1], RMS_ERROR[2]);
            //listViewItem1 = RMS_ERROR[0];
            //pictureBox3.Image = img;  
            //listBox1.ValueMember =RMS_ERROR[0];

        }

        private void OpenImage2_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
          
            Image img = new Bitmap(openFileDialog1.FileName);
        

            pictureBox3.Image = img;

            unsafe
            {
                char[] myFilename = new char[2048];
                fixed (char* myFilename2 = &myFilename[0])
                {
                    myFilename = openFileDialog1.FileName.ToCharArray();
                }
            }

            CancelEventArgs ce = new CancelEventArgs();
            openFileDialog1_FileOk(sender, ce);

        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void form1BindingSource_CurrentChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void GDI_Bitmap_Click(object sender, EventArgs e)
        {
            //openFileDialog1.ShowDialog();
            //Send to watermarking prog.
            Bitmap img=new Bitmap(openFileDialog1.FileName);
            pictureBox1.Image = img; 
     
           // OneBBP_GDI_BitmapConversion gdicl = new OneBBP_GDI_BitmapConversion();
           Bitmap img2=OneBBP_GDI_BitmapConversion.MainConv(img);
           pictureBox3.Image=img2;
        }
    }
}
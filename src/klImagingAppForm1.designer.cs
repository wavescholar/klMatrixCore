namespace WindowsApplication1
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.WaterMark = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.OpenImage = new System.Windows.Forms.Button();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.pictureBox3 = new System.Windows.Forms.PictureBox();
            this.IPP_Process = new System.Windows.Forms.Button();
            this.OpenCV_Process = new System.Windows.Forms.Button();
            this.ImageStats = new System.Windows.Forms.Button();
            this.OpenImage2 = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.GDI_Bitmap = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox3)).BeginInit();
            this.SuspendLayout();
            // 
            // WaterMark
            // 
            this.WaterMark.Location = new System.Drawing.Point(2, 157);
            this.WaterMark.Name = "WaterMark";
            this.WaterMark.Size = new System.Drawing.Size(85, 23);
            this.WaterMark.TabIndex = 0;
            this.WaterMark.Text = "WaterMark";
            this.WaterMark.UseVisualStyleBackColor = true;
            this.WaterMark.Click += new System.EventHandler(this.WaterMark_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            this.openFileDialog1.RestoreDirectory = true;
            this.openFileDialog1.ShowHelp = true;
            this.openFileDialog1.Title = "OpenFile";
            this.openFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog1_FileOk);
            // 
            // OpenImage
            // 
            this.OpenImage.Location = new System.Drawing.Point(2, 12);
            this.OpenImage.Name = "OpenImage";
            this.OpenImage.Size = new System.Drawing.Size(85, 23);
            this.OpenImage.TabIndex = 1;
            this.OpenImage.Text = "Open Image";
            this.OpenImage.UseVisualStyleBackColor = true;
            this.OpenImage.Click += new System.EventHandler(this.OpenImage_Click);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(93, 1);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(778, 1047);
            this.pictureBox1.TabIndex = 5;
            this.pictureBox1.TabStop = false;
            // 
            // pictureBox3
            // 
            this.pictureBox3.Location = new System.Drawing.Point(886, 1);
            this.pictureBox3.Name = "pictureBox3";
            this.pictureBox3.Size = new System.Drawing.Size(670, 1047);
            this.pictureBox3.TabIndex = 6;
            this.pictureBox3.TabStop = false;
            // 
            // IPP_Process
            // 
            this.IPP_Process.Location = new System.Drawing.Point(2, 70);
            this.IPP_Process.Name = "IPP_Process";
            this.IPP_Process.Size = new System.Drawing.Size(85, 23);
            this.IPP_Process.TabIndex = 7;
            this.IPP_Process.Text = "IPP_Process";
            this.IPP_Process.UseVisualStyleBackColor = true;
            this.IPP_Process.Click += new System.EventHandler(this.IPP_Process_Click);
            // 
            // OpenCV_Process
            // 
            this.OpenCV_Process.Location = new System.Drawing.Point(2, 99);
            this.OpenCV_Process.Name = "OpenCV_Process";
            this.OpenCV_Process.Size = new System.Drawing.Size(85, 23);
            this.OpenCV_Process.TabIndex = 8;
            this.OpenCV_Process.Text = "OpenCV_Process";
            this.OpenCV_Process.UseVisualStyleBackColor = true;
            this.OpenCV_Process.Click += new System.EventHandler(this.OpenCV_Process_Click);
            // 
            // ImageStats
            // 
            this.ImageStats.Location = new System.Drawing.Point(2, 332);
            this.ImageStats.Name = "ImageStats";
            this.ImageStats.Size = new System.Drawing.Size(85, 23);
            this.ImageStats.TabIndex = 10;
            this.ImageStats.Text = "ImageStats";
            this.ImageStats.UseVisualStyleBackColor = true;
            this.ImageStats.Click += new System.EventHandler(this.ImageStats_Click);
            // 
            // OpenImage2
            // 
            this.OpenImage2.Location = new System.Drawing.Point(2, 41);
            this.OpenImage2.Name = "OpenImage2";
            this.OpenImage2.Size = new System.Drawing.Size(85, 23);
            this.OpenImage2.TabIndex = 11;
            this.OpenImage2.Text = "OpenImage2";
            this.OpenImage2.UseVisualStyleBackColor = true;
            this.OpenImage2.Click += new System.EventHandler(this.OpenImage2_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(2, 379);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(100, 20);
            this.textBox1.TabIndex = 13;
            this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(2, 406);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(100, 20);
            this.textBox2.TabIndex = 14;
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(2, 433);
            this.textBox3.Name = "textBox3";
            this.textBox3.Size = new System.Drawing.Size(100, 20);
            this.textBox3.TabIndex = 15;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(2, 362);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(117, 13);
            this.label1.TabIndex = 16;
            this.label1.Text = "RGB - Ell 2  RMS Error ";
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // GDI_Bitmap
            // 
            this.GDI_Bitmap.Location = new System.Drawing.Point(2, 219);
            this.GDI_Bitmap.Name = "GDI_Bitmap";
            this.GDI_Bitmap.Size = new System.Drawing.Size(82, 23);
            this.GDI_Bitmap.TabIndex = 18;
            this.GDI_Bitmap.Text = "GDI_Bitmap";
            this.GDI_Bitmap.UseVisualStyleBackColor = true;
            this.GDI_Bitmap.Click += new System.EventHandler(this.GDI_Bitmap_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1604, 1168);
            this.Controls.Add(this.GDI_Bitmap);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBox3);
            this.Controls.Add(this.textBox2);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.OpenImage2);
            this.Controls.Add(this.ImageStats);
            this.Controls.Add(this.OpenCV_Process);
            this.Controls.Add(this.IPP_Process);
            this.Controls.Add(this.pictureBox3);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.OpenImage);
            this.Controls.Add(this.WaterMark);
            this.IsMdiContainer = true;
            this.Name = "Form1";
            this.Text = "Form1";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox3)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button WaterMark;
        private System.Windows.Forms.Button OpenImage;
        public System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.PictureBox pictureBox3;
        private System.Windows.Forms.Button IPP_Process;
        private System.Windows.Forms.Button OpenCV_Process;
        private System.Windows.Forms.Button ImageStats;
        private System.Windows.Forms.Button OpenImage2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button GDI_Bitmap;
    }
}


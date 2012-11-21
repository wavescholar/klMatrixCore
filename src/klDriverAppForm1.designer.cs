namespace klDriverApp
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
            this.KevinBaconGraph = new System.Windows.Forms.Button();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.GaussianRM = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // KevinBaconGraph
            // 
            this.KevinBaconGraph.Location = new System.Drawing.Point(13, 31);
            this.KevinBaconGraph.Name = "KevinBaconGraph";
            this.KevinBaconGraph.Size = new System.Drawing.Size(75, 23);
            this.KevinBaconGraph.TabIndex = 0;
            this.KevinBaconGraph.Text = "KevinBaconGraph";
            this.KevinBaconGraph.UseVisualStyleBackColor = true;
            this.KevinBaconGraph.Click += new System.EventHandler(this.KevinBaconGraph_Click);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox1.Location = new System.Drawing.Point(0, 0);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(1312, 755);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 1;
            this.pictureBox1.TabStop = false;
            // 
            // GaussianRM
            // 
            this.GaussianRM.Location = new System.Drawing.Point(12, 60);
            this.GaussianRM.Name = "GaussianRM";
            this.GaussianRM.Size = new System.Drawing.Size(75, 23);
            this.GaussianRM.TabIndex = 2;
            this.GaussianRM.Text = "GaussianRM";
            this.GaussianRM.UseVisualStyleBackColor = true;
            this.GaussianRM.Click += new System.EventHandler(this.GaussianRM_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1312, 755);
            this.Controls.Add(this.GaussianRM);
            this.Controls.Add(this.KevinBaconGraph);
            this.Controls.Add(this.pictureBox1);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button KevinBaconGraph;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button GaussianRM;
    }
}


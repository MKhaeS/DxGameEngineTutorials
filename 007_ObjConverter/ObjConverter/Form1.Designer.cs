namespace ObjConverter {
    partial class Form1 {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose( bool disposing ) {
            if (disposing && (components != null)) {
                components.Dispose( );
            }
            base.Dispose( disposing );
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.textBoxFileName = new System.Windows.Forms.TextBox();
            this.buttonOpenFile = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.buttonConvert = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBoxFileName
            // 
            this.textBoxFileName.Location = new System.Drawing.Point(12, 12);
            this.textBoxFileName.Name = "textBoxFileName";
            this.textBoxFileName.Size = new System.Drawing.Size(641, 20);
            this.textBoxFileName.TabIndex = 0;
            this.textBoxFileName.Text = "H:\\Projects\\Development\\3dGameTutorials\\007_ObjConverter\\tavern.obj";
            // 
            // buttonOpenFile
            // 
            this.buttonOpenFile.Location = new System.Drawing.Point(12, 38);
            this.buttonOpenFile.Name = "buttonOpenFile";
            this.buttonOpenFile.Size = new System.Drawing.Size(83, 25);
            this.buttonOpenFile.TabIndex = 1;
            this.buttonOpenFile.Text = "Browse";
            this.buttonOpenFile.UseVisualStyleBackColor = true;
            this.buttonOpenFile.Click += new System.EventHandler(this.buttonOpenFile_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            this.openFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog1_FileOk);
            // 
            // buttonConvert
            // 
            this.buttonConvert.Location = new System.Drawing.Point(101, 38);
            this.buttonConvert.Name = "buttonConvert";
            this.buttonConvert.Size = new System.Drawing.Size(83, 25);
            this.buttonConvert.TabIndex = 2;
            this.buttonConvert.Text = "Convert";
            this.buttonConvert.UseVisualStyleBackColor = true;
            this.buttonConvert.Click += new System.EventHandler(this.buttonConvert_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(676, 74);
            this.Controls.Add(this.buttonConvert);
            this.Controls.Add(this.buttonOpenFile);
            this.Controls.Add(this.textBoxFileName);
            this.Name = "Form1";
            this.Text = "ObjConverter";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxFileName;
        private System.Windows.Forms.Button buttonOpenFile;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button buttonConvert;
    }
}


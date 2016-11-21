namespace CyBLE_MTK_Application
{
    partial class MTKTestProgramAllDialog
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
            this.CloseButton = new System.Windows.Forms.Button();
            this.OKButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.BegningRadioButton = new System.Windows.Forms.RadioButton();
            this.EndRadioButton = new System.Windows.Forms.RadioButton();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // CloseButton
            // 
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CloseButton.Location = new System.Drawing.Point(114, 106);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 35;
            this.CloseButton.Text = "&Cancel";
            this.CloseButton.UseVisualStyleBackColor = true;
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(33, 106);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 34;
            this.OKButton.Text = "&OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.EndRadioButton);
            this.groupBox1.Controls.Add(this.BegningRadioButton);
            this.groupBox1.Location = new System.Drawing.Point(13, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(197, 87);
            this.groupBox1.TabIndex = 36;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Program all devices at the";
            // 
            // BegningRadioButton
            // 
            this.BegningRadioButton.AutoSize = true;
            this.BegningRadioButton.Checked = true;
            this.BegningRadioButton.Location = new System.Drawing.Point(32, 27);
            this.BegningRadioButton.Name = "BegningRadioButton";
            this.BegningRadioButton.Size = new System.Drawing.Size(132, 17);
            this.BegningRadioButton.TabIndex = 0;
            this.BegningRadioButton.TabStop = true;
            this.BegningRadioButton.Text = "begning of each batch";
            this.BegningRadioButton.UseVisualStyleBackColor = true;
            this.BegningRadioButton.CheckedChanged += new System.EventHandler(this.BegningRadioButton_CheckedChanged);
            // 
            // EndRadioButton
            // 
            this.EndRadioButton.AutoSize = true;
            this.EndRadioButton.Location = new System.Drawing.Point(32, 50);
            this.EndRadioButton.Name = "EndRadioButton";
            this.EndRadioButton.Size = new System.Drawing.Size(112, 17);
            this.EndRadioButton.TabIndex = 0;
            this.EndRadioButton.TabStop = true;
            this.EndRadioButton.Text = "end of each batch";
            this.EndRadioButton.UseVisualStyleBackColor = true;
            this.EndRadioButton.CheckedChanged += new System.EventHandler(this.EndRadioButton_CheckedChanged);
            // 
            // MTKTestProgramAllDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(222, 141);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.CloseButton);
            this.Controls.Add(this.OKButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MTKTestProgramAllDialog";
            this.Text = "Program All Devices";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button CloseButton;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton BegningRadioButton;
        private System.Windows.Forms.RadioButton EndRadioButton;
    }
}
namespace CyBLE_MTK_Application
{
    partial class MTKTestXOCalDialog
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
            this.ErrorMarginNumeric = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.CloseButton = new System.Windows.Forms.Button();
            this.OKButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.ScriptOpenButton = new System.Windows.Forms.Button();
            this.ScriptPathTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.ErrorMarginNumeric)).BeginInit();
            this.SuspendLayout();
            // 
            // ErrorMarginNumeric
            // 
            this.ErrorMarginNumeric.Location = new System.Drawing.Point(168, 12);
            this.ErrorMarginNumeric.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.ErrorMarginNumeric.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.ErrorMarginNumeric.Name = "ErrorMarginNumeric";
            this.ErrorMarginNumeric.Size = new System.Drawing.Size(94, 20);
            this.ErrorMarginNumeric.TabIndex = 36;
            this.ErrorMarginNumeric.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(61, 14);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(101, 13);
            this.label3.TabIndex = 37;
            this.label3.Text = "Margin Of Error (+/-)";
            // 
            // CloseButton
            // 
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CloseButton.Location = new System.Drawing.Point(181, 79);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 35;
            this.CloseButton.Text = "&Cancel";
            this.CloseButton.UseVisualStyleBackColor = true;
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(100, 79);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 34;
            this.OKButton.Text = "&OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(268, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(27, 13);
            this.label1.TabIndex = 37;
            this.label1.Text = "ppm";
            // 
            // ScriptOpenButton
            // 
            this.ScriptOpenButton.Location = new System.Drawing.Point(270, 40);
            this.ScriptOpenButton.Name = "ScriptOpenButton";
            this.ScriptOpenButton.Size = new System.Drawing.Size(75, 23);
            this.ScriptOpenButton.TabIndex = 42;
            this.ScriptOpenButton.Text = "&Open";
            this.ScriptOpenButton.UseVisualStyleBackColor = true;
            this.ScriptOpenButton.Click += new System.EventHandler(this.ScriptOpenButton_Click);
            // 
            // ScriptPathTextBox
            // 
            this.ScriptPathTextBox.Location = new System.Drawing.Point(54, 42);
            this.ScriptPathTextBox.Name = "ScriptPathTextBox";
            this.ScriptPathTextBox.ReadOnly = true;
            this.ScriptPathTextBox.Size = new System.Drawing.Size(207, 20);
            this.ScriptPathTextBox.TabIndex = 41;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(37, 13);
            this.label2.TabIndex = 40;
            this.label2.Text = "Script ";
            // 
            // MTKTestXOCalDialog
            // 
            this.AcceptButton = this.OKButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.CloseButton;
            this.ClientSize = new System.Drawing.Size(356, 117);
            this.Controls.Add(this.ScriptOpenButton);
            this.Controls.Add(this.ScriptPathTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.ErrorMarginNumeric);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.CloseButton);
            this.Controls.Add(this.OKButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MTKTestXOCalDialog";
            this.Text = "Crystal Calibration";
            ((System.ComponentModel.ISupportInitialize)(this.ErrorMarginNumeric)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.NumericUpDown ErrorMarginNumeric;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button CloseButton;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button ScriptOpenButton;
        private System.Windows.Forms.TextBox ScriptPathTextBox;
        private System.Windows.Forms.Label label2;
    }
}
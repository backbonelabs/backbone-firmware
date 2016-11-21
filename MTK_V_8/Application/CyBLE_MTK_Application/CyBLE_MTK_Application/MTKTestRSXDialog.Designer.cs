namespace CyBLE_MTK_Application
{
    partial class MTKTestRSXDialog
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
            this.ChannelNumber = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // CloseButton
            // 
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CloseButton.Location = new System.Drawing.Point(137, 41);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 35;
            this.CloseButton.Text = "&Cancel";
            this.CloseButton.UseVisualStyleBackColor = true;
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(56, 41);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 34;
            this.OKButton.Text = "&OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // ChannelNumber
            // 
            this.ChannelNumber.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ChannelNumber.FormattingEnabled = true;
            this.ChannelNumber.Items.AddRange(new object[] {
            "0 (2402 MHz)",
            "1 (2404 MHz)",
            "2 (2406 MHz)",
            "3 (2408 MHz)",
            "4 (2410 MHz)",
            "5 (2412 MHz)",
            "6 (2414 MHz)",
            "7 (2416 MHz)",
            "8 (2418 MHz)",
            "9 (2420 MHz)",
            "10 (2422 MHz)",
            "11 (2424 MHz)",
            "12 (2426 MHz)",
            "13 (2428 MHz)",
            "14 (2430 MHz)",
            "15 (2432 MHz)",
            "16 (2434 MHz)",
            "17 (2436 MHz)",
            "18 (2438 MHz)",
            "19 (2440 MHz)",
            "20 (2442 MHz)",
            "21 (2444 MHz)",
            "22 (2446 MHz)",
            "23 (2448 MHz)",
            "24 (2450 MHz)",
            "25 (2452 MHz)",
            "26 (2454 MHz)",
            "27 (2456 MHz)",
            "28 (2458 MHz)",
            "29 (2460 MHz)",
            "30 (2462 MHz)",
            "31 (2464 MHz)",
            "32 (2466 MHz)",
            "33 (2468 MHz)",
            "34 (2470 MHz)",
            "35 (2472 MHz)",
            "36 (2474 MHz)",
            "37 (2476 MHz)",
            "38 (2478 MHz)",
            "39 (2480 MHz)"});
            this.ChannelNumber.Location = new System.Drawing.Point(71, 12);
            this.ChannelNumber.Name = "ChannelNumber";
            this.ChannelNumber.Size = new System.Drawing.Size(179, 21);
            this.ChannelNumber.TabIndex = 36;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(46, 13);
            this.label1.TabIndex = 37;
            this.label1.Text = "Channel";
            // 
            // MTKTestRSXDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(268, 78);
            this.Controls.Add(this.ChannelNumber);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.CloseButton);
            this.Controls.Add(this.OKButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MTKTestRSXDialog";
            this.Text = "RSSI Measurment Channel";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button CloseButton;
        private System.Windows.Forms.Button OKButton;
        public System.Windows.Forms.ComboBox ChannelNumber;
        private System.Windows.Forms.Label label1;
    }
}
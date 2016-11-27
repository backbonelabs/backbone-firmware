namespace CyBLE_MTK_Application
{
    partial class MTKTestRxTxDialog
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
            this.label5 = new System.Windows.Forms.Label();
            this.PacketTypeComboBox = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.PacketLengthNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.NumberOfPackets = new System.Windows.Forms.NumericUpDown();
            this.PowerLevel = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.ChannelNumber = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.OKButton = new System.Windows.Forms.Button();
            this.CloseButton = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.PacketLengthNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.NumberOfPackets)).BeginInit();
            this.SuspendLayout();
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(37, 123);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(77, 13);
            this.label5.TabIndex = 21;
            this.label5.Text = "Packet Length";
            // 
            // PacketTypeComboBox
            // 
            this.PacketTypeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.PacketTypeComboBox.FormattingEnabled = true;
            this.PacketTypeComboBox.Items.AddRange(new object[] {
            "PRBS9",
            "11110000 Alternating Pattern",
            "10101010 Alternating Pattern",
            "PRBS15",
            "All 1s",
            "All 0s",
            "00001111 Alternating Pattern",
            "0101 Alternating Pattern"});
            this.PacketTypeComboBox.Location = new System.Drawing.Point(120, 94);
            this.PacketTypeComboBox.Name = "PacketTypeComboBox";
            this.PacketTypeComboBox.Size = new System.Drawing.Size(180, 21);
            this.PacketTypeComboBox.TabIndex = 18;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(46, 97);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(68, 13);
            this.label4.TabIndex = 19;
            this.label4.Text = "Packet Type";
            // 
            // PacketLengthNumericUpDown
            // 
            this.PacketLengthNumericUpDown.Location = new System.Drawing.Point(120, 121);
            this.PacketLengthNumericUpDown.Maximum = new decimal(new int[] {
            37,
            0,
            0,
            0});
            this.PacketLengthNumericUpDown.Name = "PacketLengthNumericUpDown";
            this.PacketLengthNumericUpDown.Size = new System.Drawing.Size(180, 20);
            this.PacketLengthNumericUpDown.TabIndex = 15;
            this.PacketLengthNumericUpDown.Value = new decimal(new int[] {
            37,
            0,
            0,
            0});
            // 
            // NumberOfPackets
            // 
            this.NumberOfPackets.Location = new System.Drawing.Point(121, 68);
            this.NumberOfPackets.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.NumberOfPackets.Name = "NumberOfPackets";
            this.NumberOfPackets.Size = new System.Drawing.Size(179, 20);
            this.NumberOfPackets.TabIndex = 13;
            this.NumberOfPackets.Value = new decimal(new int[] {
            1500,
            0,
            0,
            0});
            // 
            // PowerLevel
            // 
            this.PowerLevel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.PowerLevel.FormattingEnabled = true;
            this.PowerLevel.Items.AddRange(new object[] {
            "3 dBm",
            "0 dBm",
            "-1 dBm",
            "-2 dBm",
            "-3 dBm",
            "-6 dBm",
            "-12 dBm",
            "-18 dBm"});
            this.PowerLevel.Location = new System.Drawing.Point(121, 40);
            this.PowerLevel.Name = "PowerLevel";
            this.PowerLevel.Size = new System.Drawing.Size(179, 21);
            this.PowerLevel.TabIndex = 11;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(17, 70);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(98, 13);
            this.label3.TabIndex = 17;
            this.label3.Text = "Number of Packets";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(78, 43);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(37, 13);
            this.label2.TabIndex = 16;
            this.label2.Text = "Power";
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
            this.ChannelNumber.Location = new System.Drawing.Point(121, 12);
            this.ChannelNumber.Name = "ChannelNumber";
            this.ChannelNumber.Size = new System.Drawing.Size(179, 21);
            this.ChannelNumber.TabIndex = 10;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(69, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(46, 13);
            this.label1.TabIndex = 12;
            this.label1.Text = "Channel";
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(80, 150);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 22;
            this.OKButton.Text = "&OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // CloseButton
            // 
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CloseButton.Location = new System.Drawing.Point(161, 150);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 23;
            this.CloseButton.Text = "&Cancel";
            this.CloseButton.UseVisualStyleBackColor = true;
            // 
            // MTKTestRxTxDialog
            // 
            this.AcceptButton = this.OKButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.CloseButton;
            this.ClientSize = new System.Drawing.Size(316, 185);
            this.Controls.Add(this.CloseButton);
            this.Controls.Add(this.OKButton);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.PacketTypeComboBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.PacketLengthNumericUpDown);
            this.Controls.Add(this.NumberOfPackets);
            this.Controls.Add(this.PowerLevel);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.ChannelNumber);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MTKTestRxTxDialog";
            this.Text = "MTKTestRxTxDialog";
            ((System.ComponentModel.ISupportInitialize)(this.PacketLengthNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.NumberOfPackets)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label5;
        public System.Windows.Forms.ComboBox PacketTypeComboBox;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.NumericUpDown PacketLengthNumericUpDown;
        public System.Windows.Forms.NumericUpDown NumberOfPackets;
        public System.Windows.Forms.ComboBox PowerLevel;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.ComboBox ChannelNumber;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Button CloseButton;
    }
}
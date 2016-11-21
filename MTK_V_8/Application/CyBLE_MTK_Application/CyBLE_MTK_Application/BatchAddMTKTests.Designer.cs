namespace CyBLE_MTK_Application
{
    partial class BatchAddMTKTests
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
            this.ChannelCheckedListBox = new System.Windows.Forms.CheckedListBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.ChannelSelectAllButton = new System.Windows.Forms.Button();
            this.ChannelSelectNoneButton = new System.Windows.Forms.Button();
            this.OKButton = new System.Windows.Forms.Button();
            this.CloseButton = new System.Windows.Forms.Button();
            this.PowerAllButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.PowerNoneButton = new System.Windows.Forms.Button();
            this.PowerCheckedListBox = new System.Windows.Forms.CheckedListBox();
            this.PKTTypeAllButton = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.PKTTypeNoneButton = new System.Windows.Forms.Button();
            this.PKTTypeCheckedListBox = new System.Windows.Forms.CheckedListBox();
            this.PKTLenghtALLButton = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.PKTLengthNoneButton = new System.Windows.Forms.Button();
            this.PKTLenghtCheckedListBox = new System.Windows.Forms.CheckedListBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.PERPassCriterionNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.NumberOfPackets = new System.Windows.Forms.NumericUpDown();
            this.NUMPKTListBox = new System.Windows.Forms.ListBox();
            this.PERPassListBox = new System.Windows.Forms.ListBox();
            this.NUMPKTAddButton = new System.Windows.Forms.Button();
            this.NUMPKTRemButton = new System.Windows.Forms.Button();
            this.PERPassAddButton = new System.Windows.Forms.Button();
            this.PERPassRemButton = new System.Windows.Forms.Button();
            this.NUMPKTRemAllButton = new System.Windows.Forms.Button();
            this.PERPassRemAllButton = new System.Windows.Forms.Button();
            this.StatusLabel = new System.Windows.Forms.Label();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox6.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PERPassCriterionNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.NumberOfPackets)).BeginInit();
            this.SuspendLayout();
            // 
            // ChannelCheckedListBox
            // 
            this.ChannelCheckedListBox.FormattingEnabled = true;
            this.ChannelCheckedListBox.Items.AddRange(new object[] {
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
            this.ChannelCheckedListBox.Location = new System.Drawing.Point(6, 77);
            this.ChannelCheckedListBox.Name = "ChannelCheckedListBox";
            this.ChannelCheckedListBox.Size = new System.Drawing.Size(126, 139);
            this.ChannelCheckedListBox.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.ChannelSelectNoneButton);
            this.groupBox1.Controls.Add(this.ChannelSelectAllButton);
            this.groupBox1.Controls.Add(this.ChannelCheckedListBox);
            this.groupBox1.Location = new System.Drawing.Point(12, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(138, 223);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Channels";
            // 
            // ChannelSelectAllButton
            // 
            this.ChannelSelectAllButton.Location = new System.Drawing.Point(6, 19);
            this.ChannelSelectAllButton.Name = "ChannelSelectAllButton";
            this.ChannelSelectAllButton.Size = new System.Drawing.Size(126, 23);
            this.ChannelSelectAllButton.TabIndex = 1;
            this.ChannelSelectAllButton.Text = "Select All";
            this.ChannelSelectAllButton.UseVisualStyleBackColor = true;
            this.ChannelSelectAllButton.Click += new System.EventHandler(this.ChannelSelectAllButton_Click);
            // 
            // ChannelSelectNoneButton
            // 
            this.ChannelSelectNoneButton.Location = new System.Drawing.Point(6, 48);
            this.ChannelSelectNoneButton.Name = "ChannelSelectNoneButton";
            this.ChannelSelectNoneButton.Size = new System.Drawing.Size(126, 23);
            this.ChannelSelectNoneButton.TabIndex = 2;
            this.ChannelSelectNoneButton.Text = "Select None";
            this.ChannelSelectNoneButton.UseVisualStyleBackColor = true;
            this.ChannelSelectNoneButton.Click += new System.EventHandler(this.ChannelSelectNoneButton_Click);
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(400, 372);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 2;
            this.OKButton.Text = "&OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // CloseButton
            // 
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CloseButton.Location = new System.Drawing.Point(481, 372);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 3;
            this.CloseButton.Text = "&Cancel";
            this.CloseButton.UseVisualStyleBackColor = true;
            this.CloseButton.Click += new System.EventHandler(this.CloseButton_Click);
            // 
            // PowerAllButton
            // 
            this.PowerAllButton.Location = new System.Drawing.Point(6, 19);
            this.PowerAllButton.Name = "PowerAllButton";
            this.PowerAllButton.Size = new System.Drawing.Size(91, 23);
            this.PowerAllButton.TabIndex = 1;
            this.PowerAllButton.Text = "Select All";
            this.PowerAllButton.UseVisualStyleBackColor = true;
            this.PowerAllButton.Click += new System.EventHandler(this.PowerAllButton_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.PowerNoneButton);
            this.groupBox2.Controls.Add(this.PowerAllButton);
            this.groupBox2.Controls.Add(this.PowerCheckedListBox);
            this.groupBox2.Location = new System.Drawing.Point(156, 13);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(104, 223);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Power Level";
            // 
            // PowerNoneButton
            // 
            this.PowerNoneButton.Location = new System.Drawing.Point(6, 48);
            this.PowerNoneButton.Name = "PowerNoneButton";
            this.PowerNoneButton.Size = new System.Drawing.Size(91, 23);
            this.PowerNoneButton.TabIndex = 2;
            this.PowerNoneButton.Text = "Select None";
            this.PowerNoneButton.UseVisualStyleBackColor = true;
            this.PowerNoneButton.Click += new System.EventHandler(this.PowerNoneButton_Click);
            // 
            // PowerCheckedListBox
            // 
            this.PowerCheckedListBox.FormattingEnabled = true;
            this.PowerCheckedListBox.Items.AddRange(new object[] {
            "3 dBm",
            "0 dBm",
            "-1 dBm",
            "-2 dBm",
            "-3 dBm",
            "-6 dBm",
            "-12 dBm",
            "-18 dBm"});
            this.PowerCheckedListBox.Location = new System.Drawing.Point(6, 79);
            this.PowerCheckedListBox.Name = "PowerCheckedListBox";
            this.PowerCheckedListBox.Size = new System.Drawing.Size(91, 139);
            this.PowerCheckedListBox.TabIndex = 0;
            // 
            // PKTTypeAllButton
            // 
            this.PKTTypeAllButton.Location = new System.Drawing.Point(6, 19);
            this.PKTTypeAllButton.Name = "PKTTypeAllButton";
            this.PKTTypeAllButton.Size = new System.Drawing.Size(83, 23);
            this.PKTTypeAllButton.TabIndex = 1;
            this.PKTTypeAllButton.Text = "Select All";
            this.PKTTypeAllButton.UseVisualStyleBackColor = true;
            this.PKTTypeAllButton.Click += new System.EventHandler(this.PKTTypeAllButton_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.PKTTypeNoneButton);
            this.groupBox3.Controls.Add(this.PKTTypeAllButton);
            this.groupBox3.Controls.Add(this.PKTTypeCheckedListBox);
            this.groupBox3.Location = new System.Drawing.Point(266, 13);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(180, 223);
            this.groupBox3.TabIndex = 4;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Packet Type";
            // 
            // PKTTypeNoneButton
            // 
            this.PKTTypeNoneButton.Location = new System.Drawing.Point(95, 19);
            this.PKTTypeNoneButton.Name = "PKTTypeNoneButton";
            this.PKTTypeNoneButton.Size = new System.Drawing.Size(78, 23);
            this.PKTTypeNoneButton.TabIndex = 2;
            this.PKTTypeNoneButton.Text = "Select None";
            this.PKTTypeNoneButton.UseVisualStyleBackColor = true;
            this.PKTTypeNoneButton.Click += new System.EventHandler(this.PKTTypeNoneButton_Click);
            // 
            // PKTTypeCheckedListBox
            // 
            this.PKTTypeCheckedListBox.FormattingEnabled = true;
            this.PKTTypeCheckedListBox.Items.AddRange(new object[] {
            "PRBS9",
            "11110000 Alternating Pattern",
            "10101010 Alternating Pattern",
            "PRBS15",
            "All 1s",
            "All 0s",
            "00001111 Alternating Pattern",
            "0101 Alternating Pattern"});
            this.PKTTypeCheckedListBox.Location = new System.Drawing.Point(6, 49);
            this.PKTTypeCheckedListBox.Name = "PKTTypeCheckedListBox";
            this.PKTTypeCheckedListBox.Size = new System.Drawing.Size(167, 169);
            this.PKTTypeCheckedListBox.TabIndex = 0;
            // 
            // PKTLenghtALLButton
            // 
            this.PKTLenghtALLButton.Location = new System.Drawing.Point(6, 19);
            this.PKTLenghtALLButton.Name = "PKTLenghtALLButton";
            this.PKTLenghtALLButton.Size = new System.Drawing.Size(91, 23);
            this.PKTLenghtALLButton.TabIndex = 1;
            this.PKTLenghtALLButton.Text = "Select All";
            this.PKTLenghtALLButton.UseVisualStyleBackColor = true;
            this.PKTLenghtALLButton.Click += new System.EventHandler(this.PKTLenghtALLButton_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.PKTLengthNoneButton);
            this.groupBox4.Controls.Add(this.PKTLenghtALLButton);
            this.groupBox4.Controls.Add(this.PKTLenghtCheckedListBox);
            this.groupBox4.Location = new System.Drawing.Point(452, 13);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(104, 223);
            this.groupBox4.TabIndex = 4;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Packet Length";
            // 
            // PKTLengthNoneButton
            // 
            this.PKTLengthNoneButton.Location = new System.Drawing.Point(6, 48);
            this.PKTLengthNoneButton.Name = "PKTLengthNoneButton";
            this.PKTLengthNoneButton.Size = new System.Drawing.Size(91, 23);
            this.PKTLengthNoneButton.TabIndex = 2;
            this.PKTLengthNoneButton.Text = "Select None";
            this.PKTLengthNoneButton.UseVisualStyleBackColor = true;
            this.PKTLengthNoneButton.Click += new System.EventHandler(this.PKTLengthNoneButton_Click);
            // 
            // PKTLenghtCheckedListBox
            // 
            this.PKTLenghtCheckedListBox.FormattingEnabled = true;
            this.PKTLenghtCheckedListBox.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16",
            "17",
            "18",
            "19",
            "20",
            "21",
            "22",
            "23",
            "24",
            "25",
            "26",
            "27",
            "28",
            "29",
            "30",
            "31",
            "32",
            "33",
            "34",
            "35",
            "36",
            "37"});
            this.PKTLenghtCheckedListBox.Location = new System.Drawing.Point(6, 79);
            this.PKTLenghtCheckedListBox.Name = "PKTLenghtCheckedListBox";
            this.PKTLenghtCheckedListBox.Size = new System.Drawing.Size(91, 139);
            this.PKTLenghtCheckedListBox.TabIndex = 0;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.NUMPKTRemAllButton);
            this.groupBox5.Controls.Add(this.NUMPKTRemButton);
            this.groupBox5.Controls.Add(this.NUMPKTAddButton);
            this.groupBox5.Controls.Add(this.NUMPKTListBox);
            this.groupBox5.Controls.Add(this.NumberOfPackets);
            this.groupBox5.Location = new System.Drawing.Point(12, 242);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(269, 124);
            this.groupBox5.TabIndex = 5;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Number of Packets";
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.PERPassRemAllButton);
            this.groupBox6.Controls.Add(this.PERPassRemButton);
            this.groupBox6.Controls.Add(this.PERPassAddButton);
            this.groupBox6.Controls.Add(this.PERPassListBox);
            this.groupBox6.Controls.Add(this.PERPassCriterionNumericUpDown);
            this.groupBox6.Location = new System.Drawing.Point(287, 242);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(269, 124);
            this.groupBox6.TabIndex = 5;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "PER Pass Criterion";
            // 
            // PERPassCriterionNumericUpDown
            // 
            this.PERPassCriterionNumericUpDown.DecimalPlaces = 2;
            this.PERPassCriterionNumericUpDown.Location = new System.Drawing.Point(6, 19);
            this.PERPassCriterionNumericUpDown.Name = "PERPassCriterionNumericUpDown";
            this.PERPassCriterionNumericUpDown.Size = new System.Drawing.Size(176, 20);
            this.PERPassCriterionNumericUpDown.TabIndex = 3;
            this.PERPassCriterionNumericUpDown.Value = new decimal(new int[] {
            308,
            0,
            0,
            65536});
            // 
            // NumberOfPackets
            // 
            this.NumberOfPackets.Location = new System.Drawing.Point(6, 19);
            this.NumberOfPackets.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.NumberOfPackets.Name = "NumberOfPackets";
            this.NumberOfPackets.Size = new System.Drawing.Size(176, 20);
            this.NumberOfPackets.TabIndex = 3;
            this.NumberOfPackets.Value = new decimal(new int[] {
            1500,
            0,
            0,
            0});
            // 
            // NUMPKTListBox
            // 
            this.NUMPKTListBox.FormattingEnabled = true;
            this.NUMPKTListBox.Location = new System.Drawing.Point(7, 46);
            this.NUMPKTListBox.Name = "NUMPKTListBox";
            this.NUMPKTListBox.Size = new System.Drawing.Size(175, 69);
            this.NUMPKTListBox.TabIndex = 4;
            // 
            // PERPassListBox
            // 
            this.PERPassListBox.FormattingEnabled = true;
            this.PERPassListBox.Location = new System.Drawing.Point(7, 46);
            this.PERPassListBox.Name = "PERPassListBox";
            this.PERPassListBox.Size = new System.Drawing.Size(175, 69);
            this.PERPassListBox.TabIndex = 4;
            // 
            // NUMPKTAddButton
            // 
            this.NUMPKTAddButton.Location = new System.Drawing.Point(188, 19);
            this.NUMPKTAddButton.Name = "NUMPKTAddButton";
            this.NUMPKTAddButton.Size = new System.Drawing.Size(75, 23);
            this.NUMPKTAddButton.TabIndex = 5;
            this.NUMPKTAddButton.Text = "Add";
            this.NUMPKTAddButton.UseVisualStyleBackColor = true;
            this.NUMPKTAddButton.Click += new System.EventHandler(this.NUMPKTAddButton_Click);
            // 
            // NUMPKTRemButton
            // 
            this.NUMPKTRemButton.Location = new System.Drawing.Point(188, 55);
            this.NUMPKTRemButton.Name = "NUMPKTRemButton";
            this.NUMPKTRemButton.Size = new System.Drawing.Size(75, 23);
            this.NUMPKTRemButton.TabIndex = 6;
            this.NUMPKTRemButton.Text = "Remove";
            this.NUMPKTRemButton.UseVisualStyleBackColor = true;
            this.NUMPKTRemButton.Click += new System.EventHandler(this.NUMPKTRemButton_Click);
            // 
            // PERPassAddButton
            // 
            this.PERPassAddButton.Location = new System.Drawing.Point(188, 19);
            this.PERPassAddButton.Name = "PERPassAddButton";
            this.PERPassAddButton.Size = new System.Drawing.Size(75, 23);
            this.PERPassAddButton.TabIndex = 5;
            this.PERPassAddButton.Text = "Add";
            this.PERPassAddButton.UseVisualStyleBackColor = true;
            this.PERPassAddButton.Click += new System.EventHandler(this.PERPassAddButton_Click);
            // 
            // PERPassRemButton
            // 
            this.PERPassRemButton.Location = new System.Drawing.Point(188, 55);
            this.PERPassRemButton.Name = "PERPassRemButton";
            this.PERPassRemButton.Size = new System.Drawing.Size(75, 23);
            this.PERPassRemButton.TabIndex = 6;
            this.PERPassRemButton.Text = "Remove";
            this.PERPassRemButton.UseVisualStyleBackColor = true;
            this.PERPassRemButton.Click += new System.EventHandler(this.PERPassRemButton_Click);
            // 
            // NUMPKTRemAllButton
            // 
            this.NUMPKTRemAllButton.Location = new System.Drawing.Point(188, 91);
            this.NUMPKTRemAllButton.Name = "NUMPKTRemAllButton";
            this.NUMPKTRemAllButton.Size = new System.Drawing.Size(75, 23);
            this.NUMPKTRemAllButton.TabIndex = 7;
            this.NUMPKTRemAllButton.Text = "Remove All";
            this.NUMPKTRemAllButton.UseVisualStyleBackColor = true;
            this.NUMPKTRemAllButton.Click += new System.EventHandler(this.NUMPKTRemAllButton_Click);
            // 
            // PERPassRemAllButton
            // 
            this.PERPassRemAllButton.Location = new System.Drawing.Point(188, 91);
            this.PERPassRemAllButton.Name = "PERPassRemAllButton";
            this.PERPassRemAllButton.Size = new System.Drawing.Size(75, 23);
            this.PERPassRemAllButton.TabIndex = 7;
            this.PERPassRemAllButton.Text = "Remove All";
            this.PERPassRemAllButton.UseVisualStyleBackColor = true;
            this.PERPassRemAllButton.Click += new System.EventHandler(this.PERPassRemAllButton_Click);
            // 
            // StatusLabel
            // 
            this.StatusLabel.AutoSize = true;
            this.StatusLabel.Location = new System.Drawing.Point(12, 377);
            this.StatusLabel.Name = "StatusLabel";
            this.StatusLabel.Size = new System.Drawing.Size(150, 13);
            this.StatusLabel.TabIndex = 6;
            this.StatusLabel.Text = "Click \'Cancel\' to stop any time.";
            this.StatusLabel.Visible = false;
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(169, 372);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(225, 22);
            this.progressBar1.TabIndex = 7;
            this.progressBar1.Visible = false;
            // 
            // ConfigureMTKTests
            // 
            this.AcceptButton = this.OKButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.CloseButton;
            this.ClientSize = new System.Drawing.Size(568, 404);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.StatusLabel);
            this.Controls.Add(this.groupBox6);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.CloseButton);
            this.Controls.Add(this.OKButton);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ConfigureMTKTests";
            this.Text = "ConfigureMTKTests";
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox6.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.PERPassCriterionNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.NumberOfPackets)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckedListBox ChannelCheckedListBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button ChannelSelectAllButton;
        private System.Windows.Forms.Button ChannelSelectNoneButton;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Button CloseButton;
        private System.Windows.Forms.Button PowerAllButton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button PowerNoneButton;
        private System.Windows.Forms.CheckedListBox PowerCheckedListBox;
        private System.Windows.Forms.Button PKTTypeAllButton;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button PKTTypeNoneButton;
        private System.Windows.Forms.CheckedListBox PKTTypeCheckedListBox;
        private System.Windows.Forms.Button PKTLenghtALLButton;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Button PKTLengthNoneButton;
        private System.Windows.Forms.CheckedListBox PKTLenghtCheckedListBox;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.GroupBox groupBox6;
        public System.Windows.Forms.NumericUpDown PERPassCriterionNumericUpDown;
        public System.Windows.Forms.NumericUpDown NumberOfPackets;
        private System.Windows.Forms.ListBox NUMPKTListBox;
        private System.Windows.Forms.ListBox PERPassListBox;
        private System.Windows.Forms.Button NUMPKTRemAllButton;
        private System.Windows.Forms.Button NUMPKTRemButton;
        private System.Windows.Forms.Button NUMPKTAddButton;
        private System.Windows.Forms.Button PERPassRemAllButton;
        private System.Windows.Forms.Button PERPassRemButton;
        private System.Windows.Forms.Button PERPassAddButton;
        private System.Windows.Forms.Label StatusLabel;
        private System.Windows.Forms.ProgressBar progressBar1;
    }
}
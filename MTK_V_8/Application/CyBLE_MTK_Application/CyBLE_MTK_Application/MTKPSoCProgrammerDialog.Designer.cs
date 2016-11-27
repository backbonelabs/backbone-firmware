namespace CyBLE_MTK_Application
{
    partial class MTKPSoCProgrammerDialog
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
            this.HexGroupBox = new System.Windows.Forms.GroupBox();
            this.OpenHEXFileButton = new System.Windows.Forms.Button();
            this.HexFilePathTextBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.ProgModeComboBox = new System.Windows.Forms.ComboBox();
            this.VoltageComboBox = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.ClockComboBox = new System.Windows.Forms.ComboBox();
            this.ConnectorComboBox = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.ProgrammerPortsComboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.CloseButton = new System.Windows.Forms.Button();
            this.OKButton = new System.Windows.Forms.Button();
            this.ProgrammerPortRefreshButton = new System.Windows.Forms.Button();
            this.ProgrammerActioinGroupBox = new System.Windows.Forms.GroupBox();
            this.FlashEraseRadioButton = new System.Windows.Forms.RadioButton();
            this.ProgramRadioButton = new System.Windows.Forms.RadioButton();
            this.ValidateCheckBox = new System.Windows.Forms.CheckBox();
            this.cusProgGroupBox = new System.Windows.Forms.GroupBox();
            this.globalProgCheckBox = new System.Windows.Forms.CheckBox();
            this.HexGroupBox.SuspendLayout();
            this.ProgrammerActioinGroupBox.SuspendLayout();
            this.cusProgGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // HexGroupBox
            // 
            this.HexGroupBox.Controls.Add(this.OpenHEXFileButton);
            this.HexGroupBox.Controls.Add(this.HexFilePathTextBox);
            this.HexGroupBox.Location = new System.Drawing.Point(9, 202);
            this.HexGroupBox.Name = "HexGroupBox";
            this.HexGroupBox.Size = new System.Drawing.Size(315, 40);
            this.HexGroupBox.TabIndex = 28;
            this.HexGroupBox.TabStop = false;
            this.HexGroupBox.Text = "HEX File Path";
            // 
            // OpenHEXFileButton
            // 
            this.OpenHEXFileButton.Location = new System.Drawing.Point(286, 11);
            this.OpenHEXFileButton.Margin = new System.Windows.Forms.Padding(0);
            this.OpenHEXFileButton.Name = "OpenHEXFileButton";
            this.OpenHEXFileButton.Size = new System.Drawing.Size(26, 23);
            this.OpenHEXFileButton.TabIndex = 5;
            this.OpenHEXFileButton.Text = "...";
            this.OpenHEXFileButton.UseVisualStyleBackColor = true;
            this.OpenHEXFileButton.Click += new System.EventHandler(this.OpenHEXFileButton_Click);
            // 
            // HexFilePathTextBox
            // 
            this.HexFilePathTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.HexFilePathTextBox.Location = new System.Drawing.Point(6, 16);
            this.HexFilePathTextBox.Margin = new System.Windows.Forms.Padding(3, 5, 3, 3);
            this.HexFilePathTextBox.Name = "HexFilePathTextBox";
            this.HexFilePathTextBox.ReadOnly = true;
            this.HexFilePathTextBox.Size = new System.Drawing.Size(277, 13);
            this.HexFilePathTextBox.TabIndex = 4;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(9, 76);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(98, 13);
            this.label4.TabIndex = 24;
            this.label4.Text = "Programming Mode";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(64, 49);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(43, 13);
            this.label3.TabIndex = 25;
            this.label3.Text = "Voltage";
            // 
            // ProgModeComboBox
            // 
            this.ProgModeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ProgModeComboBox.FormattingEnabled = true;
            this.ProgModeComboBox.Location = new System.Drawing.Point(113, 73);
            this.ProgModeComboBox.Name = "ProgModeComboBox";
            this.ProgModeComboBox.Size = new System.Drawing.Size(176, 21);
            this.ProgModeComboBox.TabIndex = 23;
            // 
            // VoltageComboBox
            // 
            this.VoltageComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.VoltageComboBox.FormattingEnabled = true;
            this.VoltageComboBox.Location = new System.Drawing.Point(113, 46);
            this.VoltageComboBox.Name = "VoltageComboBox";
            this.VoltageComboBox.Size = new System.Drawing.Size(176, 21);
            this.VoltageComboBox.TabIndex = 22;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(39, 130);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(68, 13);
            this.label2.TabIndex = 26;
            this.label2.Text = "Clock Speed";
            // 
            // ClockComboBox
            // 
            this.ClockComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ClockComboBox.FormattingEnabled = true;
            this.ClockComboBox.Items.AddRange(new object[] {
            "24 MHz",
            "16 MHz",
            "12 MHz",
            "8 MHz",
            "6 MHz",
            "3.2 MHz",
            "3.0 MHz",
            "1.6 MHz",
            "1.5 MHz"});
            this.ClockComboBox.Location = new System.Drawing.Point(113, 127);
            this.ClockComboBox.Name = "ClockComboBox";
            this.ClockComboBox.Size = new System.Drawing.Size(176, 21);
            this.ClockComboBox.TabIndex = 21;
            // 
            // ConnectorComboBox
            // 
            this.ConnectorComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ConnectorComboBox.FormattingEnabled = true;
            this.ConnectorComboBox.Items.AddRange(new object[] {
            "5p",
            "10p"});
            this.ConnectorComboBox.Location = new System.Drawing.Point(113, 100);
            this.ConnectorComboBox.Name = "ConnectorComboBox";
            this.ConnectorComboBox.Size = new System.Drawing.Size(176, 21);
            this.ConnectorComboBox.TabIndex = 19;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(51, 103);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(56, 13);
            this.label5.TabIndex = 17;
            this.label5.Text = "Connector";
            // 
            // ProgrammerPortsComboBox
            // 
            this.ProgrammerPortsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ProgrammerPortsComboBox.FormattingEnabled = true;
            this.ProgrammerPortsComboBox.Location = new System.Drawing.Point(113, 19);
            this.ProgrammerPortsComboBox.Name = "ProgrammerPortsComboBox";
            this.ProgrammerPortsComboBox.Size = new System.Drawing.Size(176, 21);
            this.ProgrammerPortsComboBox.TabIndex = 20;
            this.ProgrammerPortsComboBox.SelectedIndexChanged += new System.EventHandler(this.ProgrammerPortsComboBox_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(81, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(26, 13);
            this.label1.TabIndex = 18;
            this.label1.Text = "Port";
            // 
            // CloseButton
            // 
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CloseButton.Location = new System.Drawing.Point(181, 287);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 30;
            this.CloseButton.Text = "&Cancel";
            this.CloseButton.UseVisualStyleBackColor = true;
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(100, 287);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 29;
            this.OKButton.Text = "&OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // ProgrammerPortRefreshButton
            // 
            this.ProgrammerPortRefreshButton.Image = global::CyBLE_MTK_Application.Properties.Resources.Refresh_icon;
            this.ProgrammerPortRefreshButton.Location = new System.Drawing.Point(295, 18);
            this.ProgrammerPortRefreshButton.Name = "ProgrammerPortRefreshButton";
            this.ProgrammerPortRefreshButton.Size = new System.Drawing.Size(29, 23);
            this.ProgrammerPortRefreshButton.TabIndex = 27;
            this.ProgrammerPortRefreshButton.UseVisualStyleBackColor = true;
            this.ProgrammerPortRefreshButton.Click += new System.EventHandler(this.ProgrammerPortRefreshButton_Click);
            // 
            // ProgrammerActioinGroupBox
            // 
            this.ProgrammerActioinGroupBox.Controls.Add(this.FlashEraseRadioButton);
            this.ProgrammerActioinGroupBox.Controls.Add(this.ProgramRadioButton);
            this.ProgrammerActioinGroupBox.Location = new System.Drawing.Point(9, 154);
            this.ProgrammerActioinGroupBox.Name = "ProgrammerActioinGroupBox";
            this.ProgrammerActioinGroupBox.Size = new System.Drawing.Size(154, 40);
            this.ProgrammerActioinGroupBox.TabIndex = 29;
            this.ProgrammerActioinGroupBox.TabStop = false;
            this.ProgrammerActioinGroupBox.Text = "Programmer Action";
            // 
            // FlashEraseRadioButton
            // 
            this.FlashEraseRadioButton.AutoSize = true;
            this.FlashEraseRadioButton.Location = new System.Drawing.Point(77, 17);
            this.FlashEraseRadioButton.Name = "FlashEraseRadioButton";
            this.FlashEraseRadioButton.Size = new System.Drawing.Size(52, 17);
            this.FlashEraseRadioButton.TabIndex = 1;
            this.FlashEraseRadioButton.TabStop = true;
            this.FlashEraseRadioButton.Text = "&Erase";
            this.FlashEraseRadioButton.UseVisualStyleBackColor = true;
            this.FlashEraseRadioButton.CheckedChanged += new System.EventHandler(this.FlashEraseRadioButton_CheckedChanged);
            // 
            // ProgramRadioButton
            // 
            this.ProgramRadioButton.AutoSize = true;
            this.ProgramRadioButton.Checked = true;
            this.ProgramRadioButton.Location = new System.Drawing.Point(7, 17);
            this.ProgramRadioButton.Name = "ProgramRadioButton";
            this.ProgramRadioButton.Size = new System.Drawing.Size(64, 17);
            this.ProgramRadioButton.TabIndex = 0;
            this.ProgramRadioButton.TabStop = true;
            this.ProgramRadioButton.Text = "&Program";
            this.ProgramRadioButton.UseVisualStyleBackColor = true;
            this.ProgramRadioButton.CheckedChanged += new System.EventHandler(this.ProgramRadioButton_CheckedChanged);
            // 
            // ValidateCheckBox
            // 
            this.ValidateCheckBox.AutoSize = true;
            this.ValidateCheckBox.Checked = true;
            this.ValidateCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ValidateCheckBox.Location = new System.Drawing.Point(170, 171);
            this.ValidateCheckBox.Name = "ValidateCheckBox";
            this.ValidateCheckBox.Size = new System.Drawing.Size(140, 17);
            this.ValidateCheckBox.TabIndex = 31;
            this.ValidateCheckBox.Text = "Verify after Programming";
            this.ValidateCheckBox.UseVisualStyleBackColor = true;
            // 
            // cusProgGroupBox
            // 
            this.cusProgGroupBox.Controls.Add(this.ValidateCheckBox);
            this.cusProgGroupBox.Controls.Add(this.ProgrammerPortsComboBox);
            this.cusProgGroupBox.Controls.Add(this.ProgrammerActioinGroupBox);
            this.cusProgGroupBox.Controls.Add(this.label1);
            this.cusProgGroupBox.Controls.Add(this.label5);
            this.cusProgGroupBox.Controls.Add(this.ConnectorComboBox);
            this.cusProgGroupBox.Controls.Add(this.HexGroupBox);
            this.cusProgGroupBox.Controls.Add(this.ClockComboBox);
            this.cusProgGroupBox.Controls.Add(this.ProgrammerPortRefreshButton);
            this.cusProgGroupBox.Controls.Add(this.label2);
            this.cusProgGroupBox.Controls.Add(this.label4);
            this.cusProgGroupBox.Controls.Add(this.VoltageComboBox);
            this.cusProgGroupBox.Controls.Add(this.label3);
            this.cusProgGroupBox.Controls.Add(this.ProgModeComboBox);
            this.cusProgGroupBox.Enabled = false;
            this.cusProgGroupBox.Location = new System.Drawing.Point(11, 31);
            this.cusProgGroupBox.Name = "cusProgGroupBox";
            this.cusProgGroupBox.Size = new System.Drawing.Size(333, 250);
            this.cusProgGroupBox.TabIndex = 32;
            this.cusProgGroupBox.TabStop = false;
            this.cusProgGroupBox.Text = "Custom Programmer";
            // 
            // globalProgCheckBox
            // 
            this.globalProgCheckBox.AutoSize = true;
            this.globalProgCheckBox.Checked = true;
            this.globalProgCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.globalProgCheckBox.Location = new System.Drawing.Point(13, 8);
            this.globalProgCheckBox.Name = "globalProgCheckBox";
            this.globalProgCheckBox.Size = new System.Drawing.Size(192, 17);
            this.globalProgCheckBox.TabIndex = 33;
            this.globalProgCheckBox.Text = "Use settings from DUT Programmer";
            this.globalProgCheckBox.UseVisualStyleBackColor = true;
            this.globalProgCheckBox.CheckedChanged += new System.EventHandler(this.globalProgCheckBox_CheckedChanged);
            // 
            // MTKPSoCProgrammerDialog
            // 
            this.AcceptButton = this.OKButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.CloseButton;
            this.ClientSize = new System.Drawing.Size(356, 322);
            this.Controls.Add(this.globalProgCheckBox);
            this.Controls.Add(this.CloseButton);
            this.Controls.Add(this.OKButton);
            this.Controls.Add(this.cusProgGroupBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MTKPSoCProgrammerDialog";
            this.Text = "PSoC Programmer Options";
            this.HexGroupBox.ResumeLayout(false);
            this.HexGroupBox.PerformLayout();
            this.ProgrammerActioinGroupBox.ResumeLayout(false);
            this.ProgrammerActioinGroupBox.PerformLayout();
            this.cusProgGroupBox.ResumeLayout(false);
            this.cusProgGroupBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button OpenHEXFileButton;
        private System.Windows.Forms.Button ProgrammerPortRefreshButton;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.RadioButton FlashEraseRadioButton;
        private System.Windows.Forms.RadioButton ProgramRadioButton;
        private System.Windows.Forms.GroupBox HexGroupBox;
        private System.Windows.Forms.Button CloseButton;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.TextBox HexFilePathTextBox;
        private System.Windows.Forms.ComboBox ClockComboBox;
        private System.Windows.Forms.GroupBox ProgrammerActioinGroupBox;
        private System.Windows.Forms.ComboBox ProgModeComboBox;
        private System.Windows.Forms.ComboBox VoltageComboBox;
        private System.Windows.Forms.ComboBox ConnectorComboBox;
        private System.Windows.Forms.ComboBox ProgrammerPortsComboBox;
        private System.Windows.Forms.CheckBox ValidateCheckBox;
        private System.Windows.Forms.GroupBox cusProgGroupBox;
        private System.Windows.Forms.CheckBox globalProgCheckBox;
    }
}
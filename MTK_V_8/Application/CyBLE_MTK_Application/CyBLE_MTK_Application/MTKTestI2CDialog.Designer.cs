namespace CyBLE_MTK_Application
{
    partial class MTKTestI2CDialog
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.ActionComboBox = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.AddButton = new System.Windows.Forms.Button();
            this.RemoveButton = new System.Windows.Forms.Button();
            this.UpButton = new System.Windows.Forms.Button();
            this.DownButton = new System.Windows.Forms.Button();
            this.I2CTestsDataGridView = new System.Windows.Forms.DataGridView();
            this.OKButton = new System.Windows.Forms.Button();
            this.CloseButton = new System.Windows.Forms.Button();
            this.VerifyCheckBox = new System.Windows.Forms.CheckBox();
            this.DataHexadecimalTextBox = new HexadecimalTextBoxControl.HexadecimalTextBox();
            this.AddressHexadecimalTextBox = new HexadecimalTextBoxControl.HexadecimalTextBox();
            ((System.ComponentModel.ISupportInitialize)(this.I2CTestsDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(45, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Address";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(62, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(37, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Action";
            // 
            // ActionComboBox
            // 
            this.ActionComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ActionComboBox.FormattingEnabled = true;
            this.ActionComboBox.Items.AddRange(new object[] {
            "Read",
            "Write"});
            this.ActionComboBox.Location = new System.Drawing.Point(62, 24);
            this.ActionComboBox.Name = "ActionComboBox";
            this.ActionComboBox.Size = new System.Drawing.Size(67, 21);
            this.ActionComboBox.TabIndex = 3;
            this.ActionComboBox.SelectedIndexChanged += new System.EventHandler(this.ActionComboBox_SelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(132, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(30, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Data";
            // 
            // AddButton
            // 
            this.AddButton.Location = new System.Drawing.Point(14, 51);
            this.AddButton.Name = "AddButton";
            this.AddButton.Size = new System.Drawing.Size(75, 23);
            this.AddButton.TabIndex = 4;
            this.AddButton.Text = "&Add";
            this.AddButton.UseVisualStyleBackColor = true;
            this.AddButton.Click += new System.EventHandler(this.AddButton_Click);
            // 
            // RemoveButton
            // 
            this.RemoveButton.Enabled = false;
            this.RemoveButton.Location = new System.Drawing.Point(117, 51);
            this.RemoveButton.Name = "RemoveButton";
            this.RemoveButton.Size = new System.Drawing.Size(75, 23);
            this.RemoveButton.TabIndex = 4;
            this.RemoveButton.Text = "&Remove";
            this.RemoveButton.UseVisualStyleBackColor = true;
            this.RemoveButton.Click += new System.EventHandler(this.RemoveButton_Click);
            // 
            // UpButton
            // 
            this.UpButton.Enabled = false;
            this.UpButton.Location = new System.Drawing.Point(220, 51);
            this.UpButton.Name = "UpButton";
            this.UpButton.Size = new System.Drawing.Size(75, 23);
            this.UpButton.TabIndex = 4;
            this.UpButton.Text = "&Up";
            this.UpButton.UseVisualStyleBackColor = true;
            this.UpButton.Click += new System.EventHandler(this.UpButton_Click);
            // 
            // DownButton
            // 
            this.DownButton.Enabled = false;
            this.DownButton.Location = new System.Drawing.Point(322, 51);
            this.DownButton.Name = "DownButton";
            this.DownButton.Size = new System.Drawing.Size(75, 23);
            this.DownButton.TabIndex = 4;
            this.DownButton.Text = "&Down";
            this.DownButton.UseVisualStyleBackColor = true;
            this.DownButton.Click += new System.EventHandler(this.DownButton_Click);
            // 
            // I2CTestsDataGridView
            // 
            this.I2CTestsDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.I2CTestsDataGridView.Location = new System.Drawing.Point(14, 82);
            this.I2CTestsDataGridView.Name = "I2CTestsDataGridView";
            this.I2CTestsDataGridView.Size = new System.Drawing.Size(383, 139);
            this.I2CTestsDataGridView.TabIndex = 5;
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(126, 227);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 4;
            this.OKButton.Text = "&OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // CloseButton
            // 
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CloseButton.Location = new System.Drawing.Point(207, 227);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 4;
            this.CloseButton.Text = "&Cancel";
            this.CloseButton.UseVisualStyleBackColor = true;
            this.CloseButton.Click += new System.EventHandler(this.CloseButton_Click);
            // 
            // VerifyCheckBox
            // 
            this.VerifyCheckBox.AutoSize = true;
            this.VerifyCheckBox.Location = new System.Drawing.Point(345, 26);
            this.VerifyCheckBox.Name = "VerifyCheckBox";
            this.VerifyCheckBox.Size = new System.Drawing.Size(52, 17);
            this.VerifyCheckBox.TabIndex = 6;
            this.VerifyCheckBox.Text = "&Verify";
            this.VerifyCheckBox.UseVisualStyleBackColor = true;
            this.VerifyCheckBox.CheckedChanged += new System.EventHandler(this.VerifyCheckBox_CheckedChanged);
            // 
            // DataHexadecimalTextBox
            // 
            this.DataHexadecimalTextBox.Delimiter = " ";
            this.DataHexadecimalTextBox.Location = new System.Drawing.Point(135, 25);
            this.DataHexadecimalTextBox.Name = "DataHexadecimalTextBox";
            this.DataHexadecimalTextBox.Size = new System.Drawing.Size(204, 20);
            this.DataHexadecimalTextBox.TabIndex = 0;
            this.DataHexadecimalTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.DataHexadecimalTextBox.TextChanged += new System.EventHandler(this.DataHexadecimalTextBox_TextChanged);
            // 
            // AddressHexadecimalTextBox
            // 
            this.AddressHexadecimalTextBox.Delimiter = "";
            this.AddressHexadecimalTextBox.Location = new System.Drawing.Point(14, 25);
            this.AddressHexadecimalTextBox.MaxLength = 2;
            this.AddressHexadecimalTextBox.Name = "AddressHexadecimalTextBox";
            this.AddressHexadecimalTextBox.Size = new System.Drawing.Size(42, 20);
            this.AddressHexadecimalTextBox.TabIndex = 0;
            this.AddressHexadecimalTextBox.Text = "FF";
            this.AddressHexadecimalTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.AddressHexadecimalTextBox.TextChanged += new System.EventHandler(this.AddressHexadecimalTextBox_TextChanged);
            // 
            // MTKTestI2CDialog
            // 
            this.AcceptButton = this.OKButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.CloseButton;
            this.ClientSize = new System.Drawing.Size(409, 262);
            this.Controls.Add(this.VerifyCheckBox);
            this.Controls.Add(this.I2CTestsDataGridView);
            this.Controls.Add(this.CloseButton);
            this.Controls.Add(this.OKButton);
            this.Controls.Add(this.DownButton);
            this.Controls.Add(this.UpButton);
            this.Controls.Add(this.RemoveButton);
            this.Controls.Add(this.AddButton);
            this.Controls.Add(this.ActionComboBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.DataHexadecimalTextBox);
            this.Controls.Add(this.AddressHexadecimalTextBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MTKTestI2CDialog";
            this.Text = "Configre I2C Tests";
            ((System.ComponentModel.ISupportInitialize)(this.I2CTestsDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private HexadecimalTextBoxControl.HexadecimalTextBox AddressHexadecimalTextBox;
        private HexadecimalTextBoxControl.HexadecimalTextBox DataHexadecimalTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox ActionComboBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button AddButton;
        private System.Windows.Forms.Button RemoveButton;
        private System.Windows.Forms.Button UpButton;
        private System.Windows.Forms.Button DownButton;
        private System.Windows.Forms.DataGridView I2CTestsDataGridView;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Button CloseButton;
        private System.Windows.Forms.CheckBox VerifyCheckBox;
    }
}
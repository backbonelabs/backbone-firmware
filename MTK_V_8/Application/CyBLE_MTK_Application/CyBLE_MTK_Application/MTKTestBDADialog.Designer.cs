namespace CyBLE_MTK_Application
{
    partial class MTKTestBDADialog
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
            this.ProgrammerInfoLabel = new System.Windows.Forms.Label();
            this.ProgrammerTableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.ProgrammerConfigButton = new System.Windows.Forms.Button();
            this.AutoIncrementCheckBox = new System.Windows.Forms.CheckBox();
            this.UseProgrammerCheckBox = new System.Windows.Forms.CheckBox();
            this.BDATextBox = new HexadecimalTextBoxControl.HexadecimalTextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.ProgrammerTableLayoutPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // CloseButton
            // 
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CloseButton.Location = new System.Drawing.Point(185, 166);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 32;
            this.CloseButton.Text = "&Cancel";
            this.CloseButton.UseVisualStyleBackColor = true;
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(104, 166);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 31;
            this.OKButton.Text = "&OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // ProgrammerInfoLabel
            // 
            this.ProgrammerInfoLabel.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.ProgrammerInfoLabel.AutoSize = true;
            this.ProgrammerInfoLabel.Location = new System.Drawing.Point(3, 26);
            this.ProgrammerInfoLabel.Name = "ProgrammerInfoLabel";
            this.ProgrammerInfoLabel.Size = new System.Drawing.Size(69, 13);
            this.ProgrammerInfoLabel.TabIndex = 33;
            this.ProgrammerInfoLabel.Text = "Programmer: ";
            // 
            // ProgrammerTableLayoutPanel
            // 
            this.ProgrammerTableLayoutPanel.ColumnCount = 2;
            this.ProgrammerTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 76.53959F));
            this.ProgrammerTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 23.46041F));
            this.ProgrammerTableLayoutPanel.Controls.Add(this.ProgrammerInfoLabel, 0, 0);
            this.ProgrammerTableLayoutPanel.Controls.Add(this.ProgrammerConfigButton, 1, 0);
            this.ProgrammerTableLayoutPanel.Enabled = false;
            this.ProgrammerTableLayoutPanel.Location = new System.Drawing.Point(12, 87);
            this.ProgrammerTableLayoutPanel.Name = "ProgrammerTableLayoutPanel";
            this.ProgrammerTableLayoutPanel.RowCount = 1;
            this.ProgrammerTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.ProgrammerTableLayoutPanel.Size = new System.Drawing.Size(341, 65);
            this.ProgrammerTableLayoutPanel.TabIndex = 34;
            // 
            // ProgrammerConfigButton
            // 
            this.ProgrammerConfigButton.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.ProgrammerConfigButton.Location = new System.Drawing.Point(263, 21);
            this.ProgrammerConfigButton.Name = "ProgrammerConfigButton";
            this.ProgrammerConfigButton.Size = new System.Drawing.Size(75, 23);
            this.ProgrammerConfigButton.TabIndex = 34;
            this.ProgrammerConfigButton.Text = "&Configure";
            this.ProgrammerConfigButton.UseVisualStyleBackColor = true;
            this.ProgrammerConfigButton.Click += new System.EventHandler(this.ProgrammerConfigButton_Click);
            // 
            // AutoIncrementCheckBox
            // 
            this.AutoIncrementCheckBox.AutoSize = true;
            this.AutoIncrementCheckBox.Location = new System.Drawing.Point(12, 41);
            this.AutoIncrementCheckBox.Name = "AutoIncrementCheckBox";
            this.AutoIncrementCheckBox.Size = new System.Drawing.Size(208, 17);
            this.AutoIncrementCheckBox.TabIndex = 36;
            this.AutoIncrementCheckBox.Text = "Auto-&increment BD Address after write.";
            this.AutoIncrementCheckBox.UseVisualStyleBackColor = true;
            // 
            // UseProgrammerCheckBox
            // 
            this.UseProgrammerCheckBox.AutoSize = true;
            this.UseProgrammerCheckBox.Location = new System.Drawing.Point(12, 64);
            this.UseProgrammerCheckBox.Name = "UseProgrammerCheckBox";
            this.UseProgrammerCheckBox.Size = new System.Drawing.Size(323, 17);
            this.UseProgrammerCheckBox.TabIndex = 37;
            this.UseProgrammerCheckBox.Text = "Use programmer (If unchecked, firmware updates BD Address).";
            this.UseProgrammerCheckBox.UseVisualStyleBackColor = true;
            this.UseProgrammerCheckBox.CheckedChanged += new System.EventHandler(this.UseProgrammerCheckBox_CheckedChanged);
            // 
            // BDATextBox
            // 
            this.BDATextBox.Delimiter = ":";
            this.BDATextBox.Location = new System.Drawing.Point(78, 15);
            this.BDATextBox.MaxLength = 17;
            this.BDATextBox.Name = "BDATextBox";
            this.BDATextBox.Size = new System.Drawing.Size(114, 20);
            this.BDATextBox.TabIndex = 41;
            this.BDATextBox.Text = "00:00:00:00:00:00";
            this.BDATextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(60, 13);
            this.label1.TabIndex = 42;
            this.label1.Text = "BD Adress:";
            // 
            // MTKTestBDADialog
            // 
            this.AcceptButton = this.OKButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.CloseButton;
            this.ClientSize = new System.Drawing.Size(365, 201);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.BDATextBox);
            this.Controls.Add(this.UseProgrammerCheckBox);
            this.Controls.Add(this.AutoIncrementCheckBox);
            this.Controls.Add(this.ProgrammerTableLayoutPanel);
            this.Controls.Add(this.CloseButton);
            this.Controls.Add(this.OKButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MTKTestBDADialog";
            this.Text = "BD Address Programmer Options";
            this.ProgrammerTableLayoutPanel.ResumeLayout(false);
            this.ProgrammerTableLayoutPanel.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button CloseButton;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Label ProgrammerInfoLabel;
        private System.Windows.Forms.TableLayoutPanel ProgrammerTableLayoutPanel;
        private System.Windows.Forms.Button ProgrammerConfigButton;
        private System.Windows.Forms.CheckBox AutoIncrementCheckBox;
        private System.Windows.Forms.CheckBox UseProgrammerCheckBox;
        private System.Windows.Forms.Label label1;
        public HexadecimalTextBoxControl.HexadecimalTextBox BDATextBox;
    }
}
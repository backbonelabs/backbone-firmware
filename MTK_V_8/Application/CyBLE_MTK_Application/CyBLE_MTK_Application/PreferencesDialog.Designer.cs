namespace CyBLE_MTK_Application
{
    partial class PreferencesDialog
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
            System.Windows.Forms.TreeNode treeNode8 = new System.Windows.Forms.TreeNode("General");
            System.Windows.Forms.TreeNode treeNode9 = new System.Windows.Forms.TreeNode("MTK");
            System.Windows.Forms.TreeNode treeNode10 = new System.Windows.Forms.TreeNode("Anritsu");
            System.Windows.Forms.TreeNode treeNode11 = new System.Windows.Forms.TreeNode("Test", new System.Windows.Forms.TreeNode[] {
            treeNode9,
            treeNode10});
            System.Windows.Forms.TreeNode treeNode12 = new System.Windows.Forms.TreeNode("Application Logs");
            System.Windows.Forms.TreeNode treeNode13 = new System.Windows.Forms.TreeNode("Test Logs");
            System.Windows.Forms.TreeNode treeNode14 = new System.Windows.Forms.TreeNode("Logs", new System.Windows.Forms.TreeNode[] {
            treeNode12,
            treeNode13});
            this.CloseButton = new System.Windows.Forms.Button();
            this.OKButton = new System.Windows.Forms.Button();
            this.ApplyButton = new System.Windows.Forms.Button();
            this.PreferencesTreeView = new System.Windows.Forms.TreeView();
            this.GeneralPanel = new System.Windows.Forms.Panel();
            this.CloseSerialPortDialogCheckBox = new System.Windows.Forms.CheckBox();
            this.ChangePasswordButton = new System.Windows.Forms.Button();
            this.EnableTestProgDialogMsgCheckBox = new System.Windows.Forms.CheckBox();
            this.GeneralLabel = new System.Windows.Forms.Label();
            this.MTKTestPanel = new System.Windows.Forms.Panel();
            this.IgnoreDUTsCheckBox = new System.Windows.Forms.CheckBox();
            this.PauseTestsCheckBox = new System.Windows.Forms.CheckBox();
            this.DUTConnectionComboBox = new System.Windows.Forms.ComboBox();
            this.DUTConnectionLabel = new System.Windows.Forms.Label();
            this.MultiDUTCheckBox = new System.Windows.Forms.CheckBox();
            this.NumDUTsNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.NumDUTsLabel = new System.Windows.Forms.Label();
            this.MTKTestLabel = new System.Windows.Forms.Label();
            this.ApplicationLogsPanel = new System.Windows.Forms.Panel();
            this.AppLogPathGroupBox = new System.Windows.Forms.GroupBox();
            this.ChangeAppLogDirButton = new System.Windows.Forms.Button();
            this.AppLogDirPathTextBox = new System.Windows.Forms.TextBox();
            this.AutoSaveCheckBox = new System.Windows.Forms.CheckBox();
            this.LogLevelComboBox = new System.Windows.Forms.ComboBox();
            this.LogLVLLabel = new System.Windows.Forms.Label();
            this.AppLogLabel = new System.Windows.Forms.Label();
            this.TestLogsPanel = new System.Windows.Forms.Panel();
            this.label3 = new System.Windows.Forms.Label();
            this.LogSetupComboBox = new System.Windows.Forms.ComboBox();
            this.LogSetupLabel = new System.Windows.Forms.Label();
            this.TestLogGroupBox = new System.Windows.Forms.GroupBox();
            this.ChangeTestLogDirButton = new System.Windows.Forms.Button();
            this.TestLogDirPathTextBox = new System.Windows.Forms.TextBox();
            this.AutoLogTestsCheckBox = new System.Windows.Forms.CheckBox();
            this.SeparatorLabel = new System.Windows.Forms.Label();
            this.AnritsuTestPanel = new System.Windows.Forms.Panel();
            this.AnritsuOffsetDataGridView = new System.Windows.Forms.DataGridView();
            this.AnritsuTestLabel = new System.Windows.Forms.Label();
            this.AnritsuNumPKTNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.AnritsuNumPKTSLabel = new System.Windows.Forms.Label();
            this.AnritsuScriptIDNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.ScriptIDLabel = new System.Windows.Forms.Label();
            this.GeneralPanel.SuspendLayout();
            this.MTKTestPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.NumDUTsNumericUpDown)).BeginInit();
            this.ApplicationLogsPanel.SuspendLayout();
            this.AppLogPathGroupBox.SuspendLayout();
            this.TestLogsPanel.SuspendLayout();
            this.TestLogGroupBox.SuspendLayout();
            this.AnritsuTestPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.AnritsuOffsetDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.AnritsuNumPKTNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.AnritsuScriptIDNumericUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // CloseButton
            // 
            this.CloseButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CloseButton.Location = new System.Drawing.Point(385, 247);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 0;
            this.CloseButton.Text = "&Cancel";
            this.CloseButton.UseVisualStyleBackColor = true;
            this.CloseButton.Click += new System.EventHandler(this.CloseButton_Click);
            // 
            // OKButton
            // 
            this.OKButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.OKButton.Location = new System.Drawing.Point(304, 247);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 1;
            this.OKButton.Text = "&OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // ApplyButton
            // 
            this.ApplyButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.ApplyButton.Location = new System.Drawing.Point(223, 247);
            this.ApplyButton.Name = "ApplyButton";
            this.ApplyButton.Size = new System.Drawing.Size(75, 23);
            this.ApplyButton.TabIndex = 2;
            this.ApplyButton.Text = "&Apply";
            this.ApplyButton.UseVisualStyleBackColor = true;
            this.ApplyButton.Click += new System.EventHandler(this.ApplyButton_Click);
            // 
            // PreferencesTreeView
            // 
            this.PreferencesTreeView.Location = new System.Drawing.Point(13, 13);
            this.PreferencesTreeView.Name = "PreferencesTreeView";
            treeNode8.Name = "GeneralNode";
            treeNode8.Text = "General";
            treeNode9.Name = "MTKTestNode";
            treeNode9.Text = "MTK";
            treeNode10.Name = "AnritsuTestNode";
            treeNode10.Text = "Anritsu";
            treeNode11.Name = "TestNode";
            treeNode11.Text = "Test";
            treeNode12.Name = "ApplicationLogs";
            treeNode12.Text = "Application Logs";
            treeNode13.Name = "TestLogs";
            treeNode13.Text = "Test Logs";
            treeNode14.Name = "Logs";
            treeNode14.Text = "Logs";
            this.PreferencesTreeView.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode8,
            treeNode11,
            treeNode14});
            this.PreferencesTreeView.Size = new System.Drawing.Size(140, 226);
            this.PreferencesTreeView.TabIndex = 3;
            this.PreferencesTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.PreferencesTreeView_AfterSelect);
            // 
            // GeneralPanel
            // 
            this.GeneralPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.GeneralPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.GeneralPanel.Controls.Add(this.CloseSerialPortDialogCheckBox);
            this.GeneralPanel.Controls.Add(this.ChangePasswordButton);
            this.GeneralPanel.Controls.Add(this.EnableTestProgDialogMsgCheckBox);
            this.GeneralPanel.Controls.Add(this.GeneralLabel);
            this.GeneralPanel.Location = new System.Drawing.Point(159, 12);
            this.GeneralPanel.Name = "GeneralPanel";
            this.GeneralPanel.Size = new System.Drawing.Size(301, 227);
            this.GeneralPanel.TabIndex = 5;
            // 
            // CloseSerialPortDialogCheckBox
            // 
            this.CloseSerialPortDialogCheckBox.AutoSize = true;
            this.CloseSerialPortDialogCheckBox.Location = new System.Drawing.Point(4, 63);
            this.CloseSerialPortDialogCheckBox.Name = "CloseSerialPortDialogCheckBox";
            this.CloseSerialPortDialogCheckBox.Size = new System.Drawing.Size(258, 17);
            this.CloseSerialPortDialogCheckBox.TabIndex = 4;
            this.CloseSerialPortDialogCheckBox.Text = "Close serial port dialog on successful connection.";
            this.CloseSerialPortDialogCheckBox.UseVisualStyleBackColor = true;
            this.CloseSerialPortDialogCheckBox.CheckedChanged += new System.EventHandler(this.CloseSerialPortDialogCheckBox_CheckedChanged);
            // 
            // ChangePasswordButton
            // 
            this.ChangePasswordButton.Location = new System.Drawing.Point(86, 96);
            this.ChangePasswordButton.Name = "ChangePasswordButton";
            this.ChangePasswordButton.Size = new System.Drawing.Size(124, 23);
            this.ChangePasswordButton.TabIndex = 2;
            this.ChangePasswordButton.Text = "Change &Password";
            this.ChangePasswordButton.UseVisualStyleBackColor = true;
            // 
            // EnableTestProgDialogMsgCheckBox
            // 
            this.EnableTestProgDialogMsgCheckBox.AutoSize = true;
            this.EnableTestProgDialogMsgCheckBox.Location = new System.Drawing.Point(4, 40);
            this.EnableTestProgDialogMsgCheckBox.Name = "EnableTestProgDialogMsgCheckBox";
            this.EnableTestProgDialogMsgCheckBox.Size = new System.Drawing.Size(214, 17);
            this.EnableTestProgDialogMsgCheckBox.TabIndex = 1;
            this.EnableTestProgDialogMsgCheckBox.Text = "&Enable \"Test Program\" dialog warnings.";
            this.EnableTestProgDialogMsgCheckBox.UseVisualStyleBackColor = true;
            this.EnableTestProgDialogMsgCheckBox.CheckedChanged += new System.EventHandler(this.EnableTestProgDialogMsgCheckBox_CheckedChanged);
            // 
            // GeneralLabel
            // 
            this.GeneralLabel.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            this.GeneralLabel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.GeneralLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.GeneralLabel.Location = new System.Drawing.Point(4, 4);
            this.GeneralLabel.Name = "GeneralLabel";
            this.GeneralLabel.Size = new System.Drawing.Size(290, 28);
            this.GeneralLabel.TabIndex = 0;
            this.GeneralLabel.Text = "General Preferences";
            this.GeneralLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // MTKTestPanel
            // 
            this.MTKTestPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.MTKTestPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.MTKTestPanel.Controls.Add(this.IgnoreDUTsCheckBox);
            this.MTKTestPanel.Controls.Add(this.PauseTestsCheckBox);
            this.MTKTestPanel.Controls.Add(this.DUTConnectionComboBox);
            this.MTKTestPanel.Controls.Add(this.DUTConnectionLabel);
            this.MTKTestPanel.Controls.Add(this.MultiDUTCheckBox);
            this.MTKTestPanel.Controls.Add(this.NumDUTsNumericUpDown);
            this.MTKTestPanel.Controls.Add(this.NumDUTsLabel);
            this.MTKTestPanel.Controls.Add(this.MTKTestLabel);
            this.MTKTestPanel.Location = new System.Drawing.Point(159, 12);
            this.MTKTestPanel.Name = "MTKTestPanel";
            this.MTKTestPanel.Size = new System.Drawing.Size(301, 227);
            this.MTKTestPanel.TabIndex = 6;
            // 
            // IgnoreDUTsCheckBox
            // 
            this.IgnoreDUTsCheckBox.AutoSize = true;
            this.IgnoreDUTsCheckBox.Location = new System.Drawing.Point(6, 148);
            this.IgnoreDUTsCheckBox.Name = "IgnoreDUTsCheckBox";
            this.IgnoreDUTsCheckBox.Size = new System.Drawing.Size(201, 17);
            this.IgnoreDUTsCheckBox.TabIndex = 6;
            this.IgnoreDUTsCheckBox.Text = "&Ignore DUTs that are not connected.";
            this.IgnoreDUTsCheckBox.UseVisualStyleBackColor = true;
            this.IgnoreDUTsCheckBox.CheckedChanged += new System.EventHandler(this.IgnoreDUTsCheckBox_CheckedChanged);
            // 
            // PauseTestsCheckBox
            // 
            this.PauseTestsCheckBox.AutoSize = true;
            this.PauseTestsCheckBox.Checked = true;
            this.PauseTestsCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.PauseTestsCheckBox.Location = new System.Drawing.Point(6, 125);
            this.PauseTestsCheckBox.Name = "PauseTestsCheckBox";
            this.PauseTestsCheckBox.Size = new System.Drawing.Size(186, 17);
            this.PauseTestsCheckBox.TabIndex = 5;
            this.PauseTestsCheckBox.Text = "Pause test program on test failure.";
            this.PauseTestsCheckBox.UseVisualStyleBackColor = true;
            this.PauseTestsCheckBox.CheckedChanged += new System.EventHandler(this.PauseTestsCheckBox_CheckedChanged);
            // 
            // DUTConnectionComboBox
            // 
            this.DUTConnectionComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.DUTConnectionComboBox.FormattingEnabled = true;
            this.DUTConnectionComboBox.Items.AddRange(new object[] {
            "UART",
            "BLE"});
            this.DUTConnectionComboBox.Location = new System.Drawing.Point(110, 96);
            this.DUTConnectionComboBox.Name = "DUTConnectionComboBox";
            this.DUTConnectionComboBox.Size = new System.Drawing.Size(87, 21);
            this.DUTConnectionComboBox.TabIndex = 4;
            this.DUTConnectionComboBox.SelectedIndexChanged += new System.EventHandler(this.DUTConnectionComboBox_SelectedIndexChanged);
            // 
            // DUTConnectionLabel
            // 
            this.DUTConnectionLabel.AutoSize = true;
            this.DUTConnectionLabel.Location = new System.Drawing.Point(3, 99);
            this.DUTConnectionLabel.Name = "DUTConnectionLabel";
            this.DUTConnectionLabel.Size = new System.Drawing.Size(101, 13);
            this.DUTConnectionLabel.TabIndex = 3;
            this.DUTConnectionLabel.Text = "DUT connected via";
            // 
            // MultiDUTCheckBox
            // 
            this.MultiDUTCheckBox.AutoSize = true;
            this.MultiDUTCheckBox.Location = new System.Drawing.Point(6, 39);
            this.MultiDUTCheckBox.Name = "MultiDUTCheckBox";
            this.MultiDUTCheckBox.Size = new System.Drawing.Size(143, 17);
            this.MultiDUTCheckBox.TabIndex = 2;
            this.MultiDUTCheckBox.Text = "Enable multi-DUT testing";
            this.MultiDUTCheckBox.UseVisualStyleBackColor = true;
            this.MultiDUTCheckBox.CheckedChanged += new System.EventHandler(this.MultiDUTCheckBox_CheckedChanged);
            // 
            // NumDUTsNumericUpDown
            // 
            this.NumDUTsNumericUpDown.Enabled = false;
            this.NumDUTsNumericUpDown.Location = new System.Drawing.Point(204, 58);
            this.NumDUTsNumericUpDown.Maximum = new decimal(new int[] {
            16,
            0,
            0,
            0});
            this.NumDUTsNumericUpDown.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.NumDUTsNumericUpDown.Name = "NumDUTsNumericUpDown";
            this.NumDUTsNumericUpDown.Size = new System.Drawing.Size(88, 20);
            this.NumDUTsNumericUpDown.TabIndex = 1;
            this.NumDUTsNumericUpDown.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.NumDUTsNumericUpDown.ValueChanged += new System.EventHandler(this.NumDUTsNumericUpDown_ValueChanged);
            // 
            // NumDUTsLabel
            // 
            this.NumDUTsLabel.AutoSize = true;
            this.NumDUTsLabel.Enabled = false;
            this.NumDUTsLabel.Location = new System.Drawing.Point(3, 60);
            this.NumDUTsLabel.Name = "NumDUTsLabel";
            this.NumDUTsLabel.Size = new System.Drawing.Size(195, 13);
            this.NumDUTsLabel.TabIndex = 1;
            this.NumDUTsLabel.Text = "Number of DUTs to be tested per cycle:";
            // 
            // MTKTestLabel
            // 
            this.MTKTestLabel.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            this.MTKTestLabel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.MTKTestLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MTKTestLabel.Location = new System.Drawing.Point(4, 4);
            this.MTKTestLabel.Name = "MTKTestLabel";
            this.MTKTestLabel.Size = new System.Drawing.Size(290, 28);
            this.MTKTestLabel.TabIndex = 0;
            this.MTKTestLabel.Text = "MTK Test Preferences";
            this.MTKTestLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // ApplicationLogsPanel
            // 
            this.ApplicationLogsPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.ApplicationLogsPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.ApplicationLogsPanel.Controls.Add(this.AppLogPathGroupBox);
            this.ApplicationLogsPanel.Controls.Add(this.AutoSaveCheckBox);
            this.ApplicationLogsPanel.Controls.Add(this.LogLevelComboBox);
            this.ApplicationLogsPanel.Controls.Add(this.LogLVLLabel);
            this.ApplicationLogsPanel.Controls.Add(this.AppLogLabel);
            this.ApplicationLogsPanel.Location = new System.Drawing.Point(159, 12);
            this.ApplicationLogsPanel.Name = "ApplicationLogsPanel";
            this.ApplicationLogsPanel.Size = new System.Drawing.Size(301, 227);
            this.ApplicationLogsPanel.TabIndex = 7;
            // 
            // AppLogPathGroupBox
            // 
            this.AppLogPathGroupBox.Controls.Add(this.ChangeAppLogDirButton);
            this.AppLogPathGroupBox.Controls.Add(this.AppLogDirPathTextBox);
            this.AppLogPathGroupBox.Location = new System.Drawing.Point(8, 68);
            this.AppLogPathGroupBox.Name = "AppLogPathGroupBox";
            this.AppLogPathGroupBox.Size = new System.Drawing.Size(286, 43);
            this.AppLogPathGroupBox.TabIndex = 4;
            this.AppLogPathGroupBox.TabStop = false;
            this.AppLogPathGroupBox.Text = "Application Log Path";
            // 
            // ChangeAppLogDirButton
            // 
            this.ChangeAppLogDirButton.Location = new System.Drawing.Point(254, 14);
            this.ChangeAppLogDirButton.Name = "ChangeAppLogDirButton";
            this.ChangeAppLogDirButton.Size = new System.Drawing.Size(26, 23);
            this.ChangeAppLogDirButton.TabIndex = 2;
            this.ChangeAppLogDirButton.Text = "...";
            this.ChangeAppLogDirButton.UseVisualStyleBackColor = true;
            this.ChangeAppLogDirButton.Click += new System.EventHandler(this.ChangeAppLogDirButton_Click);
            // 
            // AppLogDirPathTextBox
            // 
            this.AppLogDirPathTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.AppLogDirPathTextBox.Location = new System.Drawing.Point(6, 19);
            this.AppLogDirPathTextBox.Name = "AppLogDirPathTextBox";
            this.AppLogDirPathTextBox.ReadOnly = true;
            this.AppLogDirPathTextBox.Size = new System.Drawing.Size(242, 13);
            this.AppLogDirPathTextBox.TabIndex = 0;
            this.AppLogDirPathTextBox.TabStop = false;
            // 
            // AutoSaveCheckBox
            // 
            this.AutoSaveCheckBox.AutoSize = true;
            this.AutoSaveCheckBox.Location = new System.Drawing.Point(8, 117);
            this.AutoSaveCheckBox.Name = "AutoSaveCheckBox";
            this.AutoSaveCheckBox.Size = new System.Drawing.Size(96, 17);
            this.AutoSaveCheckBox.TabIndex = 3;
            this.AutoSaveCheckBox.Text = "Auto &save logs";
            this.AutoSaveCheckBox.UseVisualStyleBackColor = true;
            this.AutoSaveCheckBox.CheckedChanged += new System.EventHandler(this.AutoSaveCheckBox_CheckedChanged);
            // 
            // LogLevelComboBox
            // 
            this.LogLevelComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.LogLevelComboBox.FormattingEnabled = true;
            this.LogLevelComboBox.Items.AddRange(new object[] {
            "Nothing",
            "Relevant",
            "Everything"});
            this.LogLevelComboBox.Location = new System.Drawing.Point(119, 40);
            this.LogLevelComboBox.Name = "LogLevelComboBox";
            this.LogLevelComboBox.Size = new System.Drawing.Size(121, 21);
            this.LogLevelComboBox.TabIndex = 1;
            this.LogLevelComboBox.SelectedIndexChanged += new System.EventHandler(this.LogLevelComboBox_SelectedIndexChanged);
            // 
            // LogLVLLabel
            // 
            this.LogLVLLabel.AutoSize = true;
            this.LogLVLLabel.Location = new System.Drawing.Point(5, 43);
            this.LogLVLLabel.Name = "LogLVLLabel";
            this.LogLVLLabel.Size = new System.Drawing.Size(109, 13);
            this.LogLVLLabel.TabIndex = 1;
            this.LogLVLLabel.Text = "Set application to log:";
            // 
            // AppLogLabel
            // 
            this.AppLogLabel.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            this.AppLogLabel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.AppLogLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.AppLogLabel.Location = new System.Drawing.Point(4, 4);
            this.AppLogLabel.Name = "AppLogLabel";
            this.AppLogLabel.Size = new System.Drawing.Size(290, 28);
            this.AppLogLabel.TabIndex = 0;
            this.AppLogLabel.Text = "Application Log Preferences";
            this.AppLogLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // TestLogsPanel
            // 
            this.TestLogsPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.TestLogsPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.TestLogsPanel.Controls.Add(this.label3);
            this.TestLogsPanel.Controls.Add(this.LogSetupComboBox);
            this.TestLogsPanel.Controls.Add(this.LogSetupLabel);
            this.TestLogsPanel.Controls.Add(this.TestLogGroupBox);
            this.TestLogsPanel.Controls.Add(this.AutoLogTestsCheckBox);
            this.TestLogsPanel.Location = new System.Drawing.Point(159, 12);
            this.TestLogsPanel.Name = "TestLogsPanel";
            this.TestLogsPanel.Size = new System.Drawing.Size(301, 227);
            this.TestLogsPanel.TabIndex = 8;
            // 
            // label3
            // 
            this.label3.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            this.label3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(4, 4);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(290, 28);
            this.label3.TabIndex = 9;
            this.label3.Text = "Test Log Preferences";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // LogSetupComboBox
            // 
            this.LogSetupComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.LogSetupComboBox.Enabled = false;
            this.LogSetupComboBox.FormattingEnabled = true;
            this.LogSetupComboBox.Items.AddRange(new object[] {
            "session",
            "\'Run\' cycle",
            "DUT",
            "test"});
            this.LogSetupComboBox.Location = new System.Drawing.Point(154, 111);
            this.LogSetupComboBox.Name = "LogSetupComboBox";
            this.LogSetupComboBox.Size = new System.Drawing.Size(140, 21);
            this.LogSetupComboBox.TabIndex = 3;
            this.LogSetupComboBox.SelectedIndexChanged += new System.EventHandler(this.LogSetupComboBox_SelectedIndexChanged);
            // 
            // LogSetupLabel
            // 
            this.LogSetupLabel.AutoSize = true;
            this.LogSetupLabel.Enabled = false;
            this.LogSetupLabel.Location = new System.Drawing.Point(1, 114);
            this.LogSetupLabel.Name = "LogSetupLabel";
            this.LogSetupLabel.Size = new System.Drawing.Size(147, 13);
            this.LogSetupLabel.TabIndex = 11;
            this.LogSetupLabel.Text = "Create a new log file for every";
            // 
            // TestLogGroupBox
            // 
            this.TestLogGroupBox.Controls.Add(this.ChangeTestLogDirButton);
            this.TestLogGroupBox.Controls.Add(this.TestLogDirPathTextBox);
            this.TestLogGroupBox.Location = new System.Drawing.Point(4, 43);
            this.TestLogGroupBox.Name = "TestLogGroupBox";
            this.TestLogGroupBox.Size = new System.Drawing.Size(290, 43);
            this.TestLogGroupBox.TabIndex = 7;
            this.TestLogGroupBox.TabStop = false;
            this.TestLogGroupBox.Text = "Test Log Path";
            // 
            // ChangeTestLogDirButton
            // 
            this.ChangeTestLogDirButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ChangeTestLogDirButton.Location = new System.Drawing.Point(258, 14);
            this.ChangeTestLogDirButton.Name = "ChangeTestLogDirButton";
            this.ChangeTestLogDirButton.Size = new System.Drawing.Size(26, 23);
            this.ChangeTestLogDirButton.TabIndex = 1;
            this.ChangeTestLogDirButton.Text = "...";
            this.ChangeTestLogDirButton.UseVisualStyleBackColor = true;
            this.ChangeTestLogDirButton.Click += new System.EventHandler(this.ChangeTestLogDirButton_Click);
            // 
            // TestLogDirPathTextBox
            // 
            this.TestLogDirPathTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TestLogDirPathTextBox.Location = new System.Drawing.Point(6, 19);
            this.TestLogDirPathTextBox.Name = "TestLogDirPathTextBox";
            this.TestLogDirPathTextBox.ReadOnly = true;
            this.TestLogDirPathTextBox.Size = new System.Drawing.Size(246, 13);
            this.TestLogDirPathTextBox.TabIndex = 0;
            this.TestLogDirPathTextBox.TabStop = false;
            // 
            // AutoLogTestsCheckBox
            // 
            this.AutoLogTestsCheckBox.AutoSize = true;
            this.AutoLogTestsCheckBox.Location = new System.Drawing.Point(4, 92);
            this.AutoLogTestsCheckBox.Name = "AutoLogTestsCheckBox";
            this.AutoLogTestsCheckBox.Size = new System.Drawing.Size(90, 17);
            this.AutoLogTestsCheckBox.TabIndex = 2;
            this.AutoLogTestsCheckBox.Text = "&Auto log tests";
            this.AutoLogTestsCheckBox.UseVisualStyleBackColor = true;
            this.AutoLogTestsCheckBox.CheckedChanged += new System.EventHandler(this.AutoLogTestsCheckBox_CheckedChanged);
            // 
            // SeparatorLabel
            // 
            this.SeparatorLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.SeparatorLabel.Location = new System.Drawing.Point(12, 242);
            this.SeparatorLabel.Name = "SeparatorLabel";
            this.SeparatorLabel.Size = new System.Drawing.Size(449, 2);
            this.SeparatorLabel.TabIndex = 4;
            // 
            // AnritsuTestPanel
            // 
            this.AnritsuTestPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.AnritsuTestPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.AnritsuTestPanel.Controls.Add(this.AnritsuOffsetDataGridView);
            this.AnritsuTestPanel.Controls.Add(this.AnritsuTestLabel);
            this.AnritsuTestPanel.Controls.Add(this.AnritsuNumPKTNumericUpDown);
            this.AnritsuTestPanel.Controls.Add(this.AnritsuNumPKTSLabel);
            this.AnritsuTestPanel.Controls.Add(this.AnritsuScriptIDNumericUpDown);
            this.AnritsuTestPanel.Controls.Add(this.ScriptIDLabel);
            this.AnritsuTestPanel.Location = new System.Drawing.Point(159, 12);
            this.AnritsuTestPanel.Name = "AnritsuTestPanel";
            this.AnritsuTestPanel.Size = new System.Drawing.Size(301, 227);
            this.AnritsuTestPanel.TabIndex = 9;
            // 
            // AnritsuOffsetDataGridView
            // 
            this.AnritsuOffsetDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.AnritsuOffsetDataGridView.Location = new System.Drawing.Point(4, 68);
            this.AnritsuOffsetDataGridView.Name = "AnritsuOffsetDataGridView";
            this.AnritsuOffsetDataGridView.Size = new System.Drawing.Size(288, 152);
            this.AnritsuOffsetDataGridView.TabIndex = 44;
            // 
            // AnritsuTestLabel
            // 
            this.AnritsuTestLabel.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            this.AnritsuTestLabel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.AnritsuTestLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.AnritsuTestLabel.Location = new System.Drawing.Point(4, 4);
            this.AnritsuTestLabel.Name = "AnritsuTestLabel";
            this.AnritsuTestLabel.Size = new System.Drawing.Size(290, 28);
            this.AnritsuTestLabel.TabIndex = 0;
            this.AnritsuTestLabel.Text = "Anritsu Test Preferences";
            this.AnritsuTestLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // AnritsuNumPKTNumericUpDown
            // 
            this.AnritsuNumPKTNumericUpDown.Location = new System.Drawing.Point(224, 38);
            this.AnritsuNumPKTNumericUpDown.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.AnritsuNumPKTNumericUpDown.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.AnritsuNumPKTNumericUpDown.Name = "AnritsuNumPKTNumericUpDown";
            this.AnritsuNumPKTNumericUpDown.Size = new System.Drawing.Size(70, 20);
            this.AnritsuNumPKTNumericUpDown.TabIndex = 42;
            this.AnritsuNumPKTNumericUpDown.Value = new decimal(new int[] {
            1500,
            0,
            0,
            0});
            this.AnritsuNumPKTNumericUpDown.ValueChanged += new System.EventHandler(this.AnritsuScriptIDNumericUpDown_ValueChanged);
            // 
            // AnritsuNumPKTSLabel
            // 
            this.AnritsuNumPKTSLabel.AutoSize = true;
            this.AnritsuNumPKTSLabel.Location = new System.Drawing.Point(121, 42);
            this.AnritsuNumPKTSLabel.Name = "AnritsuNumPKTSLabel";
            this.AnritsuNumPKTSLabel.Size = new System.Drawing.Size(97, 13);
            this.AnritsuNumPKTSLabel.TabIndex = 43;
            this.AnritsuNumPKTSLabel.Text = "Number of packets";
            // 
            // AnritsuScriptIDNumericUpDown
            // 
            this.AnritsuScriptIDNumericUpDown.Location = new System.Drawing.Point(63, 39);
            this.AnritsuScriptIDNumericUpDown.Name = "AnritsuScriptIDNumericUpDown";
            this.AnritsuScriptIDNumericUpDown.Size = new System.Drawing.Size(47, 20);
            this.AnritsuScriptIDNumericUpDown.TabIndex = 42;
            this.AnritsuScriptIDNumericUpDown.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.AnritsuScriptIDNumericUpDown.ValueChanged += new System.EventHandler(this.AnritsuScriptIDNumericUpDown_ValueChanged);
            // 
            // ScriptIDLabel
            // 
            this.ScriptIDLabel.AutoSize = true;
            this.ScriptIDLabel.Location = new System.Drawing.Point(5, 41);
            this.ScriptIDLabel.Name = "ScriptIDLabel";
            this.ScriptIDLabel.Size = new System.Drawing.Size(48, 13);
            this.ScriptIDLabel.TabIndex = 43;
            this.ScriptIDLabel.Text = "Script ID";
            // 
            // PreferencesDialog
            // 
            this.AcceptButton = this.OKButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.CloseButton;
            this.ClientSize = new System.Drawing.Size(472, 282);
            this.Controls.Add(this.GeneralPanel);
            this.Controls.Add(this.AnritsuTestPanel);
            this.Controls.Add(this.MTKTestPanel);
            this.Controls.Add(this.TestLogsPanel);
            this.Controls.Add(this.ApplicationLogsPanel);
            this.Controls.Add(this.SeparatorLabel);
            this.Controls.Add(this.PreferencesTreeView);
            this.Controls.Add(this.ApplyButton);
            this.Controls.Add(this.OKButton);
            this.Controls.Add(this.CloseButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "PreferencesDialog";
            this.Text = "Preferences";
            this.GeneralPanel.ResumeLayout(false);
            this.GeneralPanel.PerformLayout();
            this.MTKTestPanel.ResumeLayout(false);
            this.MTKTestPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.NumDUTsNumericUpDown)).EndInit();
            this.ApplicationLogsPanel.ResumeLayout(false);
            this.ApplicationLogsPanel.PerformLayout();
            this.AppLogPathGroupBox.ResumeLayout(false);
            this.AppLogPathGroupBox.PerformLayout();
            this.TestLogsPanel.ResumeLayout(false);
            this.TestLogsPanel.PerformLayout();
            this.TestLogGroupBox.ResumeLayout(false);
            this.TestLogGroupBox.PerformLayout();
            this.AnritsuTestPanel.ResumeLayout(false);
            this.AnritsuTestPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.AnritsuOffsetDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.AnritsuNumPKTNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.AnritsuScriptIDNumericUpDown)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button CloseButton;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Button ApplyButton;
        private System.Windows.Forms.TreeView PreferencesTreeView;
        private System.Windows.Forms.Panel GeneralPanel;
        private System.Windows.Forms.Label GeneralLabel;
        private System.Windows.Forms.Panel MTKTestPanel;
        private System.Windows.Forms.Label MTKTestLabel;
        private System.Windows.Forms.Panel ApplicationLogsPanel;
        private System.Windows.Forms.Label AppLogLabel;
        private System.Windows.Forms.NumericUpDown NumDUTsNumericUpDown;
        private System.Windows.Forms.Label NumDUTsLabel;
        private System.Windows.Forms.GroupBox AppLogPathGroupBox;
        private System.Windows.Forms.Button ChangeAppLogDirButton;
        private System.Windows.Forms.TextBox AppLogDirPathTextBox;
        private System.Windows.Forms.CheckBox AutoSaveCheckBox;
        private System.Windows.Forms.ComboBox LogLevelComboBox;
        private System.Windows.Forms.Label LogLVLLabel;
        private System.Windows.Forms.Panel TestLogsPanel;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox TestLogGroupBox;
        private System.Windows.Forms.Button ChangeTestLogDirButton;
        private System.Windows.Forms.TextBox TestLogDirPathTextBox;
        private System.Windows.Forms.CheckBox AutoLogTestsCheckBox;
        private System.Windows.Forms.Label SeparatorLabel;
        private System.Windows.Forms.Label LogSetupLabel;
        private System.Windows.Forms.ComboBox LogSetupComboBox;
        private System.Windows.Forms.CheckBox EnableTestProgDialogMsgCheckBox;
        private System.Windows.Forms.Button ChangePasswordButton;
        private System.Windows.Forms.CheckBox MultiDUTCheckBox;
        private System.Windows.Forms.ComboBox DUTConnectionComboBox;
        private System.Windows.Forms.Label DUTConnectionLabel;
        private System.Windows.Forms.CheckBox PauseTestsCheckBox;
        private System.Windows.Forms.CheckBox CloseSerialPortDialogCheckBox;
        private System.Windows.Forms.CheckBox IgnoreDUTsCheckBox;
        private System.Windows.Forms.Panel AnritsuTestPanel;
        private System.Windows.Forms.Label AnritsuTestLabel;
        private System.Windows.Forms.NumericUpDown AnritsuScriptIDNumericUpDown;
        private System.Windows.Forms.Label ScriptIDLabel;
        private System.Windows.Forms.DataGridView AnritsuOffsetDataGridView;
        private System.Windows.Forms.NumericUpDown AnritsuNumPKTNumericUpDown;
        private System.Windows.Forms.Label AnritsuNumPKTSLabel;
    }
}
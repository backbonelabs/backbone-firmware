using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;
using System.Threading;

namespace CyBLE_MTK_Application
{
    public partial class PreferencesDialog : Form
    {
        private LogManager Log;
        private bool IsEdited;
        private CyBLE_MTK MainForm;

        public bool RestartRequired;

        public PreferencesDialog()
        {
            InitializeComponent();
            HideAllPanels();
            ApplyButton.Enabled = false;
            IsEdited = false;
            RestartRequired = false;
            Log = new LogManager();
            AnritsuOffsetDataGridView.CellValueChanged += new DataGridViewCellEventHandler(AnritsuOffsetDataGridView_CellValueChanged);
        }

        public PreferencesDialog(CyBLE_MTK Parent, LogManager Logger) : this()
        {
            Log = Logger;
            MainForm = Parent;
            this.ChangePasswordButton.Click += new EventHandler(MainForm.ChangePasswordButton_Click);
        }

        private void LoadAllSettings()
        {
            NumDUTsNumericUpDown.Value = CyBLE_MTK_Application.Properties.Settings.Default.NumDUTs;
            LogLevelComboBox.Text = CyBLE_MTK_Application.Properties.Settings.Default.LogLevel;
            AppLogDirPathTextBox.Text = CyBLE_MTK_Application.Properties.Settings.Default.ApplicationLogPath;
            AutoSaveCheckBox.Checked = CyBLE_MTK_Application.Properties.Settings.Default.AutoSaveAppLogs;
            TestLogDirPathTextBox.Text = CyBLE_MTK_Application.Properties.Settings.Default.TestLogPath;
            AutoLogTestsCheckBox.Checked = CyBLE_MTK_Application.Properties.Settings.Default.AutoLogTests;
            LogSetupComboBox.Text = CyBLE_MTK_Application.Properties.Settings.Default.AutoLogTestsSetting;
            EnableTestProgDialogMsgCheckBox.Checked = CyBLE_MTK_Application.Properties.Settings.Default.EnableTestProgDialogMsg;
            MultiDUTCheckBox.Checked = CyBLE_MTK_Application.Properties.Settings.Default.MultiDUTEnable;
            DUTConnectionComboBox.Text = CyBLE_MTK_Application.Properties.Settings.Default.ConnectionType;
            PauseTestsCheckBox.Checked = CyBLE_MTK_Application.Properties.Settings.Default.PauseTestsOnFailure;
            CloseSerialPortDialogCheckBox.Checked = CyBLE_MTK_Application.Properties.Settings.Default.CloseSerialDialog;
            IgnoreDUTsCheckBox.Checked = CyBLE_MTK_Application.Properties.Settings.Default.IgnoreDUTs;
            AnritsuScriptIDNumericUpDown.Value = (decimal)CyBLE_MTK_Application.Properties.Settings.Default.AnritsuScriptID;
            AnritsuNumPKTNumericUpDown.Value = (decimal)CyBLE_MTK_Application.Properties.Settings.Default.AnritsuNumPKTS;
            PopulateAnritsuOffsetInfo();

            if ((TestLogDirPathTextBox.Text == "") || (Directory.Exists(TestLogDirPathTextBox.Text) == false))
            {
                TestLogDirPathTextBox.Text = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            }

            if ((AppLogDirPathTextBox.Text == "") || (Directory.Exists(AppLogDirPathTextBox.Text) == false))
            {
                AppLogDirPathTextBox.Text = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            }
        }

        protected override void OnLoad(EventArgs e)
        {
            Log.PrintLog(this, "Loading all settings.", LogDetailLevel.LogEverything);
            LoadAllSettings();
            base.OnLoad(e);

            PreferencesTreeView.ExpandAll();
            PreferencesTreeView.SelectedNode = PreferencesTreeView.Nodes["General"];
            PreferencesTreeView.Select();
            ApplyButton.Enabled = false;
            IsEdited = false;
        }
 
        private void CloseButton_Click(object sender, EventArgs e)
        {
            if (IsEdited == false)
            {
                Log.PrintLog(this, "Cancelling all changes.", LogDetailLevel.LogEverything);
                this.DialogResult = DialogResult.Cancel;
            }
            else
            {
                this.DialogResult = DialogResult.OK;
            }
            this.Close();
        }

        private void SaveAllSettings()
        {
            int OldNumDUTs = (int)CyBLE_MTK_Application.Properties.Settings.Default.NumDUTs;
            if (CyBLE_MTK_Application.Properties.Settings.Default.NumDUTs != NumDUTsNumericUpDown.Value)
            {
                Log.PrintLog(this, NumDUTsLabel.Text + " " + CyBLE_MTK_Application.Properties.Settings.Default.NumDUTs.ToString()
                    + " > " + NumDUTsNumericUpDown.Value.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.NumDUTs = NumDUTsNumericUpDown.Value;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.LogLevel != LogLevelComboBox.Text)
            {
                Log.PrintLog(this, LogLVLLabel.Text + ": " + CyBLE_MTK_Application.Properties.Settings.Default.LogLevel
                    + " > " + LogLevelComboBox.Text, LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.LogLevel = LogLevelComboBox.Text;
                Log.LogDetails = ConvertLogLevel(CyBLE_MTK_Application.Properties.Settings.Default.LogLevel);
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.ApplicationLogPath != AppLogDirPathTextBox.Text)
            {
                Log.PrintLog(this, AppLogPathGroupBox.Text + ": " + CyBLE_MTK_Application.Properties.Settings.Default.ApplicationLogPath
                    + " > " + AppLogDirPathTextBox.Text, LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.ApplicationLogPath = AppLogDirPathTextBox.Text;
                RestartRequired = true;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.AutoSaveAppLogs != AutoSaveCheckBox.Checked)
            {
                Log.PrintLog(this, AutoSaveCheckBox.Text + ": " + CyBLE_MTK_Application.Properties.Settings.Default.AutoSaveAppLogs.ToString()
                    + " > " + AutoSaveCheckBox.Checked.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.AutoSaveAppLogs = AutoSaveCheckBox.Checked;
                RestartRequired = true;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.TestLogPath != TestLogDirPathTextBox.Text)
            {
                Log.PrintLog(this, TestLogGroupBox.Text + ": " + CyBLE_MTK_Application.Properties.Settings.Default.TestLogPath
                    + " > " + TestLogDirPathTextBox.Text, LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.TestLogPath = TestLogDirPathTextBox.Text;
                RestartRequired = true;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.AutoLogTests != AutoLogTestsCheckBox.Checked)
            {
                Log.PrintLog(this, AutoLogTestsCheckBox.Text + ": " + CyBLE_MTK_Application.Properties.Settings.Default.AutoLogTests.ToString()
                    + " > " + AutoLogTestsCheckBox.Checked.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.AutoLogTests = AutoLogTestsCheckBox.Checked;
                RestartRequired = true;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.AutoLogTestsSetting != LogSetupComboBox.Text)
            {
                Log.PrintLog(this, LogSetupLabel.Text + ": " + CyBLE_MTK_Application.Properties.Settings.Default.AutoLogTestsSetting
                    + " > " + LogSetupComboBox.Text, LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.AutoLogTestsSetting = LogSetupComboBox.Text;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.EnableTestProgDialogMsg != EnableTestProgDialogMsgCheckBox.Checked)
            {
                Log.PrintLog(this, EnableTestProgDialogMsgCheckBox.Text + ": " +
                    CyBLE_MTK_Application.Properties.Settings.Default.EnableTestProgDialogMsg.ToString()
                    + " > " + EnableTestProgDialogMsgCheckBox.Checked.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.EnableTestProgDialogMsg = EnableTestProgDialogMsgCheckBox.Checked;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.MultiDUTEnable != MultiDUTCheckBox.Checked)
            {
                Log.PrintLog(this, MultiDUTCheckBox.Text + ": " +
                    CyBLE_MTK_Application.Properties.Settings.Default.MultiDUTEnable.ToString()
                    + " > " + MultiDUTCheckBox.Checked.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.MultiDUTEnable = MultiDUTCheckBox.Checked;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.ConnectionType != DUTConnectionComboBox.Text)
            {
                Log.PrintLog(this, DUTConnectionLabel.Text + ": " + CyBLE_MTK_Application.Properties.Settings.Default.ConnectionType
                    + " > " + DUTConnectionComboBox.Text, LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.ConnectionType = DUTConnectionComboBox.Text;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.PauseTestsOnFailure != PauseTestsCheckBox.Checked)
            {
                Log.PrintLog(this, PauseTestsCheckBox.Text + ": " +
                    CyBLE_MTK_Application.Properties.Settings.Default.PauseTestsOnFailure.ToString()
                    + " > " + PauseTestsCheckBox.Checked.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.PauseTestsOnFailure = PauseTestsCheckBox.Checked;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.CloseSerialDialog != CloseSerialPortDialogCheckBox.Checked)
            {
                Log.PrintLog(this, CloseSerialPortDialogCheckBox.Text + ": " +
                    CyBLE_MTK_Application.Properties.Settings.Default.CloseSerialDialog.ToString()
                    + " > " + CloseSerialPortDialogCheckBox.Checked.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.CloseSerialDialog = CloseSerialPortDialogCheckBox.Checked;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.IgnoreDUTs != IgnoreDUTsCheckBox.Checked)
            {
                Log.PrintLog(this, IgnoreDUTsCheckBox.Text + ": " +
                    CyBLE_MTK_Application.Properties.Settings.Default.IgnoreDUTs.ToString()
                    + " > " + IgnoreDUTsCheckBox.Checked.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.IgnoreDUTs = IgnoreDUTsCheckBox.Checked;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.AnritsuScriptID != (int)AnritsuScriptIDNumericUpDown.Value)
            {
                Log.PrintLog(this, ScriptIDLabel.Text + ": " +
                    CyBLE_MTK_Application.Properties.Settings.Default.AnritsuScriptID.ToString()
                    + " > " + AnritsuScriptIDNumericUpDown.Value.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.AnritsuScriptID = (int)AnritsuScriptIDNumericUpDown.Value;
                IsEdited = true;
            }
            if (CyBLE_MTK_Application.Properties.Settings.Default.AnritsuNumPKTS != (int)AnritsuNumPKTNumericUpDown.Value)
            {
                Log.PrintLog(this, AnritsuNumPKTSLabel.Text + ": " +
                    CyBLE_MTK_Application.Properties.Settings.Default.AnritsuNumPKTS.ToString()
                    + " > " + AnritsuNumPKTNumericUpDown.Value.ToString(), LogDetailLevel.LogRelevant);
                CyBLE_MTK_Application.Properties.Settings.Default.AnritsuNumPKTS = (int)AnritsuNumPKTNumericUpDown.Value;
                IsEdited = true;
            }

            for (int i = 0; i < 16; i++)
            {
                if (OldNumDUTs > i)
                {
                    if (CyBLE_MTK_Application.Properties.Settings.Default.AnritsuOutputPower[i] != ((Decimal)AnritsuOffsetDataGridView[1, i].Value).ToString())
                    {
                        Log.PrintLog(this, "DUT #" + i.ToString() + " (Output Power): " +
                            CyBLE_MTK_Application.Properties.Settings.Default.AnritsuOutputPower[i] + "dBm"
                            + " > " + ((Decimal)AnritsuOffsetDataGridView[1, i].Value).ToString() + "dBm", LogDetailLevel.LogRelevant);
                        CyBLE_MTK_Application.Properties.Settings.Default.AnritsuOutputPower[i] = ((Decimal)AnritsuOffsetDataGridView[1, i].Value).ToString();
                        IsEdited = true;
                    }
                    if (CyBLE_MTK_Application.Properties.Settings.Default.AnritsuTXPower[i] != ((Decimal)AnritsuOffsetDataGridView[2, i].Value).ToString())
                    {
                        Log.PrintLog(this, "DUT #" + i.ToString() + " (TX Power): " +
                            CyBLE_MTK_Application.Properties.Settings.Default.AnritsuTXPower[i] + "dBm"
                            + " > " + ((Decimal)AnritsuOffsetDataGridView[2, i].Value).ToString() + "dBm", LogDetailLevel.LogRelevant);
                        CyBLE_MTK_Application.Properties.Settings.Default.AnritsuTXPower[i] = ((Decimal)AnritsuOffsetDataGridView[2, i].Value).ToString();
                        IsEdited = true;
                    }
                }
                else
                {
                    CyBLE_MTK_Application.Properties.Settings.Default.AnritsuOutputPower[i] = "0.0";
                    CyBLE_MTK_Application.Properties.Settings.Default.AnritsuTXPower[i] = "0.0";
                }
            }

            CyBLE_MTK_Application.Properties.Settings.Default.Save();
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            SaveAllSettings();
            if (IsEdited == true)
            {
                Log.PrintLog(this, "All changes saved.", LogDetailLevel.LogEverything);
                this.DialogResult = DialogResult.OK;
            }
            else
            {
                Log.PrintLog(this, "No changes to be saved.", LogDetailLevel.LogEverything);
                this.DialogResult = DialogResult.Cancel;
            }

            this.Close();
        }

        private void HideAllPanels()
        {
            GeneralPanel.Visible = false;
            MTKTestPanel.Visible = false;
            AnritsuTestPanel.Visible = false;
            ApplicationLogsPanel.Visible = false;
            TestLogsPanel.Visible = false;
        }

        private void PreferencesTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            HideAllPanels();
            if (PreferencesTreeView.SelectedNode.Text == "General")
            {
                GeneralPanel.Visible = true;
            }
            else if ((PreferencesTreeView.SelectedNode.Text == "Test") ||
                (PreferencesTreeView.SelectedNode.Text == "MTK"))
            {
                MTKTestPanel.Visible = true;
            }
            else if ((PreferencesTreeView.SelectedNode.Text == "Anritsu"))
            {
                AnritsuTestPanel.Visible = true;
            }
            else if ((PreferencesTreeView.SelectedNode.Text == "Application Logs") ||
                (PreferencesTreeView.SelectedNode.Text == "Logs"))
            {
                ApplicationLogsPanel.Visible = true;
            }
            else if (PreferencesTreeView.SelectedNode.Text == "Test Logs")
            {
                TestLogsPanel.Visible = true;
            }
        }

        private void ApplyButton_Click(object sender, EventArgs e)
        {
            SaveAllSettings();
            ApplyButton.Enabled = false;
            PopulateAnritsuOffsetInfo();
            IsEdited = true;
        }

        private void AutoLogTestsCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (AutoLogTestsCheckBox.Checked == true)
            {
                LogSetupLabel.Enabled = true;
                LogSetupComboBox.Enabled = true;
            }
            else
            {
                LogSetupLabel.Enabled = false;
                LogSetupComboBox.Enabled = false;
            }
            ApplyButton.Enabled = true;
        }

        private void ChangeTestLogDirButton_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog TestLogPathDialog = new FolderBrowserDialog();
            TestLogPathDialog.SelectedPath = TestLogDirPathTextBox.Text;

            if (TestLogPathDialog.ShowDialog() == DialogResult.OK)
            {
                TestLogDirPathTextBox.Text = TestLogPathDialog.SelectedPath;
                ApplyButton.Enabled = true;
            }
        }

        private void ChangeAppLogDirButton_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog AppLogPathDialog = new FolderBrowserDialog();
            AppLogPathDialog.SelectedPath = AppLogDirPathTextBox.Text;

            if (AppLogPathDialog.ShowDialog() == DialogResult.OK)
            {
                AppLogDirPathTextBox.Text = AppLogPathDialog.SelectedPath;
                ApplyButton.Enabled = true;
            }
        }

        public static LogDetailLevel ConvertLogLevel(string Input)
        {
            switch (Input)
            {
                case "Nothing":
                    return LogDetailLevel.NoLogs;
                case "Relevant":
                    return LogDetailLevel.LogRelevant;
                case "Everything":
                    return LogDetailLevel.LogEverything;
            }

            return LogDetailLevel.LogEverything;
        }

        private void LogSetupComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void LogLevelComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void AutoSaveCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void NumDUTsNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void EnableTestProgDialogMsgCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void MultiDUTCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (MultiDUTCheckBox.Checked == true)
            {
                NumDUTsLabel.Enabled = true;
                NumDUTsNumericUpDown.Enabled = true;
            }
            else
            {
                NumDUTsLabel.Enabled = false;
                NumDUTsNumericUpDown.Enabled = false;
                NumDUTsNumericUpDown.Value = 1;
            }
            ApplyButton.Enabled = true;
        }

        private void DUTConnectionComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void PauseTestsCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void CloseSerialPortDialogCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void IgnoreDUTsCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void AnritsuScriptIDNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            ApplyButton.Enabled = true;
        }

        private void PopulateAnritsuOffsetInfo()
        {
            DataTable myTable;
            DataColumn colItem1, colItem2, colItem3;
            DataRow NewRow;
            DataView myView;

            // DataTable to hold data that is displayed in DataGrid
            myTable = new DataTable("myTable");
            
            // the three columns in the table
            colItem1 = new DataColumn("DUT #", Type.GetType("System.Int32"));
            colItem2 = new DataColumn("Output Power (dBm)", Type.GetType("System.Decimal"));
            colItem3 = new DataColumn("TX Power (dBm)", Type.GetType("System.Decimal"));

            // add the columns to the table
            myTable.Columns.Add(colItem1);
            myTable.Columns.Add(colItem2);
            myTable.Columns.Add(colItem3);

            // Fill in some data
            for (int i = 0; i < CyBLE_MTK_Application.Properties.Settings.Default.NumDUTs; i++)
            {
                NewRow = myTable.NewRow();
                NewRow[0] = i + 1;
                NewRow[1] = Decimal.Parse(CyBLE_MTK_Application.Properties.Settings.Default.AnritsuOutputPower[i]);
                NewRow[2] = Decimal.Parse(CyBLE_MTK_Application.Properties.Settings.Default.AnritsuTXPower[i]);
                myTable.Rows.Add(NewRow);
            }

            // DataView for the DataGridView
            myView = new DataView(myTable);
            myView.AllowDelete = false;
            myView.AllowEdit = true;
            myView.AllowNew = false;

            // Assign DataView to DataGrid
            AnritsuOffsetDataGridView.DataSource = myView;
            AnritsuOffsetDataGridView.Columns[0].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            AnritsuOffsetDataGridView.Columns[0].HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            AnritsuOffsetDataGridView.Columns[0].ReadOnly = true;
            AnritsuOffsetDataGridView.Columns[0].SortMode = DataGridViewColumnSortMode.NotSortable;
            AnritsuOffsetDataGridView.Columns[1].HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            //AnritsuOffsetDataGridView.Columns[1].ReadOnly = true;
            AnritsuOffsetDataGridView.Columns[1].SortMode = DataGridViewColumnSortMode.NotSortable;
            AnritsuOffsetDataGridView.Columns[1].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            AnritsuOffsetDataGridView.Columns[1].HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            AnritsuOffsetDataGridView.Columns[2].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            AnritsuOffsetDataGridView.Columns[2].HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            //AnritsuOffsetDataGridView.Columns[0].Resizable = DataGridViewTriState.False;
            //AnritsuOffsetDataGridView.Columns[2].ReadOnly = true;
            AnritsuOffsetDataGridView.Columns[2].SortMode = DataGridViewColumnSortMode.NotSortable;
            AnritsuOffsetDataGridView.RowHeadersDefaultCellStyle.Padding = new Padding(2);
            Thread.Sleep(1000);
            int _width = AnritsuOffsetDataGridView.Width - AnritsuOffsetDataGridView.RowHeadersWidth - 1;
            if (CyBLE_MTK_Application.Properties.Settings.Default.NumDUTs > 5)
            {
                _width -= 17;
            }
            
            AnritsuOffsetDataGridView.Columns[0].Width = _width / 3;
            AnritsuOffsetDataGridView.Columns[1].Width = _width / 3;
            AnritsuOffsetDataGridView.Columns[2].Width = _width / 3;
            //int AnritsuOffsetDataGridViewColumn1Width = AnritsuOffsetDataGridView.Columns[1].Width;
        }

        private void AnritsuOffsetDataGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            ApplyButton.Enabled = true;
        }
    }
}

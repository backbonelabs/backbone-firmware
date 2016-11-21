using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CyBLE_MTK_Application
{
    public partial class MTKPSoCProgrammerDialog : Form
    {
        private MTKPSoCProgrammer MTKProg;
        public MTKPSoCProgrammer Programmer;

        public MTKPSoCProgrammerDialog()
        {
            InitializeComponent();
            MTKProg = new MTKPSoCProgrammer();
            Programmer = new MTKPSoCProgrammer();
            if (!Programmer.PSoCProgrammerInstalled || !Programmer.IsCorrectVersion())
            {
                this.Visible = false;
                return;
            }
            AddProgrammers();
        }

        public MTKPSoCProgrammerDialog(MTKPSoCProgrammer NewProgrammer) : this()
        {
            Programmer = NewProgrammer;
        }

        protected override void OnLoad(EventArgs e)
        {
            if (!Programmer.PSoCProgrammerInstalled || !Programmer.IsCorrectVersion())
            {
                this.DialogResult = DialogResult.Cancel;
                this.Close();
                return;
            }
            ProgrammerPortsComboBox.Text = Programmer.SelectedProgrammer;
            VoltageComboBox.Text = Programmer.SelectedVoltageSetting;
            AcquireModeToComboBox(Programmer.SelectedAquireMode);
            ConnectorToComboBox(Programmer.SelectedConnectorType);
            HexFilePathTextBox.Text = Programmer.SelectedHEXFilePath;
            ClockComboBox.Text = Programmer.SelectedClock;
            ValidateCheckBox.Checked = Programmer.ValidateAfterProgramming;
            globalProgCheckBox.Checked = Programmer.GlobalProgrammerSelected;
            if (Programmer.SelectedAction == PSoCProgrammerAction.Program)
            {
                ProgramRadioButton.Checked = true;
                FlashEraseRadioButton.Checked = false;
            }
            else if (Programmer.SelectedAction == PSoCProgrammerAction.Erase)
            {
                ProgramRadioButton.Checked = false;
                FlashEraseRadioButton.Checked = true;
            }

            if (ProgrammerPortsComboBox.SelectedIndex == -1)
            {
                VoltageComboBox.Enabled = false;
                ProgModeComboBox.Enabled = false;
                ConnectorComboBox.Enabled = false;
                ConnectorComboBox.SelectedIndex = -1;
                ClockComboBox.Enabled = false;
            }

            base.OnLoad(e);
        }

        private void OpenHEXFileButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog TestProgOpenFileDialog = new OpenFileDialog();
            TestProgOpenFileDialog.Filter = "HEX Files (*.hex)|*.hex|All Files (*.*)|*.*";
            TestProgOpenFileDialog.FilterIndex = 1;

            if (TestProgOpenFileDialog.ShowDialog() == DialogResult.Cancel)
            {
                return;
            }
            HexFilePathTextBox.Text = TestProgOpenFileDialog.FileName;
        }

        private void AddProgrammers()
        {
            string[] PortList = MTKProg.GetPorts();
            if (PortList.Count() > 0)
            {
                this.ProgrammerPortsComboBox.DataSource = PortList;
                Graphics ComboGraphics = ProgrammerPortsComboBox.CreateGraphics();
                Font ComboFont = ProgrammerPortsComboBox.Font;
                int MaxWidth = 0;

                foreach (string ProgrammerPort in PortList)
                {
                    int VertScrollBarWidth = (ProgrammerPortsComboBox.Items.Count > ProgrammerPortsComboBox.MaxDropDownItems) ? SystemInformation.VerticalScrollBarWidth : 0;
                    int DropDownWidth = (int)ComboGraphics.MeasureString(ProgrammerPort, ComboFont).Width + VertScrollBarWidth;
                    if (MaxWidth < DropDownWidth)
                    {
                        ProgrammerPortsComboBox.DropDownWidth = DropDownWidth;
                        MaxWidth = DropDownWidth;
                    }
                }
                if (ProgrammerPortsComboBox.Items.Count > 0)
                {
                    ProgrammerPortsComboBox.SelectedIndex = 0;
                }
                UpdateControlsWithSelectedProgrammer();
            }
        }

        private void UpdateControlsWithSelectedProgrammer()
        {
            MTKProg.SelectedProgrammer = ProgrammerPortsComboBox.Text;
            if (MTKProg.SelectedProgrammer == "")
            {
                VoltageComboBox.Enabled = false;
                VoltageComboBox.SelectedIndex = -1;
                ProgModeComboBox.Enabled = false;
                ProgModeComboBox.SelectedIndex = -1;
                ConnectorComboBox.Enabled = false;
                ConnectorComboBox.SelectedIndex = -1;
                ClockComboBox.Enabled = false;
                ClockComboBox.SelectedIndex = -1;
                return;
            }
            MTKProg.GetCapabilities();

            ProgModeComboBox.Items.Clear();
            ProgModeComboBox.Items.AddRange(MTKProg.ProgrammerSupportedAquireMode.ToArray());
            if (ProgModeComboBox.Items.Count > 0)
            {
                ProgModeComboBox.SelectedIndex = 0;
            }

            VoltageComboBox.Items.Clear();
            VoltageComboBox.Items.AddRange(MTKProg.ProgrammerSupportedVoltage.ToArray());
            if (VoltageComboBox.Items.Count > 0)
            {
                VoltageComboBox.SelectedIndex = 0;
            }

            ConnectorComboBox.Items.Clear();
            ConnectorComboBox.Items.AddRange(MTKProg.ProgrammerSupportedConnectors.ToArray());
            if (ConnectorComboBox.Items.Count > 0)
            {
                ConnectorComboBox.SelectedIndex = 0;
            }

            ClockComboBox.Items.Clear();
            ClockComboBox.Items.AddRange(MTKProg.ProgrammerSupportedClocks.ToArray());
            if (ClockComboBox.Items.Count > 0)
            {
                ClockComboBox.SelectedIndex = 0;
            }
            if (ClockComboBox.Items.Count <= 1)
            {
                ClockComboBox.Enabled = false;
            }
        }

        private void ProgrammerPortRefreshButton_Click(object sender, EventArgs e)
        {
            ProgrammerPortRefreshButton.Enabled = false;
            string[] PortList = {""};
            this.ProgrammerPortsComboBox.DataSource = PortList;
            AddProgrammers();
            ProgrammerPortRefreshButton.Enabled = true;
        }

        private void ProgrammerPortsComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (ProgrammerPortsComboBox.SelectedIndex == -1)
            {
                ProgrammerPortsComboBox.Enabled = false;
                VoltageComboBox.Enabled = false;
                ProgModeComboBox.Enabled = false;
                ConnectorComboBox.Enabled = false;
                ClockComboBox.Enabled = false;
            }
            else
            {
                ProgrammerPortsComboBox.Enabled = true;
                VoltageComboBox.Enabled = true;
                ProgModeComboBox.Enabled = true;
                ConnectorComboBox.Enabled = true;
                ClockComboBox.Enabled = true;
            }
            UpdateControlsWithSelectedProgrammer();
        }

        public string AcquireModeFromComboBox()
        {
            string ReturnValue = "Reset";
            if (ProgModeComboBox.Text == "Reset")
            {
                ReturnValue = "Reset";
            }
            else if (ProgModeComboBox.Text == "Power Cycle")
            {
                ReturnValue = "Power";
            }
            else if (ProgModeComboBox.Text == "Power Detect")
            {
                ReturnValue = "PowerDetect";
            }
            return ReturnValue;
        }

        public void AcquireModeToComboBox(string InputValue)
        {
            if (InputValue == "Reset")
            {
                ProgModeComboBox.Text = "Reset";
            }
            else if (InputValue == "Power")
            {
                ProgModeComboBox.Text = "Power Cycle";
            }
            else if (InputValue == "PowerDetect")
            {
                ProgModeComboBox.Text = "Power Detect";
            }
        }

        public int ConnectorFromComboBox()
        {
            int ReturnValue = 0;

            if (ConnectorComboBox.Text == "5p")
            {
                ReturnValue = 0;
            }
            else if (ConnectorComboBox.Text == "10p")
            {
                ReturnValue = 1;
            }

            return ReturnValue;
        }

        public void ConnectorToComboBox(int ReturnValue)
        {
            if (ReturnValue == 0)
            {
                ConnectorComboBox.Text = "5p";
            }
            else if (ReturnValue == 1)
            {
                ConnectorComboBox.Text = "10p";
            }
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            Programmer.SelectedProgrammer = ProgrammerPortsComboBox.Text;
            Programmer.SelectedVoltageSetting = VoltageComboBox.Text;
            Programmer.SelectedAquireMode = AcquireModeFromComboBox();
            Programmer.SelectedConnectorType = ConnectorFromComboBox();
            Programmer.SelectedHEXFilePath = HexFilePathTextBox.Text;
            Programmer.SelectedClock = ClockComboBox.Text;
            Programmer.ValidateAfterProgramming = ValidateCheckBox.Checked;
            Programmer.GlobalProgrammerSelected = globalProgCheckBox.Checked; 
            if (ProgramRadioButton.Checked)
            {
                Programmer.SelectedAction = PSoCProgrammerAction.Program;
            }
            else if (FlashEraseRadioButton.Checked)
            {
                Programmer.SelectedAction = PSoCProgrammerAction.Erase;
            }

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        public void SetupForBDA()
        {
            HexGroupBox.Enabled = false;
            ProgrammerActioinGroupBox.Enabled = false;
            ValidateCheckBox.Enabled = false;
            HexGroupBox.Visible = false;
            ProgrammerActioinGroupBox.Visible = false;
            ValidateCheckBox.Visible = false;
            cusProgGroupBox.Size = new Size(cusProgGroupBox.Size.Width, cusProgGroupBox.Size.Height - 92);
            Size = new Size(Size.Width, Size.Height - 92);
            OKButton.Location = new Point(OKButton.Location.X, OKButton.Location.Y - 92);
            CloseButton.Location = new Point(CloseButton.Location.X, CloseButton.Location.Y - 92);
        }

        public void SetupForMainForm()
        {
            //ProgrammerActioinGroupBox.Enabled = false;
            //ProgrammerActioinGroupBox.Visible = false;
            //ValidateCheckBox.Location = new Point(99, 164);
            globalProgCheckBox.Enabled = false;
            globalProgCheckBox.Visible = false;
            //Size = new Size(Size.Width, Size.Height - 46);
            //HexGroupBox.Location = new Point(HexGroupBox.Location.X, HexGroupBox.Location.Y - 46);
            //OKButton.Location = new Point(OKButton.Location.X, OKButton.Location.Y - 46);
            //CloseButton.Location = new Point(CloseButton.Location.X, CloseButton.Location.Y - 46);
        }

        private void FlashEraseRadioButton_CheckedChanged(object sender, EventArgs e)
        {
            //if (FlashEraseRadioButton.Checked)
            //{
                //HexFilePathTextBox.Text = "";
                //HexGroupBox.Enabled = false;
            //}
        }

        private void ProgramRadioButton_CheckedChanged(object sender, EventArgs e)
        {
            //if (ProgramRadioButton.Checked)
            //{
            //    HexGroupBox.Enabled = true;
            //}
        }

        private void globalProgCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (globalProgCheckBox.Checked)
            {
                cusProgGroupBox.Enabled = false;
            }
            else
            {
                cusProgGroupBox.Enabled = true;
            }
        }
    }
}

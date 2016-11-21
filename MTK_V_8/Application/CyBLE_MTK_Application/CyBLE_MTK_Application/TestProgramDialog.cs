using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using System.IO;
using System.IO.Ports;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace CyBLE_MTK_Application
{
    public partial class TestProgramDialog : Form
    {
        private LogManager Log;
        private bool DontClose = false;
        private bool RestoreOnCancel = false;
        private bool ListBoxDoubleClick = false;
        private List<MTKTest> BackupList;
        private string[] BackupTests;
        private int BackupSelectedTest;
        private SerialPort MTKPort, DUTPort;
        private string[] ListOfAvailableTests = {
                                                    "RX Packet Error Rate (PER) Test",
                                                    "TX Packet Error Rate (PER) Test",
                                                    "Transmit Carrier Wave",
                                                    "Transmit Packets",
                                                    "Receive Packets",
                                                    "Device Programmer",
                                                    "Delay",
                                                    "BD Address Programmer",
                                                    "Anritsu Test Program",
                                                    "Sleep->Wakeup->TX->Sleep",
                                                    "Custom Tests",
                                                    "Program All Devices",
                                                    "I2C Tests",
                                                    "Get Instantaneous RSSI",
                                                    "Read BD Address",
                                                    "Crystal Calibration"
                                                };
        protected List<MTKTest> TestProgram;
        public List<MTKTest> TestProgramList
        {
            get 
            {
                return CopyTestList(TestProgram);
            }
            set 
            {
                TestProgram = value;
                TestProgramListBox.Items.Clear();
                foreach (MTKTest Tests in TestProgram)
                {
                    TestProgramListBox.Items.Add(Tests.GetDisplayText());
                }
                if (TestProgramListBox.Items.Count > 0)
                {
                    TestProgramListBox.SelectedIndex = 0;
                }
            }
        }

        public bool EnableMessages;
        public int TestProgramOpenEditIndex;
        public MTKTestBDA BDAProgrammer;

        public TestProgramDialog()
        {
            InitializeComponent();
            this.AvailableTestListBox.MouseDoubleClick += new MouseEventHandler(this.AvailableTestListBox_DoubleClick);
            this.AvailableTestListBox.MouseDown +=new MouseEventHandler(AvailableTestListBox_MouseDown);
            this.AvailableTestListBox.SelectedIndexChanged += new EventHandler(AvailableTestListBox_SelectedIndexChanged);
            this.TestProgramListBox.MouseDoubleClick += new MouseEventHandler(this.TestProgramListBox_DoubleClick);
            this.TestProgramListBox.MouseDown += new MouseEventHandler(this.TestProgramListBox_MouseDown);
            this.TestProgramListBox.SelectedIndexChanged += new EventHandler(TestProgramListBox_SelectedIndexChanged);
            this.AvailableTestRightClickMenu.Opening += new CancelEventHandler(AvailableTestRightClickMenu_Opening);
            AvailableTestListBox.Items.AddRange(ListOfAvailableTests);
            TestProgram = new List<MTKTest>();
            Log = new LogManager();
            EnableMessages = false;
            TestProgramOpenEditIndex = -1;
        }

        public TestProgramDialog(LogManager Logger, SerialPort MTKSerialPort, SerialPort DUTSerialPort) : this()
        {
            Log = Logger;
            MTKPort = MTKSerialPort;
            DUTPort = DUTSerialPort;
        }

        public List<MTKTest> CopyTestList(List<MTKTest> InputList)
        {
            List<MTKTest> OutputList = new List<MTKTest>();

            foreach (MTKTest Test in InputList)
            {
                OutputList.Add(Test);
            }

            return OutputList;
        }

        protected override void OnLoad(EventArgs e)
        {
            BackupList = CopyTestList(TestProgram);
            BackupSelectedTest = TestProgramListBox.SelectedIndex;
            BackupTests = TestProgramListBox.Items.Cast<string>().ToArray();
            base.OnLoad(e);
        }

        protected override void OnShown(EventArgs e)
        {
            AvailableTestListBox.Focus();
            AvailableTestListBox.SelectedIndex = 0;
            if (TestProgramOpenEditIndex > -1)
            {
                TestProgramListBox.ClearSelected();
                TestProgramListBox.SelectedIndex = TestProgramOpenEditIndex;

                if (EditTestAt(TestProgramOpenEditIndex))
                {
                    TestProgramListBox.Items[TestProgramOpenEditIndex] = 
                        TestProgram[TestProgramOpenEditIndex].GetDisplayText();
                    TestProgramListBox.Refresh();
                }
            }

            if (TestProgramListBox.Items.Count == 0)
            {
                UpButton.Enabled = false;
                DownButton.Enabled = false;
                RemoveTestButton.Enabled = false;
            }

            base.OnShown(e);
        }

        private void UpdateTestProgramWithNewTest(MTKTest NewTest)
        {
            if (TestProgramListBox.SelectedIndex >= 0)
            {
                TestProgram.Insert(TestProgramListBox.SelectedIndex, NewTest);
                TestProgramListBox.Items.Insert(TestProgramListBox.SelectedIndex, NewTest.GetDisplayText());
            }
            else
            {
                TestProgram.Add(NewTest);
                TestProgramListBox.Items.Add(NewTest.GetDisplayText());
            }
            Log.PrintLog(this, "Adding " + NewTest.ToString() + " to the test list.",
                LogDetailLevel.LogEverything);
        }

        private void AddTestButton_Click(object sender, EventArgs e)
        {
            if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[0])
            {
                MTKTestRXPER RXPERTest = new MTKTestRXPER(Log, MTKPort, DUTPort);
                MTKTestPERDialog TempDialog = new MTKTestPERDialog();
                TempDialog.Text = "RX PER Test Configuration";
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    RXPERTest.ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    RXPERTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    RXPERTest.NumberOfPackets = (int)TempDialog.NumberOfPackets.Value;
                    RXPERTest.PacketType = TempDialog.PacketTypeComboBox.Text;
                    RXPERTest.PacketLength = (int)TempDialog.PacketLengthNumericUpDown.Value;
                    RXPERTest.PERPassCriterion = (double)TempDialog.PERPassCriterionNumericUpDown.Value;
                    UpdateTestProgramWithNewTest(RXPERTest);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[1])
            {
                MTKTestTXPER TXPERTest = new MTKTestTXPER(Log, MTKPort, DUTPort);
                MTKTestPERDialog TempDialog = new MTKTestPERDialog();
                TempDialog.Text = "TX PER Test Configuration";
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    TXPERTest.ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    TXPERTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    TXPERTest.NumberOfPackets = (int)TempDialog.NumberOfPackets.Value;
                    TXPERTest.PacketType = TempDialog.PacketTypeComboBox.Text;
                    TXPERTest.PacketLength = (int)TempDialog.PacketLengthNumericUpDown.Value;
                    TXPERTest.PERPassCriterion = (double)TempDialog.PERPassCriterionNumericUpDown.Value;
                    UpdateTestProgramWithNewTest(TXPERTest);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[2])
            {
                MTKTestTXCW TXCWTest = new MTKTestTXCW(Log, MTKPort, DUTPort);
                MTKTestTXCWDialog TempDialog = new MTKTestTXCWDialog();
                TempDialog.Text = "TX CW Test Configuration";
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    TXCWTest.ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    TXCWTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    TXCWTest.DurationForTXCW = (int)TempDialog.DurationForTXCW.Value;
                    UpdateTestProgramWithNewTest(TXCWTest);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[3])
            {
                MTKTestTXP TXPTest = new MTKTestTXP(Log, MTKPort, DUTPort);
                MTKTestRxTxDialog TempDialog = new MTKTestRxTxDialog();
                TempDialog.Text = "TX Packet Test Configuration";
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    TXPTest.ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    TXPTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    TXPTest.NumberOfPackets = (int)TempDialog.NumberOfPackets.Value;
                    TXPTest.PacketType = TempDialog.PacketTypeComboBox.Text;
                    TXPTest.PacketLength = (int)TempDialog.PacketLengthNumericUpDown.Value;
                    UpdateTestProgramWithNewTest(TXPTest);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[4])
            {
                MTKTestRXP RXPTest = new MTKTestRXP(Log, MTKPort, DUTPort);
                MTKTestRxTxDialog TempDialog = new MTKTestRxTxDialog();
                TempDialog.Text = "RX Packet Test Configuration";
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    RXPTest.ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    RXPTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    RXPTest.NumberOfPackets = (int)TempDialog.NumberOfPackets.Value;
                    RXPTest.PacketType = TempDialog.PacketTypeComboBox.Text;
                    RXPTest.PacketLength = (int)TempDialog.PacketLengthNumericUpDown.Value;
                    UpdateTestProgramWithNewTest(RXPTest);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[5])
            {
                MTKPSoCProgrammer MTKProgrammer = new MTKPSoCProgrammer(Log);
                if (MTKProgrammer.PSoCProgrammerInstalled && MTKProgrammer.IsCorrectVersion())
                {
                    MTKPSoCProgrammerDialog TempDialog = new MTKPSoCProgrammerDialog(MTKProgrammer);
                    if (TempDialog.ShowDialog() == DialogResult.OK)
                    {
                        UpdateTestProgramWithNewTest(MTKProgrammer);
                    }
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[6])
            {
                MTKTestDelay MTKDelayInMS = new MTKTestDelay(Log);
                MTKTestDelayDialog TempDialog = new MTKTestDelayDialog();
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    MTKDelayInMS.DelayInMS = (int)TempDialog.DelayNumericUpDown.Value;
                    UpdateTestProgramWithNewTest(MTKDelayInMS);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[7])
            {
                if (IsBDAProgrammerPresent())
                {
                    MessageBox.Show("Only one instance of BDA Programmer can be added to a test program.", "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MTKTestBDADialog TempDialog = new MTKTestBDADialog(BDAProgrammer);
                    if (BDAProgrammer.BDAProgrammer.PSoCProgrammerInstalled && BDAProgrammer.BDAProgrammer.IsCorrectVersion())
                    {
                        if (TempDialog.ShowDialog() == DialogResult.OK)
                        {
                            CyBLE_MTK_Application.Properties.Settings.Default.BDA = TempDialog.BDATextBox.GetTextWithoutDelimiters();
                            CyBLE_MTK_Application.Properties.Settings.Default.BDAIncrement = BDAProgrammer.AutoIncrementBDA;
                            CyBLE_MTK_Application.Properties.Settings.Default.BDAUseProgrammer = BDAProgrammer.UseProgrammer;
                            CyBLE_MTK_Application.Properties.Settings.Default.Save();
                        }
                        MTKTestBDAProgrammer MTKBDAProgrammer = new MTKTestBDAProgrammer(Log);
                        UpdateTestProgramWithNewTest(MTKBDAProgrammer);
                    }
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[8])
            {
                MTKTestAnritsu AnritsuTest = new MTKTestAnritsu(Log);
                UpdateTestProgramWithNewTest(AnritsuTest);
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[9])
            {
                MTKTestSTC STCTest = new MTKTestSTC(Log, MTKPort, DUTPort);
                MTKTestSTCDialog temp = new MTKTestSTCDialog(STCTest);
                if (temp.ShowDialog() == DialogResult.OK)
                {
                    UpdateTestProgramWithNewTest(STCTest);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[10])
            {
                MTKTestCUS CustomTest = new MTKTestCUS(Log, MTKPort, DUTPort);
                MTKTestCUSDialog temp = new MTKTestCUSDialog(CustomTest);
                if (temp.ShowDialog() == DialogResult.OK)
                {
                    UpdateTestProgramWithNewTest(CustomTest);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[11])
            {
                MTKPSoCProgrammer MTKProgrammer = new MTKPSoCProgrammer(Log);
                if (MTKProgrammer.PSoCProgrammerInstalled && MTKProgrammer.IsCorrectVersion())
                {
                    MTKTestProgramAll CustomTest = new MTKTestProgramAll(Log, MTKPort, DUTPort);
                    MTKTestProgramAllDialog temp = new MTKTestProgramAllDialog(CustomTest);
                    if (temp.ShowDialog() == DialogResult.OK)
                    {
                        UpdateTestProgramWithNewTest(CustomTest);
                    }
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[12])
            {
                MTKTestI2C CustomTest = new MTKTestI2C(Log);
                MTKTestI2CDialog temp = new MTKTestI2CDialog(CustomTest);
                if (temp.ShowDialog() == DialogResult.OK)
                {
                    UpdateTestProgramWithNewTest(CustomTest);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[13])
            {
                MTKTestRSX GetRSSI = new MTKTestRSX(Log, MTKPort, DUTPort);
                MTKTestRSXDialog temp = new MTKTestRSXDialog(GetRSSI);
                if (temp.ShowDialog() == DialogResult.OK)
                {
                    UpdateTestProgramWithNewTest(GetRSSI);
                }
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[14])
            {
                MTKTestRBA ReadBDA = new MTKTestRBA(Log, MTKPort, DUTPort);
                UpdateTestProgramWithNewTest(ReadBDA);
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[15])
            {
                MTKTestXOCalibration XOCalibration = new MTKTestXOCalibration(Log, MTKPort, DUTPort);
                MTKTestXOCalDialog temp = new MTKTestXOCalDialog(XOCalibration);
                if (temp.ShowDialog() == DialogResult.OK)
                {
                    UpdateTestProgramWithNewTest(XOCalibration);
                }
            }
            else if (ListBoxDoubleClick == false)
            {
                MessageBox.Show("Select a test to add to the program.", "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            ListBoxDoubleClick = false;
        }

        private bool IsBDAProgrammerPresent()
        {
            for (int i = 0; i < TestProgram.Count; i++)
            {
                if (TestProgram[i].ToString() == "MTKTestBDAProgrammer")
                {
                    return true;
                }
            }
            return false;
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            if (EnableMessages == true)
            {
                if (MessageBox.Show("All changes made to the test program will be lost. Click 'Cancel' to continue editing." +
                    "\n You can enable/disable this message at \"Edit > Preferences > General\".",
                    "Cancel all changes", MessageBoxButtons.OKCancel, MessageBoxIcon.Information) == DialogResult.OK)
                {
                    RestoreOnCancel = true;
                    DontClose = false;
                    this.Close();
                }
                else
                {
                    DontClose = true;
                }
            }
            else
            {
                RestoreOnCancel = true;
                DontClose = false;
                this.Close();
            }
        }

        protected override void  OnClosing(CancelEventArgs e)
        {
            if (DontClose == true)
            {
                DontClose = false;
                e.Cancel = true;
            }
            if (RestoreOnCancel == true)
            {
                TestProgram = CopyTestList(BackupList);
                TestProgramListBox.Items.Clear();
                TestProgramListBox.Items.AddRange(BackupTests);
                TestProgramListBox.SelectedIndex = BackupSelectedTest;
                Log.PrintLog(this, "All changes made to the test program cancelled.", LogDetailLevel.LogEverything);
            }
            base.OnClosing(e);
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            RestoreOnCancel = false;
            DontClose = false;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void RemoveTestButton_Click(object sender, EventArgs e)
        {
            if (TestProgramListBox.SelectedIndex >= 0)
            {
                do
                {
                    Log.PrintLog(this, "Removing " + TestProgram[TestProgramListBox.SelectedIndex].ToString() + " from the test list.",
                        LogDetailLevel.LogEverything);
                    TestProgram.RemoveAt(TestProgramListBox.SelectedIndex);
                    TestProgramListBox.Items.RemoveAt(TestProgramListBox.SelectedIndex);
                } while (TestProgramListBox.SelectedIndex >= 0);
            }
            else
            {
                MessageBox.Show("Select a test to remove from the program.", "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

        }

        private void AvailableTestListBox_DoubleClick(object sender, MouseEventArgs e)
        {
            ListBoxDoubleClick = true;
            AddTestButton.PerformClick();
        }

        private void AvailableTestListBox_MouseDown(object sender, MouseEventArgs e)
        {
            AvailableTestListBox.SelectedIndex = AvailableTestListBox.IndexFromPoint(e.X, e.Y);

            if (e.Button == MouseButtons.Right)
            {
                if (AvailableTestListBox.SelectedIndex != -1)
                {
                    BatchAddToolStripMenuItem.Enabled = false;

                    if (AvailableTestListBox.SelectedItem.ToString() == "RX Packet Error Rate (PER) Test" ||
                        AvailableTestListBox.SelectedItem.ToString() == "TX Packet Error Rate (PER) Test" ||
                        AvailableTestListBox.SelectedItem.ToString() == "Transmit Carrier Wave" ||
                        AvailableTestListBox.SelectedItem.ToString() == "Transmit Packets" ||
                        AvailableTestListBox.SelectedItem.ToString() == "Receive Packets")
                    //if (AvailableTestListBox.SelectedIndex >= 5)
                    //{
                    //    BatchAddToolStripMenuItem.Enabled = false;
                    //}
                    //else
                    {
                        BatchAddToolStripMenuItem.Enabled = true;
                    }
                    AvailableTestRightClickMenu.Show(Cursor.Position);
                }
            }
        }

        private void AvailableTestListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (AvailableTestListBox.SelectedIndex < 0)
            {
                AddTestButton.Enabled = false;
            }
            else
            {
                AddTestButton.Enabled = true;
            }
        }

        private void TestProgramListBox_DoubleClick(object sender, MouseEventArgs e)
        {
            for (int i = 0; i < TestProgramListBox.Items.Count; i++)
            {
                var rect = TestProgramListBox.GetItemRectangle(i);
                if (rect.Contains(e.Location))
                {
                    if (EditTestAt(i))
                    {
                        TestProgramListBox.Items[i] = TestProgram[i].GetDisplayText();
                        TestProgramListBox.Refresh();
                    }
                    break;
                }
            }
        }

        private bool EditTestAt(int TestIndex)
        {
            if (TestProgram[TestIndex].ToString() == "MTKTestRXPER")
            {
                MTKTestPERDialog TempDialog = new MTKTestPERDialog();
                TempDialog.Text = "RX PER Test Configuration";
                TempDialog.ChannelNumber.SelectedIndex = ((MTKTestRXPER)TestProgram[TestIndex]).ChannelNumber;
                int i = ((MTKTestRXPER)TestProgram[TestIndex]).PowerLevel;
                TempDialog.PowerLevel.Text = i.ToString() + " dBm";
                TempDialog.NumberOfPackets.Value = (decimal)((MTKTestRXPER)TestProgram[TestIndex]).NumberOfPackets;
                TempDialog.PacketTypeComboBox.Text = ((MTKTestRXPER)TestProgram[TestIndex]).PacketType;
                TempDialog.PacketLengthNumericUpDown.Value = (decimal)((MTKTestRXPER)TestProgram[TestIndex]).PacketLength;
                TempDialog.PERPassCriterionNumericUpDown.Value = (decimal)((MTKTestRXPER)TestProgram[TestIndex]).PERPassCriterion;
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    ((MTKTestRXPER)TestProgram[TestIndex]).ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    ((MTKTestRXPER)TestProgram[TestIndex]).PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    ((MTKTestRXPER)TestProgram[TestIndex]).NumberOfPackets = (int)TempDialog.NumberOfPackets.Value;
                    ((MTKTestRXPER)TestProgram[TestIndex]).PacketType = TempDialog.PacketTypeComboBox.Text;
                    ((MTKTestRXPER)TestProgram[TestIndex]).PacketLength = (int)TempDialog.PacketLengthNumericUpDown.Value;
                    ((MTKTestRXPER)TestProgram[TestIndex]).PERPassCriterion = (double)TempDialog.PERPassCriterionNumericUpDown.Value;
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestTXPER")
            {
                MTKTestPERDialog TempDialog = new MTKTestPERDialog();
                TempDialog.Text = "TX PER Test Configuration";
                TempDialog.ChannelNumber.SelectedIndex = ((MTKTestTXPER)TestProgram[TestIndex]).ChannelNumber;
                int i = ((MTKTestTXPER)TestProgram[TestIndex]).PowerLevel;
                TempDialog.PowerLevel.Text = i.ToString() + " dBm";
                TempDialog.NumberOfPackets.Value = (decimal)((MTKTestTXPER)TestProgram[TestIndex]).NumberOfPackets;
                TempDialog.PacketTypeComboBox.Text = ((MTKTestTXPER)TestProgram[TestIndex]).PacketType;
                TempDialog.PacketLengthNumericUpDown.Value = (decimal)((MTKTestTXPER)TestProgram[TestIndex]).PacketLength;
                TempDialog.PERPassCriterionNumericUpDown.Value = (decimal)((MTKTestTXPER)TestProgram[TestIndex]).PERPassCriterion;
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    ((MTKTestTXPER)TestProgram[TestIndex]).ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    ((MTKTestTXPER)TestProgram[TestIndex]).PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    ((MTKTestTXPER)TestProgram[TestIndex]).NumberOfPackets = (int)TempDialog.NumberOfPackets.Value;
                    ((MTKTestTXPER)TestProgram[TestIndex]).PacketType = TempDialog.PacketTypeComboBox.Text;
                    ((MTKTestTXPER)TestProgram[TestIndex]).PacketLength = (int)TempDialog.PacketLengthNumericUpDown.Value;
                    ((MTKTestTXPER)TestProgram[TestIndex]).PERPassCriterion = (double)TempDialog.PERPassCriterionNumericUpDown.Value;
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestTXP")
            {
                MTKTestRxTxDialog TempDialog = new MTKTestRxTxDialog();
                TempDialog.Text = "TX Packet Test Configuration";
                TempDialog.ChannelNumber.SelectedIndex = ((MTKTestTXP)TestProgram[TestIndex]).ChannelNumber;
                int i = ((MTKTestTXP)TestProgram[TestIndex]).PowerLevel;
                TempDialog.PowerLevel.Text = i.ToString() + " dBm";
                TempDialog.NumberOfPackets.Value = (decimal)((MTKTestTXP)TestProgram[TestIndex]).NumberOfPackets;
                TempDialog.PacketTypeComboBox.Text = ((MTKTestTXP)TestProgram[TestIndex]).PacketType;
                TempDialog.PacketLengthNumericUpDown.Value = (decimal)((MTKTestTXP)TestProgram[TestIndex]).PacketLength;
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    ((MTKTestTXP)TestProgram[TestIndex]).ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    ((MTKTestTXP)TestProgram[TestIndex]).PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    ((MTKTestTXP)TestProgram[TestIndex]).NumberOfPackets = (int)TempDialog.NumberOfPackets.Value;
                    ((MTKTestTXP)TestProgram[TestIndex]).PacketType = TempDialog.PacketTypeComboBox.Text;
                    ((MTKTestTXP)TestProgram[TestIndex]).PacketLength = (int)TempDialog.PacketLengthNumericUpDown.Value;
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestRXP")
            {
                MTKTestRxTxDialog TempDialog = new MTKTestRxTxDialog();
                TempDialog.Text = "RX Packet Test Configuration";
                TempDialog.ChannelNumber.SelectedIndex = ((MTKTestRXP)TestProgram[TestIndex]).ChannelNumber;
                int i = ((MTKTestRXP)TestProgram[TestIndex]).PowerLevel;
                TempDialog.PowerLevel.Text = i.ToString() + " dBm";
                TempDialog.NumberOfPackets.Value = (decimal)((MTKTestRXP)TestProgram[TestIndex]).NumberOfPackets;
                TempDialog.PacketTypeComboBox.Text = ((MTKTestRXP)TestProgram[TestIndex]).PacketType;
                TempDialog.PacketLengthNumericUpDown.Value = (decimal)((MTKTestRXP)TestProgram[TestIndex]).PacketLength;
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    ((MTKTestRXP)TestProgram[TestIndex]).ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    ((MTKTestRXP)TestProgram[TestIndex]).PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    ((MTKTestRXP)TestProgram[TestIndex]).NumberOfPackets = (int)TempDialog.NumberOfPackets.Value;
                    ((MTKTestRXP)TestProgram[TestIndex]).PacketType = TempDialog.PacketTypeComboBox.Text;
                    ((MTKTestRXP)TestProgram[TestIndex]).PacketLength = (int)TempDialog.PacketLengthNumericUpDown.Value;
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestTXCW")
            {
                MTKTestTXCWDialog TempDialog = new MTKTestTXCWDialog();
                TempDialog.Text = "TX CW Test Configuration";
                TempDialog.ChannelNumber.SelectedIndex = ((MTKTestTXCW)TestProgram[TestIndex]).ChannelNumber;
                int i = ((MTKTestTXCW)TestProgram[TestIndex]).PowerLevel;
                TempDialog.PowerLevel.Text = i.ToString() + " dBm";
                TempDialog.DurationForTXCW.Value = (decimal)((MTKTestTXCW)TestProgram[TestIndex]).DurationForTXCW;
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    ((MTKTestTXCW)TestProgram[TestIndex]).ChannelNumber = TempDialog.ChannelNumber.SelectedIndex;
                    ((MTKTestTXCW)TestProgram[TestIndex]).PowerLevel = int.Parse(MTKTest.GetPowerLevel(TempDialog.PowerLevel.Text));
                    ((MTKTestTXCW)TestProgram[TestIndex]).DurationForTXCW = (int)TempDialog.DurationForTXCW.Value;
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKPSoCProgrammer")
            {
                MTKPSoCProgrammerDialog TempDialog = new MTKPSoCProgrammerDialog((MTKPSoCProgrammer)TestProgram[TestIndex]);
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestDelay")
            {
                MTKTestDelayDialog TempDialog = new MTKTestDelayDialog();
                TempDialog.DelayNumericUpDown.Value = (decimal)((MTKTestDelay)TestProgram[TestIndex]).DelayInMS;
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    ((MTKTestDelay)TestProgram[TestIndex]).DelayInMS = (int)TempDialog.DelayNumericUpDown.Value;
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestBDAProgrammer")
            {
                MTKTestBDADialog TempDialog = new MTKTestBDADialog(BDAProgrammer);
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    CyBLE_MTK_Application.Properties.Settings.Default.BDA = TempDialog.BDATextBox.GetTextWithoutDelimiters();
                    CyBLE_MTK_Application.Properties.Settings.Default.BDAIncrement = BDAProgrammer.AutoIncrementBDA;
                    CyBLE_MTK_Application.Properties.Settings.Default.BDAUseProgrammer = BDAProgrammer.UseProgrammer;
                    CyBLE_MTK_Application.Properties.Settings.Default.Save();
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestProgramAll")
            {
                MTKTestProgramAllDialog temp = new MTKTestProgramAllDialog((MTKTestProgramAll)TestProgram[TestIndex]);
                if (temp.ShowDialog() == DialogResult.OK)
                {
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestSTC")
            {
                MTKTestSTCDialog TempDialog = new MTKTestSTCDialog((MTKTestSTC)TestProgram[TestIndex]);
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestCUS")
            {
                MTKTestCUSDialog TempDialog = new MTKTestCUSDialog((MTKTestCUS)TestProgram[TestIndex]);
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestI2C")
            {
                MTKTestI2CDialog TempDialog = new MTKTestI2CDialog((MTKTestI2C)TestProgram[TestIndex]);
                if (TempDialog.ShowDialog() == DialogResult.OK)
                {
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestRSX")
            {
                MTKTestRSXDialog temp = new MTKTestRSXDialog((MTKTestRSX)TestProgram[TestIndex]);
                if (temp.ShowDialog() == DialogResult.OK)
                {
                    return true;
                }
            }
            else if (TestProgram[TestIndex].ToString() == "MTKTestXOCalibration")
            {
                MTKTestXOCalDialog temp = new MTKTestXOCalDialog((MTKTestXOCalibration)TestProgram[TestIndex]);
                if (temp.ShowDialog() == DialogResult.OK)
                {
                    return true;
                }
            }

            return false;
        }

        private void TestProgramListBox_MouseDown(object sender, MouseEventArgs e)
        {
            if ((ModifierKeys & Keys.Control) != Keys.Control)
            {
                TestProgramListBox.ClearSelected();
                TestProgramListBox.SelectedIndex = TestProgramListBox.IndexFromPoint(e.X, e.Y);
            }
            else
            {
                TestProgramListBox.SetSelected(TestProgramListBox.IndexFromPoint(e.X, e.Y), true);
            }
        }

        private void TestProgramListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (TestProgramListBox.SelectedIndex < 0)
            {
                RemoveTestButton.Enabled = false;
                UpButton.Enabled = false;
                DownButton.Enabled = false;
            }
            else
            {
                RemoveTestButton.Enabled = true;
                if (TestProgramListBox.SelectedIndex == 0)
                {
                    UpButton.Enabled = false;
                }
                else
                {
                    UpButton.Enabled = true;
                }

                if (TestProgramListBox.SelectedIndex == (TestProgramListBox.Items.Count - 1))
                {
                    DownButton.Enabled = false;
                }
                else
                {
                    DownButton.Enabled = true;
                }
            }
        }

        private void UpButton_Click(object sender, EventArgs e)
        {
            int NewIndex, OldIndex;

            OldIndex = TestProgramListBox.SelectedIndex;
            MTKTest temp = TestProgram[OldIndex];
            TestProgram.RemoveAt(OldIndex);
            NewIndex = OldIndex - 1;
            if (NewIndex < 0)
            {
                NewIndex = 0;
            }

            TestProgram.Insert(NewIndex, temp);

            TestProgramListBox.Items.Clear();
            foreach (MTKTest Tests in TestProgram)
            {
                TestProgramListBox.Items.Add(Tests.GetDisplayText());
            }

            TestProgramListBox.SelectedIndex = NewIndex;
        }

        private void DownButton_Click(object sender, EventArgs e)
        {
            int NewIndex, OldIndex;

            OldIndex = TestProgramListBox.SelectedIndex;
            MTKTest temp = TestProgram[OldIndex];
            TestProgram.RemoveAt(OldIndex);
            NewIndex = OldIndex + 1;
            if (NewIndex >= TestProgram.Count)
            {
                TestProgram.Add(temp);
            }
            else
            {
                TestProgram.Insert(NewIndex, temp);
            }

            TestProgramListBox.Items.Clear();
            foreach (MTKTest Tests in TestProgram)
            {
                TestProgramListBox.Items.Add(Tests.GetDisplayText());
            }

            if (NewIndex >= TestProgram.Count)
            {
                TestProgramListBox.SelectedIndex = NewIndex - 1;
            }
            else
            {
                TestProgramListBox.SelectedIndex = NewIndex;
            }
        }

        private void AvailableTestRightClickMenu_Opening(object sender, CancelEventArgs e)
        {
        }

        private void AddToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AddTestButton.PerformClick();
        }

        private void BatchAddToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BatchAddMTKTests BatchAddTestsDialog = new BatchAddMTKTests(Log, MTKPort, DUTPort);
            if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[0])
            {
                BatchAddTestsDialog.Text = "Batch Configure " + ListOfAvailableTests[0];
                BatchAddTestsDialog.TestType = new MTKTestRXPER().ToString();
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[1])
            {
                BatchAddTestsDialog.Text = "Batch Configure " + ListOfAvailableTests[1];
                BatchAddTestsDialog.TestType = new MTKTestTXPER().ToString();
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[2])
            {
                BatchAddTestsDialog.Text = "Batch Configure " + ListOfAvailableTests[2];
                BatchAddTestsDialog.TestType = new MTKTestTXCW().ToString();
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[3])
            {
                BatchAddTestsDialog.Text = "Batch Configure " + ListOfAvailableTests[3];
                BatchAddTestsDialog.TestType = new MTKTestTXP().ToString();
            }
            else if (AvailableTestListBox.GetItemText(AvailableTestListBox.SelectedItem) == ListOfAvailableTests[4])
            {
                BatchAddTestsDialog.Text = "Batch Configure " + ListOfAvailableTests[4];
                BatchAddTestsDialog.TestType = new MTKTestRXP().ToString();
            }
            BatchAddTestsDialog.TestProgramList = this.TestProgram;
            if (BatchAddTestsDialog.ShowDialog() == DialogResult.OK)
            {
                this.TestProgramList.Clear();
                this.TestProgramList = BatchAddTestsDialog.TestProgramList;
            }
        }
    }
}

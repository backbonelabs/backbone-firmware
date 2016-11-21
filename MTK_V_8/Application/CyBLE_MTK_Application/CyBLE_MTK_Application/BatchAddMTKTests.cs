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
    public partial class BatchAddMTKTests : Form
    {
        private LogManager Log;
        private SerialPort MTKPort, DUTPort;
        public string TestType;
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
            }
        }
        private bool DontClose;
        private int _ChannelCount, _PowerCount, _PKTTypeCount, _PKTLengthCount, _NUMPKTCount, _PERPassCount, TotalTests, CurrentTestNum;
        private int CurrentChannel, CurrentPower, CurrentPKTType, CurrentPKTLength;
        private decimal CurrentNUMPKT, CurrentPERPass;
        protected Thread AddThread;

        public List<MTKTest> CopyTestList(List<MTKTest> InputList)
        {
            List<MTKTest> OutputList = new List<MTKTest>();

            foreach (MTKTest Test in InputList)
            {
                OutputList.Add(Test);
            }

            return OutputList;
        }

        public BatchAddMTKTests()
        {
            InitializeComponent();
            this.NUMPKTListBox.MouseDown += new MouseEventHandler(NUMPKTListBox_MouseDown);
            this.PERPassListBox.MouseDown += new MouseEventHandler(PERPassListBox_MouseDown);
            TestType = "";
            TestProgram = new List<MTKTest>();
            DontClose = false;
        }

        public BatchAddMTKTests(LogManager Logger, SerialPort MTKSerialPort, SerialPort DUTSerialPort)
            : this()
        {
            Log = Logger;
            MTKPort = MTKSerialPort;
            DUTPort = DUTSerialPort;
        }

        protected override void OnLoad(EventArgs e)
        {
            if (TestType == new MTKTestTXCW().ToString())
            {
                groupBox3.Enabled = false;
                groupBox4.Enabled = false;
                groupBox6.Enabled = false;

                groupBox5.Text = "Test Duration(ms)";
                NumberOfPackets.Minimum = -1;
                NumberOfPackets.Value = -1;
                NumberOfPackets.Maximum = 2147483647;
            }
            else if ((TestType == new MTKTestTXP().ToString()) || (TestType == new MTKTestRXP().ToString()))
            {
                groupBox6.Enabled = false;
            }
            base.OnLoad(e);
        }

        private void ChannelSelectAllButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < ChannelCheckedListBox.Items.Count; i++)
            {
                ChannelCheckedListBox.SetItemChecked(i, true);
            }
        }

        private void ChannelSelectNoneButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < ChannelCheckedListBox.Items.Count; i++)
            {
                ChannelCheckedListBox.SetItemChecked(i, false);
            }
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (DontClose == true)
            {
                DontClose = false;
                e.Cancel = true;
            }
        }

        private int GetChannelSelection()
        {
            int ReturnCount = 0;

            for (int i = 0; i < ChannelCheckedListBox.Items.Count; i++)
            {
                if (ChannelCheckedListBox.GetItemChecked(i) == true)
                {
                    ReturnCount++;
                }
            }
            return ReturnCount;
        }

        private int GetPowerSelection()
        {
            int ReturnCount = 0;

            for (int i = 0; i < PowerCheckedListBox.Items.Count; i++)
            {
                if (PowerCheckedListBox.GetItemChecked(i) == true)
                {
                    ReturnCount++;
                }
            }
            return ReturnCount;
        }

        private int GetPKTTypeSelection()
        {
            int ReturnCount = 0;

            for (int i = 0; i < PKTTypeCheckedListBox.Items.Count; i++)
            {
                if (PKTTypeCheckedListBox.GetItemChecked(i) == true)
                {
                    ReturnCount++;
                }
            }
            return ReturnCount;
        }

        private int GetPKTLengthSelection()
        {
            int ReturnCount = 0;

            for (int i = 0; i < PKTLenghtCheckedListBox.Items.Count; i++)
            {
                if (PKTLenghtCheckedListBox.GetItemChecked(i) == true)
                {
                    ReturnCount++;
                }
            }
            return ReturnCount;
        }

        private int GetNUMPKTSelection()
        {
            return NUMPKTListBox.Items.Count;
        }

        private int GetPERPassSelection()
        {
            return PERPassListBox.Items.Count;
        }

        private void AddNewTest(bool Channel, bool Power, bool PKTType, bool PKTLength, bool NUMPKT, bool PERPass)
        {
            if (TestType == new MTKTestRXPER().ToString())
            {
                MTKTestRXPER RXPERTest = new MTKTestRXPER(Log, MTKPort, DUTPort);
                if (Channel)
                {
                    RXPERTest.ChannelNumber = CurrentChannel;
                }
                if (Power)
                {
                    RXPERTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(PowerCheckedListBox.GetItemText(PowerCheckedListBox.Items[CurrentPower])));
                }
                if (PKTType)
                {
                    RXPERTest.PacketType = PKTTypeCheckedListBox.GetItemText(PKTTypeCheckedListBox.Items[CurrentPKTType]);
                }
                if (PKTLength)
                {
                    RXPERTest.PacketLength = CurrentPKTLength;
                }
                if (NUMPKT)
                {
                    RXPERTest.NumberOfPackets = (int)CurrentNUMPKT;
                }
                if (PERPass)
                {
                    RXPERTest.PERPassCriterion = (double)CurrentPERPass;
                }
                TestProgram.Add(RXPERTest);
            }
            else if (TestType == new MTKTestTXPER().ToString())
            {
                MTKTestTXPER TXPERTest = new MTKTestTXPER(Log, MTKPort, DUTPort);
                if (Channel)
                {
                    TXPERTest.ChannelNumber = CurrentChannel;
                }
                if (Power)
                {
                    TXPERTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(PowerCheckedListBox.GetItemText(PowerCheckedListBox.Items[CurrentPower])));
                }
                if (PKTType)
                {
                    TXPERTest.PacketType = PKTTypeCheckedListBox.GetItemText(PKTTypeCheckedListBox.Items[CurrentPKTType]);
                }
                if (PKTLength)
                {
                    TXPERTest.PacketLength = CurrentPKTLength;
                }
                if (NUMPKT)
                {
                    TXPERTest.NumberOfPackets = (int)CurrentNUMPKT;
                }
                if (PERPass)
                {
                    TXPERTest.PERPassCriterion = (double)CurrentPERPass;
                }
                TestProgram.Add(TXPERTest);
            }
            else if (TestType == new MTKTestTXCW().ToString())
            {
                MTKTestTXCW TXCWTest = new MTKTestTXCW(Log, MTKPort, DUTPort);
                if (Channel)
                {
                    TXCWTest.ChannelNumber = CurrentChannel;
                }
                if (Power)
                {
                    TXCWTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(PowerCheckedListBox.GetItemText(PowerCheckedListBox.Items[CurrentPower])));
                }
                if (NUMPKT)
                {
                    TXCWTest.DurationForTXCW = (int)CurrentNUMPKT;
                }
                TestProgram.Add(TXCWTest);
            }
            else if (TestType == new MTKTestTXP().ToString())
            {
                MTKTestTXP TXPTest = new MTKTestTXP(Log, MTKPort, DUTPort);
                if (Channel)
                {
                    TXPTest.ChannelNumber = CurrentChannel;
                }
                if (Power)
                {
                    TXPTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(PowerCheckedListBox.GetItemText(PowerCheckedListBox.Items[CurrentPower])));
                }
                if (PKTType)
                {
                    TXPTest.PacketType = PKTTypeCheckedListBox.GetItemText(PKTTypeCheckedListBox.Items[CurrentPKTType]);
                }
                if (PKTLength)
                {
                    TXPTest.PacketLength = CurrentPKTLength;
                }
                if (NUMPKT)
                {
                    TXPTest.NumberOfPackets = (int)CurrentNUMPKT;
                }
                TestProgram.Add(TXPTest);
            }
            else if (TestType == new MTKTestRXP().ToString())
            {
                MTKTestRXP RXPTest = new MTKTestRXP(Log, MTKPort, DUTPort);
                if (Channel)
                {
                    RXPTest.ChannelNumber = CurrentChannel;
                }
                if (Power)
                {
                    RXPTest.PowerLevel = int.Parse(MTKTest.GetPowerLevel(PowerCheckedListBox.GetItemText(PowerCheckedListBox.Items[CurrentPower])));
                }
                if (PKTType)
                {
                    RXPTest.PacketType = PKTTypeCheckedListBox.GetItemText(PKTTypeCheckedListBox.Items[CurrentPKTType]);
                }
                if (PKTLength)
                {
                    RXPTest.PacketLength = CurrentPKTLength;
                }
                if (NUMPKT)
                {
                    RXPTest.NumberOfPackets = (int)CurrentNUMPKT;
                }
                TestProgram.Add(RXPTest);
            }

            CurrentTestNum++;
            this.Invoke(new MethodInvoker(() => UpdateAddStatus()));
        }

        void UpdateAddStatus()
        {
            progressBar1.Value = CurrentTestNum;
        }

        private void AddTests(int NumTests)
        {
            TotalTests = NumTests;
            CurrentTestNum = 0;

            if (NumTests > 0)
            {
                if (GetChannelSelection() > 0)
                {
                    _ChannelCount = 0;
                    for (int i = 0; i < GetChannelSelection(); i++)
                    {
                        CurrentChannel = GetNextChannel();
                        if (GetPowerSelection() > 0)
                        {
                            _PowerCount = 0;
                            for (int j = 0; j < GetPowerSelection(); j++)
                            {
                                CurrentPower = GetNextPower();
                                if (GetPKTTypeSelection() > 0)
                                {
                                    _PKTTypeCount = 0;
                                    for (int k = 0; k < GetPKTTypeSelection(); k++)
                                    {
                                        CurrentPKTType = GetNextPKTType();
                                        if (GetPKTLengthSelection() > 0)
                                        {
                                            _PKTLengthCount = 0;
                                            for (int l = 0; l < GetPKTLengthSelection(); l++)
                                            {
                                                CurrentPKTLength = GetNextPKTLength();
                                                if (GetNUMPKTSelection() > 0)
                                                {
                                                    _NUMPKTCount = 0;
                                                    for (int m = 0; m < GetNUMPKTSelection(); m++)
                                                    {
                                                        CurrentNUMPKT = GetNextNUMPKT();
                                                        if (GetPERPassSelection() > 0)
                                                        {
                                                            _PERPassCount = 0;
                                                            for (int n = 0; n < GetPERPassSelection(); n++)
                                                            {
                                                                CurrentPERPass = GetNextPERPass();
                                                                AddNewTest(true, true, true, true, true, true);
                                                            }
                                                        }
                                                        else  // PER Pass
                                                        {
                                                            AddNewTest(true, true, true, true, true, false);
                                                        }
                                                    }
                                                }
                                                else  // Number Of Packets
                                                {
                                                    if (GetPERPassSelection() > 0)
                                                    {
                                                        _PERPassCount = 0;
                                                        for (int n = 0; n < GetPERPassSelection(); n++)
                                                        {
                                                            CurrentPERPass = GetNextPERPass();
                                                            AddNewTest(true, true, true, true, false, true);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        AddNewTest(true, true, true, true, false, false);
                                                    }
                                                }
                                            }
                                        }
                                        else  // PKT Length
                                        {
                                            if (GetNUMPKTSelection() > 0)
                                            {
                                                _NUMPKTCount = 0;
                                                for (int m = 0; m < GetNUMPKTSelection(); m++)
                                                {
                                                    CurrentNUMPKT = GetNextNUMPKT();
                                                    if (GetPERPassSelection() > 0)
                                                    {
                                                        _PERPassCount = 0;
                                                        for (int n = 0; n < GetPERPassSelection(); n++)
                                                        {
                                                            CurrentPERPass = GetNextPERPass();
                                                            AddNewTest(true, true, true, false, true, true);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        AddNewTest(true, true, true, false, true, false);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(true, true, true, false, false, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(true, true, true, false, false, false);
                                                }
                                            }
                                        }
                                    }
                                }
                                else  // PKT Type
                                {
                                    if (GetPKTLengthSelection() > 0)
                                    {
                                        _PKTLengthCount = 0;
                                        for (int l = 0; l < GetPKTLengthSelection(); l++)
                                        {
                                            CurrentPKTLength = GetNextPKTLength();
                                            if (GetNUMPKTSelection() > 0)
                                            {
                                                _NUMPKTCount = 0;
                                                for (int m = 0; m < GetNUMPKTSelection(); m++)
                                                {
                                                    CurrentNUMPKT = GetNextNUMPKT();
                                                    if (GetPERPassSelection() > 0)
                                                    {
                                                        _PERPassCount = 0;
                                                        for (int n = 0; n < GetPERPassSelection(); n++)
                                                        {
                                                            CurrentPERPass = GetNextPERPass();
                                                            AddNewTest(true, true, false, true, true, true);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        AddNewTest(true, true, false, true, true, false);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(true, true, false, true, false, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(true, true, false, true, false, false);
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (GetNUMPKTSelection() > 0)
                                        {
                                            _NUMPKTCount = 0;
                                            for (int m = 0; m < GetNUMPKTSelection(); m++)
                                            {
                                                CurrentNUMPKT = GetNextNUMPKT();
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(true, true, false, false, true, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(true, true, false, false, true, false);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(true, true, false, false, false, true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(true, true, false, false, false, false);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else // Power
                        {
                            if (GetPKTTypeSelection() > 0)
                            {
                                _PKTTypeCount = 0;
                                for (int k = 0; k < GetPKTTypeSelection(); k++)
                                {
                                    CurrentPKTType = GetNextPKTType();
                                    if (GetPKTLengthSelection() > 0)
                                    {
                                        _PKTLengthCount = 0;
                                        for (int l = 0; l < GetPKTLengthSelection(); l++)
                                        {
                                            CurrentPKTLength = GetNextPKTLength();
                                            if (GetNUMPKTSelection() > 0)
                                            {
                                                _NUMPKTCount = 0;
                                                for (int m = 0; m < GetNUMPKTSelection(); m++)
                                                {
                                                    CurrentNUMPKT = GetNextNUMPKT();
                                                    if (GetPERPassSelection() > 0)
                                                    {
                                                        _PERPassCount = 0;
                                                        for (int n = 0; n < GetPERPassSelection(); n++)
                                                        {
                                                            CurrentPERPass = GetNextPERPass();
                                                            AddNewTest(true, false, true, true, true, true);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        AddNewTest(true, false, true, true, true, false);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(true, false, true, true, false, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(true, false, true, true, false, false);
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (GetNUMPKTSelection() > 0)
                                        {
                                            _NUMPKTCount = 0;
                                            for (int m = 0; m < GetNUMPKTSelection(); m++)
                                            {
                                                CurrentNUMPKT = GetNextNUMPKT();
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(true, false, true, false, true, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(true, false, true, false, true, false);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(true, false, true, false, false,true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(true, false, true, false, false, false);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (GetPKTLengthSelection() > 0)
                                {
                                    _PKTLengthCount = 0;
                                    for (int l = 0; l < GetPKTLengthSelection(); l++)
                                    {
                                        CurrentPKTLength = GetNextPKTLength();
                                        if (GetNUMPKTSelection() > 0)
                                        {
                                            _NUMPKTCount = 0;
                                            for (int m = 0; m < GetNUMPKTSelection(); m++)
                                            {
                                                CurrentNUMPKT = GetNextNUMPKT();
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(true, false, false, true, true, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(true, false, false, true, true, false);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(true, false, false, true, false, true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(true, false, false, true, false, false);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if (GetNUMPKTSelection() > 0)
                                    {
                                        _NUMPKTCount = 0;
                                        for (int m = 0; m < GetNUMPKTSelection(); m++)
                                        {
                                            CurrentNUMPKT = GetNextNUMPKT();
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(true, false, false, false, true, true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(true, false, false, false, true, false);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (GetPERPassSelection() > 0)
                                        {
                                            _PERPassCount = 0;
                                            for (int n = 0; n < GetPERPassSelection(); n++)
                                            {
                                                CurrentPERPass = GetNextPERPass();
                                                AddNewTest(true, false, false, false, false, true);
                                            }
                                        }
                                        else
                                        {
                                            AddNewTest(true, false, false, false, false, false);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else  // Channel
                {
                    if (GetPowerSelection() > 0)
                    {
                        _PowerCount = 0;
                        for (int j = 0; j < GetPowerSelection(); j++)
                        {
                            CurrentPower = GetNextPower();
                            if (GetPKTTypeSelection() > 0)
                            {
                                _PKTTypeCount = 0;
                                for (int k = 0; k < GetPKTTypeSelection(); k++)
                                {
                                    CurrentPKTType = GetNextPKTType();
                                    if (GetPKTLengthSelection() > 0)
                                    {
                                        _PKTLengthCount = 0;
                                        for (int l = 0; l < GetPKTLengthSelection(); l++)
                                        {
                                            CurrentPKTLength = GetNextPKTLength();
                                            if (GetNUMPKTSelection() > 0)
                                            {
                                                _NUMPKTCount = 0;
                                                for (int m = 0; m < GetNUMPKTSelection(); m++)
                                                {
                                                    CurrentNUMPKT = GetNextNUMPKT();
                                                    if (GetPERPassSelection() > 0)
                                                    {
                                                        _PERPassCount = 0;
                                                        for (int n = 0; n < GetPERPassSelection(); n++)
                                                        {
                                                            CurrentPERPass = GetNextPERPass();
                                                            AddNewTest(false, true, true, true, true, true);
                                                        }
                                                    }
                                                    else  // PER Pass
                                                    {
                                                        AddNewTest(false, true, true, true, true, false);
                                                    }
                                                }
                                            }
                                            else  // Number Of Packets
                                            {
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(false, true, true, true, false, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(false, true, true, true, false, false);
                                                }
                                            }
                                        }
                                    }
                                    else  // PKT Length
                                    {
                                        if (GetNUMPKTSelection() > 0)
                                        {
                                            _NUMPKTCount = 0;
                                            for (int m = 0; m < GetNUMPKTSelection(); m++)
                                            {
                                                CurrentNUMPKT = GetNextNUMPKT();
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(false, true, true, false, true, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(false, true, true, false, true, false);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(false, true, true, false, false, true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(false, true, true, false, false, false);
                                            }
                                        }
                                    }
                                }
                            }
                            else  // PKT Type
                            {
                                if (GetPKTLengthSelection() > 0)
                                {
                                    _PKTLengthCount = 0;
                                    for (int l = 0; l < GetPKTLengthSelection(); l++)
                                    {
                                        CurrentPKTLength = GetNextPKTLength();
                                        if (GetNUMPKTSelection() > 0)
                                        {
                                            _NUMPKTCount = 0;
                                            for (int m = 0; m < GetNUMPKTSelection(); m++)
                                            {
                                                CurrentNUMPKT = GetNextNUMPKT();
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(false, true, false, true, true, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(false, true, false, true, true, false);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(false, true, false, true, false, true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(false, true, false, true, false, false);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if (GetNUMPKTSelection() > 0)
                                    {
                                        _NUMPKTCount = 0;
                                        for (int m = 0; m < GetNUMPKTSelection(); m++)
                                        {
                                            CurrentNUMPKT = GetNextNUMPKT();
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(false, true, false, false, true, true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(false, true, false, false, true, false);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (GetPERPassSelection() > 0)
                                        {
                                            _PERPassCount = 0;
                                            for (int n = 0; n < GetPERPassSelection(); n++)
                                            {
                                                CurrentPERPass = GetNextPERPass();
                                                AddNewTest(false, true, false, false, false, true);
                                            }
                                        }
                                        else
                                        {
                                            AddNewTest(false, true, false, false, false, false);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else // Power
                    {
                        if (GetPKTTypeSelection() > 0)
                        {
                            _PKTTypeCount = 0;
                            for (int k = 0; k < GetPKTTypeSelection(); k++)
                            {
                                CurrentPKTType = GetNextPKTType();
                                if (GetPKTLengthSelection() > 0)
                                {
                                    _PKTLengthCount = 0;
                                    for (int l = 0; l < GetPKTLengthSelection(); l++)
                                    {
                                        CurrentPKTLength = GetNextPKTLength();
                                        if (GetNUMPKTSelection() > 0)
                                        {
                                            _NUMPKTCount = 0;
                                            for (int m = 0; m < GetNUMPKTSelection(); m++)
                                            {
                                                CurrentNUMPKT = GetNextNUMPKT();
                                                if (GetPERPassSelection() > 0)
                                                {
                                                    _PERPassCount = 0;
                                                    for (int n = 0; n < GetPERPassSelection(); n++)
                                                    {
                                                        CurrentPERPass = GetNextPERPass();
                                                        AddNewTest(false, false, true, true, true, true);
                                                    }
                                                }
                                                else
                                                {
                                                    AddNewTest(false, false, true, true, true, false);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(false, false, true, true, false, true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(false, false, true, true, false, false);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if (GetNUMPKTSelection() > 0)
                                    {
                                        _NUMPKTCount = 0;
                                        for (int m = 0; m < GetNUMPKTSelection(); m++)
                                        {
                                            CurrentNUMPKT = GetNextNUMPKT();
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(false, false, true, false, true, true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(false, false, true, false, true, false);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (GetPERPassSelection() > 0)
                                        {
                                            _PERPassCount = 0;
                                            for (int n = 0; n < GetPERPassSelection(); n++)
                                            {
                                                CurrentPERPass = GetNextPERPass();
                                                AddNewTest(false, false, true, false, false, true);
                                            }
                                        }
                                        else
                                        {
                                            AddNewTest(false, false, true, false, false, false);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (GetPKTLengthSelection() > 0)
                            {
                                _PKTLengthCount = 0;
                                for (int l = 0; l < GetPKTLengthSelection(); l++)
                                {
                                    CurrentPKTLength = GetNextPKTLength();
                                    if (GetNUMPKTSelection() > 0)
                                    {
                                        _NUMPKTCount = 0;
                                        for (int m = 0; m < GetNUMPKTSelection(); m++)
                                        {
                                            CurrentNUMPKT = GetNextNUMPKT();
                                            if (GetPERPassSelection() > 0)
                                            {
                                                _PERPassCount = 0;
                                                for (int n = 0; n < GetPERPassSelection(); n++)
                                                {
                                                    CurrentPERPass = GetNextPERPass();
                                                    AddNewTest(false, false, false, true, true, true);
                                                }
                                            }
                                            else
                                            {
                                                AddNewTest(false, false, false, true, true, false);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (GetPERPassSelection() > 0)
                                        {
                                            _PERPassCount = 0;
                                            for (int n = 0; n < GetPERPassSelection(); n++)
                                            {
                                                CurrentPERPass = GetNextPERPass();
                                                AddNewTest(false, false, false, true, false, true);
                                            }
                                        }
                                        else
                                        {
                                            AddNewTest(false, false, false, true, false, false);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (GetNUMPKTSelection() > 0)
                                {
                                    _NUMPKTCount = 0;
                                    for (int m = 0; m < GetNUMPKTSelection(); m++)
                                    {
                                        CurrentNUMPKT = GetNextNUMPKT();
                                        if (GetPERPassSelection() > 0)
                                        {
                                            _PERPassCount = 0;
                                            for (int n = 0; n < GetPERPassSelection(); n++)
                                            {
                                                CurrentPERPass = GetNextPERPass();
                                                AddNewTest(false, false, false, false, true, true);
                                            }
                                        }
                                        else
                                        {
                                            AddNewTest(false, false, false, false, true, false);
                                        }
                                    }
                                }
                                else
                                {
                                    if (GetPERPassSelection() > 0)
                                    {
                                        _PERPassCount = 0;
                                        for (int n = 0; n < GetPERPassSelection(); n++)
                                        {
                                            CurrentPERPass = GetNextPERPass();
                                            AddNewTest(false, false, false, false, false, true);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            this.DialogResult = DialogResult.OK;
            this.BeginInvoke(new Action(() => this.Close()));
        }

        private decimal GetNextPERPass()
        {
            if (_PERPassCount < PERPassListBox.Items.Count)
            {
                _PERPassCount++;
            }
            return (decimal)PERPassListBox.Items[_PERPassCount - 1];
        }

        private decimal GetNextNUMPKT()
        {
            if (_NUMPKTCount < NUMPKTListBox.Items.Count)
            {
                _NUMPKTCount++;
            }
            return (decimal)NUMPKTListBox.Items[_NUMPKTCount - 1];
        }

        private int GetNextPKTLength()
        {
            for (; _PKTLengthCount < PKTLenghtCheckedListBox.Items.Count; _PKTLengthCount++)
            {
                if (PKTLenghtCheckedListBox.GetItemChecked(_PKTLengthCount) == true)
                {
                    _PKTLengthCount++;
                    break;
                }
            }
            return _PKTLengthCount - 1;
        }

        private int GetNextPKTType()
        {
            for (; _PKTTypeCount < PKTTypeCheckedListBox.Items.Count; _PKTTypeCount++)
            {
                if (PKTTypeCheckedListBox.GetItemChecked(_PKTTypeCount) == true)
                {
                    _PKTTypeCount++;
                    break;
                }
            }
            return _PKTTypeCount - 1;
        }

        private int GetNextPower()
        {
            for (; _PowerCount < PowerCheckedListBox.Items.Count; _PowerCount++)
            {
                if (PowerCheckedListBox.GetItemChecked(_PowerCount) == true)
                {
                    _PowerCount++;
                    break;
                }
            }
            return _PowerCount - 1;
        }

        private int GetNextChannel()
        {
            for (; _ChannelCount < ChannelCheckedListBox.Items.Count; _ChannelCount++)
            {
                if (ChannelCheckedListBox.GetItemChecked(_ChannelCount) == true)
                {
                    _ChannelCount++;
                    break;
                }
            }
            return _ChannelCount - 1;
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            int TotalTests = 0;
            if (GetChannelSelection() > 0)
            {
                TotalTests = (TotalTests == 0) ? 1 : TotalTests;
                TotalTests *= GetChannelSelection();
            }
            if (GetPowerSelection() > 0)
            {
                TotalTests = (TotalTests == 0) ? 1 : TotalTests;
                TotalTests *= GetPowerSelection();
            }
            if (GetPKTTypeSelection() > 0)
            {
                TotalTests = (TotalTests == 0) ? 1 : TotalTests;
                TotalTests *= GetPKTTypeSelection();
            }
            if (GetPKTLengthSelection() > 0)
            {
                TotalTests = (TotalTests == 0) ? 1 : TotalTests;
                TotalTests *= GetPKTLengthSelection();
            }
            if (GetNUMPKTSelection() > 0)
            {
                TotalTests = (TotalTests == 0) ? 1 : TotalTests;
                TotalTests *= GetNUMPKTSelection();
            }
            if (GetPERPassSelection() > 0)
            {
                TotalTests = (TotalTests == 0) ? 1 : TotalTests;
                TotalTests *= GetPERPassSelection();
            }

            if (TotalTests == 0)
            {
                this.DialogResult = DialogResult.OK;
                this.Close();
                return;
            }
            if (MessageBox.Show("Do you want to add " + TotalTests.ToString() + " tests?",
                    "Add Tests", MessageBoxButtons.YesNo, MessageBoxIcon.Information) == DialogResult.Yes)
            {
                groupBox1.Enabled = false;
                groupBox2.Enabled = false;
                groupBox3.Enabled = false;
                groupBox4.Enabled = false;
                groupBox5.Enabled = false;
                groupBox6.Enabled = false;
                StatusLabel.Visible = true;
                progressBar1.Maximum = TotalTests;
                progressBar1.Value = 0;
                progressBar1.Visible = true;
                OKButton.Enabled = false;
                AddThread = new Thread(() => this.AddTests(TotalTests));
                AddThread.Start();
            }
            else
            {
                DontClose = true;
            }
        }

        private void PowerAllButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < PowerCheckedListBox.Items.Count; i++)
            {
                PowerCheckedListBox.SetItemChecked(i, true);
            }
        }

        private void PowerNoneButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < PowerCheckedListBox.Items.Count; i++)
            {
                PowerCheckedListBox.SetItemChecked(i, false);
            }
        }

        private void PKTTypeAllButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < PKTTypeCheckedListBox.Items.Count; i++)
            {
                PKTTypeCheckedListBox.SetItemChecked(i, true);
            }
        }

        private void PKTTypeNoneButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < PKTTypeCheckedListBox.Items.Count; i++)
            {
                PKTTypeCheckedListBox.SetItemChecked(i, false);
            }
        }

        private void PKTLenghtALLButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < PKTLenghtCheckedListBox.Items.Count; i++)
            {
                PKTLenghtCheckedListBox.SetItemChecked(i, true);
            }
        }

        private void PKTLengthNoneButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < PKTLenghtCheckedListBox.Items.Count; i++)
            {
                PKTLenghtCheckedListBox.SetItemChecked(i, false);
            }
        }

        private void NUMPKTListBox_MouseDown(object sender, MouseEventArgs e)
        {
            NUMPKTListBox.SelectedIndex = NUMPKTListBox.IndexFromPoint(e.X, e.Y);
        }

        private void PERPassListBox_MouseDown(object sender, MouseEventArgs e)
        {
            PERPassListBox.SelectedIndex = PERPassListBox.IndexFromPoint(e.X, e.Y);
        }

        private void NUMPKTAddButton_Click(object sender, EventArgs e)
        {
            NUMPKTListBox.Items.Add(NumberOfPackets.Value);
        }

        private void NUMPKTRemButton_Click(object sender, EventArgs e)
        {
            if (NUMPKTListBox.SelectedIndex >= 0)
            {
                NUMPKTListBox.Items.RemoveAt(NUMPKTListBox.SelectedIndex);
            }
        }

        private void NUMPKTRemAllButton_Click(object sender, EventArgs e)
        {
            NUMPKTListBox.Items.Clear();
        }

        private void PERPassAddButton_Click(object sender, EventArgs e)
        {
            PERPassListBox.Items.Add(PERPassCriterionNumericUpDown.Value);
        }

        private void PERPassRemButton_Click(object sender, EventArgs e)
        {
            if (PERPassListBox.SelectedIndex >= 0)
            {
                PERPassListBox.Items.RemoveAt(PERPassListBox.SelectedIndex);
            }
        }

        private void PERPassRemAllButton_Click(object sender, EventArgs e)
        {
            PERPassListBox.Items.Clear();
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            if (AddThread != null)
            {
                AddThread.Abort();
            }
            this.Close();
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            progressBar1.Value = progressBar1.Maximum;
            base.OnFormClosing(e);
        }
    }
}

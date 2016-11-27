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
using System.Management;
using System.Management.Instrumentation;
using System.Threading;

namespace CyBLE_MTK_Application
{
    public enum PortType { NoType, Host, DUT, Anritsu };
    public partial class SerialPortSettingsDialog : Form
    {
        private List<COMPortInfo> ComPortInfoList;
        private LogManager Log;
        private string CommandResult, PrevDUTConnStatus, PrevHostConnStatus, CurHostConnStatus, CurDUTConnStatus;
        private bool ContinuePoll;
        private int PollInterval;
        private Thread DevicePollThread;
        private bool ConnectionTime;

        public event ConnectionEventHandler OnDUTConnectionStatusChange;
        public event ConnectionEventHandler OnHostConnectionStatusChange;

        public delegate void ConnectionEventHandler(string ConnectionStatus);

        public PortType SerialPortType;
        public bool CheckDUTPresence, AutoVerifyON;
        public bool CloseOnConnect;

        public SerialPort DeviceSerialPort
        {
            get { return _DeviceSerialPort; }
            set 
            { 
                _DeviceSerialPort = value;
                SerialPortCombo.Enabled = true;
                RefreshButton.Enabled = true;
                ConnectButton.Text = "Co&nnect";
            }
        }

        public SerialPortSettingsDialog()
        {
            Log = new LogManager();
            InitializeComponent();
            SerialPortType = PortType.NoType;
            CheckDUTPresence = false;
            CloseOnConnect = false;
            AddPorts();
            CurHostConnStatus = "";
            CurDUTConnStatus = "";
            PrevHostConnStatus = "";
            PrevDUTConnStatus = "";
            ContinuePoll = false;
            PollInterval = 1000;
            AutoVerifyON = false;
            ConnectionTime = false;
        }

        public SerialPortSettingsDialog(LogManager Logger) : this()
        {
            Log = Logger;
        }

        private void AddPorts()
        {
            ComPortInfoList = COMPortInfo.GetCOMPortsInfo();
            Graphics ComboGraphics = SerialPortCombo.CreateGraphics();
            Font ComboFont = SerialPortCombo.Font;
            int MaxWidth = 0;
            foreach (COMPortInfo ComPort in ComPortInfoList)
            {
                string s = ComPort.Name + " - " + ComPort.Description;
                SerialPortCombo.Items.Add(s);
                int VertScrollBarWidth = (SerialPortCombo.Items.Count > SerialPortCombo.MaxDropDownItems) ? SystemInformation.VerticalScrollBarWidth : 0;
                int DropDownWidth = (int)ComboGraphics.MeasureString(s, ComboFont).Width + VertScrollBarWidth;
                if (MaxWidth < DropDownWidth)
                {
                    SerialPortCombo.DropDownWidth = DropDownWidth;
                    MaxWidth = DropDownWidth;
                }
            }
            if (SerialPortCombo.Items.Count > 0)
            {
                SerialPortCombo.SelectedIndex = 0;
            }
            Log.PrintLog(this, ComPortInfoList.Count.ToString() + " serial ports found.", LogDetailLevel.LogRelevant);
        }

        private void RefreshPortList()
        {
            Log.PrintLog(this, "Removing all serial ports.", LogDetailLevel.LogEverything);
            SerialPortCombo.Items.Clear();
            Log.PrintLog(this, "Rediscovering serial ports.", LogDetailLevel.LogEverything);
            AddPorts();
        }

        private void RefreshButton_Click(object sender, EventArgs e)
        {
            RefreshPortList();
        }

        private void ConnectButton_Click(object sender, EventArgs e)
        {
            ConnectButton.Enabled = false;
            ConnectPort();
            ConnectButton.Enabled = true;
        }

        private void ConnectPort()
        {
            ConnectionTime = true;
            if (ConnectButton.Text == "Co&nnect")
            {
                _DeviceSerialPort.PortName = ComPortInfoList[SerialPortCombo.SelectedIndex].Name;
                try
                {
                    _DeviceSerialPort.Open();
                }
                catch
                {
                    MessageBox.Show(_DeviceSerialPort.PortName + " - is in use.",
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                if (_DeviceSerialPort.IsOpen == true)
                {
                    if (AutoVerifyON)
                    {
                        if (!VerifyDeviceType())
                        {
                            if (SerialPortType == PortType.Host)
                            {
                                Log.PrintLog(this, "Expecting a MTK Host device.", LogDetailLevel.LogRelevant);
                            }
                            else if (SerialPortType == PortType.DUT)
                            {
                                Log.PrintLog(this, "Expecting a DUT.", LogDetailLevel.LogRelevant);
                            }
                            else if (SerialPortType == PortType.Anritsu)
                            {
                                Log.PrintLog(this, "Expecting an Anritsu tester.", LogDetailLevel.LogRelevant);
                            }

                            _DeviceSerialPort.Close();
                            ConnectButton.Enabled = true;
                            ConnectionTime = false;
                            return;
                        }
                        ConnectionTime = false;
                        ContinuePoll = true;
                        DevicePollThread = new Thread(() => PollDevices());
                        DevicePollThread.Start();
                    }

                    SerialPortCombo.Enabled = false;
                    RefreshButton.Enabled = false;
                    ConnectButton.Text = "&Disconnect";
                    Log.PrintLog(this, "Connected to \"" + SerialPortCombo.GetItemText(SerialPortCombo.SelectedItem) + "\".", LogDetailLevel.LogRelevant);
                    if (CloseOnConnect)
                    {
                        this.DialogResult = DialogResult.OK;
                        this.Close();
                    }
                }
            }
            else
            {
                DisconnectSerialPort();
            }
            ConnectionTime = false;
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (_DeviceSerialPort.IsOpen == true)
            {
                this.DialogResult = DialogResult.OK;
            }
            else
            {
                this.DialogResult = DialogResult.Cancel;
            }
            base.OnClosing(e);
        }

        protected override void OnLoad(EventArgs e)
        {
            if (ConnectButton.Text == "Co&nnect")
            {
                RefreshPortList();
            }

            PresetConnection();

            base.OnLoad(e);
        }

        private void PresetConnection()
        {
            if (_DeviceSerialPort.IsOpen/* && !AutoVerifyON*/)
            {
                SerialPortCombo.Enabled = false;
                RefreshButton.Enabled = false;
                ConnectButton.Text = "&Disconnect";
                for (int i = 0; i < ComPortInfoList.Count; i++)
                {
                    if (_DeviceSerialPort.PortName == ComPortInfoList[i].Name)
                    {
                        SerialPortCombo.SelectedIndex = i;
                        break;
                    }
                }
            }
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        public void DisconnectSerialPort()
        {
            this.StopCheckingConnectionStatus();
            try
            {
                if (this._DeviceSerialPort.IsOpen == true)
                {
                    this._DeviceSerialPort.Close();
                    SerialPortCombo.Enabled = true;
                    ConnectButton.Text = "Co&nnect";
                    RefreshButton.Enabled = true;
                    Log.PrintLog(this, "Disconnected from \"" + SerialPortCombo.GetItemText(SerialPortCombo.SelectedItem) + "\".", LogDetailLevel.LogRelevant);
                    if (SerialPortType == PortType.Host)
                    {
                        CurHostConnStatus = "DISCONNECTED";
                        if (OnHostConnectionStatusChange != null)
                        {
                            OnHostConnectionStatusChange("DISCONNECTED");
                        }
                        if (CheckDUTPresence)
                        {
                            if (OnDUTConnectionStatusChange != null)
                            {
                                OnDUTConnectionStatusChange("DISCONNECTED");
                            }
                        }
                    }
                    else if (SerialPortType == PortType.DUT)
                    {
                        CurDUTConnStatus = "DISCONNECTED";
                        if (OnDUTConnectionStatusChange != null)
                        {
                            OnDUTConnectionStatusChange("DISCONNECTED");
                        }
                    }
                }
            }
            catch
            {
                SerialPortCombo.Enabled = true;
                ConnectButton.Text = "Co&nnect";
                RefreshButton.Enabled = true;
                Log.PrintLog(this, "Serial port already disconnected.", LogDetailLevel.LogRelevant);
                RefreshPortList();
                if (SerialPortType == PortType.Host)
                {
                    CurHostConnStatus = "DISCONNECTED";
                    if (OnHostConnectionStatusChange != null)
                    {
                        OnHostConnectionStatusChange("DISCONNECTED");
                    }
                    if (CheckDUTPresence)
                    {
                        if (OnDUTConnectionStatusChange != null)
                        {
                            OnDUTConnectionStatusChange("DISCONNECTED");
                        }
                    }
                }
                else if (SerialPortType == PortType.DUT)
                {
                    CurDUTConnStatus = "DISCONNECTED";
                    if (OnDUTConnectionStatusChange != null)
                    {
                        OnDUTConnectionStatusChange("DISCONNECTED");
                    }
                }
            }
        }

        private bool SendCommand(string Command, int WaitTimeMS)
        {
            char[] DelimiterChars = { '\n', '\r' };

            this.CommandResult = "";
            try
            {
                _DeviceSerialPort.WriteLine(Command);
                Thread.Sleep(WaitTimeMS);
                string OuputACKNAC = _DeviceSerialPort.ReadExisting();
                string[] Output = OuputACKNAC.Split(DelimiterChars);

                for (int i = 0; i < Output.Count(); i++)
                {
                    if ((Output[i] != "") && (Output[i] != "ACK") && (Output[i] != "NAC"))
                    {
                        this.CommandResult = Output[i];
                        break;
                    }
                }

                if (Output[0] == "ACK")
                {
                    return true;
                }
            }
            catch
            {
                return false;
            }

            return false;
        }

        private bool VerifyDeviceType()
        {
            if (SerialPortType == PortType.Host)
            {
                PrevHostConnStatus = CurHostConnStatus;
                SendCommand("WHO", 20);
                if (CommandResult != "HOST")
                {
                    CurHostConnStatus = "DISCONNECTED";
                    if (CurHostConnStatus != PrevHostConnStatus)
                    {
                        if (OnHostConnectionStatusChange != null)
                        {
                            OnHostConnectionStatusChange("DISCONNECTED");
                        }
                        if (CheckDUTPresence == true)
                        {
                            if (OnDUTConnectionStatusChange != null)
                            {
                                OnDUTConnectionStatusChange("DISCONNECTED");
                            }
                        }
                    }
                    return false;
                }
                CurHostConnStatus = "CONNECTED";
                if (CurHostConnStatus != PrevHostConnStatus)
                {
                    if (OnHostConnectionStatusChange != null)
                    {
                        OnHostConnectionStatusChange("CONNECTED");
                    }
                }
                if (CheckDUTPresence == true)
                {
                    PrevDUTConnStatus = CommandResult;
                    SendCommand("PCS", 200);
                    if (PrevDUTConnStatus != CommandResult)
                    {
                        if (OnDUTConnectionStatusChange != null)
                        {
                            OnDUTConnectionStatusChange(CommandResult);
                        }
                    }
                }
            }
            else if ((SerialPortType == PortType.DUT) && (CheckDUTPresence))
            {
                PrevDUTConnStatus = CurDUTConnStatus;

                try
                {
                    _DeviceSerialPort.WriteLine("AB");
                    Thread.Sleep(100);
                    string OuputACKNAC = _DeviceSerialPort.ReadExisting();
                    if (!OuputACKNAC.Contains("AB"))
                    {
                        //SendCommand("WHO", 20);
                        _DeviceSerialPort.WriteLine("\n");
                    }
                }
                catch
                {
                }

                SendCommand("WHO", 20);
                if (CommandResult != "DUT")
                {
                    CurDUTConnStatus = "DISCONNECTED";
                    if (CurDUTConnStatus != PrevDUTConnStatus)
                    {
                        if (OnDUTConnectionStatusChange != null)
                        {
                            OnDUTConnectionStatusChange("DISCONNECTED");
                        }
                    }
                    return false;
                }
                CurDUTConnStatus = "CONNECTED";
                if (CurDUTConnStatus != PrevDUTConnStatus)
                {
                    if (OnDUTConnectionStatusChange != null)
                    {
                        OnDUTConnectionStatusChange("CONNECTED");
                    }
                }
            }
            else if (SerialPortType == PortType.Anritsu)
            {
                char[] DelimiterChars = { ',' };

                if (ConnectionTime)
                {
                    _DeviceSerialPort.WriteLine("*RST");
                    Thread.Sleep(3000);
                }

                _DeviceSerialPort.WriteLine("*IDN?");
                Thread.Sleep(100);
                string OuputACKNAC = _DeviceSerialPort.ReadExisting();
                string[] Output = OuputACKNAC.Split(DelimiterChars);

                if (Output.Count() >= 4)
                {
                    if ((Output[0] == "RANRITSU") && (Output[1] == "MT8852B"))
                    {
                        if (ConnectionTime)
                        {
                            Log.PrintLog(this, "Device Found.", LogDetailLevel.LogRelevant);
                            Log.PrintLog(this, "Make: ANRITSU", LogDetailLevel.LogRelevant);
                            Log.PrintLog(this, "Model: " + Output[1], LogDetailLevel.LogRelevant);
                            Log.PrintLog(this, "Serial Number: " + Output[2], LogDetailLevel.LogRelevant);
                            Log.PrintLog(this, "Firmware Version: " + Output[3], LogDetailLevel.LogRelevant);
                        }
                    }
                    else
                    {
                        if (ConnectionTime)
                        {
                            _DeviceSerialPort.WriteLine("*RST");
                            Log.PrintLog(this, "Device unknown. Please reset the device and try again.", LogDetailLevel.LogRelevant);
                        }
                        return false;
                    }
                }
                else
                {
                    if (ConnectionTime)
                    {
                        _DeviceSerialPort.WriteLine("*RST");
                        Log.PrintLog(this, "Device misbehaviour. Please reset the device and try again.", LogDetailLevel.LogRelevant);
                    }
                    return false;
                }
            }

            return true;
        }

        private void PollDevices()
        {
            while (ContinuePoll)
            {
                if (!VerifyDeviceType())
                {
                    ContinuePoll = false;
                    return;
                }

                if ((SerialPortType == PortType.Host) && (CheckDUTPresence == true))
                {
                    PrevDUTConnStatus = CommandResult;
                    SendCommand("PCS", 200);
                    if (PrevDUTConnStatus != CommandResult)
                    {
                        if (OnDUTConnectionStatusChange != null)
                        {
                            OnDUTConnectionStatusChange(CommandResult);
                        }
                    }
                }

                Thread.Sleep(PollInterval);
            }
        }

        public void StopCheckingConnectionStatus()
        {
            ContinuePoll = false;
            if (DevicePollThread != null)
            {
                DevicePollThread.Abort();
            }
        }

        public void StartCheckingConnectionStatus()
        {
            if (_DeviceSerialPort.IsOpen == true)
            {
                if (SerialPortType == PortType.Host)
                {
                    PrevHostConnStatus = "";
                    CurHostConnStatus = "";
                    if (CheckDUTPresence == true)
                    {
                        PrevDUTConnStatus = "";
                        CurDUTConnStatus = "";
                    }
                }
                if ((SerialPortType == PortType.DUT) && (CheckDUTPresence))
                {
                    PrevDUTConnStatus = "";
                    CurDUTConnStatus = "";
                }
                ContinuePoll = true;
                DevicePollThread = new Thread(() => PollDevices());
                DevicePollThread.Start();
            }
        }

        public void SendRRS()
        {
            SendCommand("RRS", 1000);
        }

        public void ConnectSerialPort()
        {
            ConnectButton.PerformClick();
        }

        public bool FindAndConnectPortByName(string SerialPortName)
        {
            int count = 0;
            bool RetVal = false;

            if (SerialPortName == "")
            {
                return false;
            }
            RefreshPortList();
            foreach (COMPortInfo ComPort in ComPortInfoList)
            {
                if (ComPort.Name == SerialPortName)
                {
                    SerialPortCombo.SelectedIndex = count;
                    PresetConnection();
                    if (!_DeviceSerialPort.IsOpen)
                    {
                        ConnectPort();
                        RetVal = true;
                    }
                    break;
                }
                count++;
            }

            return RetVal;
        }
    }
}

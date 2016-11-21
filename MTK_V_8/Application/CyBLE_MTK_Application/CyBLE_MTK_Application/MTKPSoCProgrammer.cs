using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using System.IO.Ports;
using System.Diagnostics;
using PP_ComLib_Wrapper;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.InteropServices;

namespace CyBLE_MTK_Application
{
    public enum PSoCProgrammerAction { NoAction, Program, Erase};

    public class MTKPSoCProgrammer : MTKTest
    {
        [DllImport("kernel32.dll")]
        public static extern int GetTickCount();

        public enum BLE_FAMILY { BLE_128K, BLE_256K }
        BLE_FAMILY ble_Family = BLE_FAMILY.BLE_128K;

        #region PSoC4-BLE_Register_Map

        //Flash Characteristics
        //public const int ROW_SIZE = 128;

        //Registers
        const int CPUSS_SYSREQ = 0x40100004;
        const int CPUSS_SYSARG = 0x40100008;

        //SROM Constants (offset, masks, etc)
        const int SRAM_PARAMS_BASE = 0x20000100;
        const int SROM_KEY1 = 0xB6;
        const int SROM_KEY2 = 0xD3;
        const uint SROM_SYSREQ_BIT = 0x80000000;
        const uint SROM_PRIVILEGED_BIT = 0x10000000;
        const uint SROM_STATUS_SUCCEEDED = 0xA0000000; //Succeeded
        const uint SROM_STATUS_FAILED = 0xF0000000; //Failed
        const uint SROM_SYSREQ_TIMEOUT = 2000;		  //2 seconds

        //SROM Commands
        const byte SROM_CMD_GET_SILICON_ID = 0x00;
        const byte SROM_CMD_WRITE_NVL = 0x01;
        const byte SROM_CMD_REFRESH_NVL = 0x02;
        const byte SROM_CMD_READ_NVL = 0x03;
        const byte SROM_CMD_LOAD_LATCH = 0x04;
        const byte SROM_CMD_WRITE_ROW = 0x05;
        const byte SROM_CMD_PROGRAM_ROW = 0x06;
        const byte SROM_CMD_NB_WRITEROW = 0x07;
        const byte SROM_CMD_NB_PROGRAMROW = 0x08;
        const byte SROM_CMD_NB_RESUMECMD = 0x09;
        const byte SROM_CMD_ERASE_ALL = 0x0A;
        const byte SROM_CMD_CHECKSUM = 0x0B;
        const byte SROM_CMD_MARGIN_READFLASH = 0x0C;
        const byte SROM_CMD_WRITE_PROTECTION = 0x0D;
        const byte SROM_CMD_WRITE_USER_SFLASH_ROW = 0x18;

        //Chip Level Protection (CPUSS_PROTECTION) - [3:0] - PROT, [31] - PROT_LOCK
        const int CHIP_PROT_VIRGIN = 0x00; //0000
        const int CHIP_PROT_OPEN = 0x01; //0001
        const int CHIP_PROT_PROTECTED = 0x02; //001x
        const int CHIP_PROT_KILL = 0x04; //01xx
        const int CHIP_PROT_MASK = 0x0F; //1111

        //User Supervisory Rows Layout (4x128 Bytes, only in Flash Macro 0)
        const int SFLASH_MACRO_USER_ROW_0_BLE128 = 0x0FFFF200; //Row 0 - User Data, Key/ID etc
        const int SFLASH_MACRO_USER_ROW_1_BLE128 = 0x0FFFF280; //Row 1 - User Data, Key/ID etc
        const int SFLASH_MACRO_USER_ROW_2_BLE128 = 0x0FFFF300; //Row 2 - User Data, Key/ID etc
        const int SFLASH_MACRO_USER_ROW_3_BLE128 = 0x0FFFF380; //Row 3 - User Data, Key/ID etc

        //User Supervisory Rows Layout (4x256 Bytes, only in Flash Macro 0)
        const int SFLASH_MACRO_USER_ROW_0_BLE256 = 0x0FFFF400; //Row 0 - User Data, Key/ID etc
        const int SFLASH_MACRO_USER_ROW_1_BLE256 = 0x0FFFF500; //Row 1 - User Data, Key/ID etc
        const int SFLASH_MACRO_USER_ROW_2_BLE256 = 0x0FFFF600; //Row 2 - User Data, Key/ID etc
        const int SFLASH_MACRO_USER_ROW_3_BLE256 = 0x0FFFF700; //Row 3 - User Data, Key/ID etc

        #endregion PSoC4-BLE_Register_Map

        //Error constants
        public const int S_OK = 0;
        public const int E_FAIL = -1;

        private PP_ComLib_WrapperClass Programmer;
        public bool PSoCProgrammerInstalled;
        public bool PSoCProgrammerError;
        public string ProgrammerVersion;
        public string SelectedProgrammer;
        public bool GlobalProgrammerSelected;
        //public ToolStripStatusLabel StatusLabel;
        private bool SkipLoadingOtherParams;
        public string SelectedVoltageSetting, SelectedAquireMode, SelectedClock;
        public int SelectedConnectorType;
        public string SelectedHEXFilePath;
        public PSoCProgrammerAction SelectedAction;
        public List<string> ProgrammerSupportedAquireMode;
        public List<string> ProgrammerSupportedVoltage;
        public List<string> ProgrammerSupportedConnectors;
        public List<string> ProgrammerSupportedClocks;
        public string[] AvailableProgrammerPorts;
        public bool ValidateAfterProgramming;

        delegate bool SetBoolCallback();

        public MTKPSoCProgrammer() : base()
        {
            Init();
        }

        public MTKPSoCProgrammer(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKPSoCProgrammer(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        private void Init()
        {
            ValidateAfterProgramming = true;
            TestParameterCount = 9; 
            PSoCProgrammerError = false;
            PSoCProgrammerInstalled = false;
            SkipLoadingOtherParams = false;
            ProgrammerSupportedAquireMode = new List<string>();
            ProgrammerSupportedVoltage = new List<string>();
            ProgrammerSupportedConnectors = new List<string>();
            ProgrammerSupportedClocks = new List<string>();
            AvailableProgrammerPorts = new string[] {};
            SelectedProgrammer = "";
            SelectedVoltageSetting = "";
            SelectedAquireMode = "";
            SelectedClock = "";
            SelectedAction = PSoCProgrammerAction.Program;
            try
            {
                //Programmer = new PSoCProgrammerCOM_ObjectClass();
                Programmer = new PP_ComLib_WrapperClass();
                int hr = Programmer.w_ConnectToNotLess(3, 05);
                if (!IsSuccess(hr))
                {
                    string strError = Programmer.w_GetLastError();
                    return;
                }
                PP_Info pp_info;
                Programmer.w_GetActivePP(out pp_info);
                string msg = "Connected to PP " + pp_info.guiMajor.ToString() + "." + pp_info.guiMinor.ToString();
                Programmer.OnUpdateProgressBar += new PP_ComLib_WrapperClass.SetProgressBarDelegate(Event_UpdateProgressBar);
                Programmer.OnAppendTextToLog += new PP_ComLib_WrapperClass.SetLogDelegate(Event_AppendTextToLog);
                Programmer.OnUpdateChipName += new PP_ComLib_WrapperClass.SetDetectedChipName(Event_UpdateChipName);

                ProgrammerVersion = Programmer.Version();
                IsCorrectVersion();
                PSoCProgrammerInstalled = true;
            }
            catch
            {
                PSoCProgrammerInstalled = false;
            }

            if (PSoCProgrammerInstalled == true)
            {
                AvailableProgrammerPorts = GetPorts();
            }
        }

        void Event_UpdateChipName(string family, string device)
        {
            Log.PrintLog(this, "Automatically Detected Device Family: " + family, LogDetailLevel.LogRelevant);
        }

        void Event_UpdateProgressBar(int value, int max)
        {
            TestStatusUpdate(MTKTestMessageType.Information, value.ToString() + "/" + max.ToString());
        }

        void Event_AppendTextToLog(string action, string result, bool showTime)
        {
            Log.PrintLog(this, result, LogDetailLevel.LogRelevant);
        }

        public override string GetDisplayText()
        {
            string ReturnString = "";
            if (GlobalProgrammerSelected)
            {
                ReturnString = "Device Programmer: DUT Programmer Settings Applied";
            }
            else
            {
                if (SelectedProgrammer != "")
                {
                    string ProgrammerAction = (SelectedAction == PSoCProgrammerAction.Program) ? "Program" : "Flash Erase";
                    ReturnString = "Programmer Action: " + ProgrammerAction
                        + " | Programmer: " + SelectedProgrammer
                        + " | Programming Mode: " + AcquireModeToDispText(SelectedAquireMode)
                        + " | Connector: " + ConnectorToString(SelectedConnectorType)
                        + " | Voltage: " + SelectedVoltageSetting
                        + " | Clock Speed: " + SelectedClock
                        + " | Validate: " + ValidateAfterProgramming.ToString();
                }
                else
                {
                    ReturnString = "Device Programmer: Not Configured!";
                }
            }
            return ReturnString;
        }

        public void GetCapabilities()
        {
            if (SelectedProgrammer == "")
            {
                return;
            }

            ProgrammerSupportedAquireMode.Clear();
            ProgrammerSupportedVoltage.Clear();
            ProgrammerSupportedConnectors.Clear();
            ProgrammerSupportedClocks.Clear();
            string strError;
            //object programmerCapabilities;
            byte[] programmerCapabilities;
            int hr = Programmer.GetProgrammerCapsByName(SelectedProgrammer,
                out programmerCapabilities, out strError);

            if (IsSuccess(hr))
            {
                //byte[] caps = programmerCapabilities as byte[];
                byte[] caps = programmerCapabilities;
                //Acquire Mode:
                if ((caps[0] & (byte)enumValidAcquireModes.CAN_RESET_ACQUIRE) != 0)
                {
                    ProgrammerSupportedAquireMode.Add("Reset");
                }
                if ((caps[0] & (byte)enumValidAcquireModes.CAN_POWER_CYCLE_ACQUIRE) != 0)
                {
                    ProgrammerSupportedAquireMode.Add("Power Cycle");
                }
                if ((caps[0] & (byte)enumValidAcquireModes.CAN_POWER_DETECT_ACQUIRE) != 0)
                {
                    ProgrammerSupportedAquireMode.Add("Power Detect");
                }

                if (SelectedProgrammer.StartsWith("MiniProg3") && (PSoCProgrammerInstalled == true))
                {
                    //Voltages can be supplied by programmer
                    if ((caps[5] & (byte)enumVoltages.VOLT_50V) != 0)
                    {
                        ProgrammerSupportedVoltage.Add("5.0 V");
                    }
                    if ((caps[5] & (byte)enumVoltages.VOLT_33V) != 0)
                    {
                        ProgrammerSupportedVoltage.Add("3.3 V");
                    }
                    if ((caps[5] & (byte)enumVoltages.VOLT_25V) != 0)
                    {
                        ProgrammerSupportedVoltage.Add("2.5 V");
                    }
                    if ((caps[5] & (byte)enumVoltages.VOLT_18V) != 0)
                    {
                        ProgrammerSupportedVoltage.Add("1.8 V");
                    }

                    ProgrammerSupportedConnectors.Clear();
                    ProgrammerSupportedConnectors.Add("5p");
                    ProgrammerSupportedConnectors.Add("10p");

                    ProgrammerSupportedClocks.Clear();
                    ProgrammerSupportedClocks.Add("24 MHz");
                    ProgrammerSupportedClocks.Add("16 MHz");
                    ProgrammerSupportedClocks.Add("12 MHz");
                    ProgrammerSupportedClocks.Add("8 MHz");
                    ProgrammerSupportedClocks.Add("6 MHz");
                    ProgrammerSupportedClocks.Add("3.2 MHz");
                    ProgrammerSupportedClocks.Add("3.0 MHz");
                    ProgrammerSupportedClocks.Add("1.6 MHz");
                    ProgrammerSupportedClocks.Add("1.5 MHz");
                }
                else if (SelectedProgrammer.StartsWith("KitProg") && (PSoCProgrammerInstalled == true))
                {
                    ProgrammerSupportedVoltage.Add("5.0 V");

                    ProgrammerSupportedConnectors.Clear();
                    ProgrammerSupportedConnectors.Add("5p");

                    ProgrammerSupportedClocks.Clear();
                    ProgrammerSupportedClocks.Add("3.0 MHz");
                }
            }
        }

        private bool IsSuccess(int ReturnValue)
        {
            return (ReturnValue >= 0);
        }

        public string[] GetPorts()
        {
            //object portArray;
            string[] portArray;
            string strError;

            if (IsSuccess(Programmer.GetPorts(out portArray, out strError)) == false)
            {
                PSoCProgrammerError = true;
            }
            //PortList = portArray as string[];

            if (portArray.Count() > 0)
            {
                Log.PrintLog(this, "Discovered " + portArray.Count().ToString() +
                    " programers.", LogDetailLevel.LogRelevant);
            }

            return portArray;
        }

        public bool IsCorrectVersion()
        {
            if (PSoCProgrammerInstalled == true)
            {
                if (Double.Parse(ProgrammerVersion) >= 18.0)
                {
                    return true;
                }
            }
            PSoCProgrammerInstalled = false;
            return false;
        }

        public bool InitializeProgrammer()
        {
            int hr;
            string m_sLastError;

            Programmer.lib_SetHexFile(SelectedHEXFilePath);
            Programmer.lib_SetAcquireMode(SelectedAquireMode);
            Programmer.lib_SetVerification(ValidateAfterProgramming); //is not applicable for PSoC3/5 chips
            Programmer.lib_SetAutoChipDetection(true);
            Programmer.lib_SetProtocol(enumInterfaces.SWD);
            Programmer.lib_SetAutoReset(0x01);
            Programmer.lib_SetProtocolClock(enumFrequencies.FREQ_03_0);
            Programmer.lib_SetProtocolConnector(SelectedConnectorType); //0 -> 5-pins; 1 -> 10-pins;
            Programmer.lib_SetPartialProgram(true); //skips empty flash rows

            //Setup Power - "3.3V" and internal for reset mode
            hr = Programmer.SetPowerVoltage(SelectedVoltageSetting.Substring(0, 3), out m_sLastError);
            if (IsSuccess(hr) == false)
            {
                Log.PrintLog(this, "Unable to set acquire mode to Reset.", LogDetailLevel.LogRelevant);
                return false;
            }

            hr = Programmer.PowerOn(out m_sLastError);
            if (IsSuccess(hr) == false)
            {
                Log.PrintLog(this, "Unable to set programming voltage.", LogDetailLevel.LogRelevant);
                return false;
            }

            return true;
        }

        public bool ConnectProgrammer()
        {
            if (PSoCProgrammerInstalled == false)
            {
                return false;
            }
            
            if (!SelectedProgrammer.StartsWith("MiniProg3") && !SelectedProgrammer.StartsWith("KitProg"))
            {
                Log.PrintLog(this, "Unknown programmer device.", LogDetailLevel.LogRelevant);
                return false;
            }
            string LastError;
            int hr = Programmer.OpenPort(SelectedProgrammer, out LastError);
            if (IsSuccess(hr))
            {
                Log.PrintLog(this, "Programmer port successfully opened." + LastError, LogDetailLevel.LogRelevant);
                return true;
            }
            Log.PrintLog(this, LastError, LogDetailLevel.LogRelevant);
            if (hr == -2147483647)
            {
                return true;
            }
            return false;
        }

        public bool DisconnectProgrammer()
        {
            string LastError;
            if (PSoCProgrammerInstalled == false)
            {
                return false;
            }
            int hr = Programmer.ClosePort(out LastError);
            if (IsSuccess(hr))
            {
                Log.PrintLog(this, "Programmer port successfully closed." + LastError, LogDetailLevel.LogRelevant);
                return true;
            }
            Log.PrintLog(this, LastError, LogDetailLevel.LogRelevant);
            return false;
        }

        private int PSoC4_IsChipNotProtected()
        {
            int hr;
            string m_sLastError;
            //object flashProt, chipProt;
            byte[] flashProt, chipProt;
            byte[] data;

            //Chip Level Protection reliably can be read by below API (available in VIRGIN, OPEN, PROTECTED modes)
            //This API uses SROM call - to read current status of CPUSS_PROTECTION register (privileged)
            //This register contains current protection mode loaded from SFLASH during boot-up.
            hr = Programmer.PSoC4_ReadProtection(out flashProt, out chipProt, out m_sLastError);
            if (!IsSuccess(hr)) return E_FAIL; //consider chip as protected if any communication failure

            data = chipProt as byte[];
            //Check Result
            if ((data[0] & CHIP_PROT_PROTECTED) == CHIP_PROT_PROTECTED)
            {
                m_sLastError = "Chip is in PROTECTED mode. Any access to Flash is suppressed.";
                Log.PrintLog(this, m_sLastError, LogDetailLevel.LogRelevant);
                return E_FAIL;
            }

            return S_OK;
        }

        private int PSoC4_EraseAll()
        {
            int hr;
            string m_sLastError;

            //Check chip level protection here. If PROTECTED then move to OPEN by PSoC4_WriteProtection() API.
            //Otherwise use PSoC4_EraseAll() - in OPEN/VIRGIN modes.
            hr = PSoC4_IsChipNotProtected();
            if (IsSuccess(hr)) //OPEN mode
            {
                //Erase All - Flash and Protection bits. Still be in OPEN mode.
                hr = Programmer.PSoC4_EraseAll(out m_sLastError);
            }
            else
            {   //Move to OPEN from PROTECTED. It automatically erases Flash and its Protection bits.
                byte[] flashProt = new byte[] { }; // do not care in PROTECTED mode
                byte[] chipProt = new byte[] { CHIP_PROT_OPEN }; //move to OPEN

                hr = Programmer.PSoC4_WriteProtection(flashProt, chipProt, out m_sLastError);
                if (!IsSuccess(hr))
                {
                    Log.PrintLog(this, m_sLastError, LogDetailLevel.LogRelevant);
                    return hr;
                }

                //Need to reacquire chip here to boot in OPEN mode.
                //ChipLevelProtection is applied only after Reset.
                hr = Programmer.DAP_Acquire(out m_sLastError);
            }
            return hr;
        }

        public bool ProgramAll()
        {
            int hr;

            //-1-. Initialize programmer
            Log.PrintLog(this, "Initializing programmer.", LogDetailLevel.LogRelevant);
            if (!InitializeProgrammer())
            {
                return false;
            }

            //-2-. Program
            if (ValidateAfterProgramming)
            {
                Log.PrintLog(this, "Programming and verifying...", LogDetailLevel.LogRelevant);
            }
            else
            {
                Log.PrintLog(this, "Programming...", LogDetailLevel.LogRelevant);
            }
            hr = Programmer.lib_Program();
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, "Unable to program the flash.", LogDetailLevel.LogRelevant);
                return false;
            }

            //Find checksum of Privileged Flash. Will be used in calculation of User CheckSum later
            string m_sLastError;

            //-3-. CheckSum verification
            short hexChecksum;
            hr = Programmer.HEX_ReadChecksum(out hexChecksum, out m_sLastError);
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, m_sLastError, LogDetailLevel.LogRelevant);
                Log.PrintLog(this, "Unable to read chip checksum.", LogDetailLevel.LogRelevant);
                return false;
            }

            Log.PrintLog(this, "Checksum 0x" + hexChecksum.ToString("X"), LogDetailLevel.LogRelevant);

            return true;
        }

        public bool EraseAll()
        {
            int hr;

            Log.PrintLog(this, "Initializing programmer.", LogDetailLevel.LogRelevant);
            if (!InitializeProgrammer())
            {
                return false;
            }

            Log.PrintLog(this, "Erasing...", LogDetailLevel.LogRelevant);
            hr = Programmer.lib_EraseAll();
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, "Unable to erase the flash.", LogDetailLevel.LogRelevant);
                return false;
            }

            return true;
        }

        public override MTKTestError RunTest()
        {
            MTKTestError RetVal = MTKTestError.NoError;
            this.InitializeTestResult();

            if (!ConnectProgrammer())
            {
                TestStatusUpdate(MTKTestMessageType.Information, "Error!!!");
                Log.PrintLog(this, "Unable to connect to the programmer.", LogDetailLevel.LogRelevant);
                RetVal = MTKTestError.TestFailed;
            }

            if (SelectedAction == PSoCProgrammerAction.Program)
            {
                if (!ProgramAll())
                {
                    TestStatusUpdate(MTKTestMessageType.Information, "Error!!!");
                    //Log.PrintLog(this, "Unable to flash the hex file.", LogDetailLevel.LogRelevant);
                    RetVal = MTKTestError.TestFailed;
                }
            }
            else if (SelectedAction == PSoCProgrammerAction.Erase)
            {
                if (!EraseAll())
                {
                    TestStatusUpdate(MTKTestMessageType.Information, "Error!!!");
                    //Log.PrintLog(this, "Unable to erase flash.", LogDetailLevel.LogRelevant);
                    RetVal = MTKTestError.TestFailed;
                 }
            }

            if (!DisconnectProgrammer())
            {
                TestStatusUpdate(MTKTestMessageType.Information, "Error!!!");
                Log.PrintLog(this, "Unable to disconnect from the programmer.", LogDetailLevel.LogRelevant);
                RetVal = MTKTestError.TestFailed;
            }

            if (RetVal == MTKTestError.NoError)
            {
                TestStatusUpdate(MTKTestMessageType.Success, "PASS");
                TestResult.Result = "PASS";
            }
            else
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                TestResult.Result = "FAIL";
            }

            TestResultUpdate(TestResult);
            return RetVal;
        }

        public override string GetTestParameter(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return GlobalProgrammerSelected.ToString(); 
                case 1:
                    return SelectedProgrammer;
                case 2:
                    return SelectedVoltageSetting;
                case 3:
                    return SelectedAquireMode;
                case 4:
                    return ConnectorToString(SelectedConnectorType);
                case 5:
                    return SelectedClock;
                case 6:
                    if (SelectedHEXFilePath == "")
                    {
                        return "null";
                    }
                    return SelectedHEXFilePath;
                case 7:
                    return SelectedAction.ToString();
                case 8:
                    return ValidateAfterProgramming.ToString();
            }
            return base.GetTestParameter(TestParameterIndex);
        }

        public override string GetTestParameterName(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return "Global"; 
                case 1:
                    return "Port";
                case 2:
                    return "Voltage";
                case 3:
                    return "ProgrammingMode";
                case 4:
                    return "Connector";
                case 5:
                    return "ClockSpeed";
                case 6:
                    return "HEXFile";
                case 7:
                    return "Action";
                case 8:
                    return "Verify";
            }
            return base.GetTestParameterName(TestParameterIndex);
        }

        public override bool SetTestParameter(int TestParameterIndex, string ParameterValue)
        {
            if (ParameterValue == "")
            {
                return false;
            }
            switch (TestParameterIndex)
            {
                case 0: 
                    if (ParameterValue.ToLower() == "true")
                    {
                        GlobalProgrammerSelected = true;
                        SkipLoadingOtherParams = true;
                        return true;
                    }
                    else
                    {
                        GlobalProgrammerSelected = false;
                        return true;
                    }
                case 1:
                    if (SkipLoadingOtherParams == true)
                    {
                        return true;
                    }
                    SkipLoadingOtherParams = false;
                    for (int i = 0; i < AvailableProgrammerPorts.Count(); i++)
                    {
                        if (AvailableProgrammerPorts[i] == ParameterValue)
                        {
                            SelectedProgrammer = AvailableProgrammerPorts[i];
                            GetCapabilities();
                            return true;
                        }
                    }
                    SkipLoadingOtherParams = true;
                    return true;
                case 2:
                    if (SkipLoadingOtherParams == true)
                    {
                        return true;
                    }
                    for (int i = 0; i < ProgrammerSupportedVoltage.Count; i++)
                    {
                        if (ProgrammerSupportedVoltage[i] == ParameterValue)
                        {
                            SelectedVoltageSetting = ProgrammerSupportedVoltage[i];
                            return true;
                        }
                    }
                    return false;
                case 3:
                    if (SkipLoadingOtherParams == true)
                    {
                        return true;
                    }
                    string[] temp = { "Power", "Reset", "PowerDetect" };
                    for (int i = 0; i < temp.Count(); i++)
                    {
                        if (temp[i] == ParameterValue)
                        {
                            SelectedAquireMode = temp[i];
                            return true;
                        }
                    }
                    return false;
                case 4:
                    if (SkipLoadingOtherParams == true)
                    {
                        return true;
                    }
                    for (int i = 0; i < ProgrammerSupportedConnectors.Count; i++)
                    {
                        if (ProgrammerSupportedConnectors[i] == ParameterValue)
                        {
                            SelectedConnectorType = ConnectorFromString(ProgrammerSupportedConnectors[i]);
                            return true;
                        }
                    }
                    return false;
                case 5:
                    if (SkipLoadingOtherParams == true)
                    {
                        return true;
                    }
                    for (int i = 0; i < ProgrammerSupportedClocks.Count; i++)
                    {
                        if (ProgrammerSupportedClocks[i] == ParameterValue)
                        {
                            SelectedClock = ProgrammerSupportedClocks[i];
                            return true;
                        }
                    }
                    return false;
                case 6:
                    if (SkipLoadingOtherParams == true)
                    {
                        return true;
                    }
                    if (ParameterValue == "null")
                    {
                        SelectedHEXFilePath = "";
                    }
                    else
                    {
                        SelectedHEXFilePath = ParameterValue;
                    }
                    return true;
                case 7:
                    if (SkipLoadingOtherParams == true)
                    {
                        return true;
                    }
                    if (ParameterValue == "Program")
                    {
                        SelectedAction = PSoCProgrammerAction.Program;
                        return true;
                    }
                    else if (ParameterValue == "Erase")
                    {
                        SelectedAction = PSoCProgrammerAction.Erase;
                        return true;
                    }
                    return false;
                case 8:
                    if (SkipLoadingOtherParams == true)
                    {
                        return true;
                    }
                    if (ParameterValue.ToLower() == "true")
                    {
                        ValidateAfterProgramming = true;
                        return true;
                    }
                    else
                    {
                        ValidateAfterProgramming = false;
                        return true;
                    }
            }
            return false;
        }

        private string AcquireModeToDispText(string InputValue)
       {
            if (InputValue == "Reset")
            {
                return "Reset";
            }
            else if (InputValue == "Power")
            {
                return "Power Cycle";
            }
            else if (InputValue == "PowerDetect")
            {
                return "Power Detect";
            }

            return "Reset";
        }

        private int ConnectorFromString(string ConnType)
        {
            int ReturnValue = 0;

            if (ConnType == "5p")
            {
                ReturnValue = 0;
            }
            else if (ConnType == "10p")
            {
                ReturnValue = 1;
            }

            return ReturnValue;
        }

        private string ConnectorToString(int ConnType)
        {
            string ReturnValue = "";

            if (ConnType == 0)
            {
                ReturnValue = "5p";
            }
            else if (ConnType == 1)
            {
                ReturnValue = "10p";
            }

            return ReturnValue;
        }

        protected override void InitializeTestResult()
        {
            base.InitializeTestResult();
            TestResult.PassCriterion = "N/A";
            TestResult.Measured = "N/A";
        }

        private byte[] ObjectToByteArray(Object obj)
        {
            if (obj == null)
                return null;
            BinaryFormatter bf = new BinaryFormatter();
            using (MemoryStream ms = new MemoryStream())
            {
                bf.Serialize(ms, obj);
                return ms.ToArray();
            }
        }

        public bool AcquireAndEraseSFlash()
        {
            int hr = S_OK;
            string strError = "";

            if (!InitializeProgrammer())
            {
                return false;
            }

            //-1-. Acquire part (it automatically sets IMO to 48 MHz to enable Flash Updates)
            hr = Programmer.DAP_Acquire(out strError);
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, strError, LogDetailLevel.LogRelevant);
                return false;
            }

            //-2-. Check if this is PSoC4-BLE family
            byte[] siliconID;
            hr = Programmer.PSoC4_GetSiliconID(out siliconID, out strError);
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, strError, LogDetailLevel.LogRelevant);
                return false;
            }

            if (siliconID[3] == 0x9E) //check Family ID field for "BLE" and "BLE256K"
            {
                ble_Family = BLE_FAMILY.BLE_128K;
            }
            else if ((siliconID[3] == 0xA3) || (siliconID[3] == 0xAA) || (siliconID[3] == 0xAE)) //BLE-256, BLE-256-DMA, BLEvII
            {
                ble_Family = BLE_FAMILY.BLE_256K;
            }
            else
            {
                strError = "Detected target is not from PSoC4-BLE family. Please connect correct device.";
                return false;
            }

            //-3-. Move Chip into OPEN state (if in PROTECTED state now). User SFLASH rows can be updated only in OPEN state.
            TestStatusUpdate(MTKTestMessageType.Information, "Erasing...");
            hr = PSoC4_EraseAll();
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, "Unable to erase chip.", LogDetailLevel.LogRelevant);
                return false;
            }

            return true;
        }

        private string SROM_FormatStatus(int statusCode)
        {
            string strMsg;

            if ((statusCode & 0xF0000000) == SROM_STATUS_SUCCEEDED)
            {
                strMsg = "SROM operation completed Successfully.";
                return strMsg;
            }

            statusCode &= 0x0FFFFFFF;
            switch (statusCode)
            {
                case 0x01:
                    strMsg = "Invalid Chip Protection Mode. Command not available in the current chip protection mode.";
                    break;
                case 0x02:
                    strMsg = "Invalid NVL address.";
                    break;
                case 0x03:
                    strMsg = "Invalid Page Latch Address.";
                    break;
                case 0x04:
                    strMsg = "Invalid RowID - The ROW ID addressed is a protected row.";
                    break;
                case 0x05:
                    strMsg = "Row Protected - The ROW ID addressed is a protected row";
                    break;
                case 0x06:
                    strMsg = "SRAM Address Invalid - The SRAM address is out of bounds.";
                    break;
                case 0x07:
                    strMsg = "Test Mode Require - This command requires the chip to be in Test Mode.";
                    break;
                case 0x08:
                    strMsg = "Resume Command Completed - The non-blocking command initiated is completed and no additional resume commands need to be done.";
                    break;
                case 0x09:
                    strMsg = "Pending Resume - A non-blocking command was initiated and still needs to be completed by calling the resume command.";
                    break;
                case 0x0A:
                    strMsg = "Command still in progress - A resume command is still in progress. The SPC ISR must fire before attempting a resume command.";
                    break;
                case 0x0B:
                    strMsg = "Checksum Zero Failed - The calculated checksum was not zero.";
                    break;
                case 0x0C:
                    strMsg = "Invalid Opcode - The opcode is not a valid SROM command.";
                    break;
                case 0x0D:
                    strMsg = "Key Opcode Mismatch - The opcode given does not match KEY pair given.";
                    break;
                case 0x0E:
                    strMsg = "No SFLASH Reads in open - A Margin Read to SFLASH is not allowed in OPEN mode.";
                    break;
                case 0x0F:
                    strMsg = "Invalid Bookmark Mode – The resume command was called and the SPC Register contained an invalid resume mode.";
                    break;
                case 0x10:
                    strMsg = "The SPC Parameter Checksum returned a failure. BOOKMARK register contains the data.";
                    break;
                default:
                    strMsg = "Unknown SROM status code";
                    break;
            }
            return strMsg;
        }

        private int PollSromStatus(uint timeOut, out string strError)
        {
            int hr = S_OK;

            bool fTimeout = false;
            int baseAddr = CPUSS_SYSREQ;
            int readData;
            int timeStart, timeNow;
            //Poll for operation completion
            timeStart = GetTickCount();
            do
            {
                if (fTimeout)
                {
                    strError = "Timeout of SROM polling. Lost communication with chip.";
                    return E_FAIL;
                }
                hr = Programmer.DAP_ReadIO(baseAddr, out readData, out strError);
                if (!IsSuccess(hr)) return hr;
                timeNow = GetTickCount();
                fTimeout = ((timeNow - timeStart) >= timeOut);
                unchecked
                {
                    readData &= (int)(SROM_SYSREQ_BIT | SROM_PRIVILEGED_BIT);
                }
            } while (readData != 0x00); //this bit is reset when SROM request completed

            //Check status of operation
            int statusCode;
            hr = Programmer.DAP_ReadIO(CPUSS_SYSARG, out statusCode, out strError);
            if (!IsSuccess(hr)) return hr;

            if ((statusCode & 0xF0000000) != SROM_STATUS_SUCCEEDED)
            {
                strError = SROM_FormatStatus(statusCode);
                return E_FAIL;
            }
            return hr;
        }

        private int LoadLatchRow(byte arrayID, byte[] data, out string strError)
        {
            int hr = S_OK;
            int Params1, Params2;

            //Put command's parameters into SRAM buffer
            Params1 = (SROM_KEY1 << 0) +  							//KEY1
                    ((SROM_KEY2 + SROM_CMD_LOAD_LATCH) << 8) +		//KEY2
                    (0x00 << 16) +									//Byte number in latch from what to write
                    (arrayID << 24);	                            //Flash Macro ID (0 or 1)

            Params2 = ((data.Length - 1) << 0) +	    			//Number of Bytes to load minus 1
                    (0x00 << 8) +									//Pad Byte to next 4b boundary    (so 1 byte is sufficient)
                    (0x00 << 16) +									//Pad Byte to next 4b boundary
                    (0x00 << 24);                                   //Pad Byte to next 4b boundary

            Programmer.DAP_WriteIO(SRAM_PARAMS_BASE + 0x00, Params1, out strError);  //Write params is SRAM
            Programmer.DAP_WriteIO(SRAM_PARAMS_BASE + 0x04, Params2, out strError);  //Write params is SRAM

            //Put row data into SRAM buffer
            for (int i = 0; i < data.Length; i += 4)
            {
                Params1 = (data[i] << 0) + (data[i + 1] << 8) + (data[i + 2] << 16) + (data[i + 3] << 24);
                Programmer.DAP_WriteIO(SRAM_PARAMS_BASE + 0x08 + i, Params1, out strError); //Write params is SRAM
            }

            //Call "Load Latch" SROM API
            unchecked
            {
                Programmer.DAP_WriteIO(CPUSS_SYSARG, SRAM_PARAMS_BASE, out strError); //Set location of parameters
                Programmer.DAP_WriteIO(CPUSS_SYSREQ, (int)(SROM_SYSREQ_BIT | SROM_CMD_LOAD_LATCH), out strError);//Request SROM operation
            }
            //Read status of the operation
            hr = PollSromStatus(SROM_SYSREQ_TIMEOUT, out strError);
            if (!IsSuccess(hr)) return hr;
            return hr;
        }

        private int ProgramSFlashRow(byte rowID, byte[] data, out string strError)
        {
            int hr;
            int Params1, Params2;

            hr = LoadLatchRow(0, data, out strError);
            if (!IsSuccess(hr)) return hr;

            //Program Row - call SROM API
            Params1 = (SROM_KEY1 << 0) +	 					            //KEY1
                     ((SROM_KEY2 + SROM_CMD_WRITE_USER_SFLASH_ROW) << 8) +  //KEY2
                      (0x00 << 16) +				                        //not used
                      (0x00 << 24);					                        //not used
            Params2 = (rowID << 0) +      //RowID , correct range = [0..3]
                      (0x00 << 8) +
                      (0x00 << 16) +
                      (0x00 << 24);

            Programmer.DAP_WriteIO(SRAM_PARAMS_BASE + 0x00, Params1, out strError); //Write params is SRAM
            Programmer.DAP_WriteIO(SRAM_PARAMS_BASE + 0x04, Params2, out strError); //Write params is SRAM

            unchecked
            {
                Programmer.DAP_WriteIO(CPUSS_SYSARG, SRAM_PARAMS_BASE, out strError);               //Set location of parameters
                Programmer.DAP_WriteIO(CPUSS_SYSREQ, (int)(SROM_SYSREQ_BIT | SROM_CMD_WRITE_USER_SFLASH_ROW), out strError);//Request SROM operation
            }

            //Read status of the operation
            hr = PollSromStatus(SROM_SYSREQ_TIMEOUT, out strError);
            if (!IsSuccess(hr)) return hr;

            return hr;
        }

        private BLE_FAMILY GetBleFamily()
        {
            return ble_Family;
        }

        public int GetRowSize()
        {
            return (ble_Family == BLE_FAMILY.BLE_128K) ? 128 : 256; //row size in bytes
        }

        private int GetRowFlashMacro0()
        {
            return (ble_Family == BLE_FAMILY.BLE_128K) ? SFLASH_MACRO_USER_ROW_0_BLE128 : SFLASH_MACRO_USER_ROW_0_BLE256; //row size in bytes
        }

        private int VerifySFlashRow(byte rowID, byte[] data, out string strError)
        {
            int hr = S_OK;
            int address;
            strError = "";

            //-1-. Convert rowID to physical address
            address = GetRowFlashMacro0() + rowID * GetRowSize();

            //-2-. Read back SFLASH row and compare it against original data
            for (int i = 0; i < GetRowSize(); i += 4)
            {
                int dataExpected32 = data[i + 0] + (data[i + 1] << 8) + (data[i + 2] << 16) + (data[i + 3] << 24);
                int dataRead32;

                Programmer.DAP_ReadIO(address + i, out dataRead32, out strError);
                if (dataRead32 != dataExpected32)
                {
                    strError = "Verification failed at address 0x" + address.ToString("X8") + ". " +
                               "Expected data = 0x" + dataExpected32.ToString("X8") + ", read data = 0x" + dataRead32.ToString("X8");
                    return E_FAIL;
                }
            }

            return hr;
        }

        public int UpdateSFlashRow(byte rowID, byte[] data, out string strError)
        {
            int hr = S_OK;
            strError = "";

            //-0-. Check Input Parameters
            if ((rowID < 0) || (rowID > 3))
            {
                strError = "Wrong value of RowID parameter. Expected range is 0..3";
                return E_FAIL;
            }
            if (data.Length != GetRowSize()) //ommiting checking for NULL. It's never expected in private function.
            {
                strError = "Wrong size of Data array. It must be " + GetRowSize().ToString() + ".";
                return E_FAIL;
            }

            //-1-. Program SFLASH row
            hr = ProgramSFlashRow(rowID, data, out strError);
            if (IsSuccess(hr))
            {
                //-2-. Verify SFLASH row
                hr = VerifySFlashRow(rowID, data, out strError);
            }

            if (!IsSuccess(hr)) strError = "RowID = " + rowID.ToString() + " Failed! " + strError;

            return hr;
        }

        public int ReleaseChip(out string strError)
        {
            int hr;

            hr = Programmer.DAP_ReleaseChip(out strError);

            return hr;
        }

        public void StringToPA(string ParameterValue)
        {
            if (ParameterValue == "Erase")
            {
                SelectedAction = PSoCProgrammerAction.Erase;
            }
            else
            {
                SelectedAction = PSoCProgrammerAction.Program;
            }
        }

        public string PAToString()
        {
            if (PSoCProgrammerAction.Program == SelectedAction)
            {
                return "Program";
            }
            else if (PSoCProgrammerAction.Erase == SelectedAction)
            {
                return "Erase";
            }
            return "NoAction";
        }

        public bool RunI2CTest(UnitI2CTest[] I2CTests)
        {
            string stdErr;
            int hr;

            bool RetVal = true;

            if (!ConnectProgrammer())
            {
                TestStatusUpdate(MTKTestMessageType.Information, "Error!!!");
                Log.PrintLog(this, "Unable to connect to the programmer.", LogDetailLevel.LogRelevant);
                return false;
            }

            hr = Programmer.SetProtocol(enumInterfaces.I2C, out stdErr);
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, "Failed to set I2C protocol: " + stdErr, LogDetailLevel.LogRelevant);
                RetVal = false;
            }

            hr = Programmer.I2C_SetSpeed(enumI2Cspeed.CLK_100K, out stdErr);
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, "Failed to set I2C bus clock speed: " + stdErr, LogDetailLevel.LogRelevant);
                RetVal = false;
            }

            hr = Programmer.I2C_ResetBus(out stdErr);
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, "Failed to reset I2C bus: " + stdErr, LogDetailLevel.LogRelevant);
                RetVal = false;
            }

            byte[] DevList = new byte[0];
            hr = Programmer.I2C_GetDeviceList(out DevList, out stdErr);
            if (!IsSuccess(hr))
            {
                Log.PrintLog(this, "Failed to get device list: " + stdErr, LogDetailLevel.LogRelevant);
                RetVal = false;
            }


            Log.PrintLog(this, "I2C Bus reinitialized successfully.", LogDetailLevel.LogEverything);

            for (int i = 0; i < I2CTests.Count(); i++)
            {
                bool DeviceFound = false;
                for (int j = 0; j < DevList.Count(); j++)
                {
                    if (DevList[j] == (byte)I2CTests[i].Address)
                    {
                        DeviceFound = true;
                        break;
                    }
                }

                if (DeviceFound)
                {
                    if (I2CTests[i].Action == MTKI2CTestType.Write)
                    {
                        hr = Programmer.I2C_SendData(I2CTests[i].Address, I2CTests[i].DataBuffer, out stdErr);
                        if (!IsSuccess(hr))
                        {
                            Log.PrintLog(this, "Failed I2C write: " + stdErr, LogDetailLevel.LogRelevant);
                            RetVal = false;
                        }

                    }
                    else if (I2CTests[i].Action == MTKI2CTestType.Read)
                    {
                        hr = Programmer.I2C_ReadData(I2CTests[i].Address, I2CTests[i].NumRxBytes, out I2CTests[i].RxDataBuffer, out stdErr);
                        if (!IsSuccess(hr))
                        {
                            Log.PrintLog(this, "Failed I2C read: " + stdErr, LogDetailLevel.LogRelevant);
                            RetVal = false;
                        }
                    }
                }
                else
                {
                    Log.PrintLog(this, "Cannot find I2C device with address: " + I2CTests[i].Address.ToString("x2").ToUpper(), LogDetailLevel.LogEverything);
                    RetVal = false;
                }
            }

            if (!DisconnectProgrammer())
            {
                TestStatusUpdate(MTKTestMessageType.Information, "Error!!!");
                Log.PrintLog(this, "Unable to disconnect from the programmer.", LogDetailLevel.LogRelevant);
                RetVal = false;
            }

            return RetVal;
        }
    }

    public enum MTKI2CTestType { NoTest, Read, Write };

    public struct UnitI2CTest
    {
        public int Address;
        public MTKI2CTestType Action;
        public byte[] DataBuffer;
        public int NumRxBytes;
        public byte[] RxDataBuffer;
        public bool ValidateRxData;
    }

}

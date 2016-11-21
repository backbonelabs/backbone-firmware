using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using System.IO.Ports;
using System.Diagnostics;
using PP_ComLib_Wrapper;

namespace CyBLE_MTK_Application
{
    public class MTKTestBDA : MTKTest
    {
        public MTKPSoCProgrammer BDAProgrammer;
        public byte[] BDAddress;
        public bool UseProgrammer;
        public bool AutoIncrementBDA;
        public event BDAChangeEventHandler OnBDAChange;

        public delegate void BDAChangeEventHandler(byte[] BDAddress);

        //Status of PP COM APIs
        const int S_OK = MTKPSoCProgrammer.S_OK;
        const int E_FAIL = MTKPSoCProgrammer.E_FAIL;

        public MTKTestBDA()
            : base()
        {
            Init();
        }

        public MTKTestBDA(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKTestBDA(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        private void Init()
        {
            BDAProgrammer = new MTKPSoCProgrammer(Log);
            BDAddress = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
            UseProgrammer = false;
            AutoIncrementBDA = false;
        }

        public override string GetDisplayText()
        {
            string returnString = "Auto-increment: " + AutoIncrementBDA.ToString() + " | Program using: " + (UseProgrammer ? "Programmer" : "Firmware");
            if (UseProgrammer)
            {
                returnString += " | " + BDAProgrammer.GetDisplayText();
            }
            return returnString;
        }

        private int GetRowData(byte rowID, out byte[] data, out string strError)
        {
            strError = "";

            //-1-. Initialize row data first. By defaul all zeros.
            data = new byte[BDAProgrammer.GetRowSize()];
            for (int i = 0; i < BDAProgrammer.GetRowSize(); i++) data[i] = 0x00;

            //-2-. Copy into data
            for (int i = 0; i < BDAddress.Length; i++)
            {
                data[i] = BDAddress[(BDAddress.Length - 1) - i];
            }
            return S_OK;
        }

        private int ProgramRow(byte rowID, out string strError)
        {
            int hr;
            byte[] data = new byte[BDAProgrammer.GetRowSize()];

            hr = GetRowData(rowID, out data, out strError);
            if (!SUCCEEDED(hr)) return hr;

            hr = BDAProgrammer.UpdateSFlashRow(rowID, data, out strError);
            if (!SUCCEEDED(hr)) return hr;

            return hr;
        }

        public MTKTestError WriteBDA()
        {
            bool IsSuccess = false;
            int hr;
            string strError = "";
            MTKTestError RetVal = MTKTestError.NoError;

            this.InitializeTestResult();

            if (UseProgrammer)
            {
                try
                {
                    Log.PrintLog(this, "Programming BD Adress...", LogDetailLevel.LogRelevant);
                    //-1-. Connect to PP instance first
                    TestStatusUpdate(MTKTestMessageType.Information, "Connecting...");
                    //hr = ConnectToMiniProg3(out strError);
                    if (!BDAProgrammer.ConnectProgrammer())
                    {
                        //MessageBox.Show(strError, "MiniProg3 Initialization Failed!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        throw new SFlashException("MiniProg3 Initialization Failed!", "MiniProg3 Initialization Failed!");
                    }

                    //-2-. Acquire Part
                    TestStatusUpdate(MTKTestMessageType.Information, "Erasing...");
                    if (!BDAProgrammer.AcquireAndEraseSFlash())
                    {
                        throw new SFlashException("Acquire and erase failed!", "Acquire and erase failed!");
                    }

                    //-3-. Update all SFLASH Rows
                    TestStatusUpdate(MTKTestMessageType.Information, "Programming...");
                    hr = ProgramRow(0, out strError);
                    if (!SUCCEEDED(hr)) throw new SFlashException("SFlash update failed!", strError);

                    //-5-. Just in case try to release part (if it was acquired)
                    TestStatusUpdate(MTKTestMessageType.Information, "Disconnecting...");
                    BDAProgrammer.ReleaseChip(out strError);

                    //-6-. Disconnect from MiniProg3, so other apps can use it while it is IDLE
                    //DisconnectFromMiniProg3(out strError);
                    BDAProgrammer.DisconnectProgrammer();

                    Log.PrintLog(this, "PASS", LogDetailLevel.LogRelevant);
                    TestStatusUpdate(MTKTestMessageType.Success, "PASS");

                    IsSuccess = true;
                }
                catch (SFlashException e)
                {
                    BDAProgrammer.DisconnectProgrammer();
                    Log.PrintLog(this, e.errorMsg, LogDetailLevel.LogRelevant);
                    TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                    RetVal = MTKTestError.TestFailed;
                }
            }
            else
            {
                int PercentageComplete = 0;
                int DelayPerCommand = 20;//, msPerSecond = 1000;

                MTKTestError CommandRetVal;

                this.Log.PrintLog(this, GetDisplayText(), LogDetailLevel.LogRelevant);

                TestStatusUpdate(MTKTestMessageType.Information, PercentageComplete.ToString() + "%");

                //  Command #1
                string Command = "RRS";
                CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
                if (CommandRetVal != MTKTestError.NoError)
                {
                    return CommandRetVal;
                }

                //  Command #2
                //byte[] temp = { BDAddress[5], BDAddress[4], BDAddress[3], BDAddress[2], BDAddress[1], BDAddress[0] };
                //Int32 temp1 = BitConverter.ToInt32(temp, 0);
                //Int16 temp2 = BitConverter.ToInt16(temp, 4);
                Command = "WBA " + BitConverter.ToString(BDAddress).Replace("-", " ");
                CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
                if (CommandRetVal != MTKTestError.NoError)
                {
                    return CommandRetVal;
                }

                Thread.Sleep(100);

                //  Command #3
                Command = "RRS";
                CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
                if (CommandRetVal != MTKTestError.NoError)
                {
                    return CommandRetVal;
                }
                if (CommandResult == "")
                {
                    this.Log.PrintLog(this, Command + ": unable to find DUT.", LogDetailLevel.LogRelevant);
                    TestStatusUpdate(MTKTestMessageType.Failure, "Error!!!");
                    return MTKTestError.MissingDUT;
                }

                if (CommandResult == "ERROR")
                {
                    TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                    RetVal = MTKTestError.TestFailed;
                    TestResult.Result = "FAIL";
                    this.Log.PrintLog(this, "Result: FAIL", LogDetailLevel.LogRelevant);
                }
                else if (CommandResult == "SUCCESS")
                {
                    TestStatusUpdate(MTKTestMessageType.Success, "PASS");
                    this.Log.PrintLog(this, "Result: PASS", LogDetailLevel.LogRelevant);
                    TestResult.Result = "PASS";
                    IsSuccess = true;
                }
            }

            if (AutoIncrementBDA && IsSuccess)
            {
                IncrementBDA();
            }

            return RetVal;
        }

        public static bool SUCCEEDED(int hr)
        {
            return hr >= 0;
        }

        private void IncrementBDA()
        {
            if (((int)BDAddress[5] + 1) > 255)
            {
                if (((int)BDAddress[4] + 1) > 255)
                {
                    if (((int)BDAddress[3] + 1) > 255)
                    {
                        if (((int)BDAddress[2] + 1) > 255)
                        {
                            if (((int)BDAddress[1] + 1) > 255)
                            {
                                BDAddress[0]++;
                            }
                            BDAddress[1]++;
                        }
                        BDAddress[2]++;
                    }
                    BDAddress[3]++;
                }
                BDAddress[4]++;
            }
            BDAddress[5]++;
            OnBDAChange(BDAddress);
        }
    }

    internal class SFlashException : Exception
    {
        public string errorType;
        public string errorMsg;

        public SFlashException(string errorType, string errorMsg)
        {
            this.errorType = errorType;
            this.errorMsg = errorMsg;
        }
    }
}

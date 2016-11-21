using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;
using System.Drawing;

namespace CyBLE_MTK_Application
{
    public class MTKTestXOCalibration : MTKTest
    {
        private int MarginOfErrorHz;
        public int MarginOfError
        {
            get
            {
                return (MarginOfErrorHz / 24);
            }
            set
            {
                MarginOfErrorHz = 24 * value;
            }
        }
        public string ScriptPath;

        private const int DesiredCrystalFrequencyHz = 24000000;

        public MTKTestXOCalibration() : base()
        {
            Init();
        }

        public MTKTestXOCalibration(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKTestXOCalibration(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        void Init()
        {
            MarginOfError = 1;
            ScriptPath = "";
            TestParameterCount = 2;
        }

        public override string GetDisplayText()
        {
            return "Crystal Calibration | Margin Of Error: (+/-)" + MarginOfError.ToString() + " ppm" + " | Script File: " + Path.GetFileName(ScriptPath);
        }

        public override string GetTestParameter(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return MarginOfError.ToString();
                case 1:
                    return ScriptPath;
            }
            return base.GetTestParameter(TestParameterIndex);
        }

        public override string GetTestParameterName(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return "MarginOfError";
                case 1:
                    return "ScriptPath";
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
                    MarginOfError = int.Parse(ParameterValue);
                    return true;
                case 1:
                    ScriptPath = ParameterValue;
                    return true;
            }
            return false;
        }

        enum FreqDevSign {Positive, Negative};
        enum TRIncDec {Increment, Decrement};
        public override MTKTestError RunTest()
        {
            //UInt16[] divFactor = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
            EquipmentScriptInterpreter XOCalScript = new EquipmentScriptInterpreter(ScriptPath);
            MTKTestError CommandRetVal;
            int DelayPerCommand = 20, loopCounter = 0; ;
            //UInt16 arryIndex = 0;//, msPerSecond = 1000;
            bool ContinueCalibration = true, CalibrationSuccess = false, fineTune = false, firstTime = true, firstFineTune = true;
            UInt16 trimRegister = 0x0000, prevTrimRegister = 0x0000, dFactor = 0x0080;
            int freqDeviation = 0, prevFreqDeviation = 0, freqDeviationMod = 0, prevFreqDeviationMod = 0, signReversalCount = 0;
            FreqDevSign currSign, prevSign = FreqDevSign.Negative;
            TRIncDec TRDir = TRIncDec.Decrement;

            this.InitializeTestResult();
            TestStatusUpdate(MTKTestMessageType.Information, "Calibrating...");

            if (XOCalScript.RunCommands(ScriptCommand.OpenSerialPortCMD) != ScriptError.NoError)
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                this.Log.PrintLog(this, "Cannot open Frequency Counter port." + Environment.NewLine + "Result: FAILED", LogDetailLevel.LogRelevant);
                return MTKTestError.TestFailed;
            }
            if (XOCalScript.RunCommands(ScriptCommand.InitCMD) != ScriptError.NoError)
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                this.Log.PrintLog(this, "Cannot communicate with Frequency Counter." + Environment.NewLine + "Result: FAILED", LogDetailLevel.LogRelevant);
                return MTKTestError.TestFailed;
            }

            //  Command #1
            string Command = "RRS";
            CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            do
            {
                Command = "WTR " + trimRegister.ToString("X4");
                CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
                if (CommandRetVal != MTKTestError.NoError)
                {
                    return CommandRetVal;
                }
                Thread.Sleep(200);

                if (XOCalScript.RunCommands(ScriptCommand.MeasureCMD) != ScriptError.NoError)
                {
                    TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                    this.Log.PrintLog(this, "Cannot communicate with Frequency Counter." + Environment.NewLine + "Result: FAILED", LogDetailLevel.LogRelevant);
                    return MTKTestError.TestFailed;
                }

                prevFreqDeviation = freqDeviation;
                prevFreqDeviationMod = freqDeviationMod;
                freqDeviation = XOCalScript.MeasuredFrequency - DesiredCrystalFrequencyHz;
                freqDeviationMod = (int)Math.Sqrt(freqDeviation * freqDeviation);
                if (freqDeviation >= 0)
                {
                    currSign = FreqDevSign.Positive;
                }
                else
                {
                    currSign = FreqDevSign.Negative;
                }
                if (firstTime)
                {
                    prevSign = currSign;
                    prevTrimRegister = trimRegister;
                }

                if ((prevSign != currSign) || (signReversalCount >= 2))
                {
                    if (fineTune == true)
                    {
                        dFactor = (UInt16)(dFactor >> 1);
                        signReversalCount = 0;
                        this.Log.PrintLog(this, "Checking next LSB.", LogDetailLevel.LogEverything);
                    }
                    else
                    {
                        int bitCnt = 0;
                        while (bitCnt < 8)
                        {
                            if ((((trimRegister & 0xFF) >> bitCnt) & 0x0001) == 0x0001)
                            {
                                break;
                            }
                            bitCnt++;
                        }
                        dFactor = (UInt16)(dFactor >> (8 - bitCnt));
                    }
                    if (fineTune == false)
                    {
                        this.Log.PrintLog(this, "Fine tuning.", LogDetailLevel.LogEverything);
                    }
                    fineTune = true;
                    firstFineTune = true;
                    if (prevFreqDeviationMod < freqDeviationMod)
                    {
                        trimRegister = prevTrimRegister;
                        TRDir = TRIncDec.Increment;
                        //signReversalCount--;
                    }
                    else
                    {
                        TRDir = TRIncDec.Decrement;
                    }
                    //if (signReversalCount >= 3)
                    if (dFactor == 0x0)
                    {
                        ContinueCalibration = false;
                        CalibrationSuccess = false;
                        break;
                    }
                }

                prevSign = currSign;
                prevTrimRegister = trimRegister;
                if (fineTune)
                {
                    if ((prevFreqDeviationMod < freqDeviationMod) && (firstFineTune == false))
                    {
                        if (TRDir == TRIncDec.Increment)
                        {
                            TRDir = TRIncDec.Decrement;
                        }
                        else
                        {
                            TRDir = TRIncDec.Increment;
                        }
                        signReversalCount++;
                    }
                    if (TRDir == TRIncDec.Increment)
                    {
                        trimRegister = (UInt16)((trimRegister & 0xFF00) | (((trimRegister & 0x00FF) + dFactor) & 0x00FF));
                        //trimRegister++;
                    }
                    else if (TRDir == TRIncDec.Decrement)
                    {
                        //trimRegister -= dFactor;
                        trimRegister = (UInt16)((trimRegister & 0xFF00) | (((trimRegister & 0x00FF) - dFactor) & 0x00FF));
                        //trimRegister--;
                    }
                    firstFineTune = false;
                }
                else
                {
                    if (prevTrimRegister == 0xFFFF)
                    {
                        ContinueCalibration = false;
                        break;
                    }
                    if (trimRegister < 0xF0F0)
                    {
                        trimRegister = (UInt16)((trimRegister + 0x1010));
                    }
                    else
                    {
                        trimRegister = 0xFFFF;
                    }
                }

                if ((freqDeviation < MarginOfErrorHz) && (freqDeviation > (-1 * MarginOfErrorHz)))
                {
                    ContinueCalibration = false;
                    CalibrationSuccess = true;
                    break;
                }

                //prevTrimRegister = trimRegister;
                firstTime = false;
                loopCounter++;
                this.Log.PrintLog(this, "Iteration: " + loopCounter.ToString(), LogDetailLevel.LogEverything);
            } while (ContinueCalibration);

            if (XOCalScript.RunCommands(ScriptCommand.CloseSerialPortCMD) != ScriptError.NoError)
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                this.Log.PrintLog(this, "Cannot close Frequency Counter port.", LogDetailLevel.LogRelevant);
                this.Log.PrintLog(this, "Result: FAILED", LogDetailLevel.LogRelevant);
                return MTKTestError.TestFailed;
            }

            MTKTestError RetVal;
            if (CalibrationSuccess)
            {
                Command = "STR";
                CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
                if (CommandRetVal != MTKTestError.NoError)
                {
                    return CommandRetVal;
                }
                TestStatusUpdate(MTKTestMessageType.Success, "PASS");
                RetVal = MTKTestError.NoError;
                this.Log.PrintLog(this, "Trim Register Value: " + trimRegister.ToString("X4"), LogDetailLevel.LogRelevant);
                this.Log.PrintLog(this, "Result: PASS", LogDetailLevel.LogRelevant);
                TestResult.Result = "PASS";
            }
            else
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                RetVal = MTKTestError.TestFailed;
                TestResult.Result = "FAIL";
                this.Log.PrintLog(this, "Result: FAIL", LogDetailLevel.LogRelevant);
            }
            return RetVal;
        }

        protected override void InitializeTestResult()
        {
            base.InitializeTestResult();
            TestResult.PassCriterion = "N/A";
            TestResult.Measured = "N/A";
        }
    }
}

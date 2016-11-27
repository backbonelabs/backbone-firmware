using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;
using System.Threading;

namespace CyBLE_MTK_Application
{
    public class MTKTestTXCW : MTKTest
    {
        public int ChannelNumber;
        public int PowerLevel;
        public int DurationForTXCW;

        public MTKTestTXCW()
            : base()
        {
            Init();
        }

        public MTKTestTXCW(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKTestTXCW(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        private void Init()
        {
            ChannelNumber = 0;
            PowerLevel = 0;
            DurationForTXCW = -1;
            TestParameterCount = 3;
        }

        public override string GetDisplayText()
        {
            return "TX Carrier Wave:" + " Channel = " + ChannelNumber.ToString() + 
                "; Power = " + PowerLevel.ToString() + 
                "; Time = " + DurationForTXCW.ToString() +"ms";
        }

        public override string GetTestParameter(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return ChannelNumber.ToString();
                case 1:
                    return PowerLevel.ToString() + " dBm";
                case 2:
                    return DurationForTXCW.ToString();
            }
            return base.GetTestParameter(TestParameterIndex);
        }

        public override string GetTestParameterName(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return "ChannelNumber";
                case 1:
                    return "PowerLevel";
                case 2:
                    return "DurationForTXCW";
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
                    if (VerifyChannelNumber(int.Parse(ParameterValue)))
                    {
                        ChannelNumber = int.Parse(ParameterValue);
                        return true;
                    }
                    return false;
                case 1:
                    if (VerifyPowerLevel(int.Parse(GetPowerLevel(ParameterValue))))
                    {
                        PowerLevel = int.Parse(GetPowerLevel(ParameterValue));
                        return true;
                    }
                    return false;
                case 2:
                    if (VerifyDurationForTXCW(int.Parse(ParameterValue)))
                    {
                        DurationForTXCW = int.Parse(ParameterValue);
                        return true;
                    }
                    return false;
            }
            return false;
        }

        private MTKTestError RunTestBLE()
        {
            int PercentageComplete = 0;
            int DelayPerCommand = 20;
            int TimeForEachCommand = 700;
            int TimeSlice = 100;
            int TimeIncSteps = (int)this.DurationForTXCW / TimeSlice;
            int PercentIncSteps = 1;

            MTKTestError CommandRetVal;

            this.Log.PrintLog(this, GetDisplayText(), LogDetailLevel.LogRelevant);

            TestStatusUpdate(MTKTestMessageType.Information, PercentageComplete.ToString() + "%");

            //  Command #1
            CommandRetVal = SearchForDUT();
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #2
            string Command = "DUT 1";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #3
            Command = "TXC " + this.ChannelNumber.ToString() + " " + PowerLevel.ToString()
                +" " + this.DurationForTXCW.ToString();
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #4
            Command = "DUT 0";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #5
            Command = "DCW " + (DurationForTXCW / TimeForEachCommand).ToString();
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            for (int i = 0; i < TimeSlice; i++)
            {
                PercentageComplete += PercentIncSteps;
                TestStatusUpdate(MTKTestMessageType.Information, PercentageComplete.ToString() + "%");
                Thread.Sleep(TimeIncSteps);
            }

            //  Command #6
            CommandRetVal = SearchForDUT();
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #7
            Command = "DUT 1";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #8
            Command = "PST";
            CommandRetVal = SendCommand(MTKSerialPort, Command, 200);
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
            TestResult.Result = CommandResult;
            this.Log.PrintLog(this, "Time elapsed (ms): " + this.CommandResult, LogDetailLevel.LogRelevant);

            //  Command #9
            Command = "DUT 0";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            TestStatusUpdate(MTKTestMessageType.Complete, "DONE");
            this.Log.PrintLog(this, "Result: DONE", LogDetailLevel.LogRelevant);

            return MTKTestError.NoError;
        }

        private MTKTestError RunTestUART()
        {
            int PercentageComplete = 0;
            int TimeSlice = 100, DelayPerCommand = 20;
            int TimeIncSteps = (int)this.DurationForTXCW / TimeSlice;
            int PercentIncSteps = 1;

            MTKTestError CommandRetVal;

            this.Log.PrintLog(this, GetDisplayText(), LogDetailLevel.LogRelevant);

            TestStatusUpdate(MTKTestMessageType.Information, PercentageComplete.ToString() + "%");

            //  Command #1
            string Command = "TXC " + this.ChannelNumber.ToString() + " " + PowerLevel.ToString()
                + " " + this.DurationForTXCW.ToString();
            CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            for (int i = 0; i < TimeSlice; i++)
            {
                PercentageComplete += PercentIncSteps;
                TestStatusUpdate(MTKTestMessageType.Information, PercentageComplete.ToString() + "%");
                Thread.Sleep(TimeIncSteps);
            }

            //  Command #2
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
            TestResult.Result = CommandResult;
            this.Log.PrintLog(this, "Time elapsed (ms): " + this.CommandResult, LogDetailLevel.LogRelevant);

            TestStatusUpdate(MTKTestMessageType.Complete, "DONE");
            this.Log.PrintLog(this, "Result: DONE", LogDetailLevel.LogRelevant);

            return MTKTestError.NoError;
        }

        public override MTKTestError RunTest()
        {
            MTKTestError RetVal = MTKTestError.NoError;

            this.InitializeTestResult();

            if (this.DUTConnectionMode == DUTConnMode.BLE)
            {
                RetVal = RunTestBLE();
            }
            else if (this.DUTConnectionMode == DUTConnMode.UART)
            {
                RetVal = RunTestUART();
            }
            else
            {
                return MTKTestError.NoConnectionModeSet;
            }

            TestResultUpdate(TestResult);

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

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
    class MTKTestTXPER : MTKTest
    {
        public int ChannelNumber;
        public int NumberOfPackets;
        public int PowerLevel;
        public string PacketType;
        public int PacketLength;
        public double PERPassCriterion;
    
        public MTKTestTXPER()
            : base()
        {
            Init();
        }

        public MTKTestTXPER(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKTestTXPER(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        private void Init()
        {
            ChannelNumber = 0;
            PowerLevel = 0;
            PacketType = "PRBS9";
            NumberOfPackets = 1500;
            PERPassCriterion = 30.8;
            PacketLength = 37;
            TestParameterCount = 6;
        }

        public override string GetDisplayText()
        {
            return "TX PER Test:" + " Channel = " + ChannelNumber.ToString() + 
                "; Power = " + PowerLevel.ToString() + " dBm" + 
                "; Number of Packets = " + NumberOfPackets.ToString() +
                "; Packet Type = " + PacketType +
                "; Packet Size = " + PacketLength.ToString() +
                "; PER Pass Criterion = " + PERPassCriterion.ToString();
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
                    return NumberOfPackets.ToString();
                case 3:
                    return PacketType;
                case 4:
                    return PacketLength.ToString();
                case 5:
                    return PERPassCriterion.ToString();
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
                    return "NumberOfPackets";
                case 3:
                    return "PacketType";
                case 4:
                    return "PacketLength";
                case 5:
                    return "PERPassCriterion";
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
                    if (VerifyNumberOfPackets(int.Parse(ParameterValue)))
                    {
                        NumberOfPackets = int.Parse(ParameterValue);
                        return true;
                    }
                    return false;
                case 3:
                    if (VerifyPacketType(ParameterValue))
                    {
                        PacketType = ParameterValue;
                        return true;
                    }
                    return false;
                case 4:
                    if (VerifyPacketLength(int.Parse(ParameterValue)))
                    {
                        PacketLength = int.Parse(ParameterValue);
                        return true;
                    }
                    return false;
                case 5:
                    if (VerifyPERPassCriterion(double.Parse(ParameterValue)))
                    {
                        PERPassCriterion = double.Parse(ParameterValue);
                        return true;
                    }
                    return false;
            }
            return false;
        }

        private MTKTestError RunTestBLE()
        {
            int PercentageComplete = 0;
            int DelayPerCommand = 20, msPerSecond = 1000;
            int TimeForEachPacket = 700;
            int TotalEstTime = ((int)((int)this.NumberOfPackets * TimeForEachPacket) / msPerSecond);
            int TimeSlice = (int)Math.Ceiling((double)TotalEstTime / 100.00);

            MTKTestError CommandRetVal;

            this.Log.PrintLog(this, GetDisplayText(), LogDetailLevel.LogRelevant);
            this.Log.PrintLog(this, "Pass criteria (PER): " + PERPassCriterion + "%",
                LogDetailLevel.LogRelevant);

            TestStatusUpdate(MTKTestMessageType.Information, PercentageComplete.ToString() + "%");

            //  Command #1
            string Command = "DUT 0";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #1
            Command = "RRS";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #2
            CommandRetVal = SearchForDUT();
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #1
            Command = "DUT 1";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #2
            Command = "SPL " + PacketLength.ToString();
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #3
            Command = "SPT " + GetPacketType(PacketType).ToString();
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #4
            Command = "TXP " + this.ChannelNumber.ToString() + " " + PowerLevel.ToString() +
                " " + this.NumberOfPackets.ToString();
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #5
            Command = "DUT 0";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #6
            Command = "RXP " + this.ChannelNumber.ToString() + " " + this.NumberOfPackets.ToString();
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            for (int i = 0; i <= 100; i++)
            {
                TestStatusUpdate(MTKTestMessageType.Information, PercentageComplete.ToString() + "%");
                Thread.Sleep(TimeSlice);
                PercentageComplete++;
            }

            //  Command #7
            CommandRetVal = SearchForDUT();
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #8
            Command = "PST";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }
            this.Log.PrintLog(this, "Number of packets received: " + this.CommandResult, LogDetailLevel.LogEverything);
            int RxPacket = Int32.Parse(CommandResult);

            //  Command #9
            Command = "DUT 1";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #10
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
            this.Log.PrintLog(this, "Number of packets transmitted: " + this.CommandResult, LogDetailLevel.LogEverything);
            int TxPacket = Int32.Parse(CommandResult);

            //  Command #11
            Command = "DUT 0";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            double PERPercent = ((double)(TxPacket - RxPacket) / (double)TxPacket) * 100.00;
            if (Double.IsInfinity(PERPercent))
            {
                PERPercent = 100.0;
            }
            TestResult.Measured = PERPercent.ToString();
            this.Log.PrintLog(this, "Measured PER: " + PERPercent.ToString() + "%", LogDetailLevel.LogRelevant);

            if (PERPercent < PERPassCriterion)
            {
                TestStatusUpdate(MTKTestMessageType.Success, "PASS");
                TestResult.Result = "PASS";
                this.Log.PrintLog(this, "Result: PASS", LogDetailLevel.LogRelevant);
            }
            else
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                TestResult.Result = "FAIL";
                this.Log.PrintLog(this, "Result: FAIL", LogDetailLevel.LogRelevant);
                return MTKTestError.TestFailed;
            }
            return MTKTestError.NoError;
        }

        private MTKTestError RunTestUART()
        {
            int PercentageComplete = 0;
            int DelayPerCommand = 10, msPerSecond = 1000;
            int TimeForEachPacket = 700;
            int TotalEstTime = ((int)((int)this.NumberOfPackets * TimeForEachPacket) / msPerSecond);
            int TimeSlice = (int)Math.Ceiling((double)TotalEstTime / 100.00);

            MTKTestError CommandRetVal;

            this.Log.PrintLog(this, GetDisplayText(), LogDetailLevel.LogRelevant);
            this.Log.PrintLog(this, "Pass criteria (PER): " + PERPassCriterion.ToString() + "%",
                LogDetailLevel.LogRelevant);

            TestStatusUpdate(MTKTestMessageType.Information, PercentageComplete.ToString() + "%");

            //  Command #1
            string Command = "RRS";
            CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #1
            Command = "SPL " + PacketLength.ToString();
            CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #2
            Command = "SPT " + GetPacketType(PacketType).ToString();
            CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #1
            Command = "RRS";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #3
            Command = "RXP " + this.ChannelNumber.ToString() + " " + this.NumberOfPackets.ToString();
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #4
            Command = "TXP " + this.ChannelNumber.ToString() + " " + PowerLevel.ToString() +
                " " + this.NumberOfPackets.ToString();
            CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            for (int i = 0; i <= 100; i++)
            {
                TestStatusUpdate(MTKTestMessageType.Information, PercentageComplete.ToString() + "%");
                Thread.Sleep(TimeSlice);
                PercentageComplete++;
            }

            //  Command #5
            Command = "RRS";
            CommandRetVal = SendCommand(DUTSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }
            this.Log.PrintLog(this, "Number of packets transmitted: " + this.CommandResult, LogDetailLevel.LogEverything);
            int TxPacket = Int32.Parse(CommandResult);

            //  Command #6
            Command = "RRS";
            CommandRetVal = SendCommand(MTKSerialPort, Command, DelayPerCommand);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }
            this.Log.PrintLog(this, "Number of packets received: " + this.CommandResult, LogDetailLevel.LogEverything);
            int RxPacket = Int32.Parse(CommandResult);

            double PERPercent = ((double)(TxPacket - RxPacket) / (double)TxPacket) * 100.00;
            if (Double.IsInfinity(PERPercent))
            {
                PERPercent = 100.0;
            }
            TestResult.Measured = PERPercent.ToString();
            this.Log.PrintLog(this, "Measured PER: " + PERPercent.ToString() + "%", LogDetailLevel.LogRelevant);

            if (PERPercent < PERPassCriterion)
            {
                TestStatusUpdate(MTKTestMessageType.Success, "PASS");
                TestResult.Result = "PASS";
                this.Log.PrintLog(this, "Result: PASS", LogDetailLevel.LogRelevant);
            }
            else
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                TestResult.Result = "FAIL";
                this.Log.PrintLog(this, "Result: FAIL", LogDetailLevel.LogRelevant);
                return MTKTestError.TestFailed;
            }
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
            TestResult.PassCriterion = PERPassCriterion.ToString();
        }
    }
}

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
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace CyBLE_MTK_Application
{
    public enum MTKTestError { NoError, ReceivedNAC, MissingDUT, TestFailed, NoConnectionModeSet, MissingDUTSerialPort,
        MissingMTKSerialPort, UnknownError, MissingAnritsuSerialPort, IgnoringDUT, NotAllDevicesProgrammed };
    public enum DUTConnMode { NoModeSelected, UART, BLE};
    public enum MTKTestMessageType { Success, Failure, Information, Complete }

    public partial class MTKTest
    {
        protected LogManager Log;
        protected SerialPort MTKSerialPort, DUTSerialPort;
        protected string CommandResult;
        protected string[] CommandResults;
        protected MTKTestResult TestResult;

        private int _TestParameterCount;
        public int TestParameterCount
        {
            get { return GetTestParameterCount(); }
            set { SetTestParameterCount(value); }
        }
        public DUTConnMode DUTConnectionMode;
        public event TestStatusUpdateEventHandler OnTestStatusUpdate;
        public event TestResultEventHandler OnTestResult;

        public delegate void TestStatusUpdateEventHandler(MTKTestMessageType Type, string Message);
        public delegate void TestResultEventHandler(MTKTestResult TestResult);

        public MTKTest()
        {
            TestParameterCount = 0;
            DUTConnectionMode = DUTConnMode.UART;
            Log = new LogManager();
        }

        public MTKTest(LogManager Logger)
            : this()
        {
            Log = Logger;
        }

        public MTKTest(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : this(Logger)
        {
            MTKSerialPort = MTKPort;
            DUTSerialPort = DUTPort;
        }

        protected virtual void TestStatusUpdate(MTKTestMessageType Type, string Message)
        {
            TestStatusUpdateEventHandler handler = OnTestStatusUpdate;
            if (handler != null)
            {
                handler(Type, Message);
            }
        }

        protected virtual void TestResultUpdate(MTKTestResult TestResult)
        {
            TestResultEventHandler handler = OnTestResult;
            if (handler != null)
            {
                handler(TestResult);
            }
        }

        public override string ToString()
        {
            return this.GetType().Name;//base.ToString();
        }

        public virtual string GetTestParameter(int TestParameterIndex)
        {
            return "";
        }

        public virtual string GetTestParameterName(int TestParameterIndex)
        {
            return "";
        }

        public virtual bool SetTestParameter(int TestParameterIndex, string ParameterValue)
        {
            return false;
        }

        public virtual string GetDisplayText()
        {
            return "MTK Test Base Class";
        }

        public virtual MTKTestError RunTest()
        {
            Log.PrintLog(this, "Started", LogDetailLevel.LogRelevant);
            Thread.Sleep(1000);
            Log.PrintLog(this, "Finished", LogDetailLevel.LogRelevant);
            OnTestStatusUpdate(MTKTestMessageType.Complete, "DONE");
            return MTKTestError.NoError;
        }

        public virtual void StopTest()
        {
            Log.PrintLog(this, "Stopped", LogDetailLevel.LogRelevant);
        }

        public virtual void ResetTest()
        {
            Log.PrintLog(this, "Reset", LogDetailLevel.LogRelevant);
        }

        public MTKTestError SendCommand(SerialPort SPort,string Command, int WaitTimeMS)
        {
            char[] DelimiterChars = { '\n', '\r'};

            this.CommandResult = "";
            try
            {
                SPort.DiscardOutBuffer();
                SPort.DiscardInBuffer();
                SPort.WriteLine(Command);
                Thread.Sleep(WaitTimeMS);
                string OuputACKNAC = SPort.ReadExisting();
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
                    //this.CommandResults = Output;
                    List<string> temp = new List<string>();
                    for (int i = 0; i < Output.Count(); i++)
                    {
                        if ((Output[i] != "") && (Output[i] != "ACK") && (Output[i] != "NAC"))
                        {
                            temp.Add(Output[i]);
                        }
                    }
                    this.CommandResults = temp.ToArray();
                    return MTKTestError.NoError;
                }
                else if (Output[0] == "NAC")
                {
                    TestStatusUpdate(MTKTestMessageType.Failure, "Error!!!");
                    this.Log.PrintLog(this, "Received \"NAC\" for command: " + Command, LogDetailLevel.LogRelevant);
                    return MTKTestError.ReceivedNAC;
                }
            }
            catch
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "Error!!!");
                return MTKTestError.MissingDUTSerialPort;
            }

            TestStatusUpdate(MTKTestMessageType.Failure, "Error!!!");
            this.Log.PrintLog(this, "No response from DUT.", LogDetailLevel.LogRelevant);
            return MTKTestError.MissingDUT;
        }

        public static DUTConnMode GetConnectionModeFromText(string Input)
        {
            if (Input == "UART")
            {
                return DUTConnMode.UART;
            }
            else if (Input == "BLE")
            {
                return DUTConnMode.BLE;
            }
            return DUTConnMode.NoModeSelected;
        }

        public static bool VerifyChannelNumber(int ChannelNumber)
        {
            if ((ChannelNumber >= 0) && (ChannelNumber <= 39))
            {
                return true;
            }
            return false;
        }

        public static bool VerifyPowerLevel(int PowerLevel)
        {
            switch (PowerLevel)
            {
                case 3:
                case 0:
                case -1:
                case -2:
                case -3:
                case -6:
                case -12:
                case -18:
                    return true;
                default:
                    return false;
            }
        }

        public static bool VerifyNumberOfPackets(int NumberOfPackets)
        {
            if ((NumberOfPackets >= 0) && (NumberOfPackets <= 65535))
            {
                return true;
            }
            return false;
        }

        public static bool VerifyDurationForTXCW(int DurationForTXCW)
        {
            if ((DurationForTXCW >= -1) && (DurationForTXCW <= 2147483647))
            {
                return true;
            }
            return false;
        }

        public static bool VerifyPacketType(string PacketType)
        {
            switch (PacketType)
            {
                case "PRBS9":
                case "11110000 Alternating Pattern":
                case "10101010 Alternating Pattern":
                case "PRBS15":
                case "All 1s":
                case "All 0s":
                case "00001111 Alternating Pattern":
                case "0101 Alternating Pattern":
                    return true;
                default:
                    return false;
            }
        }

        public static bool VerifyPacketLength(int PacketLength)
        {
            if ((PacketLength >= 0) && (PacketLength <= 37))
            {
                return true;
            }
            return false;
        }

        public static bool VerifyPERPassCriterion(double PERPassCriterion)
        {
            if ((PERPassCriterion >= 0.00) && (PERPassCriterion <= 100.00))
            {
                return true;
            }
            return false;
        }

        public static int GetPacketType(string PacketType)
        {
            switch (PacketType)
            {
                case "PRBS9":
                    return 0;
                case "11110000 Alternating Pattern":
                    return 1;
                case "10101010 Alternating Pattern":
                    return 2;
                case "PRBS15":
                    return 3;
                case "All 1s":
                    return 4;
                case "All 0s":
                    return 5;
                case "00001111 Alternating Pattern":
                    return 6;
                case "0101 Alternating Pattern":
                    return 7;
                default:
                    return 0;
            }
        }

        public static string GetPowerLevel(string PowerLevel)
        {
            char[] DelimiterChars = { ' ' };
            string[] Output = PowerLevel.Split(DelimiterChars);
            return Output[0];
        }

        protected MTKTestError SearchForDUT()
        {
            MTKTestError CommandRetVal;
            //  Command #1
            string Command = "DUT 0";
            CommandRetVal = SendCommand(MTKSerialPort, Command, 20);
            if (CommandRetVal != MTKTestError.NoError)
            {
                return CommandRetVal;
            }

            //  Command #2
            TestStatusUpdate(MTKTestMessageType.Information, "Finding DUT...");
            Command = "PCS";
            for (int i = 0; i < 20; i++)
            {
                CommandRetVal = SendCommand(MTKSerialPort, Command, 20);
                if (CommandRetVal != MTKTestError.NoError)
                {
                    return CommandRetVal;
                }
                Thread.Sleep(500);
                if (this.CommandResult == "CONNECTED")
                {
                    break;
                }
            }
            if (this.CommandResult == "DISCONNECTED")
            {
                this.Log.PrintLog(this, Command + ": unable to find DUT.", LogDetailLevel.LogRelevant);
                TestStatusUpdate(MTKTestMessageType.Failure, "Error!!!");
                return MTKTestError.MissingDUT;
            }
            return MTKTestError.NoError;
        }

        protected virtual void InitializeTestResult()
        {
            TestResult = new MTKTestResult();
            TestResult.TestName = this.ToString();
            TestResult.Parameters = new string[TestParameterCount];
            TestResult.Value = new string[TestParameterCount];
            for (int i = 0; i < TestParameterCount; i++)
            {
                TestResult.Parameters[i] = GetTestParameterName(i);
                TestResult.Value[i] = GetTestParameter(i);
            }
        }

        public void UpdateDUTPort(SerialPort DUTPort)
        {
            DUTSerialPort = DUTPort;
        }

        protected virtual int GetTestParameterCount()
        {
            return _TestParameterCount;
        }

        protected virtual void SetTestParameterCount(int Count)
        {
            _TestParameterCount = Count;
        }

        //public static T Clone<T>(this T source)
        //{
        //    if (!typeof(T).IsSerializable)
        //    {
        //        throw new ArgumentException("The type must be serializable.", "source");
        //    }

        //    // Don't serialize a null object, simply return the default for that object
        //    if (Object.ReferenceEquals(source, null))
        //    {
        //        return default(T);
        //    }

        //    IFormatter formatter = new BinaryFormatter();
        //    Stream stream = new MemoryStream();
        //    using (stream)
        //    {
        //        formatter.Serialize(stream, source);
        //        stream.Seek(0, SeekOrigin.Begin);
        //        return (T)formatter.Deserialize(stream);
        //    }
        //}
    }
}

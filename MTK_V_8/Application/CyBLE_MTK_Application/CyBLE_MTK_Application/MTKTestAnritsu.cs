using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using System.IO.Ports;

namespace CyBLE_MTK_Application
{
    public class MTKTestAnritsu : MTKTest
    {
        public SerialPort AnritsuPort;
        public int TestScriptID;
        public decimal OutputPowerOffset;
        public decimal TXPowerOffset;

        public MTKTestAnritsu() : base()
        {
            Init();
        }

        public MTKTestAnritsu(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKTestAnritsu(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        private void Init()
        {
            TestParameterCount = 0;
            AnritsuPort = new SerialPort();
            OutputPowerOffset = new decimal(0.0);
            TXPowerOffset = new decimal(0.0);
        }

        public override string GetDisplayText()
        {
            return "Anritsu Test Program";
        }

        public override string GetTestParameter(int TestParameterIndex)
        {
            return "";
        }

        public override string GetTestParameterName(int TestParameterIndex)
        {
            return "";
        }

        public override bool SetTestParameter(int TestParameterIndex, string ParameterValue)
        {
            return false;
        }

        public override MTKTestError RunTest()
        {
            char[] DelimiterChars = { ',', '\n' };
            string OuputACKNAC;
            bool FailedOnce = false;

            this.InitializeTestResult();
            TestStatusUpdate(MTKTestMessageType.Information, "Applying offsets...");
            this.Log.PrintLog(this, "Applying offsets.", LogDetailLevel.LogRelevant);
            AnritsuPort.WriteLine("LEOPCFG " + TestScriptID.ToString() + ",AVGMNLIM," + OutputPowerOffset.ToString());
            this.Log.PrintLog(this, "Setting offset: LEOPCFG " + TestScriptID.ToString() + ",AVGMNLIM," + OutputPowerOffset.ToString() + ": Done", LogDetailLevel.LogEverything);
            Thread.Sleep(20);
            AnritsuPort.DiscardInBuffer();
            AnritsuPort.DiscardOutBuffer();
            AnritsuPort.WriteLine("LEOPCFG? 4,AVGMNLIM");
            Thread.Sleep(200);
            string OffsetOutput= AnritsuPort.ReadExisting();
            string[] OffsetOutputBroke = OffsetOutput.Split(DelimiterChars);
            if (decimal.Parse(OffsetOutputBroke[2]) != OutputPowerOffset)
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                this.Log.PrintLog(this, "Cannot apply output power offset.", LogDetailLevel.LogRelevant);
                return MTKTestError.TestFailed;
            }

            AnritsuPort.WriteLine("LESSCFG " + TestScriptID.ToString() + ",TXPWR," + TXPowerOffset.ToString());
            this.Log.PrintLog(this, "Setting offset: LESSCFG " + TestScriptID.ToString() + ",TXPWR," + TXPowerOffset.ToString() + ": Done", LogDetailLevel.LogEverything);
            Thread.Sleep(20);
            AnritsuPort.DiscardInBuffer();
            AnritsuPort.DiscardOutBuffer();
            AnritsuPort.WriteLine("LESSCFG? 4,TXPWR");
            Thread.Sleep(200);
            OffsetOutput = AnritsuPort.ReadExisting();
            OffsetOutputBroke = OffsetOutput.Split(DelimiterChars);
            if (decimal.Parse(OffsetOutputBroke[2]) != TXPowerOffset)
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                this.Log.PrintLog(this, "Cannot apply TX power offset.", LogDetailLevel.LogRelevant);
                return MTKTestError.TestFailed;
            }

            TestStatusUpdate(MTKTestMessageType.Information, "Running...");
            this.Log.PrintLog(this, "Running Anritus test script.", LogDetailLevel.LogRelevant);

            AnritsuPort.WriteLine("RUN");
            int LoopCount = 0;
            do
            {
                AnritsuPort.WriteLine("*INS?");
                Thread.Sleep(100);
                OuputACKNAC = AnritsuPort.ReadExisting();
                LoopCount++;
            } while ((OuputACKNAC != "R46\n") && (LoopCount < 150));

            //Thread.Sleep(2000);

            if (OuputACKNAC == "R46\n")
            {
                Log.PrintLog(this, "Getting result for: LEOP", LogDetailLevel.LogRelevant);
                string TestOutput = GetResults("ORESULT TEST,0,LEOP");
                string[] Output = TestOutput.Split(DelimiterChars);
                if (Output[0] == "R46")
                {
                    List<string> temp = Output.Cast<string>().ToList();
                    temp.Remove("R46");
                    Output = temp.ToArray();
                }
                if ((Output[0] == "RLEOP0") && (Output[1] == "TRUE"))
                {
                    MTKTestResult TestResult1 = new MTKTestResult();
                    TestResult1.TestName = this.ToString() + " - LEOP0";
                    TestResult1.PassCriterion = "N/A";
                    TestResult1.Measured = "N/A";
                    TestResult1.Parameters = new string[] { "Packet Average Power", "Test Average Maximum",
                                                            "Test Average Minimum", "Overall Peak Power", 
                                                            "Number of Failed Packets", "Number of Tested Packets"};
                    TestResult1.Value = new string[] { Output[2], Output[3], Output[4], Output[5], Output[6], Output[7] };
                    TestResult1.Result = Output[8];
                    TestResultUpdate(TestResult1);

                    Log.PrintLog(this, "Packet Average Power: " + Output[2] + "dBm", LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Test Average Maximum: " + Output[3] + "dBm", LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Test Average Minimum: " + Output[4] + "dBm", LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Overall Peak Power: " + Output[5] + "dBm", LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Number of Failed Packets: " + Output[6], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Number of Tested Packets: " + Output[7], LogDetailLevel.LogRelevant);
                    Log.PrintLog(this, "Result: " + Output[8], LogDetailLevel.LogRelevant);
                    if (Output[8] == "FAIL")
                    {
                        TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                        FailedOnce = true;
                    }
                }
                else
                {
                    Log.PrintLog(this, "Invalid Test Result: " + TestOutput, LogDetailLevel.LogRelevant);
                    //TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                    //this.Log.PrintLog(this, "Result: FAIL", LogDetailLevel.LogRelevant);
                    FailedOnce = true;
                }

                Log.PrintLog(this, "Getting result for: LEICD", LogDetailLevel.LogRelevant);
                TestOutput = GetResults("ORESULT TEST,0,LEICD");
                Output = TestOutput.Split(DelimiterChars);
                if ((Output[0] == "RLEICD0") && (Output[1] == "TRUE"))
                {
                    MTKTestResult TestResult1 = new MTKTestResult();
                    TestResult1.TestName = this.ToString() + " - LEICD0";
                    TestResult1.PassCriterion = "N/A";
                    TestResult1.Measured = "N/A";
                    TestResult1.Parameters = new string[] { "Average Fn", "Maximum Positive Fn", "Minimum Negative Fn",
                                                            "Drift Rate", "Average Drift", "Maximum Drift", "Packets Failed",
                                                            "Packets Tested"};
                    TestResult1.Value = new string[] { Output[2], Output[3], Output[4], Output[5],
                                                       Output[6], Output[7], Output[8], Output[9] };
                    TestResult1.Result = Output[10];
                    TestResultUpdate(TestResult1);

                    Log.PrintLog(this, "Average Fn: " + Output[2], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Maximum Positive Fn: " + Output[3], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Minimum Negative Fn: " + Output[4], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Drift Rate: " + Output[5], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Average Drift: " + Output[6], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Maximum Drift: " + Output[7], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Packets Failed: " + Output[8], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Packets Tested: " + Output[9], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Result: " + Output[10], LogDetailLevel.LogRelevant);
                    if (Output[10] == "FAIL")
                    {
                        TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                        FailedOnce = true;
                    }
                }
                else
                {
                    Log.PrintLog(this, "Invalid Test Result: " + TestOutput, LogDetailLevel.LogRelevant);
                    //TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                    //this.Log.PrintLog(this, "Result: FAIL", LogDetailLevel.LogRelevant);
                    FailedOnce = true;
                }

                Log.PrintLog(this, "Getting result for: LEMI", LogDetailLevel.LogRelevant);
                TestOutput = GetResults("ORESULT TEST,0,LEMI");
                Output = TestOutput.Split(DelimiterChars);
                if ((Output[0] == "RLEMI0") && (Output[1] == "TRUE"))
                {
                    MTKTestResult TestResult1 = new MTKTestResult();
                    TestResult1.TestName = this.ToString() + " - LEMI0";
                    TestResult1.PassCriterion = "N/A";
                    TestResult1.Measured = "N/A";
                    TestResult1.Parameters = new string[] { "Delta f1 max in Hz", "Delta f1 average in Hz", "Delta f2 max in Hz",
                                                            "Delta f2 average in Hz", "Delta f2avg/ delta f1avg", "F2 max Failed limit",
                                                            "F2 max count", "Packets failed", "Packets tested", "F2max % pass rate"};

                    TestResult1.Value = new string[] { Output[2], Output[3], Output[4], Output[5],
                                                       Output[6], Output[7], Output[8], Output[9], Output[10], Output[12] };
                    TestResult1.Result = Output[11];
                    TestResultUpdate(TestResult1);

                    Log.PrintLog(this, "Delta f1 max: " + Output[2] + "Hz", LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Delta f1 average: " + Output[3] + "Hz", LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Delta f2 max: " + Output[4] + "Hz", LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Delta f2 average: " + Output[5] + "Hz", LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Delta f2 avg / delta f1 avg : " + Output[6], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "F2 max Failed limit: " + Output[7], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "F2 max count: " + Output[8], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Packets failed: " + Output[9], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Packets tested: " + Output[10], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "F2max % pass rate: " + Output[12], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Result: " + Output[11], LogDetailLevel.LogRelevant);
                    if (Output[11] == "FAIL")
                    {
                        TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                        FailedOnce = true;
                    }
                }
                else
                {
                    Log.PrintLog(this, "Invalid Test Result: " + TestOutput, LogDetailLevel.LogRelevant);
                    //TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                    //this.Log.PrintLog(this, "Result: FAIL", LogDetailLevel.LogRelevant);
                    FailedOnce = true;
                }

                Log.PrintLog(this, "Getting result for: LESS", LogDetailLevel.LogRelevant);
                TestOutput = GetResults("ORESULT TEST,0,LESS");
                Output = TestOutput.Split(DelimiterChars);
                if ((Output[0] == "RLESS0") && (Output[1] == "TRUE"))
                {
                    MTKTestResult TestResult1 = new MTKTestResult();
                    TestResult1.TestName = this.ToString() + " - LESS0";
                    TestResult1.PassCriterion = "N/A";
                    TestResult1.Measured = "N/A";
                    TestResult1.Parameters = new string[] { "Overall FER", "Total Frames Counted by DUT",
                                                            "Total Frames Sent by Tester"};

                    TestResult1.Value = new string[] { Output[2], Output[3], Output[4] };
                    TestResult1.Result = Output[5];
                    TestResultUpdate(TestResult1);

                    Log.PrintLog(this, "Overall FER: " + Output[2], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Total Frames Counted by DUT: " + Output[3], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Total Frames Sent by Tester: " + Output[4], LogDetailLevel.LogEverything);
                    Log.PrintLog(this, "Result: " + Output[5], LogDetailLevel.LogRelevant);
                    if (Output[5] == "FAIL")
                    {
                        TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                        FailedOnce = true;
                    }
                }
                else
                {
                    Log.PrintLog(this, "Invalid Test Result: " + TestOutput, LogDetailLevel.LogRelevant);
                    //TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                    //this.Log.PrintLog(this, "Result: FAIL", LogDetailLevel.LogRelevant);
                    FailedOnce = true;
                }
            }
            else
            {
                Log.PrintLog(this, "Test Error: " + OuputACKNAC, LogDetailLevel.LogRelevant);
                //TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                //this.Log.PrintLog(this, "Result: FAIL", LogDetailLevel.LogRelevant);
                FailedOnce = true;
            }

            MTKTestError RetVal = MTKTestError.NoError;

            if (FailedOnce)
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
                RetVal = MTKTestError.TestFailed;
            }
            else
            {
                TestStatusUpdate(MTKTestMessageType.Success, "PASS");
            }

            TestResult.Result = "N/A";
            //TestResultUpdate(TestResult);
            return RetVal;
        }

        private string GetResults(string Command)
        {
            AnritsuPort.DiscardInBuffer();
            AnritsuPort.DiscardOutBuffer();
            AnritsuPort.WriteLine(Command);
            Thread.Sleep(200);
            string OuputACKNAC = AnritsuPort.ReadExisting();
            //Log.PrintLog(this, "'" + Command + "' Result: " + OuputACKNAC, LogDetailLevel.LogRelevant);

            return OuputACKNAC;
        }

        protected override void InitializeTestResult()
        {
            base.InitializeTestResult();
            TestResult.PassCriterion = "N/A";
            TestResult.Measured = "N/A";
        }

        public bool SwitchToAnritsu()
        {
            MTKTestError CommandRetVal = SendCommand(DUTSerialPort, "ARU", 100);
            if (CommandRetVal == MTKTestError.NoError)
            {
                return true;
            }

            return false;
        }
    
        public bool SwitchToMTK()
        {
            DUTSerialPort.WriteLine("AB");
            Thread.Sleep(100);
            string OuputACKNAC = DUTSerialPort.ReadExisting();
            return OuputACKNAC.Contains("AB");
        }
    }
}

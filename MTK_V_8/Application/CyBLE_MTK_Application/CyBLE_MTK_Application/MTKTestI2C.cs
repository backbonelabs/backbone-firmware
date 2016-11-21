using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.IO;

namespace CyBLE_MTK_Application
{
    public class MTKTestI2C : MTKTest
    {
        public MTKPSoCProgrammer Programmer;
        public UnitI2CTest[] I2CTests;

        public MTKTestI2C() : base()
        {
            Init();
        }

        public MTKTestI2C(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKTestI2C(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        void Init()
        {
            //TestParameterCount = 2;
            Programmer = new MTKPSoCProgrammer(Log);
            I2CTests = new UnitI2CTest[0];
        }

        protected override int GetTestParameterCount()
        {
            //return base.GetTestParameterCount();
            return (I2CTests.Count() * 4) + 1;
        }

        protected override void SetTestParameterCount(int Count)
        {
            I2CTests = new UnitI2CTest[(Count - 1) / 4];
            base.SetTestParameterCount(Count);
        }

        public override string GetDisplayText()
        {
            return "I2C Test | Adress: " + "Instructions";
        }

        public override string GetTestParameter(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return I2CTests.Count().ToString();
                default:
                    int TestIndex = (TestParameterIndex - 1) / 4;
                    int TestElementIndex = (TestParameterIndex - 1) % 4;
                    switch (TestElementIndex)
                    {
                        case 0:
                            return I2CTests[TestIndex].Address.ToString();
                        case 1:
                            string temp = "";
                            if (I2CTests[TestIndex].Action == MTKI2CTestType.Read)
                            {
                                temp = "Read";
                            }
                            else if (I2CTests[TestIndex].Action == MTKI2CTestType.Write)
                            {
                                temp = "Write";
                            }
                            return temp;
                        case 2:
                            StringBuilder hex = new StringBuilder(I2CTests[TestIndex].DataBuffer.Length * 2);
                            foreach (byte b in I2CTests[TestIndex].DataBuffer)
                            {
                                hex.AppendFormat("{0:x2}", b);
                            }
                            return hex.ToString().ToUpper();
                        case 3:
                            return I2CTests[TestIndex].ValidateRxData.ToString();
                        default:
                            return base.GetTestParameter(TestParameterIndex);
                    }
                    //return base.GetTestParameter(TestParameterIndex);
            }
        }

        public override string GetTestParameterName(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return "NumberOfI2CTests";
                default:
                    int TestIndex = (TestParameterIndex - 1) / 4;
                    int TestElementIndex = (TestParameterIndex - 1) % 4;
                    switch (TestElementIndex)
                    {
                        case 0:
                            return "Address" + TestIndex.ToString();
                        case 1:
                            return "Action" + TestIndex.ToString();
                        case 2:
                            return "Data" + TestIndex.ToString();
                        case 3:
                            return "Validate" + TestIndex.ToString();
                        default:
                            return base.GetTestParameterName(TestParameterIndex);
                    }
            }
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
                    int NumTests = int.Parse(ParameterValue);
                    I2CTests = new UnitI2CTest[NumTests];
                    return true;
                default:
                    int TestIndex = (TestParameterIndex - 1) / 4;
                    int TestElementIndex = (TestParameterIndex - 1) % 4;
                    switch (TestElementIndex)
                    {
                        case 0:
                            I2CTests[TestIndex].Address = int.Parse(ParameterValue);
                            return true;
                        case 1:
                            if (ParameterValue == "Read")
                            {
                                I2CTests[TestIndex].Action = MTKI2CTestType.Read;
                                return true;
                            }
                            else if (ParameterValue == "Write")
                            {
                                I2CTests[TestIndex].Action = MTKI2CTestType.Write;
                                return true;
                            }
                            return false;
                        case 2:
                            I2CTests[TestIndex].DataBuffer = ToByteArray(ParameterValue);
                            I2CTests[TestIndex].NumRxBytes = I2CTests[TestIndex].DataBuffer.Count();
                            return true;
                        case 3:
                            I2CTests[TestIndex].ValidateRxData = bool.Parse(ParameterValue);
                            return true;
                        default:
                            return false;
                    }
            }
        }

        private byte[] ToByteArray(string cleanText)
        {
            int count = 0;

            if ((cleanText.Length % 2) != 0)
            {
                cleanText = "0" + cleanText;
            }

            byte[] data = new byte[cleanText.Length / 2];
            for (int i = 0; i < cleanText.Length; i += 2)
            {
                string Char2Convert = cleanText.Substring(i, 2);
                data[count++] = Convert.ToByte(Char2Convert, 16);
            }
            return data;
        }

        public override MTKTestError RunTest()
        {
            MTKTestError RetVal = MTKTestError.NoError;
            bool FailedOnce = false;

            this.InitializeTestResult();

            if (!Programmer.RunI2CTest(I2CTests))
            {
                RetVal = MTKTestError.TestFailed;
                FailedOnce = true;
            }
            else
            {
                RetVal = MTKTestError.NoError;
                List<string> ResultParams = new List<string>();
                List<string> ResultVal = new List<string>();
                TestResult.Result = "PASS";
                for (int i = 0; i < I2CTests.Count(); i++)
                {
                    ResultParams.Add("Address#" + i.ToString());
                    ResultVal.Add(I2CTests[i].Address.ToString());
                    ResultParams.Add("Action#" + i.ToString());
                    string Temp1 = "NONE";
                    if (I2CTests[i].Action == MTKI2CTestType.Read)
                    {
                        Temp1 = "Read";
                    }
                    else if (I2CTests[i].Action == MTKI2CTestType.Write)
                    {
                        Temp1 = "Write";
                    }
                    ResultVal.Add(Temp1);
                    ResultParams.Add("Data#" + i.ToString());
                    StringBuilder hex = new StringBuilder(I2CTests[i].DataBuffer.Length * 2);
                    foreach (byte b in I2CTests[i].DataBuffer)
                    {
                        hex.AppendFormat("{0:x2}", b);
                    }
                    ResultVal.Add(hex.ToString().ToUpper());
                    if (I2CTests[i].ValidateRxData)
                    {
                        ResultParams.Add("Result#" + i.ToString());
                        if (I2CTests[i].DataBuffer.SequenceEqual(I2CTests[i].RxDataBuffer))
                        {
                            Log.PrintLog(this, "PASS", LogDetailLevel.LogEverything);
                            ResultVal.Add("PASS");
                        }
                        else
                        {
                            Log.PrintLog(this, "FAIL", LogDetailLevel.LogEverything);
                            FailedOnce = true;
                            RetVal = MTKTestError.TestFailed;
                            ResultVal.Add("FAIL");
                            TestResult.Result = "FAIL";
                        }
                    }
                }

                TestResult.Parameters = ResultParams.ToArray();
                TestResult.Value = ResultVal.ToArray();
                TestResultUpdate(TestResult);
            }
            if (FailedOnce)
            {
                TestStatusUpdate(MTKTestMessageType.Failure, "FAIL");
            }
            else
            {
                TestStatusUpdate(MTKTestMessageType.Success, "PASS");
            }
            return RetVal;
        }
    }
}

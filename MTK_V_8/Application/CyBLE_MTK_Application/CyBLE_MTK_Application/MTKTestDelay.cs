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
    public class MTKTestDelay : MTKTest
    {
        public int DelayInMS;

        public MTKTestDelay() : base()
        {
            Init();
        }

        public MTKTestDelay(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKTestDelay(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        void Init()
        {
            DelayInMS = 0;
            TestParameterCount = 1;
        }

        public override string GetDisplayText()
        {
            return "Delay:" + DelayInMS.ToString() + "ms";
        }

        public override string GetTestParameter(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return DelayInMS.ToString();
            }
            return base.GetTestParameter(TestParameterIndex);
        }

        public override string GetTestParameterName(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return "DelayInMS";
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
                    DelayInMS = int.Parse(ParameterValue);
                    return true;
            }
            return false;
        }

        public override MTKTestError RunTest()
        {
            this.InitializeTestResult();
            TestStatusUpdate(MTKTestMessageType.Information, "Waiting...");
            this.Log.PrintLog(this, "Waiting for: " + DelayInMS.ToString(), LogDetailLevel.LogRelevant);

            Thread.Sleep(DelayInMS);

            TestStatusUpdate(MTKTestMessageType.Complete, "DONE");
            this.Log.PrintLog(this, "Result: DONE", LogDetailLevel.LogRelevant);

            TestResult.Result = DelayInMS.ToString();
            //TestResultUpdate(TestResult);
            return MTKTestError.NoError;
        }

        protected override void InitializeTestResult()
        {
            base.InitializeTestResult();
            TestResult.PassCriterion = "N/A";
            TestResult.Measured = "N/A";
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CyBLE_MTK_Application
{
    public class MTKTestResult
    {
        public string[] Parameters;
        public string[] Value;
        public string PassCriterion;
        public string Measured;
        public string Result;
        public string TestName;

        public MTKTestResult()
        {
            Parameters = new string[0];// { "N/A"};
            Value = new string[0];// { "N/A"};
            PassCriterion = "N/A";
            Measured = "N/A";
            Result = "N/A";
            TestName = "N/A";
        }
    }
}

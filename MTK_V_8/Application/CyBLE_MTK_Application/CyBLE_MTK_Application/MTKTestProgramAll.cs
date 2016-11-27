using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.IO.Ports;
using System.Threading;

namespace CyBLE_MTK_Application
{
    public class MTKTestProgramAll : MTKTest
    {
        public int CurrentDUT;
        public int NumberOfDUTs;
        public List<MTKPSoCProgrammer> DUTProgrammers;
        public bool ProgramAllAtEnd = false;

        private bool programCompleted;
        public bool ProgramCompleted
        {
            get
            {
                return programCompleted;
            }
        }

        public event ProgramAllCompleteEventHandler OnProgramAllComplete;

        public delegate void ProgramAllCompleteEventHandler(List<MTKTestError> err);

        public MTKTestProgramAll()
            : base()
        {
            Init();
        }

        public MTKTestProgramAll(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKTestProgramAll(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        private void Init()
        {
            programCompleted = false;
            TestParameterCount = 0;
            NumberOfDUTs = 0;
            CurrentDUT = 0;
        }

        public override string GetDisplayText()
        {
            string temp = (ProgramAllAtEnd)?"at the end.":"at the begning.";
            return "Program all devices " + temp;
        }

        public override string GetTestParameter(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return ProgramAllAtEnd.ToString();
            }
            return base.GetTestParameter(TestParameterIndex);
        }

        public override string GetTestParameterName(int TestParameterIndex)
        {
            switch (TestParameterIndex)
            {
                case 0:
                    return "ProgramAllAtEnd";
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
                    ProgramAllAtEnd = bool.Parse(ParameterValue);
                    return true;
            }
            return false;
        }

        public override MTKTestError RunTest()
        {
            MTKTestError return_value = MTKTestError.NoError;

            if (CurrentDUT == 0)
            {
                programCompleted = false;
            }

            if (((CurrentDUT == 0) && (ProgramAllAtEnd == false)) ||
                ((CurrentDUT == (NumberOfDUTs - 1)) && (ProgramAllAtEnd == true)))
            {
                if (!ProgramAll())
                {
                    return_value = MTKTestError.NotAllDevicesProgrammed;
                }
            }

            TestStatusUpdate(MTKTestMessageType.Complete, "DONE");
            return return_value;
        }

        private bool ProgramAll()
        {
            int i;
            bool return_value = true;
            List<Thread> ProgDUT = new List<Thread>();
            List<MTKTestError> ErrDUT = new List<MTKTestError>();

            for (i = 0; i < NumberOfDUTs; i++)
            {
                if ((DUTProgrammers[i].SelectedProgrammer != "") &&
                    (DUTProgrammers[i].SelectedHEXFilePath != ""))
                {
                    try
                    {
                        ErrDUT.Add(new MTKTestError());
                        ProgDUT.Add(new Thread(() => { ErrDUT[i] = DUTProgrammers[i].RunTest(); }));
                        ProgDUT[i].Start();
                        Thread.Sleep(200);
                    }
                    catch
                    {
                        Log.PrintLog(this, "Cannot create programming thread.", LogDetailLevel.LogRelevant);
                    }
                }
                //ProgDUT[i].Join();
            }

            for (i = 0; i < ProgDUT.Count(); i++)
            {
                ProgDUT[i].Join();
            }

            for (i = 0; i < ErrDUT.Count(); i++)
            {
                if (ErrDUT[i] == MTKTestError.TestFailed)
                {
                    return_value = false;
                    break;
                }
            }

            OnProgramAllComplete(ErrDUT);
            programCompleted = true;

            return return_value;
        }
    }
}

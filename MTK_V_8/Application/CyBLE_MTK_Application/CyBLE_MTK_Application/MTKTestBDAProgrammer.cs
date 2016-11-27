using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.IO.Ports;

namespace CyBLE_MTK_Application
{
    class MTKTestBDAProgrammer : MTKTest
    {
        public event BDAProgrammerEventHandler OnBDAProgram;

        public delegate void BDAProgrammerEventHandler(SerialPort DUTPort, MTKTestErrorEventArgs e);

        public MTKTestBDAProgrammer()
            : base()
        {
            Init();
        }

        public MTKTestBDAProgrammer(LogManager Logger)
            : base(Logger)
        {
            Init();
        }

        public MTKTestBDAProgrammer(LogManager Logger, SerialPort MTKPort, SerialPort DUTPort)
            : base(Logger, MTKPort, DUTPort)
        {
            Init();
        }

        private void Init()
        {
        }

        public override string GetDisplayText()
        {
            return "BD Address Programmer";
        }

        public override MTKTestError RunTest()
        {
            MTKTestErrorEventArgs e = new MTKTestErrorEventArgs();
            OnBDAProgram(DUTSerialPort, e);
            return e.ReturnValue;
        }

    }

    class MTKTestErrorEventArgs : EventArgs
    {
        public MTKTestError ReturnValue = MTKTestError.NoError;
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CyBLE_MTK_Application
{
    public partial class MTKTestSTCDialog : Form
    {
        private MTKTestSTC STCTest;

        public MTKTestSTCDialog()
        {
            InitializeComponent();
            STCTest = new MTKTestSTC();
        }

        public MTKTestSTCDialog(MTKTestSTC NewSTCTest) : this()
        {
            STCTest = NewSTCTest;
        }

        protected override void OnLoad(EventArgs e)
        {
            PKTCNTNumericUpDown.Value = (decimal)STCTest.PacketCount;
            base.OnLoad(e);
        } 

        private void OKButton_Click(object sender, EventArgs e)
        {
            STCTest.PacketCount = (int)PKTCNTNumericUpDown.Value;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
    }
}

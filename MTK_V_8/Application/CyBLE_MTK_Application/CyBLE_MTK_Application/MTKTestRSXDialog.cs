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
    public partial class MTKTestRSXDialog : Form
    {
        MTKTestRSX GetRSSI;

        public MTKTestRSXDialog()
        {
            InitializeComponent();
            this.ChannelNumber.SelectedIndex = 0;
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            GetRSSI.ChannelNumber = this.ChannelNumber.SelectedIndex;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        public MTKTestRSXDialog(MTKTestRSX RSSITest)
            : this()
        {
            GetRSSI = RSSITest;
        }
    }
}

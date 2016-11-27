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
    public partial class MTKTestRxTxDialog : Form
    {
        public MTKTestRxTxDialog()
        {
            InitializeComponent();
            this.ChannelNumber.SelectedIndex = 0;
            this.PowerLevel.SelectedIndex = 1;
            this.PacketTypeComboBox.SelectedIndex = 0;
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
    }
}

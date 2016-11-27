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
    public partial class MTKTestAnritsuDialog : Form
    {
        public int ScriptID;

        public MTKTestAnritsuDialog()
        {
            InitializeComponent();
            ScriptID = 1;
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            ScriptID = (int)ScriptIDNumericUpDown.Value;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
    }
}

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
    public partial class MTKTestProgramAllDialog : Form
    {
        private MTKTestProgramAll ProgAll;

        public MTKTestProgramAllDialog()
        {
            InitializeComponent();
            ProgAll = new MTKTestProgramAll();
        }

        public MTKTestProgramAllDialog(MTKTestProgramAll ProgramAllTest) : this()
        {
            ProgAll = ProgramAllTest;
        }

        protected override void OnLoad(EventArgs e)
        {
            if (ProgAll.ProgramAllAtEnd)
            {
                EndRadioButton.Checked = true;
            }
            else
            {
                BegningRadioButton.Checked = true;
            }
            base.OnLoad(e);
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void BegningRadioButton_CheckedChanged(object sender, EventArgs e)
        {
            if (BegningRadioButton.Checked == true)
            {
                ProgAll.ProgramAllAtEnd = false;
            }
        }

        private void EndRadioButton_CheckedChanged(object sender, EventArgs e)
        {
            if (EndRadioButton.Checked == true)
            {
                ProgAll.ProgramAllAtEnd = true;
            }
        }
    }
}

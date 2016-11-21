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
    public partial class MTKTestBDADialog : Form
    {
        private bool InitOnGoing = false;
        private MTKTestBDA BDAProg;

        public MTKTestBDADialog()
        {
            InitializeComponent();
            BDAProg = new MTKTestBDA();
        }

        public MTKTestBDADialog(MTKTestBDA BDAProgrammer) : this()
        {
            InitOnGoing = true;
            BDAProg = BDAProgrammer;
            BDATextBox.SetTextFromByteArray(BDAProg.BDAddress);
            UseProgrammerCheckBox.Checked = BDAProg.UseProgrammer;
            AutoIncrementCheckBox.Checked = BDAProg.AutoIncrementBDA;
            ProgrammerInfoLabel.Text = BDAProg.BDAProgrammer.GetDisplayText();
            InitOnGoing = false;
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            string temp = "0";
            for (int i = BDATextBox.GetTextWithoutDelimiters().Length; i < 12; i++)
            {
                if (i > BDATextBox.GetTextWithoutDelimiters().Length)
                    temp += "0";
            }
            if (temp.Length > 1)
                BDATextBox.Text = temp + BDATextBox.Text;

            BDAProg.BDAddress = BDATextBox.ToByteArray();
            BDAProg.UseProgrammer = UseProgrammerCheckBox.Checked;
            BDAProg.AutoIncrementBDA = AutoIncrementCheckBox.Checked;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void ProgrammerConfigButton_Click(object sender, EventArgs e)
        {
            if (!BDAProg.BDAProgrammer.PSoCProgrammerInstalled || !BDAProg.BDAProgrammer.IsCorrectVersion())
            {
                return;
            }

            MTKPSoCProgrammerDialog TempDialog = new MTKPSoCProgrammerDialog(BDAProg.BDAProgrammer);
            TempDialog.SetupForBDA();

            if (TempDialog.ShowDialog() == DialogResult.OK)
            {
                ProgrammerInfoLabel.Text = BDAProg.BDAProgrammer.GetDisplayText();
            }
        }

        private void UseProgrammerCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (UseProgrammerCheckBox.Checked == true)
            {
                if (InitOnGoing == false)
                {
                    DialogResult result = MessageBox.Show("Using programmer to write BD Address will erase the entire flash. Do you want to continue?", "Warning!", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);
                    if (result == DialogResult.Yes)
                    {
                        ProgrammerTableLayoutPanel.Enabled = true;
                    }
                    else
                    {
                        UseProgrammerCheckBox.Checked = false;
                    }
                }
                else
                {
                        ProgrammerTableLayoutPanel.Enabled = true;
                }
            }
            else
            {
                ProgrammerTableLayoutPanel.Enabled = false;
            }
        }
    }
}

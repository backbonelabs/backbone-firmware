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
    public partial class MTKTestXOCalDialog : Form
    {
        private MTKTestXOCalibration XOCal;

        public MTKTestXOCalDialog()
        {
            InitializeComponent();
        }

        public MTKTestXOCalDialog(MTKTestXOCalibration XOCalTest) : this()
        {
            XOCal = XOCalTest;
        }

        protected override void OnLoad(EventArgs e)
        {
            if (XOCal.ScriptPath.Length != 0)
            {
                ScriptPathTextBox.Text = XOCal.ScriptPath;
            }

            ErrorMarginNumeric.Value = XOCal.MarginOfError;
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            XOCal.ScriptPath = ScriptPathTextBox.Text;
            XOCal.MarginOfError = (int)ErrorMarginNumeric.Value;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void ScriptOpenButton_Click(object sender, EventArgs e)
        {
            EquipmentScriptInterpreter EquipSI = new EquipmentScriptInterpreter();;

            OpenFileDialog theDialog = new OpenFileDialog();
            theDialog.Title = "Open Script File";
            theDialog.Filter = "TXT files|*.txt|All Files|*.*";
            //theDialog.InitialDirectory = @"C:\";
            if (theDialog.ShowDialog() == DialogResult.OK)
            {
                EquipSI.ScriptFileName = theDialog.FileName;
                ScriptError retVal = EquipSI.ParseScriptFile();
                if (retVal == ScriptError.NoError)
                {
                    ScriptPathTextBox.Text = theDialog.FileName;
                }
                else
                {
                    string errorString = "Error at line " + EquipSI.LineCount.ToString() + ", column " + EquipSI.ColumnCount.ToString()  + ". \nError type: " + EquipSI.GetErrorString(retVal);
                    MessageBox.Show(errorString, "Error in Script");
                    ScriptPathTextBox.Text = "";
                }
            }
        }
    }
}

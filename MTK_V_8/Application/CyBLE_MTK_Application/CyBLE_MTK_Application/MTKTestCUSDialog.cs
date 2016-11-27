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
    public partial class MTKTestCUSDialog : Form
    {
        private MTKTestCUS CustomTest;
        public MTKTestCUSDialog()
        {
            InitializeComponent();
            CustomTest = new MTKTestCUS();
        }

        public MTKTestCUSDialog(MTKTestCUS Test) : this()
        {
            CustomTest = Test;
        }

        private void SetCombo(ComboBox CB2Set, CommandResultOperator ResultOper)
        {
            if (ResultOper == CommandResultOperator.Equal)
            {
                CB2Set.SelectedIndex = 0;
            }
            else if (ResultOper == CommandResultOperator.NotEqual)
            {
                CB2Set.SelectedIndex = 1;
            }
            else if (ResultOper == CommandResultOperator.Greater)
            {
                CB2Set.SelectedIndex = 2;
            }
            else if (ResultOper == CommandResultOperator.GreaterOrEqual)
            {
                CB2Set.SelectedIndex = 3;
            }
            else if (ResultOper == CommandResultOperator.Less)
            {
                CB2Set.SelectedIndex = 4;
            }
            else if (ResultOper == CommandResultOperator.LessOrEqual)
            {
                CB2Set.SelectedIndex = 5;
            }
        }

        protected override void OnLoad(EventArgs e)
        {
//            Byte1ComboBox.SelectedIndex = 0;
            NameTextBox.Text = CustomTest.CustomCommandName;
            byte[] temp = new byte[1];
            temp[0] = CustomTest.CustomCommand;
            CMDHexadecimalTextBox.SetTextFromByteArray(temp);

            if (CustomTest.CustomCommandParamNum >= 1)
            {
                temp[0] = CustomTest.CustomCommandParam[0];
                Param1HexadecimalTextBox.SetTextFromByteArray(temp);
                Param1CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandParamNum == 2)
            {
                temp[0] = CustomTest.CustomCommandParam[1];
                Param2HexadecimalTextBox.SetTextFromByteArray(temp);
                Param2CheckBox.Checked = true;
            }
            CMDDelayNumericUpDown.Value = (decimal)CustomTest.CommandDelay;

            if (CustomTest.CustomCommandResultNum == -1)
            {
                ValidateResultCheckBox.Checked = false;
            }

            if (CustomTest.CustomCommandResultNum >= 1)
            {
                SetCombo(Byte1ComboBox, CustomTest.ResultOperation[0]);
                temp[0] = CustomTest.CustomCommandResult[0];
                Byte1HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte1CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 2)
            {
                SetCombo(Byte2ComboBox, CustomTest.ResultOperation[1]);
                temp[0] = CustomTest.CustomCommandResult[1];
                Byte2HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte2CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 3)
            {
                SetCombo(Byte3ComboBox, CustomTest.ResultOperation[2]);
                temp[0] = CustomTest.CustomCommandResult[2];
                Byte3HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte3CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 4)
            {
                SetCombo(Byte4ComboBox, CustomTest.ResultOperation[3]);
                temp[0] = CustomTest.CustomCommandResult[3];
                Byte4HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte4CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 5)
            {
                SetCombo(Byte5ComboBox, CustomTest.ResultOperation[4]);
                temp[0] = CustomTest.CustomCommandResult[4];
                Byte5HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte5CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 6)
            {
                SetCombo(Byte6ComboBox, CustomTest.ResultOperation[5]);
                temp[0] = CustomTest.CustomCommandResult[5];
                Byte6HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte6CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 7)
            {
                SetCombo(Byte7ComboBox, CustomTest.ResultOperation[6]);
                temp[0] = CustomTest.CustomCommandResult[6];
                Byte7HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte7CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 8)
            {
                SetCombo(Byte8ComboBox, CustomTest.ResultOperation[7]);
                temp[0] = CustomTest.CustomCommandResult[7];
                Byte8HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte8CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 9)
            {
                SetCombo(Byte9ComboBox, CustomTest.ResultOperation[8]);
                temp[0] = CustomTest.CustomCommandResult[8];
                Byte9HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte9CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 10)
            {
                SetCombo(Byte10ComboBox, CustomTest.ResultOperation[9]);
                temp[0] = CustomTest.CustomCommandResult[9];
                Byte10HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte10CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 11)
            {
                SetCombo(Byte11ComboBox, CustomTest.ResultOperation[10]);
                temp[0] = CustomTest.CustomCommandResult[10];
                Byte11HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte11CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 12)
            {
                SetCombo(Byte12ComboBox, CustomTest.ResultOperation[11]);
                temp[0] = CustomTest.CustomCommandResult[11];
                Byte12HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte12CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 13)
            {
                SetCombo(Byte13ComboBox, CustomTest.ResultOperation[12]);
                temp[0] = CustomTest.CustomCommandResult[12];
                Byte13HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte13CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 14)
            {
                SetCombo(Byte14ComboBox, CustomTest.ResultOperation[13]);
                temp[0] = CustomTest.CustomCommandResult[13];
                Byte14HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte14CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 15)
            {
                SetCombo(Byte15ComboBox, CustomTest.ResultOperation[14]);
                temp[0] = CustomTest.CustomCommandResult[14];
                Byte15HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte15CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 16)
            {
                SetCombo(Byte16ComboBox, CustomTest.ResultOperation[15]);
                temp[0] = CustomTest.CustomCommandResult[15];
                Byte16HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte16CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 17)
            {
                SetCombo(Byte17ComboBox, CustomTest.ResultOperation[16]);
                temp[0] = CustomTest.CustomCommandResult[16];
                Byte17HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte17CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 18)
            {
                SetCombo(Byte18ComboBox, CustomTest.ResultOperation[17]);
                temp[0] = CustomTest.CustomCommandResult[17];
                Byte18HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte18CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum >= 19)
            {
                SetCombo(Byte19ComboBox, CustomTest.ResultOperation[18]);
                temp[0] = CustomTest.CustomCommandResult[18];
                Byte19HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte19CheckBox.Checked = true;
            }
            if (CustomTest.CustomCommandResultNum == 20)
            {
                SetCombo(Byte20ComboBox, CustomTest.ResultOperation[19]);
                temp[0] = CustomTest.CustomCommandResult[19];
                Byte20HexadecimalTextBox.SetTextFromByteArray(temp);
                Byte20CheckBox.Checked = true;
            }

            base.OnLoad(e);
        }

        private void Param1CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Param1HexadecimalTextBox.Enabled = Param1CheckBox.Checked;
            if (Param2CheckBox.Checked && (Param1CheckBox.Checked == false))
            {
                Param2CheckBox.Checked = false;
            }
            Param2CheckBox.Enabled = Param1CheckBox.Checked;
        }

        private void Param2CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Param2HexadecimalTextBox.Enabled = Param2CheckBox.Checked;
        }

        private void Byte1CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte1ComboBox.Enabled = Byte1CheckBox.Checked;
            Byte1HexadecimalTextBox.Enabled = Byte1CheckBox.Checked;
            if (Byte2CheckBox.Checked && (Byte1CheckBox.Checked == false))
            {
                Byte2CheckBox.Checked = false;
            }
            Byte2CheckBox.Enabled = Byte1CheckBox.Checked;
        }

        private void Byte2CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte2ComboBox.Enabled = Byte2CheckBox.Checked;
            Byte2HexadecimalTextBox.Enabled = Byte2CheckBox.Checked;
            if (Byte3CheckBox.Checked && (Byte2CheckBox.Checked == false))
            {
                Byte3CheckBox.Checked = false;
            }
            Byte3CheckBox.Enabled = Byte2CheckBox.Checked;
        }

        private void Byte3CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte3ComboBox.Enabled = Byte3CheckBox.Checked;
            Byte3HexadecimalTextBox.Enabled = Byte3CheckBox.Checked;
            if (Byte4CheckBox.Checked && (Byte3CheckBox.Checked == false))
            {
                Byte4CheckBox.Checked = false;
            }
            Byte4CheckBox.Enabled = Byte3CheckBox.Checked;
        }

        private void Byte4CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte4ComboBox.Enabled = Byte4CheckBox.Checked;
            Byte4HexadecimalTextBox.Enabled = Byte4CheckBox.Checked;
            if (Byte5CheckBox.Checked && (Byte4CheckBox.Checked == false))
            {
                Byte5CheckBox.Checked = false;
            }
            Byte5CheckBox.Enabled = Byte4CheckBox.Checked;
        }

        private void Byte5CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte5ComboBox.Enabled = Byte5CheckBox.Checked;
            Byte5HexadecimalTextBox.Enabled = Byte5CheckBox.Checked;
            if (Byte6CheckBox.Checked && (Byte5CheckBox.Checked == false))
            {
                Byte6CheckBox.Checked = false;
            }
            Byte6CheckBox.Enabled = Byte5CheckBox.Checked;
        }

        private void Byte6CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte6ComboBox.Enabled = Byte6CheckBox.Checked;
            Byte6HexadecimalTextBox.Enabled = Byte6CheckBox.Checked;
            if (Byte7CheckBox.Checked && (Byte6CheckBox.Checked == false))
            {
                Byte7CheckBox.Checked = false;
            }
            Byte7CheckBox.Enabled = Byte6CheckBox.Checked;
        }

        private void Byte7CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte7ComboBox.Enabled = Byte7CheckBox.Checked;
            Byte7HexadecimalTextBox.Enabled = Byte7CheckBox.Checked;
            if (Byte8CheckBox.Checked && (Byte7CheckBox.Checked == false))
            {
                Byte8CheckBox.Checked = false;
            }
            Byte8CheckBox.Enabled = Byte7CheckBox.Checked;
        }

        private void Byte8CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte8ComboBox.Enabled = Byte8CheckBox.Checked;
            Byte8HexadecimalTextBox.Enabled = Byte8CheckBox.Checked;
            if (Byte9CheckBox.Checked && (Byte8CheckBox.Checked == false))
            {
                Byte9CheckBox.Checked = false;
            }
            Byte9CheckBox.Enabled = Byte8CheckBox.Checked;
        }

        private void Byte9CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte9ComboBox.Enabled = Byte9CheckBox.Checked;
            Byte9HexadecimalTextBox.Enabled = Byte9CheckBox.Checked;
            if (Byte10CheckBox.Checked && (Byte9CheckBox.Checked == false))
            {
                Byte10CheckBox.Checked = false;
            }
            Byte10CheckBox.Enabled = Byte9CheckBox.Checked;
        }

        private void Byte10CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte10ComboBox.Enabled = Byte10CheckBox.Checked;
            Byte10HexadecimalTextBox.Enabled = Byte10CheckBox.Checked;
            if (Byte11CheckBox.Checked && (Byte10CheckBox.Checked == false))
            {
                Byte11CheckBox.Checked = false;
            }
            Byte11CheckBox.Enabled = Byte10CheckBox.Checked;
        }

        private void Byte11CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte11ComboBox.Enabled = Byte11CheckBox.Checked;
            Byte11HexadecimalTextBox.Enabled = Byte11CheckBox.Checked;
            if (Byte12CheckBox.Checked && (Byte11CheckBox.Checked == false))
            {
                Byte12CheckBox.Checked = false;
            }
            Byte12CheckBox.Enabled = Byte11CheckBox.Checked;
        }

        private void Byte12CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte12ComboBox.Enabled = Byte12CheckBox.Checked;
            Byte12HexadecimalTextBox.Enabled = Byte12CheckBox.Checked;
            if (Byte13CheckBox.Checked && (Byte12CheckBox.Checked == false))
            {
                Byte13CheckBox.Checked = false;
            }
            Byte13CheckBox.Enabled = Byte12CheckBox.Checked;
        }

        private void Byte13CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte13ComboBox.Enabled = Byte13CheckBox.Checked;
            Byte13HexadecimalTextBox.Enabled = Byte13CheckBox.Checked;
            if (Byte14CheckBox.Checked && (Byte13CheckBox.Checked == false))
            {
                Byte14CheckBox.Checked = false;
            }
            Byte14CheckBox.Enabled = Byte13CheckBox.Checked;
        }

        private void Byte14CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte14ComboBox.Enabled = Byte14CheckBox.Checked;
            Byte14HexadecimalTextBox.Enabled = Byte14CheckBox.Checked;
            if (Byte15CheckBox.Checked && (Byte14CheckBox.Checked == false))
            {
                Byte15CheckBox.Checked = false;
            }
            Byte15CheckBox.Enabled = Byte14CheckBox.Checked;
        }

        private void Byte15CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte15ComboBox.Enabled = Byte15CheckBox.Checked;
            Byte15HexadecimalTextBox.Enabled = Byte15CheckBox.Checked;
            if (Byte16CheckBox.Checked && (Byte15CheckBox.Checked == false))
            {
                Byte16CheckBox.Checked = false;
            }
            Byte16CheckBox.Enabled = Byte15CheckBox.Checked;
        }

        private void Byte16CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte16ComboBox.Enabled = Byte16CheckBox.Checked;
            Byte16HexadecimalTextBox.Enabled = Byte16CheckBox.Checked;
            if (Byte17CheckBox.Checked && (Byte16CheckBox.Checked == false))
            {
                Byte17CheckBox.Checked = false;
            }
            Byte17CheckBox.Enabled = Byte16CheckBox.Checked;
        }

        private void Byte17CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte17ComboBox.Enabled = Byte17CheckBox.Checked;
            Byte17HexadecimalTextBox.Enabled = Byte17CheckBox.Checked;
            if (Byte18CheckBox.Checked && (Byte17CheckBox.Checked == false))
            {
                Byte18CheckBox.Checked = false;
            }
            Byte18CheckBox.Enabled = Byte17CheckBox.Checked;
        }

        private void Byte18CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte18ComboBox.Enabled = Byte18CheckBox.Checked;
            Byte18HexadecimalTextBox.Enabled = Byte18CheckBox.Checked;
            if (Byte19CheckBox.Checked && (Byte18CheckBox.Checked == false))
            {
                Byte19CheckBox.Checked = false;
            }
            Byte19CheckBox.Enabled = Byte18CheckBox.Checked;
        }

        private void Byte19CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte19ComboBox.Enabled = Byte19CheckBox.Checked;
            Byte19HexadecimalTextBox.Enabled = Byte19CheckBox.Checked;
            if (Byte20CheckBox.Checked && (Byte19CheckBox.Checked == false))
            {
                Byte20CheckBox.Checked = false;
            }
            Byte20CheckBox.Enabled = Byte19CheckBox.Checked;
        }

        private void Byte20CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Byte20ComboBox.Enabled = Byte20CheckBox.Checked;
            Byte20HexadecimalTextBox.Enabled = Byte20CheckBox.Checked;
        }

        private CommandResultOperator GetOperator(ComboBox CB2Oper)
        {
            if (CB2Oper.Text == "==")
            {
                return CommandResultOperator.Equal;
            }
            else if (CB2Oper.Text == "!=")
            {
                return CommandResultOperator.NotEqual;
            }
            else if (CB2Oper.Text == ">")
            {
                return CommandResultOperator.Greater;
            }
            else if (CB2Oper.Text == ">=")
            {
                return CommandResultOperator.GreaterOrEqual;
            }
            else if (CB2Oper.Text == "<")
            {
                return CommandResultOperator.Less;
            }
            else if (CB2Oper.Text == "<=")
            {
                return CommandResultOperator.LessOrEqual;
            }

            return CommandResultOperator.NoOperator;
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            if (NameTextBox.Text == "")
            {
                MessageBox.Show("'Test Name' cannot be empty.", "Empty Field", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }
            CustomTest.CustomCommandName = NameTextBox.Text;
            CustomTest.CustomCommand = (CMDHexadecimalTextBox.ToByteArray())[0];

            if (Param1CheckBox.Checked && (Param1HexadecimalTextBox.Text != ""))
            {
                CustomTest.CustomCommandParam[0] = (Param1HexadecimalTextBox.ToByteArray())[0];
                CustomTest.CustomCommandParamNum = 1;
            }
            else
            {
                CustomTest.CustomCommandParamNum = 0;
                CustomTest.CustomCommandParam[0] = 0x00;
                CustomTest.CustomCommandParam[1] = 0x00;
            }

            if (Param2CheckBox.Checked && (Param2HexadecimalTextBox.Text != ""))
            {
                CustomTest.CustomCommandParam[1] = (Param2HexadecimalTextBox.ToByteArray())[0];
                CustomTest.CustomCommandParamNum = 2;
            }

            CustomTest.CommandDelay = (int)CMDDelayNumericUpDown.Value;

            CustomTest.CustomCommandResultNum = 0;
            for (int i = 0; i < 20; i++)
            {
                CustomTest.CustomCommandResult[i] = 0x00;
                CustomTest.ResultOperation[i] = CommandResultOperator.NoOperator;
            }

            if (ValidateResultCheckBox.Checked == false)
            {
                CustomTest.CustomCommandResultNum = -1;
            }
            else
            {
                if (Byte1CheckBox.Checked && (Byte1HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[0] = (Byte1HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[0] = GetOperator(Byte1ComboBox);
                    CustomTest.CustomCommandResultNum = 1;
                }

                if (Byte2CheckBox.Checked && (Byte2HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[1] = (Byte2HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[1] = GetOperator(Byte2ComboBox);
                    CustomTest.CustomCommandResultNum = 2;
                }

                if (Byte3CheckBox.Checked && (Byte3HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[2] = (Byte3HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[2] = GetOperator(Byte3ComboBox);
                    CustomTest.CustomCommandResultNum = 3;
                }

                if (Byte4CheckBox.Checked && (Byte4HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[3] = (Byte4HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[3] = GetOperator(Byte4ComboBox);
                    CustomTest.CustomCommandResultNum = 4;
                }

                if (Byte5CheckBox.Checked && (Byte5HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[4] = (Byte5HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[4] = GetOperator(Byte5ComboBox);
                    CustomTest.CustomCommandResultNum = 5;
                }

                if (Byte6CheckBox.Checked && (Byte6HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[5] = (Byte6HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[5] = GetOperator(Byte6ComboBox);
                    CustomTest.CustomCommandResultNum = 6;
                }

                if (Byte7CheckBox.Checked && (Byte7HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[6] = (Byte7HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[6] = GetOperator(Byte7ComboBox);
                    CustomTest.CustomCommandResultNum = 7;
                }

                if (Byte8CheckBox.Checked && (Byte8HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[7] = (Byte8HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[7] = GetOperator(Byte8ComboBox);
                    CustomTest.CustomCommandResultNum = 8;
                }

                if (Byte9CheckBox.Checked && (Byte9HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[8] = (Byte9HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[8] = GetOperator(Byte9ComboBox);
                    CustomTest.CustomCommandResultNum = 9;
                }

                if (Byte10CheckBox.Checked && (Byte10HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[9] = (Byte10HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[9] = GetOperator(Byte10ComboBox);
                    CustomTest.CustomCommandResultNum = 10;
                }

                if (Byte11CheckBox.Checked && (Byte11HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[10] = (Byte11HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[10] = GetOperator(Byte11ComboBox);
                    CustomTest.CustomCommandResultNum = 11;
                }

                if (Byte12CheckBox.Checked && (Byte12HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[11] = (Byte12HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[11] = GetOperator(Byte12ComboBox);
                    CustomTest.CustomCommandResultNum = 12;
                }

                if (Byte13CheckBox.Checked && (Byte13HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[12] = (Byte13HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[12] = GetOperator(Byte13ComboBox);
                    CustomTest.CustomCommandResultNum = 13;
                }

                if (Byte14CheckBox.Checked && (Byte14HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[13] = (Byte14HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[13] = GetOperator(Byte14ComboBox);
                    CustomTest.CustomCommandResultNum = 14;
                }

                if (Byte15CheckBox.Checked && (Byte15HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[14] = (Byte15HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[14] = GetOperator(Byte15ComboBox);
                    CustomTest.CustomCommandResultNum = 15;
                }

                if (Byte16CheckBox.Checked && (Byte16HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[15] = (Byte16HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[15] = GetOperator(Byte16ComboBox);
                    CustomTest.CustomCommandResultNum = 16;
                }

                if (Byte17CheckBox.Checked && (Byte17HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[16] = (Byte17HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[16] = GetOperator(Byte17ComboBox);
                    CustomTest.CustomCommandResultNum = 17;
                }

                if (Byte18CheckBox.Checked && (Byte18HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[17] = (Byte18HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[17] = GetOperator(Byte18ComboBox);
                    CustomTest.CustomCommandResultNum = 18;
                }

                if (Byte19CheckBox.Checked && (Byte19HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[18] = (Byte19HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[18] = GetOperator(Byte19ComboBox);
                    CustomTest.CustomCommandResultNum = 19;
                }

                if (Byte20CheckBox.Checked && (Byte20HexadecimalTextBox.Text != ""))
                {
                    CustomTest.CustomCommandResult[19] = (Byte20HexadecimalTextBox.ToByteArray())[0];
                    CustomTest.ResultOperation[19] = GetOperator(Byte20ComboBox);
                    CustomTest.CustomCommandResultNum = 20;
                }
            }

            DialogResult = DialogResult.OK;
        }

        private void ValidateResultCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (ValidateResultCheckBox.Checked == false)
            {
                ((Control)tabPage2).Enabled = false;
            }
            else
            {
                ((Control)tabPage2).Enabled = true;
            }
        }
    }
}

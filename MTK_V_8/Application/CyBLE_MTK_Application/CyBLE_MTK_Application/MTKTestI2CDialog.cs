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
    public partial class MTKTestI2CDialog : Form
    {
        private MTKTestI2C I2CTests;

        private List <UnitI2CTest> I2CTestArray;
        private UnitI2CTest I2CTest;

        public MTKTestI2CDialog()
        {
            InitializeComponent();
            I2CTests = new MTKTestI2C();
            I2CTest = new UnitI2CTest();
            I2CTestArray = new List<UnitI2CTest>();
        }

        public MTKTestI2CDialog(MTKTestI2C Tests) : this()
        {
            I2CTests = Tests;
        }

        protected override void OnLoad(EventArgs e)
        {
            if (I2CTests.I2CTests.Count() > 0)
            {
                I2CTestArray = I2CTests.I2CTests.ToList<UnitI2CTest>();
                RefreshTable();
                EnableButtons();
            }

            base.OnLoad(e);
        }

        private void AddressHexadecimalTextBox_TextChanged(object sender, EventArgs e)
        {
            byte[] temp = AddressHexadecimalTextBox.ToByteArray();
            if (temp.Count() > 0)
            {
                I2CTest.Address = Convert.ToInt32(temp[0]);
            }
        }

        private void ActionComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if ((string)ActionComboBox.SelectedItem == "Read")
            {
                I2CTest.Action = MTKI2CTestType.Read;
            }
            else if ((string)ActionComboBox.SelectedItem == "Write")
            {
                I2CTest.Action = MTKI2CTestType.Write;
            }
        }

        private void DataHexadecimalTextBox_TextChanged(object sender, EventArgs e)
        {
            I2CTest.DataBuffer = DataHexadecimalTextBox.ToByteArray();
        }

        private void VerifyCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            I2CTest.ValidateRxData = VerifyCheckBox.Checked;
        }

        private void AddButton_Click(object sender, EventArgs e)
        {
            DisableButtons();

            byte[] temp = AddressHexadecimalTextBox.ToByteArray();
            I2CTest.Address = Convert.ToInt32(temp[0]);

            if ((string)ActionComboBox.SelectedItem == "Read")
            {
                I2CTest.Action = MTKI2CTestType.Read;
            }
            else if ((string)ActionComboBox.SelectedItem == "Write")
            {
                I2CTest.Action = MTKI2CTestType.Write;
            }

            I2CTest.DataBuffer = DataHexadecimalTextBox.ToByteArray();
            I2CTest.NumRxBytes = I2CTest.DataBuffer.Count();

            I2CTest.ValidateRxData = VerifyCheckBox.Checked;

            if ((I2CTest.Action != MTKI2CTestType.Write) && (I2CTest.Action != MTKI2CTestType.Read))
            {
                MessageBox.Show("Action not configured correctly", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            I2CTestArray.Add(I2CTest);
            I2CTest = new UnitI2CTest();
            RefreshTable();

            EnableButtons();
        }

        private void RemoveButton_Click(object sender, EventArgs e)
        {
            DisableButtons();
            I2CTestArray.RemoveAt(I2CTestsDataGridView.CurrentRow.Index);
            RefreshTable();
            EnableButtons();
        }

        private void RefreshTable()
        {
            DataTable myTable;
            DataColumn colItem1, colItem2, colItem3, colItem4, colItem5;
            DataRow NewRow;
            DataView myView;

            // DataTable to hold data that is displayed in DataGrid
            myTable = new DataTable("myTable");

            // the three columns in the table
            colItem1 = new DataColumn("#", Type.GetType("System.Int32"));
            colItem2 = new DataColumn("Address", Type.GetType("System.String"));
            colItem3 = new DataColumn("Action", Type.GetType("System.String"));
            colItem4 = new DataColumn("Data", Type.GetType("System.String"));
            colItem5 = new DataColumn("Verify", Type.GetType("System.String"));

            // add the columns to the table
            myTable.Columns.Add(colItem1);
            myTable.Columns.Add(colItem2);
            myTable.Columns.Add(colItem3);
            myTable.Columns.Add(colItem4);
            myTable.Columns.Add(colItem5);

            // Fill in some data
            for (int i = 0; i < I2CTestArray.Count; i++)
            {
                NewRow = myTable.NewRow();
                NewRow[0] = i + 1;
                NewRow[1] = I2CTestArray[i].Address.ToString("x2").ToUpper();
                NewRow[2] = I2CTestArray[i].Action;
                StringBuilder hex = new StringBuilder(I2CTestArray[i].DataBuffer.Length * 2);
                foreach (byte b in I2CTestArray[i].DataBuffer)
                    hex.AppendFormat("{0:x2}", b);
                NewRow[3] = hex.ToString().ToUpper();
                NewRow[4] = (I2CTestArray[i].ValidateRxData) ? "YES" : "NO";
                myTable.Rows.Add(NewRow);
            }

            // DataView for the DataGridView
            myView = new DataView(myTable);
            myView.AllowDelete = false;
            myView.AllowEdit = true;
            myView.AllowNew = false;

            // Assign DataView to DataGrid
            I2CTestsDataGridView.DataSource = myView;
            //column.Resizable = DataGridViewTriState.False;
            I2CTestsDataGridView.Columns[0].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[0].HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[0].ReadOnly = true;
            I2CTestsDataGridView.Columns[0].SortMode = DataGridViewColumnSortMode.NotSortable;

            I2CTestsDataGridView.Columns[1].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[1].HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[1].ReadOnly = true;
            I2CTestsDataGridView.Columns[1].SortMode = DataGridViewColumnSortMode.NotSortable;

            I2CTestsDataGridView.Columns[2].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[2].HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[2].ReadOnly = true;
            I2CTestsDataGridView.Columns[2].SortMode = DataGridViewColumnSortMode.NotSortable;

            //I2CTestsDataGridView.Columns[3].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[3].HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[3].ReadOnly = true;
            I2CTestsDataGridView.Columns[3].SortMode = DataGridViewColumnSortMode.NotSortable;

            I2CTestsDataGridView.Columns[4].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[4].HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            I2CTestsDataGridView.Columns[4].ReadOnly = true;
            I2CTestsDataGridView.Columns[4].SortMode = DataGridViewColumnSortMode.NotSortable;
        }

        private void DisableButtons()
        {
            AddButton.Enabled = false;
            RemoveButton.Enabled = false;
            UpButton.Enabled = false;
            DownButton.Enabled = false;
        }

        private void EnableButtons()
        {
            AddButton.Enabled = true;
            RemoveButton.Enabled = true;
            UpButton.Enabled = true;
            DownButton.Enabled = true;
        }

        private void UpButton_Click(object sender, EventArgs e)
        {
            DisableButtons();
            if (I2CTestsDataGridView.CurrentRow.Index > 0)
            {
                int SelectIndex = I2CTestsDataGridView.CurrentRow.Index - 1;
                UnitI2CTest temp = I2CTestArray[I2CTestsDataGridView.CurrentRow.Index];
                I2CTestArray.RemoveAt(I2CTestsDataGridView.CurrentRow.Index);
                I2CTestArray.Insert(I2CTestsDataGridView.CurrentRow.Index - 1, temp);
                RefreshTable();
                //I2CTestsDataGridView.row
                I2CTestsDataGridView.ClearSelection();
                int i = 0;
                foreach (DataGridViewRow row in I2CTestsDataGridView.Rows)
                {
                    if (i == SelectIndex)
                    {
                        row.Selected = true;
                        I2CTestsDataGridView.CurrentCell = row.Cells[0];
                        break;
                    }
                    i++;
                }
            }
            EnableButtons();
        }

        private void DownButton_Click(object sender, EventArgs e)
        {
            DisableButtons();
            if (I2CTestsDataGridView.CurrentRow.Index < (I2CTestArray.Count -1))
            {
                int SelectIndex = I2CTestsDataGridView.CurrentRow.Index + 1;
                UnitI2CTest temp = I2CTestArray[I2CTestsDataGridView.CurrentRow.Index];
                I2CTestArray.RemoveAt(I2CTestsDataGridView.CurrentRow.Index);
                I2CTestArray.Insert(I2CTestsDataGridView.CurrentRow.Index + 1, temp);
                RefreshTable();
                I2CTestsDataGridView.ClearSelection();
                int i = 0;
                foreach (DataGridViewRow row in I2CTestsDataGridView.Rows)
                {
                    if (i == SelectIndex)
                    {
                        row.Selected = true;
                        I2CTestsDataGridView.CurrentCell = row.Cells[0];
                        break;
                    }
                    i++;
                }
            }
            EnableButtons();
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            I2CTests.I2CTests = I2CTestArray.ToArray();
            DialogResult = DialogResult.OK;
            this.Close();
        }
    }
}

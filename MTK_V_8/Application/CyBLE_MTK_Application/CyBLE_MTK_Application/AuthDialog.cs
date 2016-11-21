using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Security;
using System.Runtime;
using System.Runtime.InteropServices;
using System.Security.Cryptography;

namespace CyBLE_MTK_Application
{
    public partial class AuthDialog : Form
    {
        private LogManager Log;
        public bool Authorized;
        private bool DontClose;

        public AuthDialog()
        {
            InitializeComponent();
            Authorized = false;
            DontClose = false;
            Log = new LogManager();
        }

        public AuthDialog(LogManager Logger) : this()
        {
            Log = Logger;
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            Log.PrintLog(this, "Authentication cancelled.", LogDetailLevel.LogEverything);
            this.Close();
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            string temp = "";

            if (CyBLE_MTK_Application.Properties.Settings.Default.Password == "null")
            {
                temp = "";
            }
            else
            {
                byte[] passtext = ProtectedData.Unprotect(Convert.FromBase64String(CyBLE_MTK_Application.Properties.Settings.Default.Password), null, DataProtectionScope.CurrentUser);
                temp = ChangePasswordDialog.GetString(passtext);
            }

            if (temp == PasswordTextBox.Text)
            {
                Log.PrintLog(this, "Password authentication successful.", LogDetailLevel.LogEverything);
                Authorized = true;
                PasswordTextBox.Text = "";
            }
            else
            {
                Log.PrintLog(this, "Password authentication failed.", LogDetailLevel.LogEverything);
                MessageBox.Show("Wrong password!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                DontClose = true;
            }
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (DontClose == true)
            {
                DontClose = false;
                this.PasswordTextBox.SelectAll();
                e.Cancel = true;
            }
            else
            {
                PasswordTextBox.Text = "";
            }
            base.OnFormClosing(e);
        }

        protected override void OnShown(EventArgs e)
        {
            PasswordTextBox.Focus();
            base.OnShown(e);
        }
    }
}

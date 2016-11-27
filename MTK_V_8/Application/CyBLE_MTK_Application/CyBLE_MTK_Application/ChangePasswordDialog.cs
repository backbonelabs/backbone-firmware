using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Security.Cryptography;

namespace CyBLE_MTK_Application
{
    public partial class ChangePasswordDialog : Form
    {
        private LogManager Log;
        public bool PasswordChanged;

        public ChangePasswordDialog()
        {
            InitializeComponent();
            PasswordChanged = false;
            Log = new LogManager();
        }

        public ChangePasswordDialog(LogManager Logger) : this()
        {
            Log = Logger;
        }

        private void OKButton(object sender, EventArgs e)
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
            if (PasswordText.Text == temp)
            {
                if (NewPassword.Text == ReEnterNewPassword.Text)
                {
                    byte[] ciphertext = ProtectedData.Protect(GetBytes(ReEnterNewPassword.Text), null, DataProtectionScope.CurrentUser);
                    CyBLE_MTK_Application.Properties.Settings.Default.Password = Convert.ToBase64String(ciphertext);
                    CyBLE_MTK_Application.Properties.Settings.Default.Save();
                    Log.PrintLog(this, "Password successfully changed.", LogDetailLevel.LogRelevant);
                    MessageBox.Show("Password successfully changed!", "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    PasswordChanged = true;
                    this.Close();
                }
                else
                {
                    Log.PrintLog(this, "Password not changed! Re-entered password doesnot match.", LogDetailLevel.LogEverything);
                    MessageBox.Show("Password not changed! Re-entered password doesnot match.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                Log.PrintLog(this, "Password not changed! Wrong password.", LogDetailLevel.LogEverything);
                MessageBox.Show("Password not changed!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        public static byte[] GetBytes(string str)
        {
            byte[] bytes = new byte[str.Length * sizeof(char)];
            System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }

        public static string GetString(byte[] bytes)
        {
            char[] chars = new char[bytes.Length / sizeof(char)];
            System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
            return new string(chars);
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            Log.PrintLog(this, "Password change cancelled.", LogDetailLevel.LogEverything);
            this.Close();
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            PasswordText.Text = "";
            NewPassword.Text = "";
            ReEnterNewPassword.Text = "";
            base.OnFormClosing(e);
        }

        protected override void OnShown(EventArgs e)
        {
            PasswordText.Focus();
            base.OnShown(e);
        }
    }
}

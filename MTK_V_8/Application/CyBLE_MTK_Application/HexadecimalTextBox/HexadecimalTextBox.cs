using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HexadecimalTextBoxControl
{
    /// <summary>
    /// A TextBox that only allows digits to be entered.
    /// </summary>
    [Description("A TextBox that only allows digits to be entered.")]
    public class HexadecimalTextBox : TextBox
    {
        #region Enums

        /// <summary>
        /// A enum of reasons why a paste was rejected.
        /// </summary>
        public enum PasteRejectReasons
        {
            Unknown = 0,
            NoData,
            InvalidCharacter,
            Accepted
        }

        #endregion

        #region Constants

        /// <summary>
        /// Windows message that is sent when a paste event occurs.
        /// </summary>
        public const int WM_PASTE = 0x0302;

        #endregion

        #region Events

        /// <summary>
        /// Occurs whenever a KeyDown event is suppressed.
        /// </summary>
        [Category("Behavior"),
        Description("Occurs whenever a KeyDown event is suppressed.")]
        public event EventHandler<KeyRejectedEventArgs> KeyRejected;

        /// <summary>
        /// Occurs whenever a paste attempt is suppressed.
        /// </summary>
        [Category("Behavior"),
        Description("Occurs whenever a paste attempt is suppressed.")]
        public event EventHandler<PasteEventArgs> PasteRejected;

        #endregion

        #region Private Variables

        /// <summary>
        /// The last Text of the control.
        /// </summary>
//        private string defaultText;

        /// <summary>
        /// Flag for zero padding.
        /// </summary>
        //private bool padded;

        ///// <summary>
        ///// Flag for init condition.
        ///// </summary>
        //private bool init;

        /// <summary>
        /// Flag for length.
        /// </summary>
        //private bool length_adjust;

        /// <summary>
        /// Store previous length of text.
        /// </summary>
        //private int previous_length;
        private int CurrentPosition;
        private int NumVisit;

        #endregion

        #region Properties

        [DefaultValue(false),
        Category("Appearance"),
        Browsable(true),
        DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        protected string _delimiter;
        public string Delimiter
        {
            get { return _delimiter; }
            set 
            { 
                string tempText = RemoveDelimiters(Text);
                _delimiter = value;
                Text = AddDelimiters(tempText);
                this.Select(Text.Length, 0);
            }
        }

        // New Text property to enable a new default value.
        /// <summary>
        /// Gets or sets the text.
        /// </summary>
        [DefaultValue(""),
        Category("Appearance"),
        Browsable(true),
        DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public override string Text
        {
            get { return base.Text; }
            set { base.Text = value; }
        }

        // New HorizontalAlignment property to enable a new default value.
        /// <summary>
        /// Gets or sets the horizontal alignment of the text.
        /// </summary>
        [DefaultValue(typeof(HorizontalAlignment), "Right"),
        Category("Appearance")]
        public new HorizontalAlignment TextAlign
        {
            get { return base.TextAlign; }
            set { base.TextAlign = value; }
        }

        #endregion

        #region Constructor

        /// <summary>
        /// Creates a new instance of the NumericTextBox
        /// </summary>
        public HexadecimalTextBox()
        {
            TextAlign = HorizontalAlignment.Center;
            _delimiter = "";
            Text = "";
            NumVisit = 0;
        }

        #endregion

        #region Overridden Methods

        /// <summary>
        /// Raises the OnGotFocus event.
        /// </summary>
        /// <param name="e">A System.EventArgs that contains the event data.</param>
        protected override void OnGotFocus(EventArgs e)
        {
            // Select all text everytime control gets focus.
            //SelectAll();
            base.OnGotFocus(e);
        }

        /// <summary>
        /// Raises the KeyDownEvent.
        /// </summary>
        /// <param name="e">A System.Windows.Forms.KeyEventArgs that contains the event data.</param>
        protected override void OnKeyDown(KeyEventArgs e)
        {
            bool result = true;

            //if ((padded == true) && (TextLength == MaxLength))
            //{
            //    MaxLength += 1;
            //    length_adjust = true;
            //}

            bool numericKeys = (
                ((e.KeyCode >= Keys.D0 && e.KeyCode <= Keys.D9) ||
                (e.KeyCode >= Keys.NumPad0 && e.KeyCode <= Keys.NumPad9))
                && e.Modifiers != Keys.Shift);

            bool hexadecimalKeys = ((e.KeyCode == Keys.A) || (e.KeyCode == Keys.B) ||
                                    (e.KeyCode == Keys.C) || (e.KeyCode == Keys.D) ||
                                    (e.KeyCode == Keys.E) || (e.KeyCode == Keys.F));

            bool ctrlA = e.KeyCode == Keys.A && e.Modifiers == Keys.Control;

            bool editKeys = (
                (e.KeyCode == Keys.Z && e.Modifiers == Keys.Control) ||
                (e.KeyCode == Keys.X && e.Modifiers == Keys.Control) ||
                (e.KeyCode == Keys.C && e.Modifiers == Keys.Control) ||
                (e.KeyCode == Keys.V && e.Modifiers == Keys.Control) ||
                e.KeyCode == Keys.Delete ||// e.KeyCode == Keys.Space ||
                e.KeyCode == Keys.Back);

            bool navigationKeys = (
                e.KeyCode == Keys.Up ||
                e.KeyCode == Keys.Right ||
                e.KeyCode == Keys.Down ||
                e.KeyCode == Keys.Left ||
                e.KeyCode == Keys.Home ||
                e.KeyCode == Keys.End);

            if ((e.KeyCode == Keys.A) && (e.Modifiers == Keys.Control))
            {
                ctrlA = true;
                SelectAll();
            }

            if (!(numericKeys || hexadecimalKeys || editKeys || navigationKeys))
            {
                if (ctrlA) // Do select all as OS/Framework does not always seem to implement this.
                    SelectAll();
                result = false;
            }
            if (!result) // If not valid key then suppress and handle.
            {
                e.SuppressKeyPress = true;
                e.Handled = true;
                if (ctrlA) { } // Do Nothing!
                else
                    OnKeyRejected(new KeyRejectedEventArgs(e.KeyCode));
            }
            else
                base.OnKeyDown(e);
        }

        /// <summary>
        /// Raises the LostFocus event.
        /// </summary>
        /// <param name="e">Event arguments.</param>
        protected override void OnLostFocus(EventArgs e)
        {
            if (string.IsNullOrEmpty(Text))
                Text = "";//defaultText;
            base.OnLostFocus(e);
        }

        /// <summary>
        /// Raises the TextChanged event.
        /// </summary>
        /// <param name="e">A System.EventArgs that contains the event data.</param>
        protected override void OnTextChanged(EventArgs e)
        {
            NumVisit++;
            if (string.IsNullOrEmpty(Text))
            {
                Text = "";
                base.OnTextChanged(e);
                return;
            }

            string cleanText = RemoveDelimiters(Text);

            foreach (char c in cleanText) // Check for any non digit characters.
            {
                bool IsHex = ((c == 'a') || (c == 'A') || (c == 'b') || (c == 'B') ||
                            (c == 'c') || (c == 'C') || (c == 'd') || (c == 'D') ||
                            (c == 'e') || (c == 'E') || (c == 'f') || (c == 'F'));
                if (!char.IsDigit(c) && !IsHex)
                {
                    Text = "";
                    base.OnTextChanged(e);
                    return;
                }
            }

            Text = AddDelimiters(cleanText);

            base.OnTextChanged(e);
            this.Select(Text.Length, 0);
            NumVisit = 0;
        }

        /// <summary>
        /// Receives Windows messages to optionally process.
        /// </summary>
        /// <param name="m">The Windows message to process.</param>
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == WM_PASTE)
            {
                PasteEventArgs e = CheckPasteValid();
                if (e.RejectReason != PasteRejectReasons.Accepted)
                {
                    m.Result = IntPtr.Zero;
                    OnPasteRejected(e);
                    return;
                }
            }
            base.WndProc(ref m);
        }

        #endregion

        #region Public Methods
        /// <summary>
        /// Receives Windows messages to optionally process.
        /// </summary>
        /// <param name="m">The Windows message to process.</param>
        public byte[] ToByteArray()
        {
            int count = 0;
            string cleanText = RemoveDelimiters(Text);

            if ((cleanText.Length % 2) != 0)
            {
                cleanText = "0" + cleanText;
            }

            byte[] data = new byte[cleanText.Length / 2];
            for (int i = 0; i < cleanText.Length; i += 2)
            {
                string Char2Convert = cleanText.Substring(i, 2);
                data[count++] = Convert.ToByte(Char2Convert, 16);
            }
            return data;
        }

        public void SetTextFromByteArray(byte[] ByteArray)
        {
            StringBuilder hex = new StringBuilder(ByteArray.Length * 2);
            foreach (byte b in ByteArray)
                hex.AppendFormat("{0:x2}", b);
            Text = hex.ToString().ToUpper();
            this.Select(Text.Length, 0);
        }

        public string GetTextWithoutDelimiters()
        {
            int index;
            string cleanText = Text;

            if (_delimiter != "")
            {
                do
                {
                    index = cleanText.IndexOf(_delimiter);
                    if (index >= 0)
                    {
                        cleanText = cleanText.Remove(index, _delimiter.Length);
                    }
                } while (index != -1);
            }
            return cleanText;
        }
        #endregion

        #region Private Methods

        /// <summary>
        /// Checks if the data on the clipboard contains text that is valid.
        /// </summary>
        /// <returns>A PasteEventArgs instance containing the relevant data.</returns>
        private PasteEventArgs CheckPasteValid()
        {
            // Default values.
            PasteRejectReasons rejectReason = PasteRejectReasons.Accepted;
            string originalText = Text;
            string clipboardText = string.Empty;
            string textResult = string.Empty;

            try
            {
                clipboardText = Clipboard.GetText(TextDataFormat.Text);
                if (clipboardText.Length > 0) // Does clipboard contain text?
                {
                    // Store text value as it will be post paste assuming it is valid.
                    textResult = (
                        Text.Remove(SelectionStart, SelectionLength).Insert(SelectionStart, clipboardText));
                    foreach (char c in clipboardText) // Check for any non digit characters.
                    {
                        bool IsHex = ((c == 'a') || (c == 'A') || (c == 'b') || (c == 'B') ||
                                    (c == 'c') || (c == 'C') || (c == 'd') || (c == 'D') ||
                                    (c == 'e') || (c == 'E') || (c == 'f') || (c == 'F'));
                        if (!char.IsDigit(c) && !IsHex)
                        {
                            rejectReason = PasteRejectReasons.InvalidCharacter;
                            break;
                        }
                    }
                }
                else
                    rejectReason = PasteRejectReasons.NoData;
            }
            catch
            {
                rejectReason = PasteRejectReasons.Unknown;
            }
            return new PasteEventArgs(originalText, clipboardText, textResult, rejectReason);
        }

        string RemoveDelimiters(string Input)
        {
            int index;
            string cleanText = Text;

            if (NumVisit < 2)
            {
                CurrentPosition = this.SelectionStart;
            }

            if (_delimiter != "")
            {
                do
                {
                    index = cleanText.IndexOf(_delimiter);
                    if (index >= 0)
                    {
                        cleanText = cleanText.Remove(index, _delimiter.Length);
                        CurrentPosition -= _delimiter.Length;
                    }
                } while (index != -1);
            }
            return cleanText;
        }

        string AddDelimiters(string Input)
        {
            if (_delimiter != "")
            {
                var FinalBuilder = new StringBuilder();
                int CharCount = 0;
                foreach (var c in Input)
                {
                    FinalBuilder.Append(c);
                    CharCount++;
                    if ((CharCount % 2 == 0) && (CharCount < Input.Length))
                    {
                        FinalBuilder.Append(_delimiter);
                        CurrentPosition += _delimiter.Length;
                    }
                }
                Input = FinalBuilder.ToString().ToUpper();
            }

            if (CurrentPosition < 0)
            {
                CurrentPosition = 0;
            }

            return Input;
        }

        #endregion

        #region Event Raise Methods

        /// <summary>
        /// Raises the KeyRejected event.
        /// </summary>
        /// <param name="e">Event arguments.</param>
        protected virtual void OnKeyRejected(KeyRejectedEventArgs e)
        {
            EventHandler<KeyRejectedEventArgs> handler = KeyRejected;
            if (handler != null)
                handler(this, e);
        }

        /// <summary>
        /// Raises the PasteRejected event.
        /// </summary>
        /// <param name="e">Event arguments.</param>
        protected virtual void OnPasteRejected(PasteEventArgs e)
        {
            EventHandler<PasteEventArgs> handler = PasteRejected;
            if (handler != null)
                handler(this, e);
        }

        #endregion

        #region EventArg Classes

        /// <summary>
        /// Event arguments class for the KeyRejected event.
        /// </summary>
        public class KeyRejectedEventArgs : EventArgs
        {
            #region Private Variables

            private Keys m_Key;

            #endregion

            #region Properties

            /// <summary>
            /// Gets the rejected key.
            /// </summary>
            [ReadOnly(true)]
            public Keys Key
            {
                get { return m_Key; }
            }

            #endregion

            #region Constructor

            /// <summary>
            /// Creates a new instance of the KeyRejectedEventArgs class.
            /// </summary>
            /// <param name="key">The rejected key.</param>
            public KeyRejectedEventArgs(Keys key)
            {
                m_Key = key;
            }

            #endregion

            #region Overridden Methods

            /// <summary>
            /// Converts this KeyRejectedEventArgs instance into it's string representation.
            /// </summary>
            /// <returns>A string indicating the rejected key.</returns>
            public override string ToString()
            {
                return string.Format("Rejected Key: {0}", Key.ToString());
            }

            #endregion
        }

        /// <summary>
        /// Event arguments class for the PasteRejected event.
        /// </summary>
        public class PasteEventArgs : EventArgs
        {
            #region Private Variables

            private string m_OriginalText;
            private string m_ClipboardText;
            private string m_TextResult;
            private PasteRejectReasons m_RejectReason;

            #endregion

            #region Properties

            /// <summary>
            /// Gets the original text.
            /// </summary>
            [ReadOnly(true)]
            public string OriginalText
            {
                get { return m_OriginalText; }
            }

            /// <summary>
            /// Gets the text from the clipboard.
            /// </summary>
            [ReadOnly(true)]
            public string ClipboardText
            {
                get { return m_ClipboardText; }
            }

            /// <summary>
            /// Gets the resulting text.
            /// </summary>
            [ReadOnly(true)]
            public string TextResult
            {
                get { return m_TextResult; }
            }

            /// <summary>
            /// Gets the reason for the paste rejection.
            /// </summary>
            [ReadOnly(true)]
            public PasteRejectReasons RejectReason
            {
                get { return m_RejectReason; }
            }

            #endregion

            #region Constructor

            /// <summary>
            /// Creates a new instance of the PasteRejectedEventArgs class.
            /// </summary>
            /// <param name="originalText">The original textl.</param>
            /// <param name="clipboardText">The text from the clipboard.</param>
            /// <param name="textResult">The resulting text.</param>
            /// <param name="rejectReason">The reason for the paste rejection.</param>
            public PasteEventArgs(string originalText, string clipboardText, string textResult,
                PasteRejectReasons rejectReason)
            {
                m_OriginalText = originalText;
                m_ClipboardText = clipboardText;
                m_TextResult = textResult;
                m_RejectReason = rejectReason;
            }

            #endregion

            #region Overridden Methods

            /// <summary>
            /// Converts this PasteRejectedEventArgs instance into it's string representation.
            /// </summary>
            /// <returns>A string indicating the rejected reason.</returns>
            public override string ToString()
            {
                return string.Format("Rejected reason: {0}", RejectReason.ToString());
            }

            #endregion
        }

        #endregion
    }
}

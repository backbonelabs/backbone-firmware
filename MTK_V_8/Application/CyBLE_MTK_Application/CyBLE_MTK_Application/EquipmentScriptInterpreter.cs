using System;
using System.IO;
using System.IO.Ports;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;

namespace CyBLE_MTK_Application
{
    enum ScriptError {  NoError, Error, FileDoesNotExist, CannotOpenFile, FileEmpty, SecBegOfLineErr, UnknownSecErr, SyntaxErr, SecRepeatErr,
                        UnknownSerialPortSetting, BaudRateResolutionErr, ParityResolutionErr, DataBitsResolutionErr, StopBitsResolutionErr,
                        HandShakeResolutionErr, UnknownCmdErr, CannotOpenSP, CannotCloseSP, SPNotOpen, CannotTalkToSP, NotEnoughParamsRet,
                        UnexpectedOutputFormat };
    
    enum ScriptSections { SerialPort, Init, Measure, Stop, NoSectionFound };

    enum ScriptCommand { OpenSerialPortCMD, CloseSerialPortCMD, InitCMD, MeasureCMD, StopCMD };

    class EquipmentScriptInterpreter
    {
        #region Properties
        private string scriptFileName;
        public string ScriptFileName
        {
            get
            {
                return this.scriptFileName;
            }
            set
            {
                this.scriptFileName = value;
            }
        }
        
        private int lineCount;
        public int LineCount
        {
            get
            {
                return this.lineCount;
            }
        }

        private int columnCount;
        public int ColumnCount
        {
            get
            {
                return this.columnCount;
            }
        }

        private string serialPortName;
        public string SerialPortName
        {
            get
            {
                return this.serialPortName;
            }
        }

        private int spBaudRate;
        public int SPBaudRate
        {
            get
            {
                return this.spBaudRate;
            }
        }

        private Parity spParity;
        public Parity SPParity 
        {
            get
            {
                return this.spParity;
            }
        }

        private int spDataBits;
        public int SPDataBits
        {
            get
            {
                return this.spDataBits;
            }
        }

        private StopBits spStopBits;
        public StopBits SPStopBits 
        {
            get
            {
                return this.spStopBits;
            }
        }

        private Handshake spHandshake;
        public Handshake SPHandshake 
        {
            get
            {
                return this.spHandshake;
            }
        }

        private List<string> secInitCommands;
        public List<string> SecInitCommands 
        {
            get
            {
                return this.secInitCommands;
            }
        }

        private List<string> secMeasureCommands;
        public List<string> SecMeasureCommands 
        {
            get
            {
                return this.secMeasureCommands;
            }
        }

        private List<string> secStopCommands;
        public List<string> SecStopCommands 
        {
            get
            {
                return this.secStopCommands;
            }
        }

        private bool scriptParsed;
        public bool ScriptParsed
        {
            get
            {
                return this.scriptParsed;
            }
        }

        private int measuredFreq;
        public int MeasuredFrequency
        {
            get
            {
                return this.measuredFreq;
            }
        }
        #endregion

        #region Constants
        private enum RXStringFormat { RXFNone, RXFInteger, RXFCharacter, RXFString, RXFFloat, RXFLong };
        private const string commentString = "//";
        private const string sectionString = "SECTION.";
        #endregion

        #region Private
        private bool hasSecSerialPort, hasSecInit, hasSecMeasure, hasSecStop;
        private ScriptSections currentSection;
        private SerialPort FreqCntrPort;
        #endregion

        public EquipmentScriptInterpreter()
        {
            this.scriptFileName = "";
            lineCount = 0;
            columnCount = 0;
            hasSecSerialPort = false;
            hasSecInit = false;
            hasSecMeasure = false;
            hasSecStop = false;
            currentSection = ScriptSections.NoSectionFound;
            serialPortName = "";
            spBaudRate = 9600;
            spParity = Parity.None;
            spDataBits = 8;
            spStopBits = StopBits.One;
            spHandshake = Handshake.None;
            scriptParsed = false;
        }

        public EquipmentScriptInterpreter(string fileName) : this()
        {
            this.scriptFileName = fileName;
        }

        public string GetErrorString(ScriptError error)
        {
            switch (error)
            {
                case ScriptError.NoError:
                    return "No error found";
                case ScriptError.Error:
                    return "Unknown Error";
                case ScriptError.FileDoesNotExist:
                    return "File does not exist";
                case ScriptError.CannotOpenFile:
                    return "Cannot open the file";
                case ScriptError.FileEmpty:
                    return "Empty file";
                case ScriptError.SecBegOfLineErr:
                    return "Begning of section invalid";
                case ScriptError.UnknownSecErr:
                    return "Unknown section";
                case ScriptError.SyntaxErr:
                    return "Syntax error";
                case ScriptError.SecRepeatErr:
                    return "Section repeated";
                case ScriptError.UnknownSerialPortSetting:
                    return "Unknown serial port setting";
                case ScriptError.BaudRateResolutionErr:
                    return "Cannot resolve baud rate";
                case ScriptError.ParityResolutionErr:
                    return "Cannot resolve parity";
                case ScriptError.DataBitsResolutionErr:
                    return "Cannot resolve data bits";
                case ScriptError.StopBitsResolutionErr:
                    return "Cannot resolve stop bits";
                case ScriptError.HandShakeResolutionErr:
                    return "Cannot resolve handshaking protocol";
                case ScriptError.UnknownCmdErr:
                    return "Unknown command";
                case ScriptError.CannotOpenSP:
                    return "Cannot open serial port";
                case ScriptError.CannotCloseSP:
                    return "Cannot close serial port";
                case ScriptError.SPNotOpen:
                    return "Serial port not open";
                case ScriptError.CannotTalkToSP:
                    return "Cannot talk toSerial port";
                case ScriptError.NotEnoughParamsRet:
                    return "Not enough prameters returned";
                case ScriptError.UnexpectedOutputFormat:
                    return "Unexpected output format";
                default:
                    return "Unknown error";
            }
        }

        public ScriptError ParseScriptFile()
        {
            #region Initialization
            string CurrentLine, CurrentLineLessComments;
            char[] charsToTrim = { '\t', ' ' };
            int commentIndex;

            hasSecSerialPort = false;
            hasSecInit = false;
            hasSecMeasure = false;
            hasSecStop = false;
            currentSection = ScriptSections.NoSectionFound;
            secInitCommands = new List<string>();
            secMeasureCommands = new List<string>();
            secStopCommands = new List<string>();

            lineCount = 0;
            #endregion
            
            try
            {
                StreamReader InputFile = new StreamReader(scriptFileName);
                while ((CurrentLine = InputFile.ReadLine()) != null)
                {
                    lineCount++;

                    #region Empty_Line_Processing
                    if (CurrentLine.Length <= 0)
                    {
                        continue;
                    }
                    #endregion

                    #region Comment_Processing
                    columnCount = 1;
                    commentIndex = CurrentLine.IndexOf(commentString);
                    if (commentIndex == 0)
                    {
                        continue;
                    }
                    else if (commentIndex > 0)
                    {
                        CurrentLineLessComments = CurrentLine.Substring(0, commentIndex);
                    }
                    else
                    {
                        CurrentLineLessComments = CurrentLine;
                    }

                    CurrentLineLessComments = CurrentLineLessComments.TrimEnd(charsToTrim);
                    #endregion

                    #region Section_Processing
                    if (CurrentLineLessComments.Contains(sectionString))
                    {
                        if (CurrentLine.IndexOf(sectionString) != 0)
                        {
                            columnCount = CurrentLineLessComments.IndexOf(sectionString);
                            InputFile.Close();
                            return ScriptError.SecBegOfLineErr;
                        }

                        int sectionStringIndex = CurrentLineLessComments.IndexOf(sectionString);
                        CurrentLineLessComments = CurrentLineLessComments.Remove(sectionStringIndex, sectionString.Length);
                        ScriptSections identifiedSection = ScriptSections.NoSectionFound;
                        foreach (ScriptSections currentScriptSection in Enum.GetValues(typeof(ScriptSections)))
                        {
                            if (CurrentLineLessComments.Contains(currentScriptSection.ToString()))
                            {
                                int sectionNameIndex = CurrentLineLessComments.IndexOf(currentScriptSection.ToString());
                                if (sectionNameIndex != 0)
                                {
                                    columnCount = sectionString.Length;
                                    InputFile.Close();
                                    return ScriptError.UnknownSecErr;
                                }

                                if (CurrentLineLessComments.Length > currentScriptSection.ToString().Length)
                                {
                                    if (CurrentLineLessComments[currentScriptSection.ToString().Length] != ' ' &&
                                        CurrentLineLessComments[currentScriptSection.ToString().Length] != '\t')
                                    {
                                        columnCount = sectionString.Length;
                                        InputFile.Close();
                                        return ScriptError.UnknownSecErr;
                                    }
                                }

                                identifiedSection = currentScriptSection;
                                break;
                            }
                        }
                        if (identifiedSection == ScriptSections.NoSectionFound)
                        {
                            InputFile.Close();
                            return ScriptError.UnknownSecErr;
                        }
                        if (identifiedSection == ScriptSections.SerialPort)
                        {
                            if (hasSecSerialPort == false)
                            {
                                hasSecSerialPort = true;
                            }
                            else
                            {
                                InputFile.Close();
                                columnCount = sectionString.Length;
                                return ScriptError.SecRepeatErr;
                            }
                        }
                        else if (identifiedSection == ScriptSections.Init)
                        {
                            if (hasSecInit == false)
                            {
                                hasSecInit = true;
                            }
                            else
                            {
                                InputFile.Close();
                                columnCount = sectionString.Length;
                                return ScriptError.SecRepeatErr;
                            }
                        }
                        else if (identifiedSection == ScriptSections.Measure)
                        {
                            if (hasSecMeasure == false)
                            {
                                hasSecMeasure = true;
                            }
                            else
                            {
                                InputFile.Close();
                                columnCount = sectionString.Length;
                                return ScriptError.SecRepeatErr;
                            }
                        }
                        else if (identifiedSection == ScriptSections.Stop)
                        {
                            if (hasSecStop == false)
                            {
                                hasSecStop = true;
                            }
                            else
                            {
                                InputFile.Close();
                                columnCount = sectionString.Length;
                                return ScriptError.SecRepeatErr;
                            }
                        }
                        
                        currentSection = identifiedSection;
                        continue;
                    }
                    #endregion

                    #region Section_Statement_Processing
                    CurrentLineLessComments = CurrentLineLessComments.Trim(charsToTrim);
                    
                    #region Section_Serial_Port
                    if (currentSection == ScriptSections.SerialPort)
                    {
                        //CurrentLineLessComments = Regex.Replace(CurrentLineLessComments, @"\s+", "");
                        if (CurrentLineLessComments.Contains("PortName"))
                        {
                            if ((CurrentLineLessComments.IndexOf("PortName") != 0) &&
                                (CurrentLineLessComments[CurrentLineLessComments.IndexOf("PortName")] != '='))
                            {
                                InputFile.Close();
                                return ScriptError.UnknownSerialPortSetting;
                            }

                            serialPortName = CurrentLineLessComments.Remove(0, "PortName=".Length);
                        }
                        else if (CurrentLineLessComments.Contains("BaudRate"))
                        {
                            if ((CurrentLineLessComments.IndexOf("BaudRate") != 0) &&
                                (CurrentLineLessComments[CurrentLineLessComments.IndexOf("BaudRate")] != '='))
                            {
                                InputFile.Close();
                                return ScriptError.UnknownSerialPortSetting;
                            }

                            CurrentLineLessComments = CurrentLineLessComments.Remove(0, "BaudRate=".Length);
                            if (Int32.TryParse(CurrentLineLessComments, out spBaudRate) == false)
                            {
                                columnCount = CurrentLine.IndexOf(CurrentLineLessComments);
                                InputFile.Close();
                                return ScriptError.BaudRateResolutionErr;
                            }
                        }
                        else if (CurrentLineLessComments.Contains("Parity"))
                        {
                            if ((CurrentLineLessComments.IndexOf("Parity") != 0) &&
                                (CurrentLineLessComments[CurrentLineLessComments.IndexOf("Parity")] != '='))
                            {
                                InputFile.Close();
                                return ScriptError.UnknownSerialPortSetting;
                            }

                            CurrentLineLessComments = CurrentLineLessComments.Remove(0, "Parity=".Length);
                            try
                            {
                                spParity = (Parity)Enum.Parse(typeof(Parity), CurrentLineLessComments, true);
                            }
                            catch (Exception ex)
                            {
                                if ((ex is ArgumentException) || (ex is OverflowException))
                                {
                                    columnCount = CurrentLine.IndexOf(CurrentLineLessComments);
                                }
                                InputFile.Close();
                                return ScriptError.ParityResolutionErr;
                            }
                        }
                        else if (CurrentLineLessComments.Contains("DataBits"))
                        {
                            if ((CurrentLineLessComments.IndexOf("DataBits") != 0) &&
                                (CurrentLineLessComments[CurrentLineLessComments.IndexOf("DataBits")] != '='))
                            {
                                InputFile.Close();
                                return ScriptError.UnknownSerialPortSetting;
                            }

                            CurrentLineLessComments = CurrentLineLessComments.Remove(0, "DataBits=".Length);
                            if (Int32.TryParse(CurrentLineLessComments, out spDataBits) == false)
                            {
                                columnCount = CurrentLine.IndexOf(CurrentLineLessComments);
                                InputFile.Close();
                                return ScriptError.DataBitsResolutionErr;
                            }

                            if ((spDataBits > 8) || (spDataBits < 5))
                            {
                                columnCount = CurrentLine.IndexOf(CurrentLineLessComments);
                                InputFile.Close();
                                return ScriptError.DataBitsResolutionErr;
                            }
                        }
                        else if (CurrentLineLessComments.Contains("StopBits"))
                        {
                            if ((CurrentLineLessComments.IndexOf("StopBits") != 0) &&
                                (CurrentLineLessComments[CurrentLineLessComments.IndexOf("StopBits")] != '='))
                            {
                                InputFile.Close();
                                return ScriptError.UnknownSerialPortSetting;
                            }

                            CurrentLineLessComments = CurrentLineLessComments.Remove(0, "StopBits=".Length);
                            try
                            {
                                spStopBits = (StopBits)Enum.Parse(typeof(StopBits), CurrentLineLessComments, true);
                            }
                            catch (Exception ex)
                            {
                                if ((ex is ArgumentException) || (ex is OverflowException))
                                {
                                    columnCount = CurrentLine.IndexOf(CurrentLineLessComments);
                                }
                                InputFile.Close();
                                return ScriptError.StopBitsResolutionErr;
                            }
                        }
                        else if (CurrentLineLessComments.Contains("Handshake"))
                        {
                            if ((CurrentLineLessComments.IndexOf("Handshake") != 0) &&
                                (CurrentLineLessComments[CurrentLineLessComments.IndexOf("Handshake")] != '='))
                            {
                                InputFile.Close();
                                return ScriptError.UnknownSerialPortSetting;
                            }

                            CurrentLineLessComments = CurrentLineLessComments.Remove(0, "Handshake=".Length);
                            try
                            {
                                spHandshake = (Handshake)Enum.Parse(typeof(Handshake), CurrentLineLessComments, true);
                            }
                            catch (Exception ex)
                            {
                                if ((ex is ArgumentException) || (ex is OverflowException))
                                {
                                    columnCount = CurrentLine.IndexOf(CurrentLineLessComments);
                                }
                                InputFile.Close();
                                return ScriptError.HandShakeResolutionErr;
                            }
                        }
                        else
                        {
                            InputFile.Close();
                            return ScriptError.UnknownSerialPortSetting;
                        }
                    }
                    #endregion
                    #region Section_Others
                    else
                    {
                        if (CurrentLineLessComments.Contains("SendString"))
                        {
                            //string processedString;
                            if (Regex.IsMatch(CurrentLineLessComments, @"\s*SendString\s*\(\s*"".*""\s*\)\s*") == false)
                            {
                                InputFile.Close();
                                return ScriptError.SyntaxErr;
                            }

                            //processedString = Regex.Replace(CurrentLineLessComments, @"\s*SendString\s*\(\s*""", "");
                            //processedString = Regex.Replace(processedString, @"""\s*\)\s*", "");

                            if (currentSection == ScriptSections.Init)
                            {
                                secInitCommands.Add(CurrentLineLessComments);
                            }
                            else if (currentSection == ScriptSections.Measure)
                            {
                                secMeasureCommands.Add(CurrentLineLessComments);
                            }
                            else if (currentSection == ScriptSections.Stop)
                            {
                                secStopCommands.Add(CurrentLineLessComments);
                            }
                        }
                        else if (CurrentLineLessComments.Contains("DelayMS"))
                        {
                            //string processedString;
                            if (Regex.IsMatch(CurrentLineLessComments, @"\s*DelayMS\s*\(\s*\d+\s*\)\s*") == false)
                            {
                                InputFile.Close();
                                return ScriptError.SyntaxErr;
                            }

                            //processedString = Regex.Replace(CurrentLineLessComments, @"\s*SendString\s*\(\s*""", "");
                            //processedString = Regex.Replace(processedString, @"""\s*\)\s*", "");

                            if (currentSection == ScriptSections.Init)
                            {
                                secInitCommands.Add(CurrentLineLessComments);
                            }
                            else if (currentSection == ScriptSections.Measure)
                            {
                                secMeasureCommands.Add(CurrentLineLessComments);
                            }
                            else if (currentSection == ScriptSections.Stop)
                            {
                                secStopCommands.Add(CurrentLineLessComments);
                            }
                        }
                        else if (CurrentLineLessComments.Contains("ReceiveString"))
                        {
                            //string processedString;
                            if (Regex.IsMatch(CurrentLineLessComments, @"\s*ReceiveString\s*\(\s*"",*%[dlfxsc],*""\s*\)\s*") == false)
                            {
                                InputFile.Close();
                                return ScriptError.SyntaxErr;
                            }

                            //processedString = Regex.Replace(CurrentLineLessComments, @"\s*SendString\s*\(\s*""", "");
                            //processedString = Regex.Replace(processedString, @"""\s*\)\s*", "");

                            if (currentSection == ScriptSections.Init)
                            {
                                secInitCommands.Add(CurrentLineLessComments);
                            }
                            else if (currentSection == ScriptSections.Measure)
                            {
                                secMeasureCommands.Add(CurrentLineLessComments);
                            }
                            else if (currentSection == ScriptSections.Stop)
                            {
                                secStopCommands.Add(CurrentLineLessComments);
                            }
                        }

                        else
                        {
                            InputFile.Close();
                            return ScriptError.UnknownCmdErr;
                        }
                    }
                    #endregion
                    #endregion
                }

                InputFile.Close();

                if (lineCount == 0)
                {
                    return ScriptError.FileEmpty;
                }
            }
            catch (Exception ex)
            {
                if (ex is ArgumentException)
                {
                    return ScriptError.CannotOpenFile;
                }
                else if(ex is ArgumentNullException)
                {
                    return ScriptError.FileDoesNotExist;
                }
            }

            scriptParsed = true;

            return ScriptError.NoError;
        }

        private bool UARTSendCMD(string Command)
        {
            try
            {
                this.FreqCntrPort.DiscardOutBuffer();
                this.FreqCntrPort.DiscardInBuffer();
                this.FreqCntrPort.WriteLine(Command);
                Thread.Sleep(20);
                return true;
            }
            catch
            {
                return false;
            }
        }

        private ScriptError ParseCommand(string CMDstring)
        {
            string processedString;

            if (CMDstring.Contains("SendString"))
            {
                processedString = Regex.Replace(CMDstring, @"\s*SendString\s*\(\s*""", "");
                processedString = Regex.Replace(processedString, @"""\s*\)\s*", "");

                if (UARTSendCMD(processedString) == false)
                {
                    return ScriptError.CannotTalkToSP;
                }
            }
            else if (CMDstring.Contains("DelayMS"))
            {
                processedString = Regex.Replace(CMDstring, @"\s*DelayMS\s*\(\s*", "");
                processedString = Regex.Replace(processedString, @"\s*\)\s*", "");

                Thread.Sleep(int.Parse(processedString));
            }
            else if (CMDstring.Contains("ReceiveString"))
            {
                char[] DelimiterChars = { ',', ';', ' ', '\t' };

                processedString = Regex.Replace(CMDstring, @"\s*ReceiveString\s*\(\s*""", "");
                processedString = Regex.Replace(processedString, @"""\s*\)\s*", "");

                int DelimiterCNTR = 0;
                RXStringFormat RXFormat = RXStringFormat.RXFNone;

                for (int i = 0; i < processedString.Length; i++)
                {
                    if (processedString[i] == ',' || processedString[i] == ';' ||
                        processedString[i] == ' ' || processedString[i] == '\t')
                    {
                        DelimiterCNTR++;
                    }
                    else if (processedString[i] == '%')
                    {
                        if (processedString[i + 1] == 'd')
                        {
                            RXFormat = RXStringFormat.RXFInteger;
                        }
                        else if (processedString[i + 1] == 'c')
                        {
                            RXFormat = RXStringFormat.RXFCharacter;
                        }
                        else if (processedString[i + 1] == 's')
                        {
                            RXFormat = RXStringFormat.RXFString;
                        }
                        else if (processedString[i + 1] == 'l')
                        {
                            RXFormat = RXStringFormat.RXFLong;
                        }
                        else if (processedString[i + 1] == 'f')
                        {
                            RXFormat = RXStringFormat.RXFFloat;
                        }
                        break;
                    }
                }

                string OuputString = this.FreqCntrPort.ReadExisting();
                string[] Output = OuputString.Split(DelimiterChars);

                if (Output.Length < DelimiterCNTR)
                {
                    return ScriptError.NotEnoughParamsRet;
                }

                if (RXFormat == RXStringFormat.RXFInteger)
                {
                    try
                    {
                        measuredFreq = int.Parse(Output[DelimiterCNTR]);
                    }
                    catch
                    {
                        return ScriptError.UnexpectedOutputFormat;
                    }
                }
            }

            return ScriptError.NoError;
        }

        public ScriptError RunCommands(ScriptCommand ScriptCMD)
        {
            if (scriptParsed == false)
            {
                ScriptError RetVal = this.ParseScriptFile();
                if (RetVal != ScriptError.NoError)
                {
                    return RetVal;
                }
            }

            if (ScriptCMD == ScriptCommand.OpenSerialPortCMD)
            {
                if (this.FreqCntrPort == null)
                {
                    this.FreqCntrPort = new SerialPort();
                }

                if (this.FreqCntrPort.IsOpen == false)
                {
                    this.FreqCntrPort.PortName = serialPortName;
                    this.FreqCntrPort.BaudRate = spBaudRate;
                    this.FreqCntrPort.Parity = spParity;
                    this.FreqCntrPort.DataBits = spDataBits;
                    this.FreqCntrPort.StopBits = spStopBits;
                    this.FreqCntrPort.Handshake = spHandshake;
                    try
                    {
                        this.FreqCntrPort.Open();
                    }
                    catch
                    {
                        return ScriptError.CannotOpenSP;
                    }
                }

            }
            else if (ScriptCMD == ScriptCommand.InitCMD)
            {
                foreach (string currentCMD in secInitCommands)
                {
                    ScriptError RetVal = this.ParseCommand(currentCMD);
                    if (RetVal != ScriptError.NoError)
                    {
                        return RetVal;
                    }
                }
            }
            else if (ScriptCMD == ScriptCommand.MeasureCMD)
            {
                foreach (string currentCMD in secMeasureCommands)
                {
                    ScriptError RetVal = this.ParseCommand(currentCMD);
                    if (RetVal != ScriptError.NoError)
                    {
                        return RetVal;
                    }
                }
            }
            else if (ScriptCMD == ScriptCommand.StopCMD)
            {
                foreach (string currentCMD in secStopCommands)
                {
                    ScriptError RetVal = this.ParseCommand(currentCMD);
                    if (RetVal != ScriptError.NoError)
                    {
                        return RetVal;
                    }
                }
            }
            else if (ScriptCMD == ScriptCommand.CloseSerialPortCMD)
            {
                if (this.FreqCntrPort != null)
                {
                    try
                    {
                        if (this.FreqCntrPort.IsOpen)
                        {
                            this.FreqCntrPort.Close();
                        }
                    }
                    catch
                    {
                        return ScriptError.CannotCloseSP;
                    }
                }
                else
                {
                    return ScriptError.SPNotOpen;
                }
            }
            return ScriptError.NoError;
        }
    }
}

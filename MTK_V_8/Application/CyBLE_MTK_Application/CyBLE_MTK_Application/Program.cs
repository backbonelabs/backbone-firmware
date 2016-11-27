using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Threading;
using System.Runtime;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace CyBLE_MTK_Application
{
    static class Program
    {
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool SetForegroundWindow(IntPtr hWnd);
        //static private bool run_in_background = false;

        static private bool check_argument(string s)
        {
            if (s == "bg")
            {
                Console.WriteLine(s + " - running in background.");
                //run_in_background = true;
                return true;
            }

            return false;
        }
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            bool createdNew = true;
            using (Mutex mutex = new Mutex(true, "CyBLEMTKApplication", out createdNew))
            {
                if (createdNew)
                {
                    //Console.WriteLine("Number of command line parameters = {0}", args.Length);
                    foreach (string s in args)
                    {
                        check_argument(s);
                    }
                    Application.EnableVisualStyles();
                    Application.SetCompatibleTextRenderingDefault(false);
                    CyBLE_MTK MTK_application = new CyBLE_MTK();

                    if (args.Length > 0)
                    {
                        if (!check_argument(args[args.Length - 1]))
                        {
                            MTK_application.LoadTest(true, args[args.Length - 1]);
                        }
                    }
                    //if (run_in_background)
                    //{
                    //    MTK_application
                    //}
                    //else
                    Application.Run(MTK_application);
                }
                else
                {
                    Process current = Process.GetCurrentProcess();
                    foreach (Process process in Process.GetProcessesByName(current.ProcessName))
                    {
                        if (process.Id != current.Id)
                        {
                            SetForegroundWindow(process.MainWindowHandle);
                            break;
                        }
                    }
                }
            }
        }
    }
}

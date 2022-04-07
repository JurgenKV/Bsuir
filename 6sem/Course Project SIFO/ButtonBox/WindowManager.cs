using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace ButtonBox
{
    class WindowManager
    {
        [DllImport("user32.dll", EntryPoint = "FindWindow", SetLastError = true)]
        static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        static extern IntPtr SendMessage(IntPtr hWnd, Int32 Msg, IntPtr wParam, IntPtr lParam);

        const int WM_COMMAND = 0x111;
        const int MIN_ALL = 419;
        const int MIN_ALL_UNDO = 416;

        public static void closeWindowsOpenTM(bool taskmng)
        {
            IntPtr lHwnd = FindWindow("Shell_TrayWnd", null);
            // свернуть
            SendMessage(lHwnd, WM_COMMAND, (IntPtr)MIN_ALL, IntPtr.Zero);
            //System.Threading.Thread.Sleep(2000);
            // развернуть
            // SendMessage(lHwnd, WM_COMMAND, (IntPtr)MIN_ALL_UNDO, IntPtr.Zero);
            if (taskmng)
                Process.Start("taskmgr");
        }
    }
}

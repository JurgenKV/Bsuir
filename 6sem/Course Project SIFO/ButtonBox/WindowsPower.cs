using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows;

namespace ButtonBox
{
    class WindowsPower
    {
        [DllImport("Powrprof.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        public static extern bool SetSuspendState(bool hiberate, bool forceCritical, bool disableWakeEvent);
        // Hibernate
        static public void doHibernate()
        {
            SetSuspendState(true, true, true);
        }
        static public void doStandby()
        {
            WindowManager.closeWindowsOpenTM(false);
            MessageBoxResult result = MessageBox.Show("Sleep mode?", "ButtonBox", MessageBoxButton.YesNo);
            switch (result)
            {
                case MessageBoxResult.Yes:
                    SetSuspendState(false, true, true);
                    break;
                case MessageBoxResult.No:
                    break;
            }
        }

        static public void doShutdown()
        {
            WindowManager.closeWindowsOpenTM(false);
            MessageBoxResult result = MessageBox.Show("Shutdown PC?", "ButtonBox", MessageBoxButton.YesNo);
            switch (result)
            {
                case MessageBoxResult.Yes:
                    var psi = new ProcessStartInfo("shutdown", "/s /t 0");
                    psi.CreateNoWindow = true;
                    psi.UseShellExecute = false;
                    Process.Start(psi);

                    break;
                case MessageBoxResult.No:
                    break;
            }
        }
    }
}

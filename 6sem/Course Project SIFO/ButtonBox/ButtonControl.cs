using Microsoft.Toolkit.Uwp.Notifications;
using System;
using System.Collections.Generic;

namespace ButtonBox
{
    class ButtonControl
    {

        static public void userChoice(string choice)
        {
            switch (choice)
            {
                case "A":
                    muteControlMicro();
                    break;
                case "B":
                    muteSystem();
                    break;

                case "C":
                    outputDeviceChange();
                    break;
                case "D":
                    closeWindowsOpenTskMng();

                    break;
                case "E":

                    break;
                case "F":

                    break;
                case "G":
                    sleepMode();
                    break;
                case "H":
                    shutdownMode();
                    break;
                case "I":
                    quickStart(choice);
                    break;
                case "J":
                    quickStart(choice);
                    break;
                case "K":
                    quickStart(choice);
                    break;
                case "L":
                    quickStart(choice);
                    break;
                case "M":
                    quickStart(choice);
                    break;
                case "N":
                    quickStart(choice);
                    break;
                case "O":
                    quickStart(choice);
                    break;
                case "P":
                    quickStart(choice);
                    break;
                case "Q":
                    upVolume();
                    break;
                case "R":
                    downVolume();
                    break;

                default:
                    {
                        Console.WriteLine("User Choice is " + choice);
                        return;
                    }

            }

        }
        static public void muteControlMicro()
        {
            try
            {

                if (WindowsSound.muteMicro())
                {
                    notification("Micro is off");
                }
                else
                {
                    notification("Micro is on");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("muteControlMicro()" + ex.Message);
            }
        }


        static public void notification(string notifText)
        {
            try
            {
                new ToastContentBuilder()
                        .AddArgument("action", "viewConversation")
                        .AddArgument("conversationId", 9813)
                        .AddText("ButtonBox")
                        .AddText(notifText)
                        .Show(toast =>
                        {
                            toast.ExpirationTime = DateTime.Now.AddSeconds(1);
                        });
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error notification()" + ex.Message);
            }

        }

        static public void muteSystem()
        {
            try
            {
                WindowsVolume.Mute();
                muteControlMicro();
                notification("System Volume and micro - off");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error muteSystem()" + ex.Message);
            }

        }

        static public void outputDeviceChange()
        {
            try
            {
                WindowsSound.outputDevice();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error outputDeviceChange()" + ex.Message);
            }

        }


        static public void closeWindowsOpenTskMng()
        {
            try
            {
                WindowManager.closeWindowsOpenTM(true);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error closeWindowsOpenTskMng()" + ex.Message);
            }

        }

        static public void sleepMode()
        {
            try
            {
                WindowsPower.doStandby();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error sleepMode()" + ex.Message);
            }
        }

        static public void shutdownMode()
        {
            try
            {
                WindowsPower.doShutdown();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error shutdownMode()" + ex.Message);
            }

        }

        static public void quickStart(string quickChoice)
        {
            try
            {
                List<QuickLaunch> quickLaunchList = FileSystem.parsBindFile();
                string path = null;
                foreach (var item in quickLaunchList)
                {
                    if (item.Signal.Equals(quickChoice))
                        path = item.Filepath;
                }

                System.Diagnostics.Process.Start(path);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error quickStart()" + ex.Message);
            }
        }

        static public void upVolume()
        {
            try
            {
                for (int i = 0; i < 2; i++)
                {
                    WindowsVolume.VolumeUp();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error upVolume()" + ex.Message);
            }
        }
        static public void downVolume()
        {
            try
            {
                for (int i = 0; i < 2; i++)
                {
                    WindowsVolume.VolumeDown();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error downVolume()" + ex.Message);
            }
        }
    }

}

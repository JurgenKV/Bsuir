using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace ButtonBox
{
    class FileSystem
    {
        const string path = "BindList.bin";
        const string settingsPath = "Settings.bin";

        static public void setBindToFile(QuickLaunch quickLaunch)
        {

            try
            {
                if (!System.IO.File.Exists(path))
                {
                    FileStream fstream = new FileStream(path, FileMode.Create);
                    fstream.Close();
                }


                string item = quickLaunch.Signal + "|" + quickLaunch.Filepath;

                var lines = File.ReadAllLines(path).ToList();
                int index = lines.FindIndex(s => s.Contains(quickLaunch.Signal + "|"));

                if (index.Equals(-1))
                {
                    index = 0;
                    lines.Insert(index, item);
                }
                else
                {
                    // добавить месс бокс с вопросом о замене
                    var result = MessageBox.Show("Button is binded. Do you want to replace the bind?", "ButtonBox Binds", MessageBoxButton.YesNo, MessageBoxImage.Question, MessageBoxResult.No);
                    switch (result)
                    {
                        case MessageBoxResult.Yes:
                            index = lines.Count - 1;
                            lines.RemoveAt(index);
                            lines.Insert(index, item);
                            break;
                        case MessageBoxResult.No:
                            break;
                    }
                    return;
                }

                File.WriteAllLines(path, lines);
                parsBindFile();
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception: " + e.Message);
            }
            finally
            {
                Console.WriteLine("Executing finally block.");
            }

        }

        static public List<QuickLaunch> parsBindFile()
        {
            List<QuickLaunch> quickLaunchesList = new List<QuickLaunch> { };
            try
            {
                if (!System.IO.File.Exists(path))
                {
                    FileStream fstream = new FileStream(path, FileMode.Create);
                    fstream.Close();
                }
                

                var lines = File.ReadAllLines(path).ToList();

                foreach (string line in lines)
                {
                    string[] temp = line.Split(new char[] { '|' });
                    QuickLaunch quickLaunch = new QuickLaunch(temp[0], temp[1]);
                    quickLaunchesList.Add(quickLaunch);
                }
            }
            catch (Exception ex)
            {

            }
            return quickLaunchesList;
        }

        static public void setFileSettings(bool autorun)
        {
            try
            {
                StreamWriter settingsFile = new StreamWriter(settingsPath);
                if (autorun.Equals(true))
                    settingsFile.WriteLine("true");
                else
                    settingsFile.WriteLine("false");

                settingsFile.Close();
            }
            catch (Exception ex) { Console.WriteLine("setFileSettings()" + ex.Message); }
        }

        static public bool parseAutorunSettings()
        {
            if (!System.IO.File.Exists("Settings.bin"))
            {
                setFileSettings(false);
            }

            try
            {
                StreamReader streamReader = new StreamReader(settingsPath);

                if (streamReader.ReadLine().Equals("true"))
                    return true;
                else
                    return false;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error parseAutorunSettings()" + ex.Message);
            }
            return false;

        }
    }
}

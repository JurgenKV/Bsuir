using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;

namespace ButtonBox
{

    public partial class MainWindow : Window
    {
        public SerialPort serialPort = new SerialPort();
        public bool init = false;
        public string mess;
        QuickLaunch quickLaunch;


        public MainWindow()
        {
            InitializeComponent();
            loadPorts();
            loadFastOpenComboBox();
            checkBox.IsChecked = FileSystem.parseAutorunSettings();
        }

        private void loadPorts()
        {
            string[] ports = SerialPort.GetPortNames();

            foreach (string port in ports)
            {
                comboBoxPortsList.Items.Add(port);
            }

        }

        private void loadFastOpenComboBox()
        {
            char signal = 'I';
            for (int i = 9; i < 17; i++)
            {
                comboBoxFastOpenAppList.Items.Add("Button " + i + " (" + signal + ")");
                signal++;
            }

        }
        public bool InitializePort(string portName)
        {
            try
            {
                serialPort.PortName = portName;
                serialPort.BaudRate = 9600;
                if (!serialPort.IsOpen)
                    serialPort.Open();

                serialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceived);

                return true;
            }
            catch
            {
                serialPort.Close();
                return false;
            }

        }

        private void DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {

                SerialPort sp = (SerialPort)sender;
                Dispatcher.Invoke((System.Action)(() =>
                {
                    mess = sp.ReadLine();
                    mess = mess.Replace("\r", "");
                    Console.WriteLine(mess);
                    ButtonControl.userChoice(mess);

                }));


            }
            catch (Exception ex)
            {
                Console.WriteLine("Received failed" + ex.Message);
            }
        }

        private void ButtonInitPort_Click(object sender, RoutedEventArgs e)
        {
            ButtonControl.userChoice("I"); // debug
            if (init == false)
            {
                try
                {
                    if (InitializePort(comboBoxPortsList.Text) == true)
                    {
                        init = true;
                        Console.WriteLine("init true");
                        labelPortChoice.Content = "Port initialized successfully";
                    }
                    else
                    {
                        //some
                    }
                }
                catch
                {
                    Console.WriteLine("init failed");
                    labelPortChoice.Content = "Port failed to initialize";
                }
            }
        }

        private void buttonMuteMicro_Click(object sender, RoutedEventArgs e)
        {
            ButtonControl.userChoice("A");
        }

        private void buttonCloseWOpenTM_Click(object sender, RoutedEventArgs e)
        {
            ButtonControl.userChoice("D");
        }

        private void buttonMuteSystem_Click(object sender, RoutedEventArgs e)
        {
            ButtonControl.userChoice("B");
        }

        private void buttonChangeOutput_Click(object sender, RoutedEventArgs e)
        {
            ButtonControl.userChoice("C");
        }

        private void buttonDoSleepMode_Click(object sender, RoutedEventArgs e)
        {
            ButtonControl.userChoice("G");
        }

        private void buttonOffPc_Click(object sender, RoutedEventArgs e)
        {
            ButtonControl.userChoice("H");
        }

        private void buttonBindFastOpen_Click(object sender, RoutedEventArgs e)
        {

            try
            {
                string selectItem = Regex.Match(input: comboBoxFastOpenAppList.SelectedItem.ToString(), @"\(([^)]*)\)").Groups[1].Value;

                string path = textBoxpathFastOpen.Text;

                if (!selectItem.Equals("") && !path.Equals(""))
                {

                    if (!System.IO.File.Exists(path))
                    {
                        MessageBox.Show("File not found");
                        return;
                    }

                    QuickLaunch quickLaunch = new QuickLaunch(selectItem, path);
                    FileSystem.setBindToFile(quickLaunch);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error buttonBindFastOpen_Click()" + ex.Message);
            }
        }

        private void comboBoxFastOpenAppList_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            ComboBox comboBox = (ComboBox)sender;
            string selectItem = Regex.Match(input: comboBox.SelectedItem.ToString(), @"\(([^)]*)\)").Groups[1].Value;
            List<QuickLaunch> list = FileSystem.parsBindFile();

            foreach (var item in list)
            {
                if (item.Signal.Equals(selectItem))
                {
                    labelBindPath.Visibility = Visibility.Visible;

                    labelBindPath.Content = "Path: " + item.Filepath.ToString();
                    return;
                }
                else
                    labelBindPath.Content = "Free to Bind";
            }

        }

        private void checkBox_Checked(object sender, RoutedEventArgs e)
        {
            FileSystem.setFileSettings(true);
            try
            {
                Microsoft.Win32.RegistryKey Key =
                Microsoft.Win32.Registry.LocalMachine.OpenSubKey(
                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", true);


                Key.SetValue(Title, Assembly.GetExecutingAssembly().Location);
                Key.Close();
            }catch(Exception ex)
            {
                Console.WriteLine("Error checkBox_Checked()" + ex.Message);
            }
        }

        private void checkBox_Unchecked(object sender, RoutedEventArgs e)
        {
            FileSystem.setFileSettings(false);
            try
            {
                Microsoft.Win32.RegistryKey key =
                Microsoft.Win32.Registry.LocalMachine.OpenSubKey(
           "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true);
                key.DeleteValue(Title, false);
                key.Close();
            }catch(Exception ex)
            {
                Console.WriteLine("Error checkBox_Unchecked()" + ex.Message);
            }
        }
    }
}

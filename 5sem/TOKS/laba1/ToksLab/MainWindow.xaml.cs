using System;
using System.IO.Ports;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;


namespace ToksLab
{
    public partial class MainWindow : Window
    {
        SerialPort serialPort = null;
        public string message = "";
        public bool init = false;

        public MainWindow()
        {
            InitializeComponent();
            UpdateTXT();
        }

        private void InputTXT_TextChanged(object sender, TextChangedEventArgs e)
        {
           
        }
        private void radioButton_Checked(object sender, RoutedEventArgs e)
        {
            if (init == false)
            {
                try
                {
                    if (InitializePort("COM1") == true){
                        DebugTXT.Text += "Initialization COM1 - success\n";
                        init = true;
                        radioButton.IsEnabled = false;
                        radioButton1.IsEnabled = false;
                    }
                    else {
                        DebugTXT.Text += "Initialization COM1 - Failed\n";
                        radioButton.IsChecked = false;
                    }
                }
                catch
                {
                    DebugTXT.Text += "Initialization COM1 - failed\n";
                }
            }
            else {
                DebugTXT.Text += "Init was done \n";
            }
        }

        private void radioButton1_Checked(object sender, RoutedEventArgs e)
        {
                try
                {
                    if (InitializePort("COM2") == true) { 
                    DebugTXT.Text += "Initialization COM2 - success\n";
                    init = true;
                    radioButton.IsEnabled = false;
                    radioButton1.IsEnabled = false;
                    }
                    else
                    {
                        DebugTXT.Text += "Initialization COM2 - Failed\n";
                        radioButton1.IsChecked = false;
                }
                }
                catch
                {
                    DebugTXT.Text += "Initialization COM2 - failed\n";
                }
            }
        

        private void button_Click(object sender, RoutedEventArgs e)
        {
            OutputTXT.Clear();
        }

        private void ButtonSend_Click(object sender, RoutedEventArgs e)
        {
            if (!radioButton.IsChecked.Value == true && !radioButton1.IsChecked.Value == true)
            {
                DebugTXT.Text += "Send - failed. Choose ports\n";
                return;
            }
            try
            {
                string str = InputTXT.Text;
                if (Regex.IsMatch(str, @"\p{IsCyrillic}"))
                {
                    DebugTXT.Text += "Txt error. No Cyrillic symbols\n";
                    InputTXT.Clear();
                    return;
                }
                InputTXT.Clear();
                byte[] strByte = Encoding.ASCII.GetBytes(str);
                serialPort_SendData(strByte);
                DebugTXT.Text += "Send - success\n";
            }
            catch
            {
                DebugTXT.Text += "Send - failed\n";
            }
        }

        public bool InitializePort(string portName)
        {
            try
            {
                serialPort = new SerialPort(portName, 9600, Parity.None, 8, StopBits.One);
                serialPort.Open();
                serialPort.ErrorReceived += new SerialErrorReceivedEventHandler(serialPort_ErrorReceived);
                serialPort.DataReceived += new SerialDataReceivedEventHandler(serialPort_DataReceived);
                return true;
            }
            catch {
                serialPort.Close();
                return false;
            }
            
        }

        private void serialPort_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
        {
            DebugTXT.Text += "Received error\n";
            throw new NotImplementedException();
        }

        void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte[] data = new byte[serialPort.BytesToRead];
            serialPort.Read(data, 0, data.Length);
            message = System.Text.Encoding.UTF8.GetString(data);
        }
        void serialPort_SendData(byte[] data)
        {
            try
            {
                serialPort.RtsEnable = true;
                serialPort.Write(data, 0, data.Length);
                Thread.Sleep(100);
                serialPort.RtsEnable = false;
            }
            catch { 
                DebugTXT.Text += "Send error\n";
            }
        }
        async Task UpdateTXT()
        {
            while (true)
            {
                await Task.Delay(1000);
                MessTxt();
                Thread.Sleep(100);
            }
        }
        void MessTxt()
        {
            if (!message.Equals("")) 
            {
                OutputTXT.Text = message;
                message = "";
                DebugTXT.Text += "Received - succses\n";
            }
        }
    }
}

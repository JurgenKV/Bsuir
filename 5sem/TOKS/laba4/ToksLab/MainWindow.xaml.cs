using System.Collections.Generic;
using System.Linq;
using System;
using System.IO.Ports;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;

namespace ToksLab
{
    public partial class MainWindow : Window
    {
        SerialPort serialPort = null;
        public string message = "";
        public bool init = false;
       
        public bool isColl = false;
        public bool isBusy = false;
        public int myTry = 0;
        public int timeDelay = 0;
        public string countColl = "";
        public int attemptCount = 0;
        public int ResDelay = 0;
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
                    if (InitializePort("COM1") == true)
                    {
                        DebugTXT.Text += "Initialization COM1 - success\n";
                        init = true;
                        radioButton.IsEnabled = false;
                        radioButton1.IsEnabled = false;
                    }
                    else
                    {
                        DebugTXT.Text += "Initialization COM1 - Failed\n";
                        radioButton.IsChecked = false;
                    }
                }
                catch
                {
                    DebugTXT.Text += "Initialization COM1 - failed\n";
                }
            }
            else
            {
                DebugTXT.Text += "Init was done \n";
            }
            DebugTXT.ScrollToEnd();
        }

        private void radioButton1_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                if (InitializePort("COM2") == true)
                {
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
            DebugTXT.ScrollToEnd();
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
                //byte[] strByte = Encoding.ASCII.GetBytes(str);
                toBitTxt(str);
                DebugTXT.Text += "Send - success\n";
            }
            catch
            {
                DebugTXT.Text += "Send - failed\n";
            }
            DebugTXT.ScrollToEnd();
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
            catch
            {
                serialPort.Close();
                return false;
            }

        }

        private void serialPort_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
        {
            DebugTXT.Text += "\nReceived error\n";
            throw new NotImplementedException();
        }

        void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte[] data = new byte[serialPort.BytesToRead];
            serialPort.Read(data, 0, data.Length);
            string st = System.Text.Encoding.UTF8.GetString(data);
            ResDelay = 7000;
            message += ToDec(st);
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
            catch
            {
                DebugTXT.Text += "Send error\n";
                DebugTXT.ScrollToEnd();
            }
        }
        async Task UpdateTXT()
        {
            while (true)
            {
                await Task.Delay(1000);
                await Task.Delay(ResDelay);
                ResDelay = 0;
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
                DebugTXT.ScrollToEnd();
            }
        }
        //полином = 7
        public void toBitTxt(string mess)
        {
            var resultList = (from Match m in Regex.Matches(mess, @".{1," + 10 + "}")
                              select m.Value).ToList();
            for (int i = 0; i < resultList.Count(); i++)
            {
                byte[] strByte = Encoding.ASCII.GetBytes(resultList[i]);
                string st = ToBinary(strByte, "");

                while (st.Length < 80)
                {
                    st += "00000000";
                }
                resultList[i] = st;
            }

            for (int i = 0; i < resultList.Count(); i++)
            {
                    Thread.Sleep(50);
                    randomBusy();
                    if (isBusy)
                    {
                        DebugTXT.Text += "Channel is busy\n";
                        --i;
                        continue;
                    }
                    else
                    {
                        writeCh(resultList[i]);
                        Thread.Sleep(50);
                        randomCollision();
                        Thread.Sleep(8);
                        if (!isColl)
                        {
                            string s = ToDec(resultList[i]);
                            byte[] strByte = Encoding.ASCII.GetBytes(resultList[i]);

                            if (countColl.Equals(""))
                                DebugTXT.Text += "Sent - " + s + "\n";
                            else
                                DebugTXT.Text += "Sent - " + s + " : " + countColl + "\n";

                            countColl = "";
                            attemptCount = 0;
                       
                        serialPort_SendData(strByte);
                            continue;
                        }
                        else
                        {
                            countColl += "#";
                            attemptCount++;
                            if (attemptCount > 10)
                            {
                                DebugTXT.Text += "Error! attemptCount > 10";
                            }
                            else
                            {
                                int timeDelay = randomTimeDelay(attemptCount);
                                DebugTXT.Text += "Random Delay = " + timeDelay + "\n";
                                Thread.Sleep(timeDelay);
                                DebugTXT.Text += "Delay is over\n";
                                continue;
                            }
                            --i;
                        }
                    }
                if(attemptCount > 10)
                {
                    attemptCount = 0;
                    break;
                }
                
            }
            if (attemptCount < 10)
                DebugTXT.Text += "All data sended \n";
            resultList.Clear();
        }

        public static string ToBinary(Byte[] data, string s)
        {
            return string.Join(s, data.Select(byt => Convert.ToString(byt, 2).PadLeft(8, '0')));
        }
        private void randomBusy()
        {
            Random random = new Random();
            int rnd = random.Next(0, 99);
            if (rnd < 30)
                isBusy = true;
            else
                isBusy = false;
        }
        private void randomCollision()
        {
            Random random = new Random();
            int rnd = random.Next(0, 99);
            if (rnd < 30)
                isColl = true;
            else
                isColl = false;
        }

        private int randomTimeDelay(int countAtt)
        {
            Random random = new Random();
            int maxBorder = Convert.ToInt32(Math.Pow(2, countAtt));
            return random.Next(0, maxBorder);
        }

        public void writeCh(string data) { }

        private string ToDec(string s)
        {
            List<Byte> byteList = new List<Byte>();
            for (int i = 0; i < s.Length; i += 8)
            {
                byteList.Add(Convert.ToByte(s.Substring(i, 8), 2));
            }
            return Encoding.UTF8.GetString(byteList.ToArray());
        }

    }
}

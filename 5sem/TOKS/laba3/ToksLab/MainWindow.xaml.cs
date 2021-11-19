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
        public string poly = "10001001";
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
                byte[] strByte = Encoding.ASCII.GetBytes(str);
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
            string st;
            string showSt;
            string ss;
            st = System.Text.Encoding.UTF8.GetString(data);
            ss = st;
            st = st.Replace("[0]", "0");
            st = st.Replace("[1]", "1");
            st = Recovery(st);
            showSt = st;
            st = st.Substring(0, st.Length - 7);
            Dispatcher.Invoke((Action)(() => {
            DebugTXT.Text += ss.Substring(0,ss.Length - 7) + " : " + showSt.Substring(showSt.Length - 7) + "\n";
            DebugTXT.ScrollToEnd();
            }));
            
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
                await Task.Delay(5000);
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
                resultList[i] += "0000000";
                string XORed = XorFunc(resultList[i]);
                resultList[i] = resultList[i].Remove(resultList[i].Length - 7, 7) + XORed;
            }
            randomMistake(resultList);
            for (int i = 0; i < resultList.Count(); i++)
            {
                byte[] strByte = Encoding.ASCII.GetBytes(resultList[i]);
                serialPort_SendData(strByte);
            }
            resultList.Clear();
        }

        public static string ToBinary(Byte[] data, string s)
        {
            return string.Join(s, data.Select(byt => Convert.ToString(byt, 2).PadLeft(8, '0')));
        }

        public void randomMistake(List<string> list)
        {
            Random random = new Random();
            for (int i = 0; i < list.Count(); i++)
            {
                int rnd = random.Next(0, 99);
                if (rnd < 30)
                {
                    int pos = random.Next(0, list[i].Length - 7);
                    string s = list[i];
                    if (s[pos] == '1')
                    {
                        s = s.Remove(pos, 1);
                        s = s.Insert(pos, "[0]");
                    }
                    else
                    {
                        s = s.Remove(pos, 1);
                        s = s.Insert(pos, "[1]");
                    }

                    DebugTXT.Text += "Mistake generated\n";
                    CoderBox.Text += s + "\n\n";
                    
                    list[i] = s;
                }
                CoderBox.Text += list[i] + "\n\n";
                CoderBox.ScrollToEnd();
                DebugTXT.ScrollToEnd();
            }
        }
        private string Recovery(string s)
        {
            string test = XorFunc(s);
            if (test == "0000000")
            {
                return s;
            }
            else
            {
                int temp = 0;
                while (true)
                {
                    int counter = 0;
                    if (temp == s.Length)
                    {
                        return s;
                    }
                    for (int i = 0; i < 7; i++)
                    {
                        if (test[i] == '1')
                        {
                            counter++;
                        }
                    }
                    if (counter == 1)
                    {
                        int a = s.Length - 7;
                        for (int i = 0; i < 7; i++)
                        {
                            if (test[i] == s[a + i])
                            {
                                s = s.Remove(a + i, 1);
                                s = s.Insert(a + i, "0");
                            }
                            else
                            {
                                s = s.Remove(a + i, 1);
                                s = s.Insert(a + i, "1");
                            }
                        }
                        for (int i = 0; i < temp; i++)
                        {
                            char symb = s[s.Length - 1];
                            s = s.Remove(s.Length - 1, 1);
                            s = s.Insert(0, symb.ToString());
                        }
                        return s;
                    }
                    else if (counter > 1)
                    {
                        char symb = s[0];
                        s = s.Remove(0, 1);
                        s = s.Insert(s.Length, symb.ToString());
                        test = XorFunc(s);
                        temp++;
                    }
                }
            }
        }
        private string XorFunc(string s)
        {
            while (true)
            {
                int a = s.IndexOf("1", 0);
                if (a > s.Length - 8 || a == -1)
                    break;
                for (int i = 0; i < 8; i++)
                {
                    if (poly[i] == s[a + i])
                    {
                        s = s.Remove(a + i, 1);
                        s = s.Insert(a + i, "0");
                    }
                    else
                    {
                        s = s.Remove(a + i, 1);
                        s = s.Insert(a + i, "1");
                    }
                }
            }
            s = s.Substring(s.Length - 7);
            return s;
        }
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

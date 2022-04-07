using NAudio.CoreAudioApi;
using NAudio.Mixer;
using NAudio.Wave;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Management.Automation.Runspaces;
using System.Reflection;

namespace ButtonBox
{
    class WindowsSound
    {

        static public bool muteMicro()
        {
            try
            {
                for (int deviceIndex = 0; deviceIndex < NAudio.Wave.WaveIn.DeviceCount; deviceIndex++)
                {
                    var device = WaveIn.GetCapabilities(deviceIndex);
                }

                var waveInEvent = new NAudio.Wave.WaveInEvent();

                //получить миксер аудио устройства ввода по умолчанию
                var mixer = waveInEvent.GetMixerLine();
                var muteControl = mixer.Controls.FirstOrDefault(x => x.ControlType == NAudio.Mixer.MixerControlType.Mute) as BooleanMixerControl;
                // вкл/выкл
                if (muteControl.Value.Equals(true))
                {
                    muteControl.Value = false;
                    return false;
                }
                else
                {
                    muteControl.Value = true;
                    return true;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error muteControlMicro()" + ex.Message);
            }
            return false;
        }

        static public void OutputDevicetoToChangeToFile()
        {
            try
            {
                MMDeviceEnumerator _DeviceEnumerator = new MMDeviceEnumerator();
                var enumerator = new MMDeviceEnumerator();
                var devices = _DeviceEnumerator.EnumerateAudioEndPoints(DataFlow.Render, DeviceState.Active);

                var AudioOutDefaultDevice = _DeviceEnumerator.GetDefaultAudioEndpoint(DataFlow.Render, Role.Multimedia);
                List<string> names = new List<string>();

                StreamWriter fileDevList = new StreamWriter("OutputDeviceList.bin");
                foreach (var device in devices)
                {
                    fileDevList.WriteLine(device.ID.ToString());

                }
                fileDevList.Close();
                StreamWriter currDev = new StreamWriter("CurrOutputDevice.bin");
                currDev.WriteLine(AudioOutDefaultDevice.ID.ToString());
                currDev.Close();

                _DeviceEnumerator.Dispose();
                enumerator.Dispose();
                AudioOutDefaultDevice.Dispose();
            }
            catch (Exception e)
            {
                Console.WriteLine("Error OutputDevicetoToChangeToFile() " + e.Message);
            }

        }

        static public void outputDevice()
        {
            OutputDevicetoToChangeToFile();
            try
            {
                string[] devList = File.ReadAllLines("OutputDeviceList.bin");

                StreamReader fileCurrDev = new StreamReader("CurrOutputDevice.bin");
                string currDev = fileCurrDev.ReadLine();
                fileCurrDev.Close();

                int next = 0;

                for (int i = 0; i < devList.Length; i++)
                {
                    if (devList[i].Equals(currDev))
                    {
                        next = i + 1;
                        if (next >= devList.Length)
                        {
                            next = 0;
                            break;
                        }
                        break;
                    }
                }

                StreamWriter fileCurrDevWR = new StreamWriter("CurrOutputDevice.bin");
                fileCurrDevWR.WriteLine(devList[next]);
                fileCurrDevWR.Close();

                InitialSessionState iss = InitialSessionState.CreateDefault();
                iss.ImportPSModule(new string[]
                {
                     Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "AudioDeviceCmdlets.dll")
                });

                Runspace runspace = RunspaceFactory.CreateRunspace(iss);
                runspace.Open();

                Pipeline pipeline = runspace.CreatePipeline();

                Command command_set = new Command("Set-AudioDevice");

                CommandParameter param_set = new CommandParameter("ID", devList[next]);
                command_set.Parameters.Add(param_set);
                pipeline.Commands.Add(command_set);
                var results = pipeline.Invoke();

            }
            catch (Exception e)
            {
                Console.WriteLine("Error outputDeviceChange() " + e.Message);
            }
        }
    }
}

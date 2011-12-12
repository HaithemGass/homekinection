using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Data;
using System.ComponentModel;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;


namespace HomeKinection
{
    [Serializable]
    public class IRControlModule : ModuleBox
	{	
        [NonSerialized]
		private NetworkProtocol.IRCommandData Packet;
		
		[Serializable]
		public class IRCommand
		{
			public IRCommand()
			{
			    sequence = new UInt16[128];	
			}
			public IRCommand(String n, UInt16[] s, byte l)
			{
				name = n;
				sequence = s;
                length = l;
			}

            public unsafe IRCommand(NetworkProtocol.remoteSequence command)
            {

                sequence = new UInt16[128];
                for (int i = 0; i < command.length; i++)
                {
                    sequence[i] = command.transitions[i];
                }
                length = command.length;
            }

            public unsafe IRCommand(String n, NetworkProtocol.remoteSequence command)
            {
                name = n;
                for (int i = 0; i < command.length; i++)
                {
                   sequence[i] = command.transitions[i];
                }
                length = command.length;
            }
			
			public UInt16[] sequence;
			public String name
			{
				get;
				set;
			}
			
			public String toString()
			{
				return name;
			}

            public byte length
            {
                get;
                set;
            }
		}
		
		
		public static Dictionary<String, Dictionary<String, IRCommand>> CommandList = new Dictionary<String, Dictionary<String, IRCommand>>();	
		
		public Dictionary<String, Dictionary<String, IRCommand>>.KeyCollection DeviceList
		{
			get
			{
				return CommandList.Keys;
			}
		}
		
		public static Dictionary<String, Dictionary<String, IRCommand>>.KeyCollection getDeviceList()
		{
			return CommandList.Keys;
		}
		
		public static Dictionary<String, IRCommand>.KeyCollection getCommandsForDevice(String device)
		{
			if(!CommandList.ContainsKey(device)) return null;
			return CommandList[device].Keys;
		}
		
		public static void addCommand(String device, IRCommand command)
		{
			if(CommandList.ContainsKey(device))
			{
				if(!CommandList[device].ContainsKey(command.name))
				{
					CommandList[device].Add(command.name, command);				
				}
				else
				{
					CommandList[device][command.name] =  command;									
				}
			}
			else
			{
				CommandList.Add(device, new Dictionary<String, IRCommand>());				
				CommandList[device].Add(command.name, command);
			}
			SerializeCommandList();
		}
		
		public static void addDevice(String device)
		{
			if(!CommandList.ContainsKey(device))CommandList.Add(device, new Dictionary<String, IRCommand>());
		}
		
		public static void addCommand(String device, String name, NetworkProtocol.remoteSequence command)
		{
			
			addCommand(device,new IRCommand(name, command));
		}
		
		public NetworkProtocol.IRCommandData packet{
            get
            {
                return Packet;
            }
            set
            {
                Packet = value;
                if (NetworkProtocol.Initialized) NetworkProtocol.sendIRMessage(address, Packet);
                NotifyPropertyChanged("packet");
            }						
        }
		
		public unsafe void sendCommand(IRCommand command)
		{
			NetworkProtocol.IRCommandData packet = new NetworkProtocol.IRCommandData();
            for (int i = 0; i < command.sequence.Length; i++)
            {
                packet.sequence.transitions[i] = command.sequence[i];
            }
            packet.sequence.length = command.length;
			
			packet.record = System.Convert.ToByte(0);
            if (NetworkProtocol.Initialized) NetworkProtocol.sendIRMessage(address, packet);
		}
		
		public unsafe void sendCommand(String device, String command)
		{
			NetworkProtocol.IRCommandData packet = new NetworkProtocol.IRCommandData();
			if(CommandList.ContainsKey(device) && CommandList[device].ContainsKey(command))
			{
                for (int i = 0; i < CommandList[device][command].sequence.Length; i++)
                {
                    packet.sequence.transitions[i] = CommandList[device][command].sequence[i];
                }
                packet.sequence.length = CommandList[device][command].length;

				packet.record = System.Convert.ToByte(0);
                if (NetworkProtocol.Initialized) NetworkProtocol.sendIRMessage(address, packet);
			}			
		}
		
		public void recordNewCommand()
		{
			NetworkProtocol.IRCommandData packet = new NetworkProtocol.IRCommandData();			
			packet.record = System.Convert.ToByte(1);
            if (NetworkProtocol.Initialized) NetworkProtocol.sendIRMessage(address, packet);
		}
		
		public static void SerializeCommandList()
        {
			
            String filename =  commonFilePath + "\\IRLibrary\\Library.cl"; //use a .md file for modules
            Stream stream = File.Open(filename, FileMode.Create);
            BinaryFormatter bFormatter = new BinaryFormatter();
			Dictionary<String, Dictionary<String, IRCommand>> listToSerialize = CommandList;
            bFormatter.Serialize(stream, listToSerialize);
            stream.Close();
        }

        public static void DeSerializeCommandList()
        {
			String filename =  commonFilePath + "\\IRLibrary\\Library.cl";

            try
            {
                Stream stream = File.Open(filename, FileMode.Open);
                BinaryFormatter bFormatter = new BinaryFormatter();
                CommandList = (Dictionary<String, Dictionary<String, IRCommand>>)bFormatter.Deserialize(stream);
                stream.Close();
            }catch(Exception e)
            {

            }

        }		
		
		public IRControlModule()
		{
		}
	}
}
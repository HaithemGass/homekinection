using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Data;
using System.ComponentModel;

namespace HomeKinection
{
    [Serializable]
    public class DimmerControlModule : ModuleBox
	{   
        [NonSerialized]
		private NetworkProtocol.DimmerCommandData packet;
		public int intensity{
            get
            {
                return packet.intensity;
            }
            set
            {                
				packet.intensity = System.Convert.ToByte(value);
                if (NetworkProtocol.Initialized && updateModule) NetworkProtocol.sendDimmerMessage(address, packet);
                NotifyPropertyChanged("intensity");
            }						
        }
		
		public bool fadeToValue{
            get
            {
                return (packet.fadeToValue == 1);
            }
            set
            {
                packet.fadeToValue = (value) ? ((byte)1) : ((byte)0);				
                NotifyPropertyChanged("fadeToValue");
            }						
        }
		
		
		public DimmerControlModule()
		{
			updateModule = true;
		}

        public override unsafe void handleStatusUpdate(NetworkProtocol.StatusMessageData packet)
        {
            updateModule = false;
            intensity = packet.message[0];
            updateModule = true;
        }
	}
}
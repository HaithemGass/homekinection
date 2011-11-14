using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Data;
using System.ComponentModel;

namespace HomeKinection
{
    public class ShadeControlModule : ModuleBox
	{
		
		public NetworkProtocol network;
		private NetworkProtocol.ShadeCommandData Packet;		
		public NetworkProtocol.ShadeCommandData packet{
            get
            {
                return Packet;
            }
            set
            {
                Packet = value;
				if(network != null) network.sendShadeMessage(address, Packet);
                NotifyPropertyChanged("packet");
            }						
        }
		public ShadeControlModule()
		{
		}
	}
}
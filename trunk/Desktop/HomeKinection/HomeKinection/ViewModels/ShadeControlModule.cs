using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Data;
using System.ComponentModel;

namespace HomeKinection
{
    [Serializable]
    public class ShadeControlModule : ModuleBox
	{	
        [NonSerialized]
		private NetworkProtocol.ShadeCommandData Packet;		
		public NetworkProtocol.ShadeCommandData packet{
            get
            {
                return Packet;
            }
            set
            {
                Packet = value;
                if (NetworkProtocol.Initialized) NetworkProtocol.sendShadeMessage(address, Packet);
                NotifyPropertyChanged("packet");
            }						
        }
		public ShadeControlModule()
		{
		}
	}
}
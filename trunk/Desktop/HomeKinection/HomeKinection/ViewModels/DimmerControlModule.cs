using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Data;
using System.ComponentModel;

namespace HomeKinection
{
    public class DimmerControlModule : ModuleBox
	{
        private int inten;
		public NetworkProtocol network;
		private NetworkProtocol.DimmerCommandData packet;
		public int intensity{
            get
            {
                return inten;
            }
            set
            {
                inten = value;
				packet.intensity = System.Convert.ToByte(value);
				if(network != null) network.sendDimmerMessage(address, packet);
                NotifyPropertyChanged("intensity");
            }						
        }
		
		
		public DimmerControlModule()
		{
		}
		public DimmerControlModule(NetworkProtocol net, String n, String l, UInt16 sa, UInt64 u, MODULE_TYPE t)
		{
            network = net;
            name = n;
            location = l;
            address = sa;
            uid = u;
            type = t;

		}
	}
}
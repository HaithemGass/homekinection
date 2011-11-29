using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Data;
using System.ComponentModel;

namespace HomeKinection
{
    [Serializable]
    public class HIDControlModule : ModuleBox
	{
		[NonSerialized]
		private NetworkProtocol.HIDCommandData Packet;		
		public NetworkProtocol.HIDCommandData packet{
            get
            {
                return Packet;
            }
            set
            {
                Packet = value;
                if (NetworkProtocol.Initialized) NetworkProtocol.sendHIDMessage(address, Packet);
                NotifyPropertyChanged("packet");
            }						
        }
		
		private double x, y;
		
		private int waitCount = 0;
        private bool doubleClick = false;
		private const int CLICK_WAIT = 75;
		
		
		public override void updateAbsoluteControl(SkeletalGesturePoint sgp)
		{
			NetworkProtocol.HIDCommandData hidPacket = new NetworkProtocol.HIDCommandData();
			
			y = 100 * RecognitionEngine.VerticalExtent(SkeletalGesture.TransformPoint(sgp));

			x = 100 * RecognitionEngine.HorizontalExtent(SkeletalGesture.TransformPoint(sgp));		

			hidPacket.mouseData.Wheel = 0;
			hidPacket.mouseData.mouseButtons = 0;

		

			if (Math.Sqrt(Math.Pow((x - 50), 2) + Math.Pow((y - 50), 2)) > 10)
			{
                double magnitude = Math.Sqrt(Math.Pow((x - 50), 2) + Math.Pow((y - 50), 2)) - 10;
                double direction = Math.Atan2((y - 50), (x- 50));
                hidPacket.mouseData.X = (byte)(System.Convert.ToSByte(magnitude * Math.Cos(direction)));
                hidPacket.mouseData.Y = (byte)(System.Convert.ToSByte(-magnitude * Math.Sin(direction)));	
				waitCount = 0;
                doubleClick = false;
				packet =  hidPacket;												
			}
			else
			{
				waitCount ++;
				if(waitCount > CLICK_WAIT)
				{
					hidPacket.mouseData.mouseButtons = 1;
					hidPacket.mouseData.X = 0;
					hidPacket.mouseData.Y = 0;
					packet =  hidPacket;                    
                    if(doubleClick)
                    {
                        waitCount = CLICK_WAIT-1;
                    }
                    else
                    {
                        waitCount = 0;
                    }
                    doubleClick = !doubleClick;
					
				}
			}
		}
		
		
		public HIDControlModule()
		{
		}
	}
}
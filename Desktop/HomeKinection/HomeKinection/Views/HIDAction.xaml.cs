using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace HomeKinection
{
	/// <summary>
	/// Interaction logic for HIDAction.xaml
	/// </summary>
	public partial class HIDAction : UserControl
	{
		public ModuleAction action;
		public HIDAction(UInt16 addr)
		{						
			this.InitializeComponent();
			action = (ModuleAction)this.FindResource("dataModel");
			action.packet.addr = addr;
			action.packet.moduleType = (byte)MODULE_TYPE.HID_MODULE;
			action.packet.messageType = (byte) NetworkProtocol.MESSAGE_TYPE.MESSAGE_TYPE_CONTROL;
		}
		
		
		private unsafe void EnsureUnchecked(object sender, System.Windows.RoutedEventArgs e)
		{
			if(sender == StopMouseButton) 
			{				
				DesktopButton.IsChecked = false;
				SwitchButton.IsChecked = false;
				MouseButton.IsChecked = false;
				action.enableAbsoluteControl = false;
			}
			
			if(sender == MouseButton) 
			{				
				DesktopButton.IsChecked = false;
				SwitchButton.IsChecked = false;
				StopMouseButton.IsChecked = false;
				action.enableAbsoluteControl = true;
			}
			if(sender == SwitchButton)
			{				
				MouseButton.IsChecked = false;
				DesktopButton.IsChecked = false;
				StopMouseButton.IsChecked = false;
				NetworkProtocol.HIDCommandData packet = new NetworkProtocol.HIDCommandData();
				
				
				packet.mouseData.mouseButtons = 0;
				packet.mouseData.X = 0;
				packet.mouseData.Y = 0;
				packet.mouseData.Wheel = 0;
				packet.keySequence.length = 1;
				NetworkProtocol.Key key = new NetworkProtocol.Key();
				key.shiftcode = 0x04;
				key.character = 0x2B;
				packet.keySequence.keys[0] = NetworkProtocol.translateKey(key);
				
				action.packet.hidPacket = packet;
				
			}
			if(sender == DesktopButton) 
			{
				
				MouseButton.IsChecked = false;
				SwitchButton.IsChecked = false;
				StopMouseButton.IsChecked = false;
				NetworkProtocol.HIDCommandData packet = new NetworkProtocol.HIDCommandData();
				
				
				packet.mouseData.mouseButtons = 0;
				packet.mouseData.X = 0;
				packet.mouseData.Y = 0;
				packet.mouseData.Wheel = 0;
				packet.keySequence.length = 1;
				NetworkProtocol.Key key = new NetworkProtocol.Key();
				key.shiftcode = 0x08;
				key.character = 0x07;
				packet.keySequence.keys[0] = NetworkProtocol.translateKey(key);
				
				action.packet.hidPacket = packet;
			}
		}
		
	}
}
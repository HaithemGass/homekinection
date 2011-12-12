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
	/// Interaction logic for IRAction.xaml
	/// </summary>
	public partial class IRAction : UserControl
	{
		public ModuleAction action;
		public IRAction(UInt16 addr)
		{						
			this.InitializeComponent();
			action = (ModuleAction)this.FindResource("dataModel");
			action.packet.addr = addr;
			action.packet.moduleType = (byte)MODULE_TYPE.IR_MODULE;
			action.packet.messageType = (byte) NetworkProtocol.MESSAGE_TYPE.MESSAGE_TYPE_CONTROL;
		}
		
		private void DeviceBox_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
		{
			
			CommandBox.Items.Clear();
            if (DeviceBox.SelectedItem == null) return;
			foreach (String command in IRControlModule.getCommandsForDevice(DeviceBox.SelectedItem.ToString()))
			{
				CommandBox.Items.Add(command);
			}
		}

		private void ReloadDeviceList(object sender, System.EventArgs e)
		{
			DeviceBox.Items.Clear();			
			foreach (String device in IRControlModule.getDeviceList())
			{
				DeviceBox.Items.Add(device);
			}
		}

		private void CommandBox_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
		{
			// TODO: Add event handler implementation here.
			if(DeviceBox.SelectedIndex < 0 || CommandBox.SelectedIndex < 0) return;
			setCommand(DeviceBox.SelectedItem.ToString(),CommandBox.SelectedItem.ToString());
		}
		
		private unsafe void setCommand(String device, String command)
		{
			
			NetworkProtocol.IRCommandData packet = new NetworkProtocol.IRCommandData();	
			for (int i = 0; i < IRControlModule.CommandList[device][command].sequence.Length; i++)
			{
				packet.sequence.transitions[i] = IRControlModule.CommandList[device][command].sequence[i];
			}
			packet.sequence.length = IRControlModule.CommandList[device][command].length;
		
			packet.record = System.Convert.ToByte(0);
			
			action.packet.irPacket = packet;
                
						
		}
	}
}
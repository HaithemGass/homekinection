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
	/// Interaction logic for ShadeAction.xaml
	/// </summary>
	public partial class ShadeAction : UserControl
	{   

		public ModuleAction action;
		public ShadeAction(UInt16 addr)
		{						
			this.InitializeComponent();
			action = (ModuleAction)this.FindResource("dataModel");
			action.packet.addr = addr;
			action.packet.moduleType = (byte)MODULE_TYPE.SHADE_MODULE;
			action.packet.messageType = (byte) NetworkProtocol.MESSAGE_TYPE.MESSAGE_TYPE_CONTROL;
		}

		private void EnsureUnchecked(object sender, System.Windows.RoutedEventArgs e)
		{
			if(sender == DownButton) 
			{
				action.packet.shadePacket.ButtonMask = NetworkProtocol.SHADE_DIRECTION_DOWN;
				StopButton.IsChecked = false;
				UpButton.IsChecked = false;
			}
			if(sender == UpButton)
			{
				action.packet.shadePacket.ButtonMask = NetworkProtocol.SHADE_DIRECTION_UP;
				DownButton.IsChecked = false;
				StopButton.IsChecked = false;
			}
			if(sender == StopButton) 
			{
				action.packet.shadePacket.ButtonMask = 0;
				UpButton.IsChecked = false;
				DownButton.IsChecked = false;
			}
		}

		private void UpdateDuration(object sender, System.Windows.Controls.TextChangedEventArgs e)
		{
			TextBox t = sender as TextBox;
			try
			{
				action.packet.shadePacket.Duration = System.Convert.ToUInt16(System.Convert.ToDouble(t.Text)*1000);
			}
			catch(System.FormatException ex)
			{
                System.Console.Error.WriteLine(ex);	
			}
		}
	}
}
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
	/// Interaction logic for ShadeControl.xaml
	/// </summary>
	public partial class ShadeControl : UserControl
	{
		private ShadeControlModule module;
		private ItemsControl UIModuleList;
		public ShadeControl(ItemsControl uml, String n, String l, UInt16 sa, UInt64 u, MODULE_TYPE t)
		{
			this.InitializeComponent();
			module = (ShadeControlModule)this.FindResource("dataModel");
			
			module.address = sa;
			module.name = n;			
			module.location = l;
			module.uid = u;
			module.type = t;
			
			UIModuleList = uml;
		}

		private void UpPress(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			NetworkProtocol.ShadeCommandData packet = new NetworkProtocol.ShadeCommandData();
			packet.Duration = 0;
			packet.ButtonMask = NetworkProtocol.SHADE_DIRECTION_UP;
			module.packet = packet;
		}
		private void ButtonRelease(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			NetworkProtocol.ShadeCommandData packet = new NetworkProtocol.ShadeCommandData();
			packet.Duration = 0;
			packet.ButtonMask = 0;
			module.packet = packet;
		}

		private void DownPress(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			NetworkProtocol.ShadeCommandData packet = new NetworkProtocol.ShadeCommandData();
			packet.Duration = 0;
			packet.ButtonMask = NetworkProtocol.SHADE_DIRECTION_DOWN;
			module.packet = packet;
		}

		private void EditModule(object sender, System.Windows.RoutedEventArgs e)
		{
			this.NameTextBox.Visibility = System.Windows.Visibility.Visible;
			this.LocationTextBox.Visibility = System.Windows.Visibility.Visible;
            this.SaveButton.Visibility = System.Windows.Visibility.Visible;

            module.DeleteModuleFile();
		}

		private void SaveModule(object sender, System.Windows.RoutedEventArgs e)
		{
			this.NameTextBox.Visibility = System.Windows.Visibility.Hidden;
			this.LocationTextBox.Visibility = System.Windows.Visibility.Hidden;
			this.SaveButton.Visibility = System.Windows.Visibility.Hidden;

            module.Serialize();
		}

		private void DeleteModule(object sender, System.Windows.RoutedEventArgs e)
		{
            module.DeleteModuleFile();
            ModuleBox.modules.Remove(module.uid);
            UIModuleList.Items.Remove(this);
		}
	}
}
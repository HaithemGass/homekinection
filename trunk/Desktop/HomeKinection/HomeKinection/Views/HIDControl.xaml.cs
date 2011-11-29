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
    public partial class HIDControl : UserControl
    {
        private HIDControlModule module;
		private ItemsControl UIModuleList;
        public HIDControl(ItemsControl uml, String n, String l, UInt16 sa, UInt64 u, MODULE_TYPE t)
        {
            this.InitializeComponent();
            module = (HIDControlModule)this.FindResource("dataModel");

            module.address = sa;
            module.name = n;            
            module.location = l;
            module.uid = u;
            module.type = t;

			UIModuleList = uml;
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

		private unsafe void SwitchApplications(object sender, System.Windows.RoutedEventArgs e)
		{
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
            module.packet = packet;
			
		}
				

		private unsafe void ShowDesktop(object sender, System.Windows.RoutedEventArgs e)
		{
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
			module.packet = packet;
			
		}  
    }
}
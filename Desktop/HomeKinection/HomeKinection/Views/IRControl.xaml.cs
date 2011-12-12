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
	public partial class IRControl : UserControl
	{
		private IRControlModule module;
		private ItemsControl UIModuleList;
		public IRControl(ItemsControl uml, String n, String l, UInt16 sa, UInt64 u, MODULE_TYPE t)
		{
			this.InitializeComponent();
			module = (IRControlModule)this.FindResource("dataModel");
			
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

		private void sendCommandToModule(object sender, System.Windows.RoutedEventArgs e)
		{
			if(DeviceBox.SelectedIndex < 0 || CommandBox.SelectedIndex < 0) return;
			module.sendCommand(DeviceBox.SelectedItem.ToString(),CommandBox.SelectedItem.ToString());
		}

		private void recordNewCommand(object sender, System.Windows.RoutedEventArgs e)
		{
			module.recordNewCommand();
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
	}
}
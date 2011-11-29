﻿using System;
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
	/// Interaction logic for DimmerControl.xaml
	/// </summary>
	public partial class DimmerControl : UserControl
	{

        private DimmerControlModule module;
		private ItemsControl UIModuleList;
		public DimmerControl(ItemsControl uml, String n, String l, UInt16 sa, UInt64 u, MODULE_TYPE t)
		{
			this.InitializeComponent();
			module = (DimmerControlModule)this.FindResource("dataModel");

            module.address = sa;
            module.name = n;            
            module.location = l;
            module.uid = u;
            module.type = t;
			UIModuleList = uml;
			
		}

		private void adjustNumberPosition(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			intensityLabel.Margin = new Thickness( intensityLabel.Margin.Right, intensityLabel.Margin.Top, intensitySlider.Margin.Left - intensityLabel.Width -  intensitySlider.Value / 100 * (intensitySlider.Width - intensityLabel.Width/2) , intensityLabel.Margin.Bottom);
			
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
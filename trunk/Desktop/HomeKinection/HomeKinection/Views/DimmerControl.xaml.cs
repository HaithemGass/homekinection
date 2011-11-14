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
	/// Interaction logic for DimmerControl.xaml
	/// </summary>
	public partial class DimmerControl : UserControl
	{
		public DimmerControl(NetworkProtocol net, String n, String l, UInt16 sa, UInt64 u, MODULE_TYPE t)
		{
			this.InitializeComponent();
			DimmerControlModule dc = (DimmerControlModule)this.FindResource("dataModel");
			
			dc.address = sa;			
			dc.name = n;
			dc.network = net;
			dc.location = l;
			dc.uid = u;
			dc.type = t;
			
		}
	
	}
}
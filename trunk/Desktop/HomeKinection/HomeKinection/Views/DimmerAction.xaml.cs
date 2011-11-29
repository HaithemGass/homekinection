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
    /// DoubleToIntegerValueConverter provides a two-way conversion between
    /// a double value and an integer.
    /// </summary>
    public class DoubleToIntegerValueConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter,
              System.Globalization.CultureInfo culture)
        {
            return System.Convert.ToInt32(value);
        }

        public object ConvertBack(object value, Type targetType,
            object parameter, System.Globalization.CultureInfo culture)
        {
            return System.Convert.ToDouble(value);
        }

    }
	
	/// <summary>
	/// Interaction logic for DimmerAction.xaml
	/// </summary>
	public partial class DimmerAction : UserControl
	{
		public ModuleAction action;
		public DimmerAction(UInt16 addr)
		{						
			this.InitializeComponent();
			action = (ModuleAction)this.FindResource("dataModel");
			action.packet.addr = addr;
			action.packet.moduleType = (byte)MODULE_TYPE.DIMMER_MODULE;
			action.packet.messageType = (byte) NetworkProtocol.MESSAGE_TYPE.MESSAGE_TYPE_CONTROL;
		}

        private void adjustNumberPosition(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
        {
            intensityLabel.Margin = new Thickness(intensityLabel.Margin.Left, intensityLabel.Margin.Top, intensitySlider.Margin.Right  + intensitySlider.Width - intensityLabel.Width - intensitySlider.Value / 100 * (intensitySlider.Width - intensityLabel.Width / 2), intensityLabel.Margin.Bottom);
			action.packet.dimmerPacket.intensity = System.Convert.ToByte(intensitySlider.Value);
        }

        private void SetFade(object sender, System.Windows.RoutedEventArgs e)
        {
        	action.packet.dimmerPacket.fadeToValue = (byte)(1);
        }
		private void ClearFade(object sender, System.Windows.RoutedEventArgs e)
        {
        	action.packet.dimmerPacket.fadeToValue = (byte)(0);
        }
	}
}
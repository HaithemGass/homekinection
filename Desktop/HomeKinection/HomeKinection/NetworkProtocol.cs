/*******************************************************************************
********************************************************************************
* YOU MUST CHANGE THE NETWORK PROTOCOL IN THE C CODE IF YOU CHANGE STUFF HERE  *
********************************************************************************
********************************************************************************/



using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.IO.Ports;
using System.Threading;
using System.Windows.Threading;
using System.Windows.Data;
using System.ComponentModel;

namespace HomeKinection
{

    public class ModuleBox : INotifyPropertyChanged
    {
        private String Name;

        public String name
        {
            get
            {
                return Name;
            }

            set
            {
                Name = value;
                NotifyPropertyChanged("name");
            }
        }

        private String Location;

        public String location
        {
            get
            {
                return Location;
            }

            set
            {
                Location = value;
                NotifyPropertyChanged("location");
            }
        }
        public UInt16 address{get;set;}
        public UInt64 uid{get;set;}
        public MODULE_TYPE type{get;set;}  
      

        #region INotifyPropertyChanged
        public event PropertyChangedEventHandler PropertyChanged;

        protected void NotifyPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }
        #endregion
    }


    public enum MODULE_TYPE
    {
        CONTROLLER_MODULE,
        DIMMER_MODULE,
        SHADE_MODULE,
        IR_MODULE,
        HID_MODULE,
        MODULE_TYPE_MAX
    };

    public class NetworkProtocol{

/*****************************************************************************
******************************************************************************
*                                                                            *
*                           STRUCTS AND ENUMS                                *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/

        public const byte DEVICE_MESSAGE_STATUS = 1;
        public const byte DEVICE_MESSAGE_DIMMER = 2;
        public const byte DEVICE_MESSAGE_SHADE =  4;
        public const byte DEVICE_MESSAGE_IR =  8;
        public const byte DEVICE_MESSAGE_HID = 16;
        public const byte DEVICE_MESSAGE_ALL  = (DEVICE_MESSAGE_STATUS|DEVICE_MESSAGE_DIMMER|DEVICE_MESSAGE_SHADE|DEVICE_MESSAGE_IR|DEVICE_MESSAGE_HID);

        public const int USART_DATA_SIZE = 128;

        public static Dictionary<byte, String> typeString = new Dictionary<byte,string>()
        {
        	{(byte)MODULE_TYPE.DIMMER_MODULE, "Dimmer Module"},	        
	        {(byte)MODULE_TYPE.HID_MODULE, "HID Module"},
	        {(byte)MODULE_TYPE.SHADE_MODULE, "Shade Module"},	 
            {(byte)MODULE_TYPE.IR_MODULE, "IR Module"},	 
        };

        enum NETWORK_ENDPOINT
        {
	        RESERVED,
             MODULE_STATUS,
	        DIMMER_CONTROL,
	        SHADE_CONTROL,
	        IR_CONTROL,
	        HID_CONTROL
        };

        enum MESSAGE_TYPE
        {
            MESSAGE_TYPE_STATUS,
            MESSAGE_TYPE_CONTROL,
            MESSAGE_TYPE_NETWORK
        };

        [StructLayout(LayoutKind.Explicit, Pack = 1)]
        public unsafe struct UsartMessagePacket
        {
            [FieldOffset(0)]
            public byte messageType;
            [FieldOffset(1)]
            public byte moduleType;
            [FieldOffset(2)]
            public UInt16 addr; 
            [FieldOffset(4)]
            public DimmerCommandData dimmerPacket;
            [FieldOffset(4)]
            public StatusMessageData statusPacket;
            [FieldOffset(4)]
            public NetworkJoinData networkPacket;
            [FieldOffset(4)]
            public IRCommandData irPacket;
            [FieldOffset(4)]
            public HIDCommandData hidPacket;
            [FieldOffset(4)]
            public ShadeCommandData shadePacket;
        } ;

        [StructLayout(LayoutKind.Explicit, Pack = 1, Size = 11)]
        public unsafe struct NetworkJoinData
        {
            [FieldOffset(0)]
            public byte deviceType;
            [FieldOffset(1)]
            public UInt16 shortAddress;
            [FieldOffset(3)]
            public UInt64 deviceUID;
        } ;

        [StructLayout(LayoutKind.Explicit, Pack = 1, Size = 86)]
	    public unsafe struct StatusMessageData
	    {
            [FieldOffset(0)]
		     public byte deviceType;
            [FieldOffset(1)]
             public byte statusMessageType;
            [FieldOffset(2)]
             public UInt16 shortAddress;
            [FieldOffset(4)]
             public UInt16 length;
            [FieldOffset(6)]
             public fixed byte message[80];
	    } ;		 	        
	
        [StructLayout(LayoutKind.Sequential,Pack=1)]        
	    public struct DimmerCommandData  
	    {
            public byte intensity;
	    };

        public const byte SHADE_DIRECTION_DOWN  = 2;
	    public const byte SHADE_DIRECTION_UP =  1;

        [StructLayout(LayoutKind.Explicit, Pack = 1, Size = 3)]  
	    public struct ShadeCommandData 
	    {
            [FieldOffset(0)]
            public byte ButtonMask;
            [FieldOffset(1)]
            public UInt16 Duration; // ms
	    };

        [StructLayout(LayoutKind.Explicit, Pack = 1, Size = 2)]  
	    public struct ShadeButtonStatus 
	    {
            [FieldOffset(0)]
            public byte UpButton;
            [FieldOffset(1)]
            public byte DownButton;
	    } ;
    	
        [StructLayout(LayoutKind.Sequential,Pack=1)]
        public struct remoteTransition  
	    {
            public UInt16 duration;
	    } ;

        [StructLayout(LayoutKind.Explicit,Pack=1, Size= 129)]
        public unsafe struct remoteSequence
	    {
            [FieldOffset(0)]
            public byte length;
            [FieldOffset(1)]
            public remoteTransition *transitions;            
	    } ;

        [StructLayout(LayoutKind.Explicit, Pack = 1, Size = 130)]  
	    public unsafe struct IRCommandData  
	    {
            [FieldOffset(0)]
            public byte remoteSequenceNumber;
            [FieldOffset(1)]
            public remoteSequence sequence;
	    };

        [StructLayout(LayoutKind.Explicit, Pack = 1, Size = 3)]  
	    public struct Key
	    {
            [FieldOffset(0)]
            public byte shiftcode;
            [FieldOffset(1)]
            public byte blank;//should always be 0x00
            [FieldOffset(2)]
            public byte character;		
	    } ;	        	

	    [StructLayout(LayoutKind.Explicit,Pack=1, Size=61)] 
	    public unsafe struct KeySequence
	    {
            [FieldOffset(0)]
            public byte length;

            [FieldOffset(1)]
            public Key *keys;
	    };

        [StructLayout(LayoutKind.Explicit, Pack = 1, Size = 4)]  
	    public struct MouseData
	    {
            [FieldOffset(0)]
            public byte mouseButtons;
            [FieldOffset(1)]
            public byte X;
            [FieldOffset(2)]
            public byte Y;
            [FieldOffset(3)]
            public byte Wheel;
	    };


        [StructLayout(LayoutKind.Explicit, Pack = 1, Size = 65)]  
	    public unsafe struct HIDCommandData 
	    {
            [FieldOffset(0)]
            public MouseData mouseData;
            [FieldOffset(4)]
            public KeySequence keySequence;
	    };


        [StructLayout(LayoutKind.Explicit, Pack = 1, Size = 4)]    
	    public struct HIDStatus 
	    {
            [FieldOffset(0)]
            public byte connected;
            [FieldOffset(1)]
            public byte usbSetup;
            [FieldOffset(2)]
            public byte keyBoardBusy;
            [FieldOffset(3)]
            public byte mouseBusy;
	    };


        /*****************************************************************************
        ******************************************************************************
        *                                                                            *
        *                               FUNCTIONS                                    *
        *                                                                            *
        *                                                                            *
        ******************************************************************************
        *****************************************************************************/
        SerialPort serialPort;
        MainWindow mw;

        public NetworkProtocol(MainWindow m)
        {
            mw = m;
            serialPort = new SerialPort();
            serialPort.BaudRate = 38400;
            serialPort.DataBits = 8;
            serialPort.StopBits = StopBits.One;

            if (SerialPort.GetPortNames().GetLength(0) >= 1)
            {
                serialPort.PortName = SerialPort.GetPortNames()[0];
                serialPort.Parity = Parity.None;
                serialPort.Handshake = Handshake.None;
                serialPort.DataReceived += new SerialDataReceivedEventHandler(dataReceivedHandler); ;
                serialPort.Open();
            }
        }


        

        private static bool UARTBusy = false;


        public void dataReceivedHandler(object sender,
                        SerialDataReceivedEventArgs e)
        {
            UsartMessagePacket packet;
            SerialPort sp = (SerialPort)sender;
            int cBytes = sp.BytesToRead;
            if (cBytes < Marshal.SizeOf(new UsartMessagePacket())) return;
            byte[] buffer = new byte[cBytes];
            sp.Read(buffer, 0, cBytes);

            //alright now to deserialize;
            packet = ReadStruct<UsartMessagePacket>(buffer);

            //now to handle it.
            if (packet.messageType == (byte)MESSAGE_TYPE.MESSAGE_TYPE_NETWORK)
            {
                mw.Dispatcher.Invoke(DispatcherPriority.Send,
                 new Action<UsartMessagePacket>(mw.HandleNetworkJoin), packet); // need to inform UI thread to make our popup;
            }

        }
    

        //helper function from http://www.developerfusion.com/article/84519/mastering-structs-in-c/
        private static byte[] RawSerialize(object anything)
        {
            int rawsize =
                Marshal.SizeOf(anything);
            byte[] rawdata = new byte[rawsize];
            GCHandle handle =
                GCHandle.Alloc(rawdata,
                GCHandleType.Pinned);
            Marshal.StructureToPtr(anything,
                handle.AddrOfPinnedObject(),
                false);
            handle.Free();
            return rawdata;
        }

        //helper function from http://www.developerfusion.com/article/84519/mastering-structs-in-c/
        public T ReadStruct<T>(byte[] buffer)
        {            
            GCHandle handle =
                GCHandle.Alloc(buffer,
                GCHandleType.Pinned);
            T temp = (T)
                Marshal.PtrToStructure(
                handle.AddrOfPinnedObject(),
                typeof(T));
            handle.Free();
            return temp;
        }


        //NEED TO DEFINE SENDING AND RECEIVING FUNCTIONS HERE!!!!!
        public unsafe void sendShadeMessage(UInt16 addr, ShadeCommandData packet)
        {
            if (UARTBusy) return;
            UARTBusy = true;
            Int32 size = Marshal.SizeOf(packet);
            UsartMessagePacket toSend = new UsartMessagePacket();
            toSend.messageType = (byte)(MESSAGE_TYPE.MESSAGE_TYPE_CONTROL);
            toSend.moduleType = (byte)(MODULE_TYPE.SHADE_MODULE);
            toSend.shadePacket = packet;
            toSend.addr = addr;    

            if (serialPort.IsOpen)
            {
                serialPort.Write(RawSerialize(toSend), 0, (int)Marshal.SizeOf(toSend));
            }
            UARTBusy = false;
        }


        public unsafe void sendHIDMessage(UInt16 addr, HIDCommandData packet)
        {
            if (UARTBusy) return;
            UARTBusy = true;
            Int32 size = Marshal.SizeOf(packet);
            UsartMessagePacket toSend = new UsartMessagePacket();
            toSend.messageType = (byte)(MESSAGE_TYPE.MESSAGE_TYPE_CONTROL);
            toSend.moduleType = (byte)(MODULE_TYPE.HID_MODULE);
            toSend.hidPacket = packet;
            toSend.addr = addr;    

            if (serialPort.IsOpen)
            {
                serialPort.Write(RawSerialize(toSend), 0, (int)Marshal.SizeOf(toSend));
            }
            UARTBusy = false;

        }

        public unsafe void sendDimmerMessage(UInt16 addr, DimmerCommandData packet)
        {
            if (UARTBusy) return;
            UARTBusy = true;
            Int32 size = Marshal.SizeOf(packet);
            UsartMessagePacket toSend = new UsartMessagePacket();
            toSend.messageType = (byte)(MESSAGE_TYPE.MESSAGE_TYPE_CONTROL);
            toSend.moduleType = (byte)(MODULE_TYPE.DIMMER_MODULE);
            toSend.addr = addr;            
            toSend.dimmerPacket = packet;

            if (serialPort.IsOpen)
            {
                serialPort.Write(RawSerialize(toSend), 0, (int)Marshal.SizeOf(toSend));
            }
            UARTBusy = false;

                
        }

    }

}

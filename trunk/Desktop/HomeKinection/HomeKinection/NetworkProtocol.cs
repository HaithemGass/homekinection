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

namespace HomeKinection
{

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


        enum MODULE_TYPE
        {
             CONTROLLER_MODULE,
	        DIMMER_MODULE,
	        SHADE_MODULE,
	        IR_MODULE,
	        HID_MODULE,
             MODULE_TYPE_MAX
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

        [StructLayout(LayoutKind.Explicit, Pack = 1)]
        public unsafe struct UsartMessagePacket
        {
            [FieldOffset(0)]
            public byte type; // Message type... will correspond to one of our other ___MessageData structs.... use our NetworkEndpoint to decide.		
            [FieldOffset(1)]
            public DimmerCommandData dimmerPacket;
            [FieldOffset(1)]
            public StatusMessageData statusPacket;
            [FieldOffset(1)]
            public IRCommandData irPacket;
            [FieldOffset(1)]
            public HIDCommandData hidPacket;
            [FieldOffset(1)]
            public ShadeCommandData shadePacket;
        } ;
	       

	    [StructLayout(LayoutKind.Sequential,Pack=1)]
	    public unsafe struct StatusMessageData
	    {
		     public byte deviceType;
             public byte statusMessageType;
             public UInt16 shortAddress;
             public UInt16 length;
             public fixed byte message[80];
	    } ;		 	        
	
        [StructLayout(LayoutKind.Sequential,Pack=1)]        
	    public struct DimmerCommandData  
	    {
            public byte intensity;
	    };

        public const byte SHADE_DIRECTION_DOWN  = 2;
	    public const byte SHADE_DIRECTION_UP =  1;

	    [StructLayout(LayoutKind.Sequential,Pack=1)]   
	    public struct ShadeCommandData 
	    {
            public byte ButtonMask;
            public UInt16 Duration; // ms
	    };

        [StructLayout(LayoutKind.Sequential,Pack=1)]
	    public struct ShadeButtonStatus 
	    {
            public byte UpButton;
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

        [StructLayout(LayoutKind.Sequential,Pack=1)]
	    public unsafe struct IRCommandData  
	    {
            public byte remoteSequenceNumber;
            public remoteSequence sequence;
	    };
	        		     	
        [StructLayout(LayoutKind.Sequential,Pack=1)] 
	    public struct Key
	    {
            public byte shiftcode;
            public byte blank;//should always be 0x00
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
	        	  	 
	    [StructLayout(LayoutKind.Sequential,Pack=1)] 
	    public struct MouseData
	    {
            public byte mouseButtons;
            public byte X;
            public byte Y;
            public byte Wheel;
	    }; 
             	
	 	 			 
        [StructLayout(LayoutKind.Sequential,Pack=1)] 	        
	    public unsafe struct HIDCommandData 
	    {
            public MouseData mouseData;
            public KeySequence keySequence;
	    };
	        
	
	    [StructLayout(LayoutKind.Sequential,Pack=1)]    
	    public struct HIDStatus 
	    {
            public byte connected;
            public byte usbSetup;
            public byte keyBoardBusy;
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

        public NetworkProtocol()
        {
            serialPort = new SerialPort();
            serialPort.BaudRate = 38400;
            serialPort.DataBits = 8;
            serialPort.StopBits = StopBits.One;

            if (SerialPort.GetPortNames().GetLength(0) >= 1)
            {
                serialPort.PortName = SerialPort.GetPortNames()[0];
                serialPort.Parity = Parity.None;
                serialPort.Handshake = Handshake.None;
                serialPort.Open();
            }
        }


        

        private static bool UARTBusy = false;

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



        //NEED TO DEFINE SENDING AND RECEIVING FUNCTIONS HERE!!!!!
        public unsafe void sendShadeMessage(ShadeCommandData packet)
        {
            if (UARTBusy) return;
            UARTBusy = true;
            Int32 size = Marshal.SizeOf(packet);
            UsartMessagePacket toSend = new UsartMessagePacket();
            toSend.type = (byte)(NETWORK_ENDPOINT.SHADE_CONTROL);
            toSend.shadePacket = packet;

            if (serialPort.IsOpen)
            {
                serialPort.Write(RawSerialize(toSend), 0, (int)Marshal.SizeOf(toSend));
            }
            UARTBusy = false;
        }

        
        public unsafe void sendHIDMessage(HIDCommandData packet)
        {
            if (UARTBusy) return;
            UARTBusy = true;
            Int32 size = Marshal.SizeOf(packet);
            UsartMessagePacket toSend = new UsartMessagePacket();
            toSend.type = (byte)(NETWORK_ENDPOINT.HID_CONTROL);
            toSend.hidPacket = packet;

            if (serialPort.IsOpen)
            {
                serialPort.Write(RawSerialize(toSend), 0, (int)Marshal.SizeOf(toSend));
            }
            UARTBusy = false;

        }

        public unsafe void sendDimmerMessage(DimmerCommandData packet)
        {
            if (UARTBusy) return;
            UARTBusy = true;
            Int32 size = Marshal.SizeOf(packet);
            UsartMessagePacket toSend = new UsartMessagePacket();
            toSend.type = (byte)(NETWORK_ENDPOINT.DIMMER_CONTROL);
            toSend.dimmerPacket = packet;

            if (serialPort.IsOpen)
            {
                serialPort.Write(RawSerialize(toSend), 0, (int)Marshal.SizeOf(toSend));
            }
            UARTBusy = false;

                
        }

    }

}

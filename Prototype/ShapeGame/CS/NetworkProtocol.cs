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

namespace ShapeGame
{

    static public class NetworkProtocol{

/*****************************************************************************
******************************************************************************
*                                                                            *
*                           STRUCTS AND ENUMS                                *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/

        public static const byte DEVICE_MESSAGE_STATUS = 1;
        public static const byte DEVICE_MESSAGE_DIMMER = 2;
        public static const byte DEVICE_MESSAGE_SHADE =  4;
        public static const byte DEVICE_MESSAGE_IR =  8;
        public static const byte DEVICE_MESSAGE_HID = 16;
        public static const byte DEVICE_MESSAGE_ALL  = (DEVICE_MESSAGE_STATUS|DEVICE_MESSAGE_DIMMER|DEVICE_MESSAGE_SHADE|DEVICE_MESSAGE_IR|DEVICE_MESSAGE_HID);


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

	    [StructLayout(LayoutKind.Sequential,Pack=1)]
	    public unsafe struct StatusMessageData
	    {
		        byte deviceType;
		        byte statusMessageType;
		        UInt16 shortAddress;
		        UInt16 length;
		        fixed byte message[80];
	    } ;

	
		[StructLayout(LayoutKind.Sequential,Pack=1)]        
	    public unsafe struct UsartMessagePacket
	    {
		    byte type; // Message type... will correspond to one of our other ___MessageData structs.... use our NetworkEndpoint to decide.		
	        fixed byte data[128]; // Where to shove the actual message
	    } ;
	        	        
	
        [StructLayout(LayoutKind.Sequential,Pack=1)]        
	    public struct DimmerCommandData  
	    {
		    byte intensity;
	    };

        public static const byte SHADE_DIRECTION_DOWN  = 2;
	    public static const byte SHADE_DIRECTION_UP =  1;

	    [StructLayout(LayoutKind.Sequential,Pack=1)]   
	    public struct ShadeCommandData 
	    {
		    byte ButtonMask;	
		    UInt16 Duration; // ms
	    };

        [StructLayout(LayoutKind.Sequential,Pack=1)]
	    public struct ShadeButtonStatus 
	    {
		    byte UpButton;
		    byte DownButton;
	    } ;
    	
        [StructLayout(LayoutKind.Sequential,Pack=1)]
        public struct remoteTransition  
	    {
		        UInt16 duration;
	    } ;

        [StructLayout(LayoutKind.Sequential,Pack=1)]
        public unsafe struct remoteSequence
	    {
		        fixed remoteTransition transitions[64];
		        UInt16 length;
	    } ;

        [StructLayout(LayoutKind.Sequential,Pack=1)]
	    public unsafe struct IRCommandData  
	    {
		        byte remoteSequenceNumber;
		        remoteSequence sequence;
	    };
	        		     	
        [StructLayout(LayoutKind.Sequential,Pack=1)] 
	    public struct Key
	    {		
		    byte shiftcode;
		    byte blank;//should always be 0x00
		    byte character;		
	    } ;	        	

	    [StructLayout(LayoutKind.Sequential,Pack=1)] 
	    public unsafe struct KeySequence
	    {
		    byte length;
		    fixed Key keys[20];
	    };
	        	  	 
	    [StructLayout(LayoutKind.Sequential,Pack=1)] 
	    public struct MouseData
	    {
		    byte mouseButtons;
		    byte X;
		    byte Y;
		    byte Wheel;
	    }; 
             	
	 	 			 
        [StructLayout(LayoutKind.Sequential,Pack=1)] 	        
	    public unsafe struct HIDCommandData 
	    {
		        MouseData mouseData;
		        KeySequence keySequence;
	    };
	        
	
	    [StructLayout(LayoutKind.Sequential,Pack=1)]    
	    public struct HIDStatus 
	    {
		        byte connected;
		        byte usbSetup;
		        byte keyBoardBusy;
		        byte mouseBusy;
	    };
	

        /*****************************************************************************
        ******************************************************************************
        *                                                                            *
        *                               FUNCTIONS                                    *
        *                                                                            *
        *                                                                            *
        ******************************************************************************
        *****************************************************************************/
        
        //NEED TO DEFINE SENDING AND RECEIVING FUNCTIONS HERE!!!!!

    }

}

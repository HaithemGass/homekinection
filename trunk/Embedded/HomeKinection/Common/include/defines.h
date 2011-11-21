/*******************************************************************************
********************************************************************************
* YOU MUST CHANGE THE NETWORK PROTOCOL IN THE C# CODE IF YOU CHANGE STUFF HERE *
********************************************************************************
********************************************************************************/

#define DEVICE_MESSAGE_STATUS 1
#define DEVICE_MESSAGE_DIMMER 2
#define DEVICE_MESSAGE_SHADE 4
#define DEVICE_MESSAGE_IR 8
#define DEVICE_MESSAGE_HID 16
#define DEVICE_MESSAGE_ALL (DEVICE_MESSAGE_STATUS|DEVICE_MESSAGE_DIMMER|DEVICE_MESSAGE_SHADE|DEVICE_MESSAGE_IR|DEVICE_MESSAGE_HID)



/*****************************************************************************
******************************************************************************
*                                                                            *
*                           STRUCTS AND ENUMS                                *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/
typedef enum
{
	APP_INTIALIZATION,
	APP_NETWORK_WAITING_TO_JOIN,
	APP_NETWORK_JOINED,
	APP_NETWORK_SEND_STATUS,
     APP_NETWORK_SEND_DIMMER,
     APP_NETWORK_SEND_SHADE,
	APP_NETWORK_SEND_IR,     
	APP_NETWORK_SEND_HID,
	APP_NETWORK_IDLE	
} AppStateMachine;

typedef enum
{
     CONTROLLER_MODULE,
	DIMMER_MODULE,
	SHADE_MODULE,
	IR_MODULE,
	HID_MODULE,
     MODULE_TYPE_MAX
} MODULE_TYPE;

typedef enum
{
	RESERVED,
     MODULE_STATUS,
	DIMMER_CONTROL,
	SHADE_CONTROL,
	IR_CONTROL,
	HID_CONTROL,
	MODULE_NETWORK_JOIN
} NETWORK_ENDPOINT;

//-------------------------Status
#if ((DEVICE_MESSAGE_SUPPORT & DEVICE_MESSAGE_STATUS) != 0)

	BEGIN_PACK
	typedef struct  
	{
		 uint8_t deviceType;
		 uint8_t statusMessageType;
		 uint16_t shortAddress;		 
		 size_t length;
		 uint8_t message[80];
	}PACK StatusMessageData;
	END_PACK
	
	BEGIN_PACK
	typedef struct  
	{
		 uint8_t deviceType;
		 uint16_t shortAddr;
		 uint64_t deviceUID;
	}PACK NetworkJoinData;
	END_PACK	
	
	
	BEGIN_PACK
	typedef struct
	{
		uint8_t header[APS_ASDU_OFFSET]; // Header
		StatusMessageData data; // Application data
		uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET]; //Footer
	} PACK StatusMessagePacket;
	END_PACK	
	
	
	BEGIN_PACK
	typedef struct
	{
		uint8_t header[APS_ASDU_OFFSET]; // Header
		NetworkJoinData data; // Application data
		uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET]; //Footer
	} PACK NetworkJoinPacket;
	END_PACK	
	
	static APS_RegisterEndpointReq_t statusEndpointParams;
     static SimpleDescriptor_t statusEndpoint = {MODULE_STATUS,1, 1, 1, 0, 0, NULL, 0, NULL};     
		 
	static APS_RegisterEndpointReq_t networkJoinEndpointParams;
     static SimpleDescriptor_t networkJoinEndpoint = {MODULE_NETWORK_JOIN,1, 1, 1, 0, 0, NULL, 0, NULL};     	 
	
#endif

//-------------------------DIMMER
#if ((DEVICE_MESSAGE_SUPPORT & DEVICE_MESSAGE_DIMMER) != 0)
	BEGIN_PACK
	typedef struct  
	{
		 uint8_t fadeToValue;
		 uint8_t intensity;
	}PACK DimmerCommandData;
	END_PACK
	
	BEGIN_PACK
	typedef struct
	{
		uint8_t header[APS_ASDU_OFFSET]; // Header
		DimmerCommandData data; // Application data
		uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET]; //Footer
	} PACK DimmerCommandPacket;
	END_PACK
	
	static APS_RegisterEndpointReq_t dimmerEndpointParams;
     static SimpleDescriptor_t dimmerEndpoint = {DIMMER_CONTROL,1, 1, 1, 0, 0, NULL, 0, NULL};     
#endif

//-------------------------SHADE
#if ((DEVICE_MESSAGE_SUPPORT & DEVICE_MESSAGE_SHADE) != 0)

     #define SHADE_DIRECTION_DOWN 2
	#define SHADE_DIRECTION_UP   1

	BEGIN_PACK
	typedef struct  
	{
		 uint8_t ButtonMask;	//0x01 - down (with a sickness)
								//0x02 - (pick me) up (before you go go)
		 uint16_t Duration; // ms
	}PACK ShadeCommandData;
	END_PACK
	
	BEGIN_PACK
	typedef struct
	{
		uint8_t header[APS_ASDU_OFFSET]; // Header
		 ShadeCommandData data;
		uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET]; //Footer
	} PACK ShadeCommandPacket;
	END_PACK
	
	BEGIN_PACK
	typedef struct  
	{
		 bool UpButton;
		 bool DownButton;
	}PACK ShadeButtonStatus;
	END_PACK
	
	static APS_RegisterEndpointReq_t shadeEndpointParams;
     static SimpleDescriptor_t shadeEndpoint = {SHADE_CONTROL,1, 1, 1, 0, 0, NULL, 0, NULL};
#endif

//-------------------------IR
#if ((DEVICE_MESSAGE_SUPPORT & DEVICE_MESSAGE_IR) != 0)
     
	BEGIN_PACK
     typedef struct  
	{
		 uint16_t duration;
	}PACK remoteTransition;
	END_PACK     
	 
	BEGIN_PACK
     typedef struct  
	{
		uint8_t length;
		remoteTransition transitions[64];		
	}PACK remoteSequence;
	END_PACK
	 
	BEGIN_PACK
	typedef struct  
	{
		 uint8_t record;		 
		 remoteSequence sequence;
	}PACK IRCommandData;
	END_PACK	
	
	BEGIN_PACK
	typedef struct
	{
		uint8_t header[APS_ASDU_OFFSET]; // Header
		IRCommandData data;
		uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET]; //Footer
	} PACK IRCommandPacket;
	END_PACK
	
	static APS_RegisterEndpointReq_t irEndpointParams;
     static SimpleDescriptor_t irEndpoint = {IR_CONTROL,1, 1, 1, 0, 0, NULL, 0, NULL};
#endif

//-------------------------HID
#if ((DEVICE_MESSAGE_SUPPORT & DEVICE_MESSAGE_HID) != 0)
     BEGIN_PACK
	typedef struct  
	{		
		uint8_t shiftcode;
		uint8_t blank;//should always be 0x00
		uint8_t character;		
	}PACK Key;
	END_PACK
	
	BEGIN_PACK
	typedef struct  
	{
		uint8_t length;
		Key keys[20];
	}PACK KeySequence;
	END_PACK	  
	 
	BEGIN_PACK
	typedef struct  
	{
		uint8_t mouseButtons;
		uint8_t X;
		uint8_t Y;
		uint8_t Wheel;
	}PACK MouseData; 
     END_PACK	
	 	 			 
	BEGIN_PACK
	typedef struct  
	{
		 MouseData mouseData;
		 KeySequence keySequence;
	}PACK HIDCommandData;
	END_PACK
	
	BEGIN_PACK
	typedef struct  
	{
		 bool connected;
		 bool usbSetup;
		 bool keyBoardBusy;
		 bool mouseBusy;
	}PACK HIDStatus;
	
	END_PACK		
	
	BEGIN_PACK
	typedef struct
	{
		uint8_t header[APS_ASDU_OFFSET]; // Header
		HIDCommandData data;
		uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET]; //Footer
	} PACK HIDCommandPacket;
	END_PACK
	
	static APS_RegisterEndpointReq_t hidEndpointParams;
     static SimpleDescriptor_t hidEndpoint = {HID_CONTROL,1, 1, 1, 0, 0, NULL, 0, NULL};
#endif


#if ((DEVICE_MESSAGE_SUPPORT & DEVICE_MESSAGE_ALL) == DEVICE_MESSAGE_ALL)

     typedef enum{		 
          MESSAGE_TYPE_STATUS,
	     MESSAGE_TYPE_CONTROL,
	     MESSAGE_TYPE_NETWORK	  
     } MESSAGE_TYPE;

     BEGIN_PACK
	typedef struct
	{
		uint8_t messageType; // Message type... 
		uint8_t moduleType; //  will correspond to one of our other ___MessageData structs.... use our MODULE_TYPE to decide.		
		ShortAddr_t addr;
	     union
		{
			//NETWORK MESSAGE TYPESs			
			NetworkJoinData networkPacket;
			
			//STATUS MESSAGE TYPES
		     StatusMessageData statusPacket;
			 
			//CONTROL MESSAGE TYPES 	 
			DimmerCommandData dimmerPacket;
			HIDCommandData hidPacket;
			ShadeCommandData shadePacket;
			IRCommandData irPacket;			 
		};
	} PACK UsartMessagePacket;
	END_PACK
#endif

/*****************************************************************************
******************************************************************************
*                                                                            *
*                               FUNCTIONS                                    *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/
inline static void stuffStatusPacket(uint8_t *message, size_t length, StatusMessagePacket *packet)
{
	for(size_t i = 0; i< length; i++)
	{
		packet->data.message[i] = message[i];		
	}
	packet->data.length = length;
	return;
}

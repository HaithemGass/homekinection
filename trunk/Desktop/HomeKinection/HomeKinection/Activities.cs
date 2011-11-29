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

using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using HomeKinection_Speech;

namespace HomeKinection
{
    [Serializable]
    public abstract class PropertyNotifier : INotifyPropertyChanged
    {
        #region INotifyPropertyChanged
        [field: NonSerialized]
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
    [Serializable]
    public class Activity : PropertyNotifier
    {
        protected static string commonFilePath = System.IO.Directory.GetParent(System.IO.Directory.GetCurrentDirectory()).Parent.FullName + "\\Activities\\";
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
		
		public ActivityTrigger entranceTrigger;
		public ActivityTrigger exitTrigger;
        public List<ActivityTrigger> triggers;  
		
		public Activity()
		{
			triggers = new List<ActivityTrigger>();
			entranceTrigger = new ActivityTrigger();
			exitTrigger = new ActivityTrigger();
		}

        public void ActiviateAutomaticTriggers()
        {
            foreach (ActivityTrigger t in triggers)
            {
                if (!t.hasConditions())
                {
                    t.ActivateActions();
                }
            }
        }

        public void AttachVoiceTriggers(Recognizer voiceRecognizer)
        {
            foreach (ActivityTrigger t in triggers)
            {                
                voiceRecognizer.SaidSomething += t.voiceRecognizer_SaidSomething;
            }
        }

        public void DetachVoiceTriggers( Recognizer voiceRecognizer)
        {
            foreach (ActivityTrigger t in triggers)
            {                
                voiceRecognizer.SaidSomething -= t.voiceRecognizer_SaidSomething;
            }
        }

        public void AttachGestureTriggers(RecognitionEngine gestureRecognizer)
        {
            foreach(ActivityTrigger t in triggers)
            {
                gestureRecognizer.SawSomething += t.gestureRecognizer_SawSomething;				
            }
        }

        public void DetachGestureTriggers(RecognitionEngine gestureRecognizer)
        {
            foreach (ActivityTrigger t in triggers)
            {
                gestureRecognizer.SawSomething -= t.gestureRecognizer_SawSomething;                
            }
        }

        public override String ToString()
        {
            return Name;
        }

        public void Serialize()
        {
            String filename = commonFilePath + name + ".ac"; //use a .ac file for modules
            Stream stream = File.Open(filename, FileMode.Create);
            BinaryFormatter bFormatter = new BinaryFormatter();
            bFormatter.Serialize(stream, this);
            stream.Close();
        }

        public static Activity DeSerialize(string filename)
        {
            Activity objectToSerialize;
            Stream stream = File.Open(filename, FileMode.Open);
            BinaryFormatter bFormatter = new BinaryFormatter();
            objectToSerialize = (Activity)bFormatter.Deserialize(stream);
            stream.Close();
            return objectToSerialize;
        }

        public void DeleteFile()
        {
            File.Delete(commonFilePath + name + ".ac");
        }

    }

    [Serializable]
	public class ModuleAction
	{
		public NetworkProtocol.UsartMessagePacket packet;
        public bool enableAbsoluteControl;
		public ModuleAction()
		{    
			packet = new NetworkProtocol.UsartMessagePacket();
            enableAbsoluteControl = false;
		}
	}


    [Serializable]
    public class ActivityTrigger : PropertyNotifier
    {
        private String VoiceCondition;
        private String GestureCondition;
		
		private bool VoiceEnable;
		private bool GestureEnable;
		
		public bool voiceEnable
		{
			get
			{
				return VoiceEnable;
			}
			set
			{
				VoiceEnable = value;
				NotifyPropertyChanged("voiceEnable");
			}
		}
		
		public bool gestureEnable
		{
			get
			{
				return GestureEnable;
			}
			set
			{
				GestureEnable = value;
				NotifyPropertyChanged("gestureEnable");
			}
		}
		
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
		
		
        public String voiceCondition
        {
            get
            {
                return VoiceCondition;
            }
            set
            {
                VoiceCondition = value;
                NotifyPropertyChanged("voiceCondition");
            }
        }			

        public String gestureCondition
        {
            get
            {
                return GestureCondition;
            }
            set
            {
                GestureCondition = value;
                NotifyPropertyChanged("gestureCondition");
            }
        }

        public List<ActivityAction> actions;
		
		public override String ToString()
        {
            return Name;
        }

        public bool hasConditions()
        {
            return voiceEnable || gestureEnable;
        }

        public void ActivateActions()
        {
            foreach (ActivityAction a in actions)
            {
                a.Execute();
            }
        }


        public void gestureRecognizer_SawSomething(object sender, RecognitionEngine.SawSomethingArgs e)
        {
            if (gestureEnable && e.Gesture.name.Equals(GestureCondition))
            {
                this.ActivateActions();
            }
        }
		
		public void voiceRecognizer_SaidSomething(object sender, Recognizer.SaidSomethingArgs e)
        {
        	if(voiceEnable && e.Matched.Trim('.').Equals(VoiceCondition))
			{
				this.ActivateActions();				
			}	
                   
        }


		public ActivityTrigger()
		{
			actions = new List<ActivityAction>();
		}

    }

    [Serializable]
    public class ActivityAction : PropertyNotifier
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

        private bool AbsoluteControl;

        public bool absoluteControl
        {
            get
            {
                return AbsoluteControl;
            }
            set
            {
                AbsoluteControl = value;
                NotifyPropertyChanged("absoluteControl");
            }
        }
		
		public override String ToString()
        {
            return Name;
        }
		
        public ModuleBox module;
        public NetworkProtocol.UsartMessagePacket packet;

        public void Execute()
        {
            if (module.disableGestures)
            {
                if (!absoluteControl)
                {
                    module.disableGestures = false;
                }
            }
            else
            {
                if (!absoluteControl)
                {
                    NetworkProtocol.sendMessageToModule(module.address, packet);
                }
                else
                {
                    module.disableGestures = true;
                }
            }
            
        }

    }

}

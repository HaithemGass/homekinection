/////////////////////////////////////////////////////////////////////////
//
// This module provides sample code used to demonstrate the use
// of the KinectAudioSource for speech recognition in a game setting.
//
// Copyright © Microsoft Corporation.  All rights reserved.  
// This code is licensed under the terms of the 
// Microsoft Kinect for Windows SDK (Beta) from Microsoft Research 
// License Agreement: http://research.microsoft.com/KinectSDK-ToU
//
/////////////////////////////////////////////////////////////////////////
/*
 * IMPORTANT: This sample requires the following components to be installed:
 * 
 * Speech Platform Runtime (v10.2) x86. Even on x64 platforms the x86 needs to be used because the MSR Kinect SDK runtime is x86
 * http://www.microsoft.com/downloads/en/details.aspx?FamilyID=bb0f72cb-b86b-46d1-bf06-665895a313c7
 *
 * Kinect English Language Pack: MSKinectLangPack_enUS.msi 
 * http://go.microsoft.com/fwlink/?LinkId=220942
 * 
 * Speech Platform SDK (v10.2) 
 * http://www.microsoft.com/downloads/en/details.aspx?FamilyID=1b1604d3-4f66-4241-9a21-90a294a5c9a4&displaylang=en
 * */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Media;
using System.Windows.Threading;
using Microsoft.Research.Kinect.Audio;
using Microsoft.Speech.AudioFormat;
using Microsoft.Speech.Recognition;
using HomeKinection;

namespace HomeKinection_Speech
{
    public class Recognizer
    {
        public static string RecordString = "Record Gesture";
        public static string StopRecordString = "Recording Finished";

        public class SaidSomethingArgs : EventArgs
        {            
            public string Phrase { get; set; }
            public string Matched {get; set; }
        }
        
        public event EventHandler<SaidSomethingArgs> SaidSomething;
        

        private KinectAudioSource kinectSource;
        private SpeechRecognitionEngine sre;
        private const string RecognizerId = "SR_MS_en-US_Kinect_10.0";        
        private bool valid = false;
        private String keyword = null;


        public Recognizer(String kw, Dictionary<String, Activity> activities)
        {
            RecognizerInfo ri = SpeechRecognitionEngine.InstalledRecognizers().Where(r => r.Id == RecognizerId).FirstOrDefault();
            if (ri == null)
                return;
                
            sre = new SpeechRecognitionEngine(ri.Id);

            BuildGrammar(kw, activities);
           
            sre.SpeechRecognized += sre_SpeechRecognized;
            sre.SpeechHypothesized += sre_SpeechHypothesized;
            sre.SpeechRecognitionRejected += new EventHandler<SpeechRecognitionRejectedEventArgs>(sre_SpeechRecognitionRejected);

            var t = new Thread(StartDMO);
            t.Start();

            valid = true;
	    }

        public void BuildGrammar(String kw, Dictionary<String,Activity> activities)
        {
            if((String.Compare(kw,String.Empty) == 0)) return;
             // Build a grammar of all voice commands given
            var single = new Choices();
            single.Add(RecordString);
            single.Add(StopRecordString);

            foreach (KeyValuePair<String, Activity> entry in activities)
            {
                single.Add(entry.Key); // switch contexts
                foreach (ActivityTrigger t in entry.Value.triggers)
                {
                    if (t.voiceEnable && t.voiceCondition!= null)
                    {
                        single.Add(t.voiceCondition);
                    }
                }
            }
                           

            var allChoices = new Choices();            
            allChoices.Add(single);            

            keyword = kw;            
            var gb = new GrammarBuilder(keyword,0,1);
            SemanticResultKey keyworkKey = new SemanticResultKey("Keyword", gb);            

            var gb2 = new GrammarBuilder();
            gb2.AppendWildcard();
            gb2.Append(keyworkKey);
            gb2.Append(allChoices);
            gb2.AppendWildcard();

            var g = new Grammar(gb2);
            sre.LoadGrammar(g);
        }

        public bool IsValid()
        {
            return valid;
        }

        private void StartDMO()
        {
            kinectSource = new KinectAudioSource();
            kinectSource.SystemMode = SystemMode.OptibeamArrayOnly;
            kinectSource.FeatureMode = true;
            kinectSource.AutomaticGainControl = false;
            kinectSource.MicArrayMode = MicArrayMode.MicArrayAdaptiveBeam;
            var kinectStream = kinectSource.Start();
            sre.SetInputToAudioStream(kinectStream, new SpeechAudioFormatInfo(
                                                  EncodingFormat.Pcm, 16000, 16, 1,
                                                  32000, 2, null));
            sre.RecognizeAsync(RecognizeMode.Multiple);
        }

        public void Stop()
        {
            if (sre != null)
            {
                sre.RecognizeAsyncCancel();
                sre.RecognizeAsyncStop();
                kinectSource.Dispose();
            }
        }

        void sre_SpeechRecognitionRejected(object sender, SpeechRecognitionRejectedEventArgs e)
        {
            var said = new SaidSomethingArgs();
            said.Matched = "?";
            SaidSomething(new object(), said);
            //Console.WriteLine("\nSpeech Rejected");
        }

        void sre_SpeechHypothesized(object sender, SpeechHypothesizedEventArgs e)
        {
           // Console.Write("\rSpeech Hypothesized: \t{0}", e.Result.Text);
        }

        void sre_SpeechRecognized(object sender, SpeechRecognizedEventArgs e)
        {
            //Console.Write("\rSpeech Recognized: \t{0}", e.Result.Text);
            
            if (SaidSomething == null)
                return;

            if (e.Result == null) return;

            RecognitionResult result = e.Result;
            SemanticValue semantics = e.Result.Semantics;

            if (!semantics.ContainsKey("Keyword")) return; //no keyword;

            if (! (0 == String.Compare((String)(semantics["Keyword"].Value),keyword)) || e.Result.Confidence < .50)
            {
                return; //didn't start with the right key phrase.
            }

            var said = new SaidSomethingArgs();
            said.Phrase = e.Result.Text;
            //+1 for the space
            said.Matched = e.Result.Text.Substring(keyword.Length + 1, e.Result.Text.Length - (keyword.Length + 1));
                    

            SaidSomething(new object(), said);
        }
    }
}
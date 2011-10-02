using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Text;
using System.Media;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Threading;
using Microsoft.Research.Kinect.Nui;
using ShapeGame_Utils;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;


namespace ShapeGame
{    
    [Serializable]
    public struct SkeletalGesturePoint
    {
        public KinectSkeletonData skeletalData;
        public DateTime timestamp;

        public SkeletalGesturePoint(SkeletonData sd, DateTime dt)
        {
            timestamp = dt;
            skeletalData = new KinectSkeletonData(sd);
        }
    }
    
    public class SkeletalGesture : InputGesture
    {
        [Serializable]
        private struct GestureSerializationInfo
        {
            public List<SkeletalGesturePoint> gestureData;
            public String name;
            public Dictionary<JointID, Boolean> jointsToTrack;
            public Rect playerBounds;

            public GestureSerializationInfo(SkeletalGesture sg)
            {
                name = sg.name;
                gestureData = sg.gestureData;
                jointsToTrack = sg.jointsToTrack;
                playerBounds = sg.playerBounds;
            }
        }

        public List<SkeletalGesturePoint> gestureData;
        public String name;
        public Dictionary<JointID, Boolean> jointsToTrack;
        public Rect playerBounds;

        private SkeletalGesture(GestureSerializationInfo gsi)
        {
            gestureData = gsi.gestureData;
            name = gsi.name;
            jointsToTrack = gsi.jointsToTrack;
            playerBounds = gsi.playerBounds;        
        }
        public SkeletalGesture()
        {
            gestureData = new List<SkeletalGesturePoint>();
            name = "";
            jointsToTrack = new Dictionary<JointID, bool>();
            playerBounds = new Rect();
        }

        public SkeletalGesture(List<SkeletalGesturePoint> gd, Rect r)
        {
            name = "";
            jointsToTrack = new Dictionary<JointID, bool>();
            gestureData = gd;
            playerBounds = r;
        }

        public override bool Matches(Object targetElement, InputEventArgs inputEventArgs)
        {
            return false;
        }

        public void AddPoint(SkeletalGesturePoint sgp)
        {
            gestureData.Add(SkeletalGesture.TransformPoint(sgp));
        }

        public override String ToString()
        {
            return this.name;
        }

        private static double Distance(KinectVector p1, KinectVector p2 )
        {
            return Math.Sqrt( Math.Pow(p1.X - p2.X, 2) + Math.Pow(p1.Y - p2.Y, 2) + Math.Pow(p1.Z- p2.Z, 2));
        }

        static public SkeletalGesturePoint TransformPoint(SkeletalGesturePoint sgp)
        {            
            KinectVector playerPos = sgp.skeletalData.Position;            

            //translate
            foreach (JointID jid in Enum.GetValues((typeof(JointID))))
            {
                if (jid == JointID.Count) continue;
                Joint translatedJoint = new Joint();
                Microsoft.Research.Kinect.Nui.Vector translatedPos = new Microsoft.Research.Kinect.Nui.Vector();
                translatedPos.W = sgp.skeletalData.Joints[jid].Position.W - playerPos.W; //is this rotation? ... Nope
                translatedPos.X = sgp.skeletalData.Joints[jid].Position.X - playerPos.X;
                translatedPos.Y = sgp.skeletalData.Joints[jid].Position.Y - playerPos.Y;                
                translatedPos.Z = sgp.skeletalData.Joints[jid].Position.Z - playerPos.Z;
                translatedJoint.Position = translatedPos;
                translatedJoint.ID = jid;
                translatedJoint.TrackingState = sgp.skeletalData.Joints[jid].TrackingState;
                sgp.skeletalData.Joints[jid] = new KinectJoint(translatedJoint);                
            }
            playerPos.W = 0;
            playerPos.X = 0;
            playerPos.Y = 0;
            playerPos.Z = 0;
            sgp.skeletalData.Position = playerPos;

            //rotate ... only need to rotate about Y (modify x and z)
            double delta = Math.Atan2((double)(sgp.skeletalData.Joints[JointID.HipRight].Position.Z - sgp.skeletalData.Joints[JointID.HipLeft].Position.Z) ,
                (double)(sgp.skeletalData.Joints[JointID.HipRight].Position.X - sgp.skeletalData.Joints[JointID.HipLeft].Position.X)); //get angle off of hips

            foreach (JointID jid in Enum.GetValues((typeof(JointID))))
            {
                if (jid == JointID.Count) continue;

                Joint rotatedJoint = new Joint();
                Microsoft.Research.Kinect.Nui.Vector rotatedPos = new Microsoft.Research.Kinect.Nui.Vector();
                

                double theta = ( Math.Atan2((double)sgp.skeletalData.Joints[jid].Position.X , (double)sgp.skeletalData.Joints[jid].Position.Z));
                float rad = (float)Math.Sqrt(Math.Pow((double)sgp.skeletalData.Joints[jid].Position.Z, 2) + 
                    Math.Pow((double)sgp.skeletalData.Joints[jid].Position.X, 2));
                theta += delta;

                float rotatedX = rad * (float)Math.Sin(theta);
                float rotatedZ = rad * (float)Math.Cos(theta);

                rotatedPos.W = sgp.skeletalData.Joints[jid].Position.W; //is this rotation? 
                rotatedPos.X = (double.IsNaN((double) rotatedX)) ?  sgp.skeletalData.Joints[jid].Position.X : rotatedX;
                rotatedPos.Y = sgp.skeletalData.Joints[jid].Position.Y;
                rotatedPos.Z = (double.IsNaN((double)rotatedZ)) ? sgp.skeletalData.Joints[jid].Position.Z : rotatedZ;
                rotatedJoint.Position = rotatedPos;
                rotatedJoint.ID = jid;
                rotatedJoint.TrackingState = sgp.skeletalData.Joints[jid].TrackingState;
                sgp.skeletalData.Joints[jid] = new KinectJoint(rotatedJoint);
            }


            //normalize (lets make it so we have an upperarm  of length .5) "scale"
            //float scaleFactor = (float)(1 / (Distance(sgp.skeletalData.Joints[JointID.ShoulderLeft].Position, sgp.skeletalData.Joints[JointID.ElbowLeft].Position)));
            float scaleFactor = 1;
            foreach (JointID jid in Enum.GetValues((typeof(JointID))))
            {
                if (jid == JointID.Count) continue;
                Joint scaledJoint = new Joint();
                Microsoft.Research.Kinect.Nui.Vector scaledPos = new Microsoft.Research.Kinect.Nui.Vector();
                scaledPos.W = sgp.skeletalData.Joints[jid].Position.W * scaleFactor;
                scaledPos.X = sgp.skeletalData.Joints[jid].Position.X * scaleFactor;
                scaledPos.Y = sgp.skeletalData.Joints[jid].Position.Y * scaleFactor;
                scaledPos.Z = sgp.skeletalData.Joints[jid].Position.Z * scaleFactor;
                scaledJoint.Position = scaledPos;
                scaledJoint.ID = jid;
                scaledJoint.TrackingState = sgp.skeletalData.Joints[jid].TrackingState;
                sgp.skeletalData.Joints[jid] = new KinectJoint(scaledJoint);
            }

            return sgp;
        }

       
        public static void Serialize(string filepath, SkeletalGesture objectToSerialize)
        {
            String filename = filepath + objectToSerialize.name + ".sg"; //use a .sg file for skeletal gestures
            Stream stream = File.Open(filename, FileMode.Create);
            BinaryFormatter bFormatter = new BinaryFormatter();
            bFormatter.Serialize(stream, new GestureSerializationInfo(objectToSerialize));
            stream.Close();
        }

        public static SkeletalGesture DeSerialize(string filename)
        {
            GestureSerializationInfo objectToSerialize;
            Stream stream = File.Open(filename, FileMode.Open);
            BinaryFormatter bFormatter = new BinaryFormatter();
            objectToSerialize = (GestureSerializationInfo)bFormatter.Deserialize(stream);
            stream.Close();
            return new SkeletalGesture(objectToSerialize);
        }       
    }
}

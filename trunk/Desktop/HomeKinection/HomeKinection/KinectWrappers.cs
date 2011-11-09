using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Research.Kinect.Nui;

namespace HomeKinection
{
    public interface ISkeletonFrame
    {
        Int64 TimeStamp { get; }
        int FrameNumber { get; }
        SkeletonFrameQuality Quality { get; }
        Vector FloorClipPlane { get; }
        Vector NormalToGravity { get; }
        ISkeletonCollection Skeletons { get; }
    }

    public interface ISkeletonCollection : IEnumerable
    {
        int Count { get; }
        KinectSkeletonData this[int index] { get; }
        new IEnumerator GetEnumerator();
    }

    [Serializable]
    public class KinectSkeletonFrame : ISkeletonFrame
    {
        public KinectSkeletonFrame(SkeletonFrame frame) { _frame = frame; }
        public Int64 TimeStamp { get { return _frame.TimeStamp; } }
        public int FrameNumber { get { return _frame.FrameNumber; } }
        public SkeletonFrameQuality Quality { get { return _frame.Quality; } }
        public Vector FloorClipPlane { get { return _frame.FloorClipPlane; } }
        public Vector NormalToGravity { get { return _frame.NormalToGravity; } }
        public ISkeletonCollection Skeletons { get { return new KinectSkeletonCollection(_frame.Skeletons); } }
        private SkeletonFrame _frame;
    }

    [Serializable]
    public class KinectSkeletonCollection : ISkeletonCollection
    {
        public KinectSkeletonCollection(SkeletonData[] skeletons)
        {
            _skeletons = new KinectSkeletonData[skeletons.GetLength(0)];
            int i = 0;
            foreach (SkeletonData skeleton in skeletons)
            {
                _skeletons[i++] = new KinectSkeletonData(skeleton);
            }
        }
        public int Count { get { return _skeletons.GetLength(0); } }
        public KinectSkeletonData this[int i] { get { return _skeletons[i]; } }
        public IEnumerator GetEnumerator() { return _skeletons.GetEnumerator(); }
        private KinectSkeletonData[] _skeletons;    
    }

    [Serializable]
    public class KinectSkeletonData
    {   
        public KinectSkeletonData(SkeletonData data) { 
            TrackingState = data.TrackingState;
            TrackingID = data.TrackingID;
            UserIndex = data.UserIndex;
            Quality = data.Quality;
            Position = new KinectVector(data.Position);
            Joints = new KinectJointsCollection(data.Joints);
        }
        public SkeletonTrackingState TrackingState;
        public int TrackingID;
        public int UserIndex ;
        public KinectVector Position;
        public KinectJointsCollection Joints; 
        public SkeletonQuality Quality; 
    }

    [Serializable]
    public class KinectJointsCollection
    {
        public KinectJointsCollection(JointsCollection joints) 
        { 
            _joints = new Dictionary<JointID,KinectJoint>();
            foreach (Joint j in joints)
            {
                _joints.Add(j.ID,new KinectJoint(j));
            }
        }
        public int Count { get { return _joints.Count; } }
        public KinectJoint this[JointID i] { get { return _joints[i]; }
                                       set { _joints[i] = value; }
        }
        public IEnumerator GetEnumerator() { return _joints.GetEnumerator(); }       
        private Dictionary<JointID,KinectJoint> _joints;
    }

    [Serializable]
    public struct KinectJoint
    {
        public KinectJoint(Joint joint) {
            Position = new KinectVector(joint.Position);
            ID = joint.ID;
            TrackingState = joint.TrackingState;
                
        }
        public JointTrackingState TrackingState;
        public JointID ID;
        public KinectVector Position;
    }

    [Serializable]
    public struct KinectVector
    {

        public float W;
        public float X;
        public float Y;
        public float Z;

        public KinectVector(Vector v)
        {
            W = v.W;
            X = v.X;
            Y = v.Y;
            Z = v.Z;
        }

    }
}   
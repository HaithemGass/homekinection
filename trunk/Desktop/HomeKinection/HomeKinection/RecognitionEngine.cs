using System;
using System.IO;
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
using System.Windows.Shapes;
using System.Windows.Threading;
using Microsoft.Research.Kinect.Nui;
using HomeKinection_Speech;


namespace HomeKinection
{
    class RecognitionEngine
    {
        private enum spikeState
        {
            spikeStarted, spikeMinimum, spikeFinished, spikeNeutral
        }
        private Dictionary<SkeletalGesture, double[,]> P;
        private Dictionary<SkeletalGesture, double[,]> D;
        private Dictionary<SkeletalGesture,double[]> A;
        private Dictionary<SkeletalGesture, double> Avg;
        private Dictionary<SkeletalGesture, double> AvgCnt;
        private SkeletalGesturePoint prev_sgp;

        private const int NUM_SAMPLES = 2;
        private const double MOVEMENT_FACTOR = 32;
        private const double FILTER_WEIGHT = .5;


        private bool firstUpdate;
        
        private StreamWriter fout;
        private Dictionary<SkeletalGesture, spikeState> SpikeState;
        private Dictionary<SkeletalGesture, double> vel;
        private Dictionary<SkeletalGesture, double> prev_vel;
        private Dictionary<SkeletalGesture, double> upper_avg;
        private Dictionary<SkeletalGesture, double> lower_avg;
        private Dictionary<SkeletalGesture, bool> firstNonInfinite;

        public class SawSomethingArgs : EventArgs
        {
            public SkeletalGesture Gesture { get; set; }

        }

        public event EventHandler<SawSomethingArgs> SawSomething;
        

        public RecognitionEngine(List<SkeletalGesture> library)
        {

            //P is the DP table for least cost paths. P[0][0-T] is present, P[1] is t-1 and P[2] is t-2. 
            P = new Dictionary<SkeletalGesture, double[,] >();
            D = new Dictionary<SkeletalGesture, double[,]>();
            A = new Dictionary<SkeletalGesture, double[]>();
            Avg = new Dictionary<SkeletalGesture, double>();
            AvgCnt = new Dictionary<SkeletalGesture, double>();
            SpikeState = new Dictionary<SkeletalGesture, spikeState>();
            firstNonInfinite = new Dictionary<SkeletalGesture, bool>();
            vel = new Dictionary<SkeletalGesture, double>();
            prev_vel = new Dictionary<SkeletalGesture, double>();
            lower_avg = new Dictionary<SkeletalGesture, double>();
            upper_avg = new Dictionary<SkeletalGesture, double>();
            fout = File.CreateText(System.IO.Directory.GetParent(System.IO.Directory.GetCurrentDirectory()).Parent.FullName + "\\Gesture_Log.csv");            
            prev_sgp = new SkeletalGesturePoint();
            firstUpdate = true;
            String gestures = "";
            foreach (SkeletalGesture sg in library)
            {
                
                gestures = gestures + sg.name + ", ";
                SpikeState.Add(sg, spikeState.spikeNeutral);
                firstNonInfinite.Add(sg, false);
                P.Add(sg, new double[3,sg.gestureData.Count -1]);
                D.Add(sg, new double[2,sg.gestureData.Count -1]);
                A.Add(sg, new double[NUM_SAMPLES]);
                vel.Add(sg, 0);
                prev_vel.Add(sg, 0);
                upper_avg.Add(sg, 0);
                lower_avg.Add(sg, 0);
                Avg.Add(sg, 0);
                AvgCnt.Add(sg, 0);
                for (int i = A[sg].GetLength(0) - 1; i >= 0; i--)
                {
                    A[sg][i] = double.PositiveInfinity;
                } 
                for (int i = 0; i < 3; i++)
                {
                    for (int j = 0; j < sg.gestureData.Count - 1; j++)
                    {
                        P[sg][i,j] = double.PositiveInfinity; // preset to positive infinity
                        if (i < 2)
                        {
                            D[sg][i, j] = double.PositiveInfinity; // preset to positive infinity
                        }
                    }
                }
            }
            fout.WriteLine(gestures);
            fout.Close();
        }

        //update with our new point in time.
        public Dictionary<SkeletalGesture, double[]> Update(SkeletalGesturePoint sgp)
        {
            if (firstUpdate)
            {
                firstUpdate = false;
                prev_sgp = sgp;
                return A;
            }
            //Console.WriteLine(">>>>>>>>>>>  " + (sgp.timestamp.Ticks - prev_sgp.timestamp.Ticks) + "\n");

            //update the DP tables for each gesture we are keeping track of.
            foreach (SkeletalGesture s in P.Keys)
            {
                int count = s.gestureData.Count - 1;

                for (int i = A[s].GetLength(0) - 1; i > 0; i--)
                {
                    A[s][i] = A[s][i-1]; // A[0] is the current. A[1] is 1 frame old.
                }                                

                //new Distance Table first
                for (int i = 0; i < count; i++)
                {
                    //move old stuff out of the way
                    D[s][1,i] = D[s][0,i];
                    P[s][2,i] = P[s][1,i];
                    P[s][1,i] = P[s][0,i];

                    //put in new value
                    //D[s][0, i] = LocalRelativeDistance(sgp.skeletalData.Joints, prev_sgp.skeletalData.Joints, sgp.timestamp.Ticks - prev_sgp.timestamp.Ticks, s.gestureData[i + 1].skeletalData.Joints, s.gestureData[i].skeletalData.Joints, s.gestureData[i].timestamp.Ticks - s.gestureData[i + 1].timestamp.Ticks, s.jointsToTrack);
                    D[s][0, i] = LocalAbsoluteDistance(sgp.skeletalData.Joints, s.gestureData[i].skeletalData.Joints, s.jointsToTrack);                    
                }
                
                //new P table (accumulated distances)
                P[s][0, 0] = 3 * D[s][0, 0];
                P[s][0, 1] = Min(P[s][2, 0] + 2 * D[s][1, 1] + D[s][0, 1],
                    P[s][1,0] + 3* D[s][0,1],
                    P[s][0,0] + 3* D[s][0,1]);

                for (int i = 2; i < count; i++)
                {
                    P[s][0, i] = Min(P[s][2, i - 1] + 2 * D[s][1, i] + D[s][0, i],
                        P[s][1, i - 1] + 3 * D[s][0, i],
                        P[s][1, i - 2] + 3 * D[s][0, i - 1] + 3 * D[s][0, i]);
                }

                //okay now update our total "cost" for this gesture.
                A[s][0] = P[s][0, count - 1] / (3 * count); 
                //update our averages..as long as we get some legit data
                if (A[s][0] != double.PositiveInfinity)
                {
                    Avg[s] = (Avg[s] * AvgCnt[s] + A[s][0]) / (AvgCnt[s] + 1);
                    AvgCnt[s] += 1;
                }
                
            }
                        

            //check to see if we "recognize" anything at this timestep (right after a local minimum on the minimum gesture.)
            checkForRecognizedGesture();
            prev_sgp = sgp;
            return A;
        }


        private void checkForRecognizedGesture()
        {
            LowPassFilter();
            CheckForSpikes();
            //local min ... might be unsafe if we have no min
            SkeletalGesture s = LocalMinDetect();
            if (s != null && A[s][0] < double.PositiveInfinity)
            {
                SawSomethingArgs args = new SawSomethingArgs();
                args.Gesture = s;
                SawSomething(new object(), args);
            }
        }
        private void CheckForSpikes()
        {            
            foreach (SkeletalGesture s in A.Keys)
            {
                prev_vel[s] = vel[s];
                vel[s] = (A[s][0] - A[s][1]);
                if (!firstNonInfinite[s])
                {
                    if (A[s][0] < double.PositiveInfinity)
                    {
                        firstNonInfinite[s] = true;
                        upper_avg[s] = A[s][0];
                        lower_avg[s] = A[s][0];
                    }
                }
                else
                {
                    if (A[s][0] >= upper_avg[s])
                    {
                        double p = Math.Abs(upper_avg[s] - A[s][0])/ Math.Abs(upper_avg[s] + A[s][0]);
                        upper_avg[s] = p * A[s][0] + (1 - p) * upper_avg[s];
                        if (SpikeState[s] == spikeState.spikeMinimum)
                        {
                            SpikeState[s] = spikeState.spikeFinished;
                        }
                        else
                        {
                            SpikeState[s] = spikeState.spikeNeutral;
                        }
                    }
                    else if (A[s][0] <= lower_avg[s])
                    {
                        double p = Math.Abs(lower_avg[s] - A[s][0]) / Math.Abs(lower_avg[s] + A[s][0]);
                        lower_avg[s] = p * A[s][0] + (1 - p) * lower_avg[s];
                        SpikeState[s] = spikeState.spikeMinimum;                        
                    }
                    else
                    {
                        //okay so we are between the two... probably not a spike just yet.
                        double n = Math.Abs(A[s][0] - lower_avg[s]) / (upper_avg[s] - lower_avg[s]);

                        //weight it a bit towards affecting the upper_avg.... use a lower p to say that we aren't as sure how to move the average.
                        if (n < .4)
                        {
                            double p = Math.Abs(lower_avg[s] - A[s][0]) /(MOVEMENT_FACTOR * Math.Abs(lower_avg[s] + A[s][0]));
                            lower_avg[s] = p * A[s][0] + (1 - p) * lower_avg[s];                    
                            if(SpikeState[s] == spikeState.spikeNeutral)
                            {
                                SpikeState[s] = spikeState.spikeStarted;
                            }
                        }
                        else
                        {
                            double p = Math.Abs(upper_avg[s] - A[s][0]) / (MOVEMENT_FACTOR * Math.Abs(upper_avg[s] + A[s][0]));
                            upper_avg[s] = p * A[s][0] + (1 - p) * upper_avg[s];
                            if (SpikeState[s] == spikeState.spikeMinimum)
                            {
                                SpikeState[s] = spikeState.spikeFinished;
                            }
                            else if (SpikeState[s] == spikeState.spikeFinished)
                            {
                                SpikeState[s] = spikeState.spikeNeutral;
                            }
                             
                            
                        }                                                                               
                        
                        
                    }
                }

                //Console.WriteLine(">>>>>>>>>>>  " + (vel[s] - prev_vel[s]) / prev_vel[s] + "\n");
                //if(  (Math.Abs(vel[s] / A[s][1]) > .2) && (vel[s] < 0) )
                //{
                    //SpikeStarted[s] = true;                    
                //}
            }

        }
        private void LowPassFilter()
        {
            foreach (SkeletalGesture s in A.Keys)
            {
                A[s][0] = (A[s][1] < double.PositiveInfinity) ? (A[s][0] * FILTER_WEIGHT + A[s][1] * (1 - FILTER_WEIGHT)) : (A[s][0]);
            }
        }

        private SkeletalGesture LocalMinDetect()
        {
            Dictionary<SkeletalGesture, double> Mins = new Dictionary<SkeletalGesture, double>();
            SkeletalGesture minSg = null;
            double minA = double.PositiveInfinity;

            fout = File.AppendText(System.IO.Directory.GetParent(System.IO.Directory.GetCurrentDirectory()).Parent.FullName + "\\Gesture_Log.csv");

            //grab everyone at a spike min.
            foreach (SkeletalGesture s in P.Keys)
            {
                fout.Write(A[s][0] + ", ");
                if (SpikeState[s] == spikeState.spikeMinimum)
                {
                    Mins.Add(s, A[s][0]);
                }
            }
            fout.WriteLine("");
            fout.Close();

            //only consider our lowest spike.
            foreach (SkeletalGesture s in Mins.Keys)
            {                
                if (A[s][0] < minA)
                {
                    minA = Mins[s];
                    minSg = s;
                }
            }
            

            if (minSg == null)
            {
                return null;
            }

            //.2 is a magic number seen in data from me... lets use .25 just as a general guideline to filter out any large error spikes.  Most false positives stay above this. Most true gestures don't.  Maybe do a voting system as well.
            if (A[minSg][0] < 0.25 && (vel[minSg] >0) && (prev_vel[minSg] <0))
            {
                //invalidate any other competeing gestures...should help with "double detect" problems                
                foreach (SkeletalGesture s in P.Keys)
                {
                    SpikeState[s] = spikeState.spikeNeutral;
                }                
                return minSg;                
            }
            return null;
        }

        private double Min(double d1, double d2, double d3)
        {
            return Math.Min(d1, Math.Min(d2, d3));
        }


        //should normalize beforehand ... also need to normalize afterwards for the different number of joints to track
        private double LocalRelativeDistance(KinectJointsCollection s1, KinectJointsCollection s1_prev, long deltat1, KinectJointsCollection s2, KinectJointsCollection s2_prev, long deltat2, Dictionary<JointID, Boolean> jointsToCheck)
        {
            double dist = 0;
            int numJoints = 0;
            foreach (JointID j in Enum.GetValues((typeof(JointID))))
            {
                if (!jointsToCheck[j])
                {
                    continue;
                }
                numJoints++;

                double s1_deltaX = (s1[j].Position.X - s1_prev[j].Position.X) / deltat1;
                double s1_deltaY = (s1[j].Position.Y - s1_prev[j].Position.Y) / deltat1;
                double s1_deltaZ = (s1[j].Position.Z - s1_prev[j].Position.Z) / deltat1;

                double s2_deltaX = (s2[j].Position.X - s2_prev[j].Position.X) / deltat2;
                double s2_deltaY = (s2[j].Position.Y - s2_prev[j].Position.Y) / deltat2;
                double s2_deltaZ = (s2[j].Position.Z - s2_prev[j].Position.Z) / deltat2;

                //3D distance from each point
                dist += Math.Sqrt(Math.Pow(Math.Abs(s1_deltaX - s2_deltaX), 2) + Math.Pow(Math.Abs(s1_deltaY - s2_deltaY), 2) + Math.Pow(Math.Abs(s1_deltaZ - s2_deltaZ), 2));
            }
            dist = dist / numJoints;
            return dist;
        }

        //should normalize beforehand ... also need to normalize afterwards for the different number of joints to track
        private double LocalAbsoluteDistance(KinectJointsCollection s1, KinectJointsCollection s2, Dictionary<JointID, Boolean> jointsToCheck)
        {
            double dist = 0;
            int numJoints = 0;
            foreach (JointID j in Enum.GetValues((typeof(JointID))))
            {
                if (!jointsToCheck[j])
                {
                    continue;
                }
                numJoints++;

                double s1_deltaX = s1[j].Position.X;
                double s1_deltaY = s1[j].Position.Y;
                double s1_deltaZ = s1[j].Position.Z;

                double s2_deltaX = s2[j].Position.X;
                double s2_deltaY = s2[j].Position.Y;
                double s2_deltaZ = s2[j].Position.Z;

                //3D distance from each point
                dist += Math.Sqrt(Math.Pow(Math.Abs(s1_deltaX - s2_deltaX), 2) + Math.Pow(Math.Abs(s1_deltaY - s2_deltaY), 2) + Math.Pow(Math.Abs(s1_deltaZ - s2_deltaZ), 2));
            }
            dist = dist / numJoints;
            return dist;
        }
		
		private double JointDistance(KinectJoint j1, KinectJoint j2)
		{
			return Math.Sqrt(Math.Pow(j1.Position.X - j2.Position.X,2) + Math.Pow(j1.Position.Y - j2.Position.Y,2) + Math.Pow(j1.Position.Z - j2.Position.Z,2));
		}
		
        public double VerticalSlider(SkeletalGesturePoint sgp)
        {
			double armSpan = JointDistance(sgp.skeletalData.Joints[JointID.WristRight], sgp.skeletalData.Joints[JointID.ElbowRight]) + JointDistance(sgp.skeletalData.Joints[JointID.ShoulderRight], sgp.skeletalData.Joints[JointID.ElbowRight]);			
            return Math.Min( 1, Math.Max(0, (sgp.skeletalData.Joints[JointID.HandRight].Position.Y - (sgp.skeletalData.Joints[JointID.ShoulderRight].Position.Y - armSpan)/
                (2*armSpan))));  
        }
		
		public double HorizontalSlider(SkeletalGesturePoint sgp)
        {
            double armSpan = JointDistance(sgp.skeletalData.Joints[JointID.WristRight], sgp.skeletalData.Joints[JointID.ElbowRight]) + JointDistance(sgp.skeletalData.Joints[JointID.ShoulderRight], sgp.skeletalData.Joints[JointID.ElbowRight]);			
            return Math.Min( 1, Math.Max(0, (sgp.skeletalData.Joints[JointID.HandRight].Position.X - (sgp.skeletalData.Joints[JointID.ShoulderRight].Position.X - armSpan)/
                (2*armSpan))));  
        }
    }
}

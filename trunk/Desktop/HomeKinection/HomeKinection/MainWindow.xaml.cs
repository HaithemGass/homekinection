﻿/////////////////////////////////////////////////////////////////////////
//
// This module contains code to do Kinect NUI initialization,
// processing, displaying players on screen, and sending updated player
// positions to the game portion for hit testing.
//
// Copyright © Microsoft Corporation.  All rights reserved.  
// This code is licensed under the terms of the 
// Microsoft Kinect for Windows SDK (Beta) from Microsoft Research 
// License Agreement: http://research.microsoft.com/KinectSDK-ToU
//
/////////////////////////////////////////////////////////////////////////
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
using System.Windows.Shapes;
using System.Speech;
using System.Windows.Threading;

using System.IO.Ports;

using Microsoft.Research.Kinect.Nui;
using HomeKinection_Speech;


// Since the timer resolution defaults to about 10ms precisely, we need to
// increase the resolution to get framerates above between 50fps with any
// consistency.
using System.Runtime.InteropServices;
using System.Speech.Synthesis;
public class Win32
{
    [DllImport("Winmm.dll")]
    public static extern int timeBeginPeriod(UInt32 uPeriod);
}


namespace HomeKinection
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

   

    public partial class MainWindow : Window
    {
        const int TimerResolution = 2;  // ms
        const int NumIntraFrames = 3;
        const double MaxFramerate = 70;
        const double MinFramerate = 15; 
		

        public MainWindow()
        {
            InitializeComponent();
            // Restore window state to that last used
            //Rect bounds = Properties.Settings.Default.PrevWinPosition;
            //if (bounds.Right != bounds.Left)
            //{
                //this.Top = bounds.Top;
                //this.Left = bounds.Left;
               // this.Height = bounds.Height;
             //   this.Width = bounds.Width;
           // }
            //this.WindowState = (WindowState)Properties.Settings.Default.WindowState;                            
        }

        public class Player
        {
            public bool isAlive;            
            public DateTime lastUpdated;
            private Brush brJoints;
            private Brush brBones;
            private Rect playerBounds;
            private Point playerCenter;
            private double playerScale;
            private int id;
            private static int colorId = 0;

            private const double BONE_SIZE = 0.01;
            private const double HEAD_SIZE = 0.075;
            private const double HAND_SIZE = 0.03;

            // Keeping track of all bone segments of interest as well as head, hands and feet
            public Dictionary<Bone, BoneData> segments = new Dictionary<Bone, BoneData>();

            public Player(int SkeletonSlot)
            {
                id = SkeletonSlot;

                // Generate one of 7 colors for player
                int[] iMixr = { 0, 1, 1, 0, 1, 0, 1 };
                int[] iMixg = { 0, 1, 0, 1, 0, 1, 1 };
                int[] iMixb = { 1, 0, 1, 1, 0, 0, 1 };
                byte[] iJointCols = { 245, 200 };
                byte[] iBoneCols = { 235, 160 };

                int i = colorId;
                colorId = (colorId + 1) % iMixr.Count();

                brJoints = new SolidColorBrush(Color.FromRgb(iJointCols[iMixr[i]], iJointCols[iMixg[i]], iJointCols[iMixb[i]]));
                brBones = new SolidColorBrush(Color.FromRgb(iBoneCols[iMixr[i]], iBoneCols[iMixg[i]], iBoneCols[iMixb[i]]));
                lastUpdated = DateTime.Now;
            }

            public int getId()
            {
                return id;
            }

            public void setBounds(Rect r)
            {
                playerBounds = r;
                playerCenter.X = (playerBounds.Left + playerBounds.Right) / 2;
                playerCenter.Y = (playerBounds.Top + playerBounds.Bottom) / 2;
                playerScale = Math.Min(playerBounds.Width, playerBounds.Height / 2);
            }

            void UpdateSegmentPosition(JointID j1, JointID j2, Segment seg)
            {
                var bone = new Bone(j1, j2);
                if (segments.ContainsKey(bone))
                {
                    BoneData data = segments[bone];
                    data.UpdateSegment(seg);
                    segments[bone] = data;
                }
                else
                    segments.Add(bone, new BoneData(seg));
            }

            public void UpdateBonePosition(Microsoft.Research.Kinect.Nui.JointsCollection joints, JointID j1, JointID j2)
            {
                var seg = new Segment(joints[j1].Position.X * playerScale + playerCenter.X,
                                      playerCenter.Y - joints[j1].Position.Y * playerScale,
                                      joints[j2].Position.X * playerScale + playerCenter.X,
                                      playerCenter.Y - joints[j2].Position.Y * playerScale);
                seg.radius = Math.Max(3.0, playerBounds.Height * BONE_SIZE) / 2;
                UpdateSegmentPosition(j1, j2, seg);
            }

            public void UpdateBonePosition(KinectJointsCollection joints, JointID j1, JointID j2)
            {
                var seg = new Segment(joints[j1].Position.X * playerScale + playerCenter.X,
                                      playerCenter.Y - joints[j1].Position.Y * playerScale,
                                      joints[j2].Position.X * playerScale + playerCenter.X,
                                      playerCenter.Y - joints[j2].Position.Y * playerScale);
                seg.radius = Math.Max(3.0, playerBounds.Height * BONE_SIZE) / 2;
                UpdateSegmentPosition(j1, j2, seg);
            }

            public void UpdateJointPosition(KinectJointsCollection joints, JointID j)
            {
                var seg = new Segment(joints[j].Position.X * playerScale + playerCenter.X,
                                      playerCenter.Y - joints[j].Position.Y * playerScale);
                seg.radius = playerBounds.Height * ((j == JointID.Head) ? HEAD_SIZE : HAND_SIZE) / 2;
                UpdateSegmentPosition(j, j, seg);
            }

            public void UpdateJointPosition(Microsoft.Research.Kinect.Nui.JointsCollection joints, JointID j)
            {
                var seg = new Segment(joints[j].Position.X * playerScale + playerCenter.X,
                                      playerCenter.Y - joints[j].Position.Y * playerScale);
                seg.radius = playerBounds.Height * ((j == JointID.Head) ? HEAD_SIZE : HAND_SIZE) / 2;
                UpdateSegmentPosition(j, j, seg);
            }

            public void Draw(UIElementCollection children)
            {
                if (!isAlive)
                    return;

                // Draw all bones first, then circles (head and hands).
                DateTime cur = DateTime.Now;
                foreach (var segment in segments)
                {
                    Segment seg = segment.Value.GetEstimatedSegment(cur);

                    if (!seg.IsCircle())
                    {
                        var line = new Line();
                        line.StrokeThickness = seg.radius * 2;
                        line.X1 = seg.x1;
                        line.Y1 = seg.y1;
                        line.X2 = seg.x2;
                        line.Y2 = seg.y2;
                        line.Stroke = brBones;
                        line.StrokeEndLineCap = PenLineCap.Round;
                        line.StrokeStartLineCap = PenLineCap.Round;
                        children.Add(line);
                    }
                }
                foreach (var segment in segments)
                {
                    Segment seg = segment.Value.GetEstimatedSegment(cur); 
                    if (seg.IsCircle())
                    {
                        var circle = new Ellipse();
                        circle.Width = seg.radius * 2;
                        circle.Height = seg.radius * 2;
                        circle.SetValue(Canvas.LeftProperty, seg.x1 - seg.radius);
                        circle.SetValue(Canvas.TopProperty, seg.y1 - seg.radius);
                        circle.Stroke = brJoints;
                        circle.StrokeThickness = 1;
                        circle.Fill = brBones;
                        children.Add(circle);
                    }
                }

                // Remove unused players after 1/2 second.
                if (DateTime.Now.Subtract(lastUpdated).TotalMilliseconds > 500)
                    isAlive = false;
            }
        }

        public Dictionary<int, Player> players = new Dictionary<int, Player>();      
        
        DateTime lastFrameDrawn = DateTime.MinValue;
        DateTime predNextFrame = DateTime.MinValue;
        double actualFrameTime = 0;

        // Player(s) placement in scene (z collapsed):
        Rect playerBounds;
        Rect screenRect;
        double targetFramerate = MaxFramerate;
        int frameCount = 0;

        bool isRecording;
        bool isRecordable = true;
		bool replayInProgress;

        public Player replayActor = new Player(0);
        SkeletalGesture currentGesture;
        List<SkeletalGesture> gestureLibrary;
        RecognitionEngine gestureRecognizer;


        bool runningGameThread = false;
        bool nuiInitialized = false;

        Runtime nui = Runtime.Kinects[0];
        Recognizer recognizer = null;

        void LoadGestureLibrary()
        {
            gestureLibrary = new List<SkeletalGesture>();
            System.IO.DirectoryInfo gestureDir = new System.IO.DirectoryInfo(System.IO.Directory.GetParent(System.IO.Directory.GetCurrentDirectory()).Parent.FullName + "\\Gestures");
            System.IO.FileInfo[] files = null;

            // First, process all the files directly under this folder
            try
            {
                files = gestureDir.GetFiles("*.sg"); //skeletal gesture files.
            }
            // This is thrown if even one of the files requires permissions greater
            // than the application provides.
            catch (UnauthorizedAccessException e)
            {
                System.Console.WriteLine("There seems to be a problem with your file structure.\n" + e.Message);
            }

            
            GestureLibraryBox.Items.Clear();
			IdleGesturePicker.Items.Clear();
            
            foreach (System.IO.FileInfo fi in files)
            {
                SkeletalGesture sg = SkeletalGesture.DeSerialize(fi.FullName) ;
                gestureLibrary.Add(sg);
                GestureLibraryBox.Items.Add(sg);
				IdleGesturePicker.Items.Add(sg);
            }

            gestureRecognizer = new RecognitionEngine(gestureLibrary);
            gestureRecognizer.SawSomething += gestureRecognizer_SawSomething;
        }

        private Boolean isJointEnabled(JointID j)
        {
            switch (j)
            {
                case JointID.AnkleLeft:
                    return (LeftAnkle.IsChecked == true);
                case JointID.AnkleRight:
                    return (RightAnkle.IsChecked == true); 
                case JointID.ElbowLeft:
                    return (LeftElbow.IsChecked == true);
                case JointID.ElbowRight:
                    return (RightElbow.IsChecked == true);
                case JointID.FootLeft:
                    return (LeftFoot.IsChecked == true);
                case JointID.FootRight:
                    return (RightFoot.IsChecked == true);
                case JointID.HandLeft:
                    return (LeftHand.IsChecked == true);
                case JointID.HandRight:
                    return (RightHand.IsChecked == true);
                case JointID.Head:
                    return (Head.IsChecked == true);
                case JointID.HipCenter:
                    return (HipCenter.IsChecked == true);
                case JointID.HipLeft:
                    return (LeftHip.IsChecked == true);
                case JointID.HipRight:
                    return (RightHip.IsChecked == true);
                case JointID.KneeLeft:
                    return (LeftKnee.IsChecked == true);
                case JointID.KneeRight:
                    return (RightKnee.IsChecked == true);
                case JointID.ShoulderCenter:
                    return (ShoulderCenter.IsChecked == true);
                case JointID.ShoulderLeft:
                    return (LeftShoulder.IsChecked == true);
                case JointID.ShoulderRight:
                    return (RightShoulder.IsChecked == true);
                case JointID.Spine:
                    return (Spine.IsChecked == true);
                case JointID.WristLeft:
                    return (LeftWrist.IsChecked == true);
                case JointID.WristRight:
                    return (RightWrist.IsChecked == true);
                default:
                    return false;
            }            
        }
        void nui_SkeletonFrameReady(object sender, SkeletonFrameReadyEventArgs e)
        {
            SkeletonFrame skeletonFrame = e.SkeletonFrame;

            int iSkeletonSlot = 0;
            bool firstPlayer = true;
            if (skeletonFrame == null || skeletonFrame.Skeletons == null) return;
            foreach (SkeletonData data in skeletonFrame.Skeletons)
            {
                

                if (SkeletonTrackingState.Tracked == data.TrackingState)
                {
                    Player player;
                    if (players.ContainsKey(iSkeletonSlot))
                    {
                        player = players[iSkeletonSlot];
                    }
                    else
                    {
                        player = new Player(iSkeletonSlot);
                        player.setBounds(playerBounds);
                        players.Add(iSkeletonSlot, player);
                    }

                    player.lastUpdated = DateTime.Now;                    

                    // Update player's bone and joint positions
                    if (data.Joints.Count > 0)
                    {
                        if (firstPlayer)
                        {
                            Dispatcher.Invoke(DispatcherPriority.Send,
                            new Action<SkeletalGesturePoint>(HandleGestureRecognizerFrame), new SkeletalGesturePoint(data, DateTime.Now));
                        }

                        //only record Player 1?
                        if (isRecording && firstPlayer)
                        {
                            firstPlayer = false;
                            replayActor.setBounds(playerBounds);
                            currentGesture.AddPoint(new SkeletalGesturePoint(data, DateTime.Now));
                        }

                        player.isAlive = true;
                        
                        // Head, hands, feet (hit testing happens in order here)
                        player.UpdateJointPosition(data.Joints, JointID.Head);
                        player.UpdateJointPosition(data.Joints, JointID.HandLeft);
                        player.UpdateJointPosition(data.Joints, JointID.HandRight);
                        player.UpdateJointPosition(data.Joints, JointID.FootLeft);
                        player.UpdateJointPosition(data.Joints, JointID.FootRight);

                        // Hands and arms
                        player.UpdateBonePosition(data.Joints, JointID.HandRight, JointID.WristRight);
                        player.UpdateBonePosition(data.Joints, JointID.WristRight, JointID.ElbowRight);
                        player.UpdateBonePosition(data.Joints, JointID.ElbowRight, JointID.ShoulderRight);

                        player.UpdateBonePosition(data.Joints, JointID.HandLeft, JointID.WristLeft);
                        player.UpdateBonePosition(data.Joints, JointID.WristLeft, JointID.ElbowLeft);
                        player.UpdateBonePosition(data.Joints, JointID.ElbowLeft, JointID.ShoulderLeft);
                        
                        // Head and Shoulders
                        player.UpdateBonePosition(data.Joints, JointID.ShoulderCenter, JointID.Head);
                        player.UpdateBonePosition(data.Joints, JointID.ShoulderLeft, JointID.ShoulderCenter);
                        player.UpdateBonePosition(data.Joints, JointID.ShoulderCenter, JointID.ShoulderRight);

                        // Legs
                        player.UpdateBonePosition(data.Joints, JointID.HipLeft, JointID.KneeLeft);
                        player.UpdateBonePosition(data.Joints, JointID.KneeLeft, JointID.AnkleLeft);
                        player.UpdateBonePosition(data.Joints, JointID.AnkleLeft, JointID.FootLeft);

                        player.UpdateBonePosition(data.Joints, JointID.HipRight, JointID.KneeRight);
                        player.UpdateBonePosition(data.Joints, JointID.KneeRight, JointID.AnkleRight);
                        player.UpdateBonePosition(data.Joints, JointID.AnkleRight, JointID.FootRight);

                        player.UpdateBonePosition(data.Joints, JointID.HipLeft, JointID.HipCenter);
                        player.UpdateBonePosition(data.Joints, JointID.HipCenter, JointID.HipRight);

                        // Spine
                        player.UpdateBonePosition(data.Joints, JointID.HipCenter, JointID.ShoulderCenter);
                    }
                }
                iSkeletonSlot++;
            }
        }

        void CheckPlayers()
        {
            foreach (var player in players)
            {
                if (!player.Value.isAlive)
                {
                    // Player left scene since we aren't tracking it anymore, so remove from dictionary
                    players.Remove(player.Value.getId());
                    break;
                }
            }

            // Count alive players
            int alive = 0;
            foreach (var player in players)
            {
                if (player.Value.isAlive)
                    alive++;
            }
        }

        void nui_ColorFrameReady(object sender, ImageFrameReadyEventArgs e)
        {
            // 32-bit per pixel, RGBA image
            PlanarImage Image = e.ImageFrame.Image;
            video.Source = BitmapSource.Create(
                Image.Width, Image.Height, 96, 96, PixelFormats.Bgr32, null, Image.Bits, Image.Width * Image.BytesPerPixel);
        }
      
        private bool InitializeNui()
        {
            UninitializeNui();
            if (nui == null)
                return false;
            try
            {
                nui.Initialize(RuntimeOptions.UseDepthAndPlayerIndex | RuntimeOptions.UseSkeletalTracking | RuntimeOptions.UseColor);
            }
            catch (Exception _Exception)
            {
                Console.WriteLine(_Exception.ToString());
                return false;
            }

            nui.DepthStream.Open(ImageStreamType.Depth, 2, ImageResolution.Resolution320x240, ImageType.DepthAndPlayerIndex);
            nui.VideoStream.Open(ImageStreamType.Video, 2, ImageResolution.Resolution640x480, ImageType.Color);
            nui.SkeletonEngine.TransformSmooth = true;
            nuiInitialized = true;
            return true;
        }

        private void UninitializeNui()
        {
            if ((nui != null) && (nuiInitialized))
                nui.Uninitialize();
            nuiInitialized = false;
        }

        private void Playfield_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            UpdatePlayfieldSize();
        }

        private void UpdatePlayfieldSize()
        {
            // Size of player wrt size of playfield, putting ourselves low on the screen.
            screenRect.X = 0;
            screenRect.Y = 0;
            screenRect.Width = skeletalVideo.ActualWidth;
            screenRect.Height = skeletalVideo.ActualHeight;

            BannerText.UpdateBounds(screenRect);

            playerBounds.X = 0;
            playerBounds.Width = skeletalVideo.ActualWidth;
            playerBounds.Y = skeletalVideo.ActualHeight * 0.2;
            playerBounds.Height = skeletalVideo.ActualHeight * 0.75;

            foreach (var player in players)
                player.Value.setBounds(playerBounds);

            Rect rFallingBounds = playerBounds;
            rFallingBounds.Y = 0;
            rFallingBounds.Height = skeletalVideo.ActualHeight;
            //if (//fallingThings != null)
           // {
                //fallingThings.SetBoundaries(rFallingBounds);
           // }
        }


        private NetworkProtocol networkProtocol;
        private void Window_Loaded(object sender, EventArgs e)
        {
            skeletalVideo.ClipToBounds = true;

            UpdatePlayfieldSize();

            if ((nui != null) && InitializeNui())
            {
                nui.VideoFrameReady += new EventHandler<ImageFrameReadyEventArgs>(nui_ColorFrameReady);
                nui.SkeletonFrameReady += new EventHandler<SkeletonFrameReadyEventArgs>(nui_SkeletonFrameReady);
                
                try
                {
                    recognizer = new Recognizer(keyWordTextBox.Text.Trim());
                }
                catch
                {
                    recognizer = null;
                }
                if ((recognizer== null) || !recognizer.IsValid())
                {
                    BannerText.NewBanner(Properties.Resources.NoSpeechError, screenRect, false, Color.FromArgb(90, 255, 255, 255));
                    recognizer = null;
                }
                else
                    recognizer.SaidSomething += recognizer_SaidSomething;
            }
            else
            {
                BannerText.NewBanner(Properties.Resources.NoKinectError, screenRect, false, Color.FromArgb(90, 255, 255, 255));
            }

            Win32.timeBeginPeriod(TimerResolution);
            var gameThread = new Thread(GameThread);
            gameThread.SetApartmentState(ApartmentState.STA);
            gameThread.Start();

            var networkThread = new Thread(NetworkThread);
            networkThread.SetApartmentState(ApartmentState.STA);
            networkThread.Start();

            FlyingText.NewFlyingText(screenRect.Width / 30, new Point(screenRect.Width / 2, screenRect.Height / 2), "Shapes!");
            LoadGestureLibrary();
        }

        private void NetworkThread()
        {
            networkProtocol = new NetworkProtocol(); //make sure a different thread owns network protocol.
        }

        private void GameThread()
        {
            runningGameThread = true;
            predNextFrame = DateTime.Now;
            actualFrameTime = 1000.0 / targetFramerate;

            // Try to dispatch at as constant of a framerate as possible by sleeping just enough since
            // the last time we dispatched.
            while (runningGameThread)
            {
                // Calculate average framerate.  
                DateTime now = DateTime.Now;
                if (lastFrameDrawn == DateTime.MinValue)
                    lastFrameDrawn = now;
                double ms = now.Subtract(lastFrameDrawn).TotalMilliseconds;
                actualFrameTime = actualFrameTime * 0.95 + 0.05 * ms;
                lastFrameDrawn = now;
                
                // Adjust target framerate down if we're not achieving that rate
                frameCount++;
                if (((frameCount % 100) == 0) && (1000.0 / actualFrameTime < targetFramerate * 0.92))
                    targetFramerate = Math.Max(MinFramerate, (targetFramerate + 1000.0 / actualFrameTime) / 2);

                if (now > predNextFrame)
                    predNextFrame = now;
                else
                {
                    double msSleep = predNextFrame.Subtract(now).TotalMilliseconds;
                    if (msSleep >= TimerResolution)  
                        Thread.Sleep((int)(msSleep + 0.5));
                }
                predNextFrame += TimeSpan.FromMilliseconds(1000.0 / targetFramerate);

                Dispatcher.Invoke(DispatcherPriority.Send,
                    new Action<int>(HandleGameTimer), 0);
            }
        }

        private bool allowstop = false;
        private void HandleGestureRecognizerFrame(SkeletalGesturePoint sgp)
        {
            NetworkProtocol.ShadeCommandData shadePacket = new NetworkProtocol.ShadeCommandData();
            NetworkProtocol.HIDCommandData hidPacket = new NetworkProtocol.HIDCommandData();
            NetworkProtocol.DimmerCommandData dimmerPacket = new NetworkProtocol.DimmerCommandData();
            shadePacket.Duration = 6000;
            hidPacket.keySequence.length = 0;
            


                if (gestureRecognizer != null)
                {
                    gestureRecognizer.Update(SkeletalGesture.TransformPoint(sgp));
                    double yPrev = ySlider.Value;
                    ySlider.Value = 100 * gestureRecognizer.VerticalSlider(SkeletalGesture.TransformPoint(sgp));

                    double xPrev = xSlider.Value;
                    xSlider.Value = 100 * gestureRecognizer.HorizontalSlider(SkeletalGesture.TransformPoint(sgp));

                    double x, y;
                    x = Math.Max(-127, Math.Min(128, xSlider.Value - xPrev));
                    y = Math.Max(-127, Math.Min(128, ySlider.Value - yPrev));

                    hidPacket.mouseData.Wheel = 0;
                    hidPacket.mouseData.mouseButtons = 0;

                    hidPacket.mouseData.X = (byte)(System.Convert.ToSByte(.8*(xSlider.Value-50)));
                    hidPacket.mouseData.Y = (byte)(System.Convert.ToSByte(-.8* (ySlider.Value-50)));

                    dimmerPacket.intensity = System.Convert.ToByte(ySlider.Value);

                    if (ySlider.Value != yPrev)
                    {
                       // Dispatcher.Invoke(DispatcherPriority.Send,
                        //    new Action<NetworkProtocol.DimmerCommandData>(networkProtocol.sendDimmerMessage), dimmerPacket);
                       // networkProtocol.sendDimmerMessage(dimmerPacket);
                    }

                    if (Math.Sqrt(Math.Pow((xSlider.Value - 50),2) + Math.Pow((ySlider.Value - 50),2)) > 10)
                    {

                        //NetworkProtocol.sendHIDMessage(serialPort, hidPacket);
                        Dispatcher.Invoke(DispatcherPriority.Send,
                            new Action<NetworkProtocol.HIDCommandData>(networkProtocol.sendHIDMessage), hidPacket);
                    }


                    if (ySlider.Value > 85)
                    {
                        if (yPrev < 85)
                        {
                            shadePacket.ButtonMask = NetworkProtocol.SHADE_DIRECTION_UP;
                            //NetworkProtocol.sendShadeMessage(serialPort, shadePacket);
                            allowstop = true;
                        }
                    }
                    else if (ySlider.Value < 15)
                    {
                        if (yPrev > 15)
                        {
                            shadePacket.ButtonMask = NetworkProtocol.SHADE_DIRECTION_DOWN;
                            //NetworkProtocol.sendShadeMessage(serialPort, shadePacket);
                            allowstop = true;
                        }
                    }
                    else
                    {
                        if (allowstop)
                        {
                            shadePacket.ButtonMask = 0;
                            //NetworkProtocol.sendShadeMessage(serialPort, shadePacket);
                            allowstop = false;
                        }
                    }
                }
            
        }

        private void DisplayGesture(Dictionary<SkeletalGesture, double[]> A)
        {
            double min = double.PositiveInfinity;
            String name = "";
            foreach (SkeletalGesture s in gestureLibrary)
            {
                if (A[s][0] < min)
                {
                    min = A[s][0];
                    name = s.ToString();
                }
            }
            Console.WriteLine(name + ": " + min);
        }
        private void HandleGameTimer(int param)
        {
            // Every so often, notify what our actual framerate is
            if ((frameCount % 100) == 0)
               // fallingThings.SetFramerate(1000.0 / actualFrameTime);

            // Advance animations, and do hit testing.
            for (int i = 0; i < NumIntraFrames; ++i)
            {
                foreach (var pair in players)
                {
                   // HitType hit = fallingThings.LookForHits(pair.Value.segments, pair.Value.getId());
                  //  if ((hit & HitType.Squeezed) != 0)
                  //      squeezeSound.Play();
                  //  else if ((hit & HitType.Popped) != 0)
                 //       popSound.Play();
                  //  else if ((hit & HitType.Hand) != 0)
                   //     hitSound.Play();
                }
                //fallingThings.AdvanceFrame();
            }

            // Draw new Wpf scene by adding all objects to canvas
            skeletalVideo.Children.Clear();
			if(!replayInProgress)recordedVideo.Children.Clear();
            //fallingThings.DrawFrame(skeletalVideo.Children);
            foreach (var player in players)
			{
				player.Value.Draw(skeletalVideo.Children);
			    if(!replayInProgress)player.Value.Draw(recordedVideo.Children);
			}

            PeekingText.Draw(skeletalVideo.Children);
            BannerText.Draw(skeletalVideo.Children);
            FlyingText.Draw(skeletalVideo.Children);            

            CheckPlayers();
        }

        void gestureRecognizer_SawSomething(object sender, RecognitionEngine.SawSomethingArgs e)
        {
            PeekingText.NewPeek(e.Gesture.ToString(), screenRect, .05, Color.FromArgb(200, 255, 255, 255));
            GestureTextBox.Text = GestureTextBox.Text + e.Gesture.ToString() + "\n";
        }


        void recognizer_SaidSomething(object sender, Recognizer.SaidSomethingArgs e)
        {            
            //FlyingText.NewFlyingText(screenRect.Width / 30, new Point(screenRect.Width / 2, screenRect.Height / 2), e.Matched);
            AudioTextBox.Text = AudioTextBox.Text + e.Matched + "\n";
            switch (e.Verb)
            {
               
                case Recognizer.Verbs.Record:
                    BeginGestureRecording();
                    break;
                case Recognizer.Verbs.StopRecord:
                    StopGestureRecording();
                    break;
            }
        }

        private void BeginGestureRecording()
        {
            if (isRecording || !isRecordable) return; //lets not try to start a new recording if we are in process here.
            isRecording = true;
            replayButton.IsEnabled = false;
            saveButton.IsEnabled = false;
            stopRecordButton.IsEnabled = true;
            recordButton.IsEnabled = false;
			recordButton.Visibility = System.Windows.Visibility.Hidden;
			stopRecordButton.Visibility = System.Windows.Visibility.Visible;
            currentGesture = new SkeletalGesture();
        }

        private void StopGestureRecording()
        {
            if (currentGesture != null)
            {
                currentGesture.playerBounds = playerBounds;
            }
            if (!isRecording) return; //nothing to do
            isRecording = false;
            if (!replayButton.IsEnabled)
            {
                replayButton.IsEnabled = true;
                saveButton.IsEnabled = true;
                recordButton.IsEnabled = true;
                stopRecordButton.IsEnabled = false;
				recordButton.Visibility = System.Windows.Visibility.Visible;
			    stopRecordButton.Visibility = System.Windows.Visibility.Hidden;
            }
        }



                                  /// <summary>
                                  /// 
                                  /// </summary>
                                  /// <param name="sender"></param>
                                  /// <param name="e"></param>
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            runningGameThread = false;
            Properties.Settings.Default.PrevWinPosition = this.RestoreBounds;
            Properties.Settings.Default.WindowState = (int)this.WindowState;
            Properties.Settings.Default.Save();
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            if (recognizer != null)
                recognizer.Stop();
            UninitializeNui();
            Environment.Exit(0);
        }

        private void replayGesture(object sender, System.Windows.RoutedEventArgs e)
        {
			replayInProgress = true;
			recordedVideo.Children.Clear();
            isRecordable = false;
            replayButton.IsEnabled = false;            
            var replayThread = new Thread(ReplayThread);
            replayThread.SetApartmentState(ApartmentState.STA);
            replayThread.Start();
        }

        private void ReplayThread()
        {
            SkeletalGesturePoint previous = currentGesture.gestureData[0]; //maybe dangerous
            
            foreach (SkeletalGesturePoint p in currentGesture.gestureData)
            {
                // Update player's bone and joint positions
                if (p.skeletalData.Joints.Count > 0)
                {
                    replayActor.isAlive = true;

                    // Head, hands, feet (hit testing happens in order here)
                    replayActor.UpdateJointPosition(p.skeletalData.Joints, JointID.Head);
                    replayActor.UpdateJointPosition(p.skeletalData.Joints, JointID.HandLeft);
                    replayActor.UpdateJointPosition(p.skeletalData.Joints, JointID.HandRight);
                    replayActor.UpdateJointPosition(p.skeletalData.Joints, JointID.FootLeft);
                    replayActor.UpdateJointPosition(p.skeletalData.Joints, JointID.FootRight);

                    // Hands and arms
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.HandRight, JointID.WristRight);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.WristRight, JointID.ElbowRight);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.ElbowRight, JointID.ShoulderRight);

                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.HandLeft, JointID.WristLeft);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.WristLeft, JointID.ElbowLeft);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.ElbowLeft, JointID.ShoulderLeft);

                    // Head and Shoulders
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.ShoulderCenter, JointID.Head);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.ShoulderLeft, JointID.ShoulderCenter);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.ShoulderCenter, JointID.ShoulderRight);

                    // Legs
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.HipLeft, JointID.KneeLeft);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.KneeLeft, JointID.AnkleLeft);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.AnkleLeft, JointID.FootLeft);

                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.HipRight, JointID.KneeRight);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.KneeRight, JointID.AnkleRight);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.AnkleRight, JointID.FootRight);

                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.HipLeft, JointID.HipCenter);
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.HipCenter, JointID.HipRight);

                    // Spine
                    replayActor.UpdateBonePosition(p.skeletalData.Joints, JointID.HipCenter, JointID.ShoulderCenter);
                }

                Dispatcher.Invoke(DispatcherPriority.Send,
                  new Action<int>(HandleReplayFrame), 0); // need to inform UI thread to draw stuff.

                //DateTime t1 = p.timestamp;
                //DateTime t2 = previous.timestamp;
                //long ticks = (Math.Abs(t1.Ticks - t2.Ticks)); // should always be positive but just in case
                //if (ticks > 0)
                //{
                    //Thread.Sleep((int)(ticks/10000)); //conversion
                //}
                Thread.Sleep(22);
            }
            isRecordable = true;
            Dispatcher.Invoke(DispatcherPriority.Send,
            new Action<int>(HandleReplayFinished), 0); // need to inform UI thread to draw stuff.
            Thread.CurrentThread.Abort();
        }

        private void HandleReplayFrame(int param)
        {
            recordedVideo.Children.Clear(); //kill the children           
            replayActor.Draw(recordedVideo.Children);            
          
        }

        private void HandleReplayFinished(int param)
        {
            replayButton.IsEnabled = true;
			replayInProgress = false;
        }

        private void changeKeyword(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
        	if (recognizer != null)
            {
                recognizer.BuildGrammar(keyWordTextBox.Text.Trim());
            }
        }

        private void saveGesture(object sender, System.Windows.RoutedEventArgs e)
        {
            currentGesture.name = NameTextBox.Text;           
            currentGesture.jointsToTrack = new Dictionary<JointID, bool>();
            foreach (JointID jid in Enum.GetValues((typeof(JointID))))
            {
                currentGesture.jointsToTrack.Add(jid, isJointEnabled(jid));                
            }
            SkeletalGesture.Serialize(System.IO.Directory.GetParent(System.IO.Directory.GetCurrentDirectory()).Parent.FullName + "\\Gestures\\", currentGesture);
            LoadGestureLibrary();
        }

        private void loadGesture(object sender, System.Windows.RoutedEventArgs e)
        {
            currentGesture = (SkeletalGesture) GestureLibraryBox.SelectedItem;
            if (currentGesture != null) replayButton.IsEnabled = true;
            replayActor.setBounds(currentGesture.playerBounds);
            saveButton.IsEnabled = true;
            NameTextBox.Text = currentGesture.name;
        }

        private void stopRecordClick(object sender, System.Windows.RoutedEventArgs e)
        {
            StopGestureRecording();
        }

        private void recordClick(object sender, System.Windows.RoutedEventArgs e)
        {
            BeginGestureRecording();
        }

        private void selectAllChecked(object sender, System.Windows.RoutedEventArgs e)
        {
        	for(int i = 0; i< BodyParts.Children.Count; i++)
			{
				CheckBox c = (CheckBox)(BodyParts.Children[i]);
				c.IsChecked = SelectAll.IsChecked;
			}
        }

        private void updateIdleGesture(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
        	gestureRecognizer.setIdleGesture((SkeletalGesture)IdleGesturePicker.SelectedItem);
        }

        private void enableIdle(object sender, System.Windows.RoutedEventArgs e)
        {        	
			IdleGesturePicker.IsEnabled = (bool)idleEnable.IsChecked;	
        }
    }
}

/////////////////////////////////////////////////////////////////////////
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
using System.IO;

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
        public Dictionary<int, RecognitionEngine> gestureRecognizers;
		private Dictionary<UInt64, ModuleBox> modules;
        private Dictionary<String, Activity> activities;


        bool runningGameThread = false;
        bool nuiInitialized = false;

        Runtime nui = Runtime.Kinects[0];
        Recognizer recognizer = null;

        void LoadActivities()
        {
            activities = new Dictionary<string, Activity>();
            ActivitiesListBox.Items.Clear();
            System.IO.DirectoryInfo modulesDir = new System.IO.DirectoryInfo(System.IO.Directory.GetParent(System.IO.Directory.GetCurrentDirectory()).Parent.FullName + "\\Modules");
            System.IO.FileInfo[] files = null;

            // First, process all the files directly under this folder
            try
            {
                files = modulesDir.GetFiles("*.ac"); //activity files.
            }
            // This is thrown if even one of the files requires permissions greater
            // than the application provides.
            catch (UnauthorizedAccessException e)
            {
                System.Console.WriteLine("There seems to be a problem with your file structure.\n" + e.Message);
            }

            foreach (System.IO.FileInfo fi in files)
            {
                Activity a = Activity.DeSerialize(fi.FullName);
                activities.Add(a.name, a);
            }
            reloadActivityList();
        }

        void LoadModules()
        {
            modules = new Dictionary<ulong,ModuleBox>();
            System.IO.DirectoryInfo modulesDir = new System.IO.DirectoryInfo(System.IO.Directory.GetParent(System.IO.Directory.GetCurrentDirectory()).Parent.FullName + "\\Modules");
            System.IO.FileInfo[] files = null;

            // First, process all the files directly under this folder
            try
            {
                files = modulesDir.GetFiles("*.md"); //module files.
            }
            // This is thrown if even one of the files requires permissions greater
            // than the application provides.
            catch (UnauthorizedAccessException e)
            {
                System.Console.WriteLine("There seems to be a problem with your file structure.\n" + e.Message);
            }

            foreach (System.IO.FileInfo fi in files)
            {
                ModuleBox b = ModuleBox.DeSerialize(fi.FullName);
                switch (b.type)
                {
                    case MODULE_TYPE.DIMMER_MODULE:
                        DimmerControlModule dc = (DimmerControlModule)b;
                        DimmerControl d = new DimmerControl(ModulesList, dc.name, dc.location,
                                dc.address, dc.uid, dc.type);

                        ModulesList.Items.Add(d);

                        modules.Add(dc.uid, (DimmerControlModule)d.FindResource("dataModel"));
                        break;

                    case MODULE_TYPE.SHADE_MODULE:
                        ShadeControlModule sc = (ShadeControlModule)b;
                        ShadeControl s = new ShadeControl(ModulesList, sc.name, sc.location,
                                sc.address, sc.uid, sc.type);

                        ModulesList.Items.Add(s);

                        modules.Add(sc.uid, (ShadeControlModule)s.FindResource("dataModel"));
                        break;

                    case MODULE_TYPE.HID_MODULE:
                        HIDControlModule hc = (HIDControlModule)b;
                        HIDControl h = new HIDControl(ModulesList, hc.name, hc.location,
                                hc.address, hc.uid, hc.type);

                        ModulesList.Items.Add(h);

                        modules.Add(hc.uid, (HIDControlModule)h.FindResource("dataModel"));
                        break;


                    default:
                        break;
                }
            }

            ModuleBox.modules = modules;
        }

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
            gestureRecognizers = new Dictionary<int, RecognitionEngine>();

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
                            new Action<SkeletalGesturePoint, int>(HandleGestureRecognizerFrame), new SkeletalGesturePoint(data, DateTime.Now), iSkeletonSlot);
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
                    gestureRecognizers.Remove(player.Value.getId());
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
            PeekingText.UpdateBounds(screenRect);

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

        
        private void Window_Loaded(object sender, EventArgs e)
        {
            skeletalVideo.ClipToBounds = true;

            UpdatePlayfieldSize();

            LoadGestureLibrary();
            LoadModules();
            LoadActivities();

            if ((nui != null) && InitializeNui())
            {
                nui.VideoFrameReady += new EventHandler<ImageFrameReadyEventArgs>(nui_ColorFrameReady);
                nui.SkeletonFrameReady += new EventHandler<SkeletonFrameReadyEventArgs>(nui_SkeletonFrameReady);
                
                try
                {
                    recognizer = new Recognizer(keyWordTextBox.Text.Trim(), activities);
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
          
        }

        private void NetworkThread()
        {
            NetworkProtocol.InitializeNetworkProtocol(this); //make sure a different thread owns network protocol.
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
        


        public void HandleStatusPacket(NetworkProtocol.UsartMessagePacket packet)
        {
            NetworkProtocol.StatusMessageData status = packet.statusPacket;
            if(modules == null) return; //aww snap we have a status packet but no modules thats not good.
            foreach (KeyValuePair<UInt64, ModuleBox> entry in modules)
            {
                if (entry.Value.address == status.shortAddress)
                {
                    entry.Value.handleStatusUpdate(status);
                }
            }
        }
        public void HandleNetworkJoin(NetworkProtocol.UsartMessagePacket packet)
        {
			if(modules == null || !(modules.ContainsKey(packet.networkPacket.deviceUID)))
			{
            	ModuleAddressLabel.Content = "0x"+packet.networkPacket.deviceUID.ToString("X");
            	ModuleTypeLabel.Content = NetworkProtocol.typeString[packet.moduleType];
            	NewModulePopup.IsOpen = true;
				moduleToSave = new ModuleBox();
				moduleToSave.address = packet.addr;
				moduleToSave.uid = packet.networkPacket.deviceUID;
				moduleToSave.type = (MODULE_TYPE)packet.moduleType;
			}
			else
			{
				//already know this guy but we rejoined... probably out short address changed when we power cycled.
				modules[packet.networkPacket.deviceUID].address = packet.addr;
                modules[packet.networkPacket.deviceUID].Serialize();
			}
        }

        private void HandleGestureRecognizerFrame(SkeletalGesturePoint sgp, int player)
        {      
			
            if (gestureRecognizers.ContainsKey(player))
            {
				bool gesturesEnabled = true;
				foreach(ModuleBox m in modules.Values)
				{
					if(m.disableGestures)
					{
						gesturesEnabled = false;
						m.updateAbsoluteControl(sgp);
					}
				}

                if(gesturesEnabled)
				{
					gestureRecognizers[player].Update(SkeletalGesture.TransformPoint(sgp));
				}
				              
            }
            else
            {
                gestureRecognizers.Add(player, new RecognitionEngine(gestureLibrary));
                gestureRecognizers[player].SawSomething += gestureRecognizer_SawSomething;
                if (currentActivity != null)
                {
                    currentActivity.AttachGestureTriggers(gestureRecognizers[player]);
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
            PeekingText.NewPeek(e.Gesture.ToString(), screenRect, .01, Color.FromArgb(200, 255, 255, 255));
            GestureTextBox.Text = GestureTextBox.Text + e.Gesture.ToString() + "\n";
        }


        void recognizer_SaidSomething(object sender, Recognizer.SaidSomethingArgs e)
        {
            PeekingText.NewPeek(e.Matched, screenRect, .01, Color.FromArgb(200, 255, 255, 255));
            AudioTextBox.Text = AudioTextBox.Text + e.Matched + "\n";
              
                 if(e.Matched.Equals(Recognizer.RecordString))
                    BeginGestureRecording();

                 if (e.Matched.Equals(Recognizer.StopRecordString))
                    StopGestureRecording();
				
				if(activities.ContainsKey(e.Matched.Trim('.')))
				{
					SwitchActivityContext(activities[e.Matched.Trim('.')]);
				}

        }

        private void BeginGestureRecording()
        {
            if (isRecording || !isRecordable) return; //lets not try to start a new recording if we are in process here.
            isRecording = true;
            replayButton.IsEnabled = false;
            saveButton.IsEnabled = false;
			deleteButton.IsEnabled = false;
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
				deleteButton.IsEnabled = true;
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
            if (currentGesture.gestureData.Count < 1) return;
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
                recognizer.BuildGrammar(keyWordTextBox.Text.Trim(), activities);
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
            replayButton.IsEnabled = true;
			deleteButton.IsEnabled = true;
            NameTextBox.Text = currentGesture.name;
        }

		private void deleteGesture(object sender, System.Windows.RoutedEventArgs e)
        {
            currentGesture = (SkeletalGesture) GestureLibraryBox.SelectedItem;
            if (currentGesture != null) replayButton.IsEnabled = true;
            replayActor.setBounds(currentGesture.playerBounds);
            replayButton.IsEnabled = false;
			deleteButton.IsEnabled = false;
            File.Delete(System.IO.Directory.GetParent(System.IO.Directory.GetCurrentDirectory()).Parent.FullName + "\\Gestures\\" + currentGesture.name + ".sg");
            LoadGestureLibrary();
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
            foreach (KeyValuePair<int, RecognitionEngine> entry in gestureRecognizers)
            {
                entry.Value.setIdleGesture((SkeletalGesture)IdleGesturePicker.SelectedItem);
            }
        	
        }

        private void enableIdle(object sender, System.Windows.RoutedEventArgs e)
        {        	
			IdleGesturePicker.IsEnabled = (bool)idleEnable.IsChecked;	
        }

        private void launchSaveDialog(object sender, System.Windows.RoutedEventArgs e)
        {
        	SaveDialog.IsOpen = true;
        }

        private void hideSaveDialog(object sender, System.Windows.RoutedEventArgs e)
        {
        	SaveDialog.IsOpen = false;
        }
		
		private ModuleBox moduleToSave;

        private void NewModuleSave(object sender, System.Windows.RoutedEventArgs e)
        {
			if(modules == null) modules = new Dictionary<UInt64,ModuleBox>();
			
            //add to Array here!!!
            NewModulePopup.IsOpen = false;            
			switch(moduleToSave.type)
			{
				case MODULE_TYPE.DIMMER_MODULE:
					DimmerControl dc = 
					new DimmerControl(ModulesList, ModuleNameBox.Text.ToString(), ModuleLocationBox.Text.ToString(), 
						moduleToSave.address,moduleToSave.uid, moduleToSave.type);
			
					ModulesList.Items.Add(dc);			        
					modules.Add(moduleToSave.uid, (DimmerControlModule)dc.FindResource("dataModel"));                    
                    DimmerControlModule d = (DimmerControlModule)dc.FindResource("dataModel");
					d.Serialize();
			    break;
					
				case MODULE_TYPE.SHADE_MODULE:
					ShadeControl sc = 
					new ShadeControl(ModulesList, ModuleNameBox.Text.ToString(), ModuleLocationBox.Text.ToString(), 
						moduleToSave.address,moduleToSave.uid, moduleToSave.type);
			
					ModulesList.Items.Add(sc);
			
					modules.Add(moduleToSave.uid, (ShadeControlModule)sc.FindResource("dataModel"));
                    ShadeControlModule s = (ShadeControlModule)sc.FindResource("dataModel");
					s.Serialize();
			    break;

                case MODULE_TYPE.HID_MODULE:
                HIDControl hc =
                new HIDControl(ModulesList, ModuleNameBox.Text.ToString(), ModuleLocationBox.Text.ToString(),
                    moduleToSave.address, moduleToSave.uid, moduleToSave.type);

                ModulesList.Items.Add(hc);

                modules.Add(moduleToSave.uid, (HIDControlModule)hc.FindResource("dataModel"));
				HIDControlModule h = (HIDControlModule)hc.FindResource("dataModel");	
                h.Serialize();
                break;
					
				default:
				break;
			}
			
			
        }
		
		

		private void reloadNewIRPicker()
		{
			NewIRPicker.Items.Clear();
			foreach(String s in IRControlModule.getDeviceList())
			{
				NewIRPicker.Items.Add(s);
			}
			String newItem = "New...";
			NewIRPicker.Items.Add(newItem);
		}
		
        private void AddDeviceToIRCommandList(object sender, System.Windows.RoutedEventArgs e)
        {
        	IRControlModule.addDevice(NewDeviceNameBox.Text);
			NewDevicePopup.IsOpen = false;
			reloadNewIRPicker();
			for(int i = 0; i < NewIRPicker.Items.Count; i++)
			{
				if(NewIRPicker.Items[i].ToString().Equals(NewDeviceNameBox.Text))
				{
					NewIRPicker.SelectedIndex = i;
				}
			}
        }

        private void NewIRDeviceSelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
        	if(NewIRPicker.SelectedIndex + 1 > IRControlModule.getDeviceList().Count)
			{
				NewDevicePopup.IsOpen = true;	
			}
        }

		private IRControlModule.IRCommand IRCommandToSave;
		
		public void HandleNewIRCommand(IRControlModule.IRCommand command)
        {
			reloadNewIRPicker();
			IRCommandToSave = command;
			NewIRBox.Text = "";
			IRRecordPopup.IsOpen = true;
		}

		private void SaveNewIRCommand(object sender, System.Windows.RoutedEventArgs e)
		{
			IRCommandToSave.name = NewIRBox.Text;
			IRControlModule.addCommand(NewIRPicker.SelectedItem.ToString(), IRCommandToSave);
			IRRecordPopup.IsOpen = false;
		}

		private void AddNewActivity(object sender, System.Windows.RoutedEventArgs e)
		{
            if (activities == null)
            {
                activities = new Dictionary<string, Activity>();
            }
            NewActivityPopup.IsOpen = true;
			currentEditActivity = new Activity();
			reloadTriggersList();
			reloadEntranceActionsList();
		}

		private void SaveNewActivity(object sender, System.Windows.RoutedEventArgs e)
		{
			if(ActivityNameText.Text.Equals("")) return;
			if(!activities.ContainsKey(ActivityNameText.Text))
			{				
				currentEditActivity.name = ActivityNameText.Text;
				activities.Add(currentEditActivity.name, currentEditActivity);
			}
			else
			{
				//This if for editing an existing.
				currentEditActivity = activities[ActivityNameText.Text]; 
				reloadTriggersList();
			}
			
			NewActivityPopup.IsOpen = false;
			ActivityNameText.Text = "";
			reloadActivityList();
			reloadTriggersList();

            if (recognizer != null)
            {
                recognizer.BuildGrammar(keyWordTextBox.Text.Trim(), activities);
            }

            currentEditActivity.Serialize();

		}
		
		private Activity currentEditActivity;
		
		private void reloadTriggersList()
		{
			TriggersListBox.Items.Clear();
			
			foreach(ActivityTrigger t in currentEditActivity.triggers)
			{
				TriggersListBox.Items.Add(t.name);
			}
		}
		
		private void reloadActivityList()
		{
			ActivitiesListBox.Items.Clear();
		
			foreach(String s in activities.Keys)
			{
				ActivitiesListBox.Items.Add(s);
			}
		}
		
		private void reloadActionsList()
		{
			ActionsListBox.Items.Clear();
		
			foreach(ActivityAction a in currentEditTrigger.actions)
			{
				ActionsListBox.Items.Add(a.name);
			}
		}
		
		private void reloadEntranceActionsList()
		{
			EntranceActionsListBox.Items.Clear();
		
			foreach(ActivityAction a in currentEditActivity.entranceTrigger.actions)
			{
				EntranceActionsListBox.Items.Add(a.name);
			}
		}
		
		private void loadModulesToNewActionPopup()
		{
			NewActionModuleComboBox.Items.Clear();
			foreach(ModuleBox mb in modules.Values)
			{
				NewActionModuleComboBox.Items.Add(mb);
			}
		}

		private ActivityTrigger currentEditTrigger;
		
		private void AddNewTrigger(object sender, System.Windows.RoutedEventArgs e)
		{			
            NewTriggerPopup.IsOpen = true;
			currentEditTrigger = new ActivityTrigger();			
			NewTriggerVoiceToggle.IsChecked = false;
			NewTriggerGestureToggle.IsChecked = false;
			NewTriggerVoiceBox.Text = "";
			NewTriggerGestureBox.Items.Clear();
			addGesturesToTriggerBox();
			reloadActionsList();
		}
		
		private void addGesturesToTriggerBox()
		{
			foreach(SkeletalGesture g in gestureLibrary)
			{
				NewTriggerGestureBox.Items.Add(g);
			}
			
		}

		private void SaveNewTrigger(object sender, System.Windows.RoutedEventArgs e)
		{			
			currentEditTrigger.name = "";
			if(currentEditTrigger.voiceEnable)
			{
				currentEditTrigger.name += "VOICE: " + currentEditTrigger.voiceCondition + " ";
			} 
			if(currentEditTrigger.gestureEnable)
			{
				currentEditTrigger.name += "GESTURE: " + currentEditTrigger.gestureCondition;
			} 
			currentEditActivity.triggers.Add(currentEditTrigger);
			
			NewTriggerPopup.IsOpen = false;			
			reloadTriggersList();
		}

		private void EditCurrentActivity(object sender, System.Windows.RoutedEventArgs e)
		{
			
			if(ActivitiesListBox.SelectedItem == null || !activities.ContainsKey(ActivitiesListBox.SelectedItem.ToString())) return;
			//delete OLD HERE
			
			NewActivityPopup.IsOpen = true;			
			currentEditActivity = activities[ActivitiesListBox.SelectedItem.ToString()];
            activities[ActivitiesListBox.SelectedItem.ToString()].DeleteFile();
			ActivityNameText.Text = currentEditActivity.name;			
			reloadTriggersList();
		}

		private void DeleteTrigger(object sender, System.Windows.RoutedEventArgs e)
		{
			if(TriggersListBox.SelectedIndex < 0 || TriggersListBox.SelectedIndex > currentEditActivity.triggers.Count) return;
			currentEditActivity.triggers.RemoveAt(TriggersListBox.SelectedIndex);				
			reloadTriggersList();

		}

		private void DeleteActivity(object sender, System.Windows.RoutedEventArgs e)
		{
		  // TODO: Delete off file system as well... once we actually get that working
			if(ActivitiesListBox.SelectedItem == null || !activities.ContainsKey(ActivitiesListBox.SelectedItem.ToString())) return;
            activities[ActivitiesListBox.SelectedItem.ToString()].DeleteFile();
			activities.Remove(ActivitiesListBox.SelectedItem.ToString());
			reloadActivityList();
		}

		private ActivityAction currentEditTriggerAction;
		
		
		private void addNewTriggerAction(object sender, System.Windows.RoutedEventArgs e)
		{
			NewActionPopup.IsOpen = true;
			currentEditTriggerAction = new ActivityAction();
            NewActionItemControl.Items.Clear();	
			NewActionSaveButton.Visibility = System.Windows.Visibility.Hidden;
			loadModulesToNewActionPopup();
		}

		private void DeleteAction(object sender, System.Windows.RoutedEventArgs e)
		{
			if(ActionsListBox.SelectedIndex < 0 || ActionsListBox.SelectedIndex > currentEditTrigger.actions.Count) return;
			currentEditTrigger.actions.RemoveAt(ActionsListBox.SelectedIndex);				
			reloadActionsList();
		}

		private void SaveNewAction(object sender, System.Windows.RoutedEventArgs e)
		{
			currentEditTriggerAction.name = currentEditModule.name;
			currentEditTriggerAction.packet = currentEditModuleAction.packet;
            currentEditTriggerAction.absoluteControl = currentEditModuleAction.enableAbsoluteControl;
            currentEditTriggerAction.module = currentEditModule;
			currentEditTrigger.actions.Add(currentEditTriggerAction);
			NewActionPopup.IsOpen = false;			
			reloadActionsList();
			reloadEntranceActionsList(); // just in case
		}

		private ModuleBox currentEditModule;
		private ModuleAction currentEditModuleAction; 
		private void UpdateActionModule(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
		{
			if(NewActionModuleComboBox.SelectedItem == null) return;
			currentEditModule = (ModuleBox)NewActionModuleComboBox.SelectedItem;
			NewActionItemControl.Items.Clear();		
			currentEditModuleAction = new ModuleAction();
			NewActionSaveButton.Visibility = System.Windows.Visibility.Visible;
			switch(currentEditModule.type)
			{
				case MODULE_TYPE.DIMMER_MODULE:
					DimmerAction da = new DimmerAction(currentEditModule.address);
					NewActionItemControl.Items.Add(da);
					currentEditModuleAction = da.action;
					break;
				
				case MODULE_TYPE.SHADE_MODULE:
					ShadeAction sa = new ShadeAction(currentEditModule.address);
					NewActionItemControl.Items.Add(sa);
					currentEditModuleAction = sa.action;
					break;
					
				case MODULE_TYPE.HID_MODULE:
					HIDAction ha = new HIDAction(currentEditModule.address);
					NewActionItemControl.Items.Add(ha);
					currentEditModuleAction = ha.action;
					break;
					
				default:
					break;
			}
			
			
		}

		private void SetTriggerVoiceEnable(object sender, System.Windows.RoutedEventArgs e)
		{
			currentEditTrigger.voiceEnable = true;
		}
		private void ClearTriggerVoiceEnable(object sender, System.Windows.RoutedEventArgs e)
		{
			currentEditTrigger.voiceEnable = false;
		}
		private void SetTriggerGestureEnable(object sender, System.Windows.RoutedEventArgs e)
		{
			currentEditTrigger.gestureEnable =  true;  
		}
		private void ClearTriggerGestureEnable(object sender, System.Windows.RoutedEventArgs e)
		{
			currentEditTrigger.gestureEnable = false;
		}

		private void UpdateTriggerVoiceText(object sender, System.Windows.Controls.TextChangedEventArgs e)
		{
			currentEditTrigger.voiceCondition = NewTriggerVoiceBox.Text;
		}
		private void UpdateTriggerGestureText(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
		{
			if(NewTriggerGestureBox.SelectedItem == null) return;
			currentEditTrigger.gestureCondition = NewTriggerGestureBox.SelectedItem.ToString();
		}

		
		private Activity currentActivity;
		
		private void StartActivityContext(object sender, System.Windows.RoutedEventArgs e)
		{
			if(ActivitiesListBox.SelectedItem == null) return;
			
			SwitchActivityContext(activities[ActivitiesListBox.SelectedItem.ToString()]);
           
			
		}
		private void SwitchActivityContext(Activity a)
		{
			if(currentActivity != null)
			{
				currentActivity.exitTrigger.ActivateActions();
                foreach(RecognitionEngine re in gestureRecognizers.Values)
                {
                    currentActivity.DetachGestureTriggers(re);
                }
                currentActivity.DetachVoiceTriggers(recognizer);
			}
			currentActivity = a;
            foreach (RecognitionEngine re in gestureRecognizers.Values)
            {
                currentActivity.AttachGestureTriggers(re);
            }
            currentActivity.AttachVoiceTriggers(recognizer);
			currentActivity.entranceTrigger.ActivateActions();
		}

		private void EditTrigger(object sender, System.Windows.RoutedEventArgs e)
		{			
			if(TriggersListBox.SelectedIndex < 0 || TriggersListBox.SelectedIndex > currentEditActivity.triggers.Count) return;
			currentEditTrigger = currentEditActivity.triggers[TriggersListBox.SelectedIndex];
			currentEditActivity.triggers.RemoveAt(TriggersListBox.SelectedIndex);
			NewTriggerPopup.IsOpen = true;			
			NewTriggerVoiceToggle.IsChecked = currentEditTrigger.voiceEnable;
			NewTriggerGestureToggle.IsChecked = currentEditTrigger.gestureEnable;
			NewTriggerVoiceBox.Text = currentEditTrigger.voiceCondition;
			NewTriggerGestureBox.Items.Clear();
			addGesturesToTriggerBox();
			foreach(Object o in NewTriggerGestureBox.Items)
			{
				if(o.ToString().Equals(currentEditTrigger.gestureCondition))
				{
					NewTriggerGestureBox.SelectedItem = o;
				}
					
			}
			reloadActionsList();
			
		}

		private void AddNewEntranceAction(object sender, System.Windows.RoutedEventArgs e)
		{
			currentEditTrigger = currentEditActivity.entranceTrigger;
			addNewTriggerAction(sender, e);
		}  
		private void AddNewExitAction(object sender, System.Windows.RoutedEventArgs e)
		{
			currentEditTrigger = currentEditActivity.exitTrigger;
			addNewTriggerAction(sender, e);
		}

		private void DeleteEntranceAction(object sender, System.Windows.RoutedEventArgs e)
		{
			
			if(EntranceActionsListBox.SelectedIndex < 0 || EntranceActionsListBox.SelectedIndex > currentEditActivity.entranceTrigger.actions.Count) return;
			currentEditActivity.entranceTrigger.actions.RemoveAt(EntranceActionsListBox.SelectedIndex);				
			reloadEntranceActionsList();
		} 
    }
}
